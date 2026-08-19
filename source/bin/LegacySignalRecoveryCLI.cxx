#include "LegacySignalRecoveryCLI.h"

#include <cerrno>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <system_error>

#ifdef _WIN32
#include <process.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace REST_LegacySignalRecoveryCLI {
namespace {

constexpr int kUsageError = 2;
constexpr int kSetupError = 3;
constexpr int kUnsupportedError = 4;

std::filesystem::path DefaultFixedOutput(const std::filesystem::path& input) {
    std::filesystem::path output = input;
    if (output.extension() == ".root") {
        output.replace_filename(output.stem().string() + "_Fixed.root");
    } else {
        output += "_Fixed.root";
    }
    return output;
}

bool PathEntryExists(const std::filesystem::path& path, std::error_code& error) {
    const auto status = std::filesystem::symlink_status(path, error);
    if (status.type() == std::filesystem::file_type::not_found) {
        error.clear();
        return false;
    }
    return !error;
}

bool RequireUnused(const std::filesystem::path& path, const char* description, std::ostream& errors) {
    std::error_code error;
    const bool exists = PathEntryExists(path, error);
    if (error) {
        errors << "ERROR: cannot inspect " << description << " '" << path.string() << "': " << error.message()
               << ".\n";
        return false;
    }
    if (exists) {
        errors << "ERROR: " << description << " already exists: " << path.string() << "\n";
        return false;
    }
    return true;
}

bool RequireMacro(const std::filesystem::path& path, std::ostream& errors) {
    std::error_code error;
    if (std::filesystem::is_regular_file(path, error) && !error) return true;
    errors << "ERROR: recovery macro is not installed at " << path.string() << ".\n"
           << "Check REST_PATH and reinstall REST if necessary.\n";
    return false;
}

void ReportRetainedWorkDirectory(const std::filesystem::path& workDirectory, std::ostream& errors) {
    errors << "Recovery work files were retained for diagnosis at: " << workDirectory.string() << "\n";
}

std::string PrependSearchPath(const std::filesystem::path& entry, const char* variable) {
    const char* inherited = std::getenv(variable);
#ifdef _WIN32
    constexpr char separator = ';';
#else
    constexpr char separator = ':';
#endif
    return inherited == nullptr || inherited[0] == '\0' ? entry.string()
                                                        : entry.string() + separator + std::string(inherited);
}

#ifndef _WIN32
volatile std::sig_atomic_t gActiveChildProcessGroup = -1;

void ForwardSignalToChild(int signal) {
    const auto group = gActiveChildProcessGroup;
    if (group > 0) kill(-group, signal);
}

class SignalForwardingGuard {
   public:
    explicit SignalForwardingGuard(pid_t child) {
        gActiveChildProcessGroup = child;
        struct sigaction action {};
        action.sa_handler = ForwardSignalToChild;
        sigemptyset(&action.sa_mask);
        action.sa_flags = 0;
        sigaction(SIGINT, &action, &fPreviousInterrupt);
        sigaction(SIGTERM, &action, &fPreviousTerminate);
        sigaction(SIGHUP, &action, &fPreviousHangup);
    }

    ~SignalForwardingGuard() {
        gActiveChildProcessGroup = -1;
        sigaction(SIGINT, &fPreviousInterrupt, nullptr);
        sigaction(SIGTERM, &fPreviousTerminate, nullptr);
        sigaction(SIGHUP, &fPreviousHangup, nullptr);
    }

   private:
    struct sigaction fPreviousInterrupt {};
    struct sigaction fPreviousTerminate {};
    struct sigaction fPreviousHangup {};
};
#endif

}  // namespace

ParseResult ParseArguments(const Arguments& arguments) {
    ParseResult result;
    auto recoveryFlag = arguments.end();
    for (auto argument = arguments.begin() + (arguments.empty() ? 0 : 1); argument != arguments.end();
         ++argument) {
        if (*argument == "--recover-legacy-signals") {
            recoveryFlag = argument;
            break;
        }
    }
    if (recoveryFlag == arguments.end()) return result;
    if (recoveryFlag != arguments.begin() + 1) {
        result.action = ParseAction::kError;
        result.error = "--recover-legacy-signals must be the first restRoot option";
        return result;
    }

    if (arguments.size() == 3 && (arguments[2] == "--help" || arguments[2] == "-h")) {
        result.action = ParseAction::kHelp;
        return result;
    }

    result.action = ParseAction::kError;
    bool positionalOnly = false;
    for (std::size_t index = 2; index < arguments.size(); ++index) {
        const auto& argument = arguments[index];
        if (!positionalOnly && argument == "--") {
            positionalOnly = true;
        } else if (!positionalOnly && argument == "--in-place") {
            if (result.options.inPlace) {
                result.error = "--in-place was specified more than once";
                return result;
            }
            result.options.inPlace = true;
        } else if (!positionalOnly && argument == "--output") {
            if (result.options.outputWasSpecified) {
                result.error = "--output was specified more than once";
                return result;
            }
            if (++index >= arguments.size()) {
                result.error = "--output requires a path";
                return result;
            }
            if (arguments[index].rfind("-", 0) == 0) {
                result.error = "--output requires a path (prefix an option-like filename with ./)";
                return result;
            }
            result.options.output = arguments[index];
            result.options.outputWasSpecified = true;
        } else if (!positionalOnly && argument.rfind("-", 0) == 0) {
            result.error = "unknown recovery option: " + argument;
            return result;
        } else if (result.options.input.empty()) {
            result.options.input = argument;
        } else {
            result.error = "more than one input file was specified";
            return result;
        }
    }

    if (result.options.input.empty()) {
        result.error = "an input ROOT file is required";
    } else if (result.options.inPlace && result.options.outputWasSpecified) {
        result.error = "--output and --in-place are mutually exclusive";
    } else if (result.options.outputWasSpecified && result.options.output.empty()) {
        result.error = "--output requires a non-empty path";
    } else {
        result.action = ParseAction::kRun;
    }
    return result;
}

void PrintHelp(std::ostream& output) {
    output << "Usage:\n"
           << "  restRoot --recover-legacy-signals INPUT [--output OUTPUT | --in-place]\n\n"
           << "Safely convert a legacy TRestDetectorSignalEvent branch from vector<float>\n"
           << "to the current vector<double> schema. Stage 1 runs in an isolated plain ROOT\n"
           << "process; stage 2 runs in a fresh REST process.\n\n"
           << "By default the fixed file is written beside INPUT as <stem>_Fixed.root.\n"
           << "Existing outputs are never overwritten. --in-place must be explicit and\n"
           << "keeps the original as INPUT.bak.\n";
}

ProcessSpec BuildStage1Process(const Runtime& runtime, const std::filesystem::path& wrapper,
                               const std::filesystem::path& workDirectory, const std::filesystem::path& input,
                               const std::filesystem::path& intermediate) {
    ProcessSpec process;
    const auto rootPath = std::filesystem::path(runtime.rootExecutable).parent_path().parent_path();
    process.arguments = {runtime.rootExecutable,
                         "-l",
                         "-b",
                         "-n",
                         "-x",
                         "-q",
                         "-e",
                         "gSystem->SetBuildDir(gSystem->Getenv(\"REST_LEGACY_RECOVERY_WORK_DIR\"),kTRUE)",
                         wrapper.string() + "+"};
    process.environment = {{"REST_LEGACY_RECOVERY_INPUT", input.string()},
                           {"REST_LEGACY_RECOVERY_INTERMEDIATE", intermediate.string()},
                           {"REST_LEGACY_RECOVERY_WORK_DIR", workDirectory.string()},
                           {"ROOTSYS", rootPath.string()},
                           {"PATH", PrependSearchPath(rootPath / "bin", "PATH")},
                           {"LD_LIBRARY_PATH", PrependSearchPath(rootPath / "lib", "LD_LIBRARY_PATH")}};
#ifdef __APPLE__
    process.environment.emplace_back("DYLD_LIBRARY_PATH",
                                     PrependSearchPath(rootPath / "lib", "DYLD_LIBRARY_PATH"));
#endif
    return process;
}

ProcessSpec BuildStage2Process(const Runtime& runtime, const std::filesystem::path& wrapper,
                               const std::filesystem::path& input, const std::filesystem::path& intermediate,
                               const std::filesystem::path& output, bool inPlace) {
    ProcessSpec process;
    process.arguments = {runtime.restRootExecutable, "-l", "-b", "-n", "-x", "-q", wrapper.string()};
    process.environment = {
        {"REST_PATH", runtime.restPath.string()},
        {"PATH", PrependSearchPath(runtime.restPath / "bin", "PATH")},
        {"LD_LIBRARY_PATH", PrependSearchPath(runtime.restPath / "lib", "LD_LIBRARY_PATH")},
        {"REST_LEGACY_RECOVERY_INPUT", input.string()},
        {"REST_LEGACY_RECOVERY_INTERMEDIATE", intermediate.string()},
        {"REST_LEGACY_RECOVERY_OUTPUT", output.string()},
        {"REST_LEGACY_RECOVERY_IN_PLACE", inPlace ? "1" : "0"},
        {"REST_LEGACY_RECOVERY_REQUIRE_COMPLETE", "1"}};
#ifdef __APPLE__
    process.environment.emplace_back("DYLD_LIBRARY_PATH",
                                     PrependSearchPath(runtime.restPath / "lib", "DYLD_LIBRARY_PATH"));
#endif
    return process;
}

int RunChildProcess(const ProcessSpec& process) {
    if (process.arguments.empty() || process.arguments.front().empty()) return 127;

#ifdef _WIN32
    return kUnsupportedError;
#else
    std::vector<char*> childArguments;
    childArguments.reserve(process.arguments.size() + 1);
    for (const auto& argument : process.arguments)
        childArguments.push_back(const_cast<char*>(argument.c_str()));
    childArguments.push_back(nullptr);

    sigset_t forwardedSignals;
    sigemptyset(&forwardedSignals);
    sigaddset(&forwardedSignals, SIGINT);
    sigaddset(&forwardedSignals, SIGTERM);
    sigaddset(&forwardedSignals, SIGHUP);
    sigset_t previousMask;
    if (sigprocmask(SIG_BLOCK, &forwardedSignals, &previousMask) != 0) return 127;

    const pid_t child = fork();
    if (child < 0) {
        sigprocmask(SIG_SETMASK, &previousMask, nullptr);
        return 127;
    }
    if (child == 0) {
        if (setpgid(0, 0) != 0 || sigprocmask(SIG_SETMASK, &previousMask, nullptr) != 0) _exit(126);
        for (const auto& variable : process.environment) {
            if (setenv(variable.first.c_str(), variable.second.c_str(), 1) != 0) _exit(126);
        }
        execvp(childArguments.front(), childArguments.data());
        _exit(errno == ENOENT ? 127 : 126);
    }

    setpgid(child, child);
    int status = 0;
    int result = 127;
    {
        // Install forwarding while the relevant signals remain blocked. A
        // signal received after fork is delivered only after the handler and
        // child process group are ready.
        SignalForwardingGuard signalForwarding(child);
        if (sigprocmask(SIG_SETMASK, &previousMask, nullptr) == 0) {
            pid_t waited;
            do {
                waited = waitpid(child, &status, 0);
            } while (waited < 0 && errno == EINTR);
            if (waited == child) {
                if (WIFEXITED(status))
                    result = WEXITSTATUS(status);
                else if (WIFSIGNALED(status))
                    result = 128 + WTERMSIG(status);
            }
        } else {
            kill(-child, SIGTERM);
            while (waitpid(child, &status, 0) < 0 && errno == EINTR) {
            }
        }
        // Restore the previous handlers only while forwarding signals are
        // blocked again, then restore the caller's exact original mask.
        sigprocmask(SIG_BLOCK, &forwardedSignals, nullptr);
    }
    sigprocmask(SIG_SETMASK, &previousMask, nullptr);
    return result;
#endif
}

std::string ResolveExecutableFromSearchPath(const std::string& executable, const std::string& searchPath) {
    const std::filesystem::path path(executable);
#ifdef _WIN32
    constexpr char separator = ';';
#else
    constexpr char separator = ':';
#endif
    std::istringstream entries(searchPath);
    std::string entry;
    std::error_code error;
    while (std::getline(entries, entry, separator)) {
        if (entry.empty()) entry = ".";
        const auto candidate = std::filesystem::path(entry) / path;
        bool usable = std::filesystem::is_regular_file(candidate, error) && !error;
#ifndef _WIN32
        usable = usable && access(candidate.c_str(), X_OK) == 0;
#endif
        if (usable) {
            const auto resolved = std::filesystem::canonical(candidate, error);
            return error ? std::filesystem::absolute(candidate).lexically_normal().string()
                         : resolved.string();
        }
        error.clear();
    }
    return executable;
}

std::string ResolveExecutablePath(const std::string& executable) {
    std::error_code error;
#ifdef __linux__
    const auto procExecutable = std::filesystem::read_symlink("/proc/self/exe", error);
    if (!error && !procExecutable.empty()) return procExecutable.string();
    error.clear();
#endif
    const std::filesystem::path path(executable);
    if (path.has_parent_path()) {
        const auto resolved = std::filesystem::canonical(path, error);
        if (!error) return resolved.string();
        return std::filesystem::absolute(path).lexically_normal().string();
    }

    const char* searchPath = std::getenv("PATH");
    if (searchPath != nullptr) return ResolveExecutableFromSearchPath(executable, searchPath);
    return executable;
}

bool CreateUniqueWorkDirectory(const std::filesystem::path& parent, std::filesystem::path& result,
                               std::string& error) {
    std::error_code filesystemError;
    if (!std::filesystem::is_directory(parent, filesystemError) || filesystemError) {
        error = "working directory parent is unavailable: " + parent.string();
        return false;
    }

    const auto timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
    for (unsigned int attempt = 0; attempt < 128; ++attempt) {
        std::ostringstream name;
        name << ".rest-legacy-signals-" << std::hex << timestamp << '-' << attempt;
        const auto candidate = parent / name.str();
        filesystemError.clear();
#ifdef _WIN32
        const bool created = std::filesystem::create_directory(candidate, filesystemError);
#else
        const bool created = mkdir(candidate.c_str(), S_IRWXU) == 0;
        if (!created && errno != EEXIST) filesystemError = std::error_code(errno, std::generic_category());
#endif
        if (created) {
            std::filesystem::permissions(candidate, std::filesystem::perms::owner_all,
                                         std::filesystem::perm_options::replace, filesystemError);
            if (filesystemError) {
                std::filesystem::remove(candidate);
                error =
                    "cannot restrict permissions on recovery work directory: " + filesystemError.message();
                return false;
            }
#ifndef _WIN32
            struct stat directoryStatus {};
            if (stat(candidate.c_str(), &directoryStatus) != 0 ||
                (directoryStatus.st_mode & 0777) != S_IRWXU) {
                std::filesystem::remove(candidate);
                error = "recovery work directory does not have mode 0700";
                return false;
            }
#endif
            result = candidate;
            return true;
        }
        if (filesystemError && filesystemError != std::errc::file_exists) {
            error = "cannot create recovery work directory in " + parent.string() + ": " +
                    filesystemError.message();
            return false;
        }
    }
    error = "cannot allocate a unique recovery work directory in " + parent.string();
    return false;
}

int Execute(const Options& options, Runtime runtime, std::ostream& output, std::ostream& errors) {
#ifdef _WIN32
    errors << "ERROR: legacy signal recovery orchestration is not supported on Windows yet.\n";
    return kUnsupportedError;
#endif
    if (options.input.empty()) {
        errors << "ERROR: an input ROOT file is required.\n";
        return kUsageError;
    }
    if (options.inPlace && options.outputWasSpecified) {
        errors << "ERROR: --output and --in-place are mutually exclusive.\n";
        return kUsageError;
    }
    if (!runtime.runProcess) runtime.runProcess = RunChildProcess;
    if (!runtime.createWorkDirectory) runtime.createWorkDirectory = CreateUniqueWorkDirectory;

    const auto inputText = options.input.string();
    if (inputText.find("://") != std::string::npos || inputText.rfind("file:", 0) == 0) {
        errors << "ERROR: recovery accepts local filesystem paths only, not URLs or ROOT protocols.\n";
        return kUsageError;
    }

    std::error_code filesystemError;
    if (!std::filesystem::is_regular_file(options.input, filesystemError) || filesystemError) {
        errors << "ERROR: input file is not a readable regular file: " << options.input.string() << "\n";
        return kSetupError;
    }
    if (options.inPlace && std::filesystem::is_symlink(options.input, filesystemError) && !filesystemError) {
        errors << "ERROR: in-place recovery through a symbolic link is not supported; use the target path.\n";
        return kSetupError;
    }

    filesystemError.clear();
    const auto suppliedInputPath =
        std::filesystem::absolute(options.input, filesystemError).lexically_normal();
    if (filesystemError) {
        errors << "ERROR: cannot resolve supplied input path '" << options.input.string()
               << "': " << filesystemError.message() << ".\n";
        return kSetupError;
    }
    const auto inputPath = std::filesystem::canonical(suppliedInputPath, filesystemError);
    if (filesystemError) {
        errors << "ERROR: cannot resolve input file '" << options.input.string()
               << "': " << filesystemError.message() << ".\n";
        return kSetupError;
    }
    auto outputPath = options.inPlace ? inputPath
                                      : (options.outputWasSpecified ? options.output
                                                                    : DefaultFixedOutput(suppliedInputPath));
    if (!options.inPlace) {
        const auto outputText = outputPath.string();
        if (outputText.find("://") != std::string::npos || outputText.rfind("file:", 0) == 0) {
            errors << "ERROR: recovery output must be a local filesystem path.\n";
            return kUsageError;
        }
        outputPath =
            std::filesystem::weakly_canonical(std::filesystem::absolute(outputPath), filesystemError);
        if (filesystemError) {
            errors << "ERROR: cannot resolve fixed output path: " << filesystemError.message() << ".\n";
            return kSetupError;
        }
    }
    if (!options.inPlace && inputPath == outputPath) {
        errors << "ERROR: fixed output resolves to the input file; use --in-place explicitly.\n";
        return kUsageError;
    }

    if (options.inPlace) {
        if (!RequireUnused(inputPath.string() + ".bak", "backup", errors)) {
            return kSetupError;
        }
    } else if (!RequireUnused(outputPath, "fixed output", errors)) {
        return kSetupError;
    }

    auto workParent = options.inPlace ? inputPath.parent_path() : outputPath.parent_path();
    if (workParent.empty()) workParent = ".";
    if (!std::filesystem::is_directory(workParent, filesystemError) || filesystemError) {
        errors << "ERROR: output directory is unavailable: " << workParent.string() << "\n";
        return kSetupError;
    }

    filesystemError.clear();
    runtime.restPath = std::filesystem::canonical(runtime.restPath, filesystemError);
    if (filesystemError) {
        errors << "ERROR: cannot resolve the REST installation prefix: " << filesystemError.message()
               << ". Install REST before using recovery.\n";
        return kSetupError;
    }
    const auto stage1Wrapper = runtime.restPath / "macros/legacy/recoverLegacySignalData.C";
    const auto stage2Wrapper = runtime.restPath / "macros/legacy/REST_RebuildLegacySignalFile.C";
    if (!RequireMacro(stage1Wrapper, errors) || !RequireMacro(stage2Wrapper, errors)) return kSetupError;

    std::filesystem::path workDirectory;
    std::string workError;
    if (!runtime.createWorkDirectory(workParent, workDirectory, workError)) {
        errors << "ERROR: " << workError << ".\n";
        return kSetupError;
    }
    const auto intermediate = workDirectory / "LegacySignalData.root";
    const auto candidate = workDirectory / "FixedCandidate.root";

    output << "Legacy signal recovery stage 1/2: extracting with plain ROOT.\n" << std::flush;
    const int stage1Status = runtime.runProcess(
        BuildStage1Process(runtime, stage1Wrapper, workDirectory, inputPath, intermediate));
    if (stage1Status != 0) {
        errors << "ERROR: legacy signal extraction failed with exit status " << stage1Status << ".\n";
        ReportRetainedWorkDirectory(workDirectory, errors);
        return stage1Status;
    }
    if (!std::filesystem::is_regular_file(intermediate, filesystemError) || filesystemError) {
        errors << "ERROR: stage 1 reported success but did not create the intermediate file.\n";
        ReportRetainedWorkDirectory(workDirectory, errors);
        return kSetupError;
    }

    output << "Legacy signal recovery stage 2/2: rebuilding with REST.\n" << std::flush;
    const int stage2Status = runtime.runProcess(
        BuildStage2Process(runtime, stage2Wrapper, inputPath, intermediate, candidate, options.inPlace));
    if (stage2Status != 0) {
        errors << "ERROR: REST rebuild failed with exit status " << stage2Status << ".\n";
        ReportRetainedWorkDirectory(workDirectory, errors);
        return stage2Status;
    }

    if (options.inPlace) {
        if (!std::filesystem::is_regular_file(inputPath, filesystemError) || filesystemError ||
            !std::filesystem::is_regular_file(inputPath.string() + ".bak", filesystemError) ||
            filesystemError) {
            errors << "ERROR: in-place recovery reported success but the fixed file or backup is "
                      "unavailable.\n";
            ReportRetainedWorkDirectory(workDirectory, errors);
            return kSetupError;
        }
    } else {
        if (!std::filesystem::is_regular_file(candidate, filesystemError) || filesystemError) {
            errors << "ERROR: stage 2 reported success but the validated candidate is unavailable.\n";
            ReportRetainedWorkDirectory(workDirectory, errors);
            return kSetupError;
        }
        if (!RequireUnused(outputPath, "fixed output", errors)) {
            ReportRetainedWorkDirectory(workDirectory, errors);
            return kSetupError;
        }
        std::filesystem::create_hard_link(candidate, outputPath, filesystemError);
        if (filesystemError) {
            errors << "ERROR: cannot install the validated candidate without overwriting '"
                   << outputPath.string() << "': " << filesystemError.message() << ".\n";
            ReportRetainedWorkDirectory(workDirectory, errors);
            return kSetupError;
        }
        filesystemError.clear();
        std::filesystem::remove(candidate, filesystemError);
        if (filesystemError) {
            errors << "WARNING: fixed output was installed, but its work-directory hard link could not be "
                      "removed: "
                   << candidate.string() << " (" << filesystemError.message() << ").\n";
        }
    }

    filesystemError.clear();
    std::filesystem::remove_all(workDirectory, filesystemError);
    if (filesystemError) {
        errors << "WARNING: recovery succeeded, but the owned work directory could not be removed: "
               << workDirectory.string() << " (" << filesystemError.message() << ").\n";
    }
    output << "Legacy signal recovery completed successfully: " << outputPath.string() << "\n";
    return 0;
}

}  // namespace REST_LegacySignalRecoveryCLI
