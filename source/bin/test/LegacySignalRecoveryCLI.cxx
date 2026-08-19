#include "LegacySignalRecoveryCLI.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <atomic>
#include <cerrno>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#ifndef _WIN32
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

namespace {

namespace fs = std::filesystem;
using namespace REST_LegacySignalRecoveryCLI;

class TemporaryDirectory {
   public:
    TemporaryDirectory() {
        static std::atomic<unsigned long> sequence{0};
        path = fs::temp_directory_path() /
               ("rest_legacy_cli_test_" +
                std::to_string(std::chrono::high_resolution_clock::now().time_since_epoch().count()) + "_" +
                std::to_string(sequence++));
        fs::create_directories(path);
    }

    ~TemporaryDirectory() {
        std::error_code error;
        fs::remove_all(path, error);
    }

    fs::path path;
};

void WriteText(const fs::path& path, const std::string& text) {
    std::ofstream output(path);
    ASSERT_TRUE(output.is_open());
    output << text;
}

std::string ReadText(const fs::path& path) {
    std::ifstream input(path);
    return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
}

std::string EnvironmentValue(const ProcessSpec& process, const std::string& name) {
    for (const auto& variable : process.environment) {
        if (variable.first == name) return variable.second;
    }
    return "";
}

Runtime MakeRuntime(const fs::path& prefix, const fs::path& workDirectory, const ProcessRunner& runner) {
    const auto macros = prefix / "macros/legacy";
    fs::create_directories(macros);
    WriteText(macros / "recoverLegacySignalData.C", "// test wrapper");
    WriteText(macros / "REST_RebuildLegacySignalFile.C", "// test wrapper");

    Runtime runtime;
    runtime.restPath = prefix;
    runtime.rootExecutable = "/matched/root";
    runtime.restRootExecutable = "/matched/restRoot";
    runtime.runProcess = runner;
    runtime.createWorkDirectory = [workDirectory](const fs::path& parent, fs::path& result, std::string&) {
        EXPECT_EQ(parent, workDirectory.parent_path());
        std::error_code error;
        const bool created = fs::create_directory(workDirectory, error);
        if (!created || error) return false;
        fs::permissions(workDirectory, fs::perms::owner_all, fs::perm_options::replace, error);
        if (error) return false;
        result = workDirectory;
        return true;
    };
    return runtime;
}

TEST(LegacySignalRecoveryCLI, ParsesOneShotInterfaceAndRejectsAmbiguity) {
    auto parsed = ParseArguments({"restRoot", "--recover-legacy-signals", "input.root"});
    ASSERT_EQ(parsed.action, ParseAction::kRun);
    EXPECT_EQ(parsed.options.input, "input.root");
    EXPECT_FALSE(parsed.options.inPlace);
    EXPECT_FALSE(parsed.options.outputWasSpecified);

    parsed = ParseArguments({"restRoot", "--recover-legacy-signals", "input.root", "--output", "fixed.root"});
    ASSERT_EQ(parsed.action, ParseAction::kRun);
    EXPECT_EQ(parsed.options.output, "fixed.root");

    parsed = ParseArguments({"restRoot", "--recover-legacy-signals", "input.root", "--in-place"});
    ASSERT_EQ(parsed.action, ParseAction::kRun);
    EXPECT_TRUE(parsed.options.inPlace);

    EXPECT_EQ(ParseArguments({"restRoot", "--recover-legacy-signals", "--help"}).action, ParseAction::kHelp);
    EXPECT_EQ(ParseArguments({"restRoot", "-l", "--recover-legacy-signals", "input.root"}).action,
              ParseAction::kError);
    EXPECT_EQ(ParseArguments({"restRoot", "--recover-legacy-signals"}).action, ParseAction::kError);
    EXPECT_EQ(ParseArguments({"restRoot", "--recover-legacy-signals", "input.root", "--output"}).action,
              ParseAction::kError);
    EXPECT_EQ(ParseArguments({"restRoot", "--recover-legacy-signals", "input.root", "--output", "--in-place"})
                  .action,
              ParseAction::kError);
    EXPECT_EQ(
        ParseArguments({"restRoot", "--recover-legacy-signals", "input.root", "--output", "--help"}).action,
        ParseAction::kError);
    EXPECT_EQ(ParseArguments({"restRoot", "--recover-legacy-signals", "input.root", "--output", "fixed.root",
                              "--in-place"})
                  .action,
              ParseAction::kError);
    EXPECT_EQ(ParseArguments({"restRoot", "--recover-legacy-signals", "--", "-input.root"}).action,
              ParseAction::kRun);
    EXPECT_EQ(ParseArguments({"restRoot", "--help"}).action, ParseAction::kNotRequested);
}

TEST(LegacySignalRecoveryCLI, BuildsIsolatedProcessesWithoutPuttingDataPathsInRootExpressions) {
    Runtime runtime;
    runtime.restPath = "/matching REST;prefix";
    runtime.rootExecutable = "/matching ROOT/bin/root";
    runtime.restRootExecutable = "/matching REST/bin/restRoot";
    const fs::path input = "/data/a path/quote\"; gSystem->Exec(\"bad\").root";
    const fs::path intermediate = "/work/legacy; \"signal\".root";
    const fs::path candidate = "/work/fixed candidate.root";
    const fs::path work = "/work/a directory";

    const auto stage1 =
        BuildStage1Process(runtime, "/matching REST;prefix/stage1.C", work, input, intermediate);
    ASSERT_FALSE(stage1.arguments.empty());
    EXPECT_EQ(stage1.arguments.front(), runtime.rootExecutable);
    EXPECT_NE(std::find(stage1.arguments.begin(), stage1.arguments.end(), "-n"), stage1.arguments.end());
    EXPECT_NE(std::find(stage1.arguments.begin(), stage1.arguments.end(), "-x"), stage1.arguments.end());
    for (const auto& argument : stage1.arguments) {
        EXPECT_EQ(argument.find(input.string()), std::string::npos);
        EXPECT_EQ(argument.find(intermediate.string()), std::string::npos);
    }
    EXPECT_EQ(EnvironmentValue(stage1, "REST_LEGACY_RECOVERY_INPUT"), input);
    EXPECT_EQ(EnvironmentValue(stage1, "REST_LEGACY_RECOVERY_INTERMEDIATE"), intermediate);
    EXPECT_EQ(EnvironmentValue(stage1, "ROOTSYS"), "/matching ROOT");
    EXPECT_EQ(EnvironmentValue(stage1, "PATH").rfind("/matching ROOT/bin", 0), 0U);
    EXPECT_EQ(EnvironmentValue(stage1, "LD_LIBRARY_PATH").rfind("/matching ROOT/lib", 0), 0U);

    const auto stage2 =
        BuildStage2Process(runtime, "/matching REST;prefix/stage2.C", input, intermediate, candidate, true);
    EXPECT_EQ(stage2.arguments.front(), runtime.restRootExecutable);
    EXPECT_NE(std::find(stage2.arguments.begin(), stage2.arguments.end(), "-n"), stage2.arguments.end());
    EXPECT_NE(std::find(stage2.arguments.begin(), stage2.arguments.end(), "-x"), stage2.arguments.end());
    for (const auto& argument : stage2.arguments) {
        EXPECT_EQ(argument.find(input.string()), std::string::npos);
        EXPECT_EQ(argument.find(intermediate.string()), std::string::npos);
        EXPECT_EQ(argument.find(candidate.string()), std::string::npos);
    }
    EXPECT_EQ(EnvironmentValue(stage2, "REST_PATH"), runtime.restPath);
    EXPECT_EQ(EnvironmentValue(stage2, "PATH").rfind((runtime.restPath / "bin").string(), 0), 0U);
    EXPECT_EQ(EnvironmentValue(stage2, "LD_LIBRARY_PATH").rfind((runtime.restPath / "lib").string(), 0), 0U);
    EXPECT_EQ(EnvironmentValue(stage2, "REST_LEGACY_RECOVERY_OUTPUT"), candidate);
    EXPECT_EQ(EnvironmentValue(stage2, "REST_LEGACY_RECOVERY_IN_PLACE"), "1");
    EXPECT_EQ(EnvironmentValue(stage2, "REST_LEGACY_RECOVERY_REQUIRE_COMPLETE"), "1");
}

#ifndef _WIN32
TEST(LegacySignalRecoveryCLI, CreatesPrivateWorkDirectory) {
    TemporaryDirectory temporary;
    fs::path work;
    std::string error;
    ASSERT_TRUE(CreateUniqueWorkDirectory(temporary.path, work, error)) << error;
    struct stat status {};
    ASSERT_EQ(stat(work.c_str(), &status), 0);
    EXPECT_EQ(status.st_mode & 0777, 0700);
}

TEST(LegacySignalRecoveryCLI, ChildProcessPreservesSpecialEnvironmentAndReportsSignals) {
    const std::string special = "a path with spaces; quote\" and $()";
    ProcessSpec process{{REST_LEGACY_RECOVERY_PROCESS_PROBE, "REST_TEST_SPECIAL", special},
                        {{"REST_TEST_SPECIAL", special}}};
    EXPECT_EQ(RunChildProcess(process), 0);

    process = {{REST_LEGACY_RECOVERY_PROCESS_PROBE, "--terminate"}, {}};
    EXPECT_EQ(RunChildProcess(process), 128 + SIGTERM);

    process = {{"/definitely/missing/rest-process"}, {}};
    EXPECT_EQ(RunChildProcess(process), 127);
}

TEST(LegacySignalRecoveryCLI, ForwardsParentSignalToChildGroupAndReapsGrandchild) {
    TemporaryDirectory temporary;
    const auto grandchildPidFile = temporary.path / "grandchild.pid";
    const auto reapedFile = temporary.path / "grandchild.reaped";
    ProcessSpec process{{REST_LEGACY_RECOVERY_PROCESS_PROBE, "--request-parent-signal-with-grandchild",
                         grandchildPidFile.string(), reapedFile.string()},
                        {}};

    EXPECT_EQ(RunChildProcess(process), 128 + SIGTERM);
    EXPECT_EQ(ReadText(reapedFile), "reaped");

    std::ifstream pidInput(grandchildPidFile);
    pid_t grandchild = -1;
    ASSERT_TRUE(pidInput >> grandchild);
    errno = 0;
    EXPECT_EQ(kill(grandchild, 0), -1);
    EXPECT_EQ(errno, ESRCH);
}

TEST(LegacySignalRecoveryCLI, PathFallbackRequiresExecutableFile) {
    TemporaryDirectory temporary;
    const auto command = temporary.path / "candidate-command";
    WriteText(command, "#!/bin/sh\nexit 0\n");
    fs::permissions(command, fs::perms::owner_read | fs::perms::owner_write, fs::perm_options::replace);
    const auto commandName = command.filename().string();
    EXPECT_EQ(ResolveExecutableFromSearchPath(commandName, temporary.path.string()), commandName);

    fs::permissions(command, fs::perms::owner_all, fs::perm_options::replace);
    EXPECT_EQ(ResolveExecutableFromSearchPath(commandName, temporary.path.string()),
              fs::canonical(command).string());
}
#endif

TEST(LegacySignalRecoveryCLI, SuccessfulSiblingRecoveryInstallsCandidateAndCleansOwnedWork) {
    TemporaryDirectory temporary;
    const auto input = temporary.path / "legacy input; \"quoted\".root";
    const auto output = temporary.path / "fixed output; \"quoted\".root";
    const auto work = temporary.path / ".owned-work";
    const auto prefix = temporary.path / "matching prefix";
    WriteText(input, "legacy");

    std::vector<ProcessSpec> calls;
    const auto runner = [&](const ProcessSpec& process) {
        calls.push_back(process);
        if (calls.size() == 1) {
            WriteText(EnvironmentValue(process, "REST_LEGACY_RECOVERY_INTERMEDIATE"), "intermediate");
        } else {
            WriteText(EnvironmentValue(process, "REST_LEGACY_RECOVERY_OUTPUT"), "fixed");
        }
        return 0;
    };
    auto runtime = MakeRuntime(prefix, work, runner);

    Options options;
    options.input = input;
    options.output = output;
    options.outputWasSpecified = true;
    std::ostringstream messages;
    std::ostringstream errors;
    EXPECT_EQ(Execute(options, runtime, messages, errors), 0) << errors.str();
    EXPECT_EQ(calls.size(), 2U);
    EXPECT_EQ(ReadText(output), "fixed");
    EXPECT_EQ(ReadText(input), "legacy");
    EXPECT_FALSE(fs::exists(work));
    EXPECT_TRUE(errors.str().empty());
}

TEST(LegacySignalRecoveryCLI, DefaultOutputStaysBesideSuppliedInputSymlink) {
    TemporaryDirectory temporary;
    const auto targetDirectory = temporary.path / "target";
    const auto linkDirectory = temporary.path / "links";
    fs::create_directories(targetDirectory);
    fs::create_directories(linkDirectory);
    const auto target = targetDirectory / "canonical.root";
    const auto link = linkDirectory / "friendly.root";
    const auto expectedOutput = linkDirectory / "friendly_Fixed.root";
    const auto wrongOutput = targetDirectory / "canonical_Fixed.root";
    const auto work = linkDirectory / ".owned-work";
    const auto prefix = temporary.path / "matching-prefix";
    WriteText(target, "legacy");
    std::error_code symlinkError;
    fs::create_symlink(target, link, symlinkError);
    if (symlinkError) GTEST_SKIP() << symlinkError.message();

    int calls = 0;
    const auto runner = [&](const ProcessSpec& process) {
        ++calls;
        if (calls == 1) {
            EXPECT_EQ(EnvironmentValue(process, "REST_LEGACY_RECOVERY_INPUT"), fs::canonical(target));
            WriteText(EnvironmentValue(process, "REST_LEGACY_RECOVERY_INTERMEDIATE"), "intermediate");
        } else {
            WriteText(EnvironmentValue(process, "REST_LEGACY_RECOVERY_OUTPUT"), "fixed");
        }
        return 0;
    };
    auto runtime = MakeRuntime(prefix, work, runner);

    Options options;
    options.input = link;
    std::ostringstream messages;
    std::ostringstream errors;
    EXPECT_EQ(Execute(options, runtime, messages, errors), 0) << errors.str();
    EXPECT_EQ(calls, 2);
    EXPECT_EQ(ReadText(expectedOutput), "fixed");
    EXPECT_FALSE(fs::exists(wrongOutput));
    EXPECT_EQ(ReadText(target), "legacy");
}

TEST(LegacySignalRecoveryCLI, RetainsOwnedWorkAndLeavesOutputAbsentAfterEitherStageFails) {
    for (const int failingStage : {1, 2}) {
        TemporaryDirectory temporary;
        const auto input = temporary.path / "input.root";
        const auto output = temporary.path / "fixed.root";
        const auto work = temporary.path / ".owned-work";
        const auto prefix = temporary.path / "prefix";
        WriteText(input, "legacy");

        int call = 0;
        const auto runner = [&](const ProcessSpec& process) {
            ++call;
            if (call == 1) {
                WriteText(EnvironmentValue(process, "REST_LEGACY_RECOVERY_INTERMEDIATE"),
                          failingStage == 1 ? "partial" : "intermediate");
            }
            return call == failingStage ? 17 : 0;
        };
        auto runtime = MakeRuntime(prefix, work, runner);
        Options options{input, output, true, false};
        std::ostringstream messages;
        std::ostringstream errors;
        EXPECT_EQ(Execute(options, runtime, messages, errors), 17);
        EXPECT_EQ(call, failingStage);
        EXPECT_FALSE(fs::exists(output));
        EXPECT_TRUE(fs::exists(work));
        EXPECT_NE(errors.str().find(work.string()), std::string::npos);
    }
}

TEST(LegacySignalRecoveryCLI, RefusesExistingAndRacingOutputsWithoutOverwriting) {
    for (const bool outputExistsInitially : {true, false}) {
        TemporaryDirectory temporary;
        const auto input = temporary.path / "input.root";
        const auto output = temporary.path / "fixed.root";
        const auto work = temporary.path / ".owned-work";
        const auto prefix = temporary.path / "prefix";
        WriteText(input, "legacy");
        if (outputExistsInitially) WriteText(output, "existing");

        int calls = 0;
        const auto runner = [&](const ProcessSpec& process) {
            ++calls;
            if (calls == 1) {
                WriteText(EnvironmentValue(process, "REST_LEGACY_RECOVERY_INTERMEDIATE"), "intermediate");
            } else {
                WriteText(EnvironmentValue(process, "REST_LEGACY_RECOVERY_OUTPUT"), "fixed");
                WriteText(output, "racing writer");
            }
            return 0;
        };
        auto runtime = MakeRuntime(prefix, work, runner);
        Options options{input, output, true, false};
        std::ostringstream messages;
        std::ostringstream errors;
        EXPECT_NE(Execute(options, runtime, messages, errors), 0);
        EXPECT_EQ(ReadText(output), outputExistsInitially ? "existing" : "racing writer");
        EXPECT_EQ(calls, outputExistsInitially ? 0 : 2);
        if (!outputExistsInitially) EXPECT_TRUE(fs::exists(work));
    }
}

TEST(LegacySignalRecoveryCLI, RefusesDanglingOutputAndBackupSymlinks) {
    TemporaryDirectory temporary;
    const auto input = temporary.path / "input.root";
    WriteText(input, "legacy");
    std::error_code error;

    const auto output = temporary.path / "fixed.root";
    fs::create_symlink(temporary.path / "missing", output, error);
    if (error) GTEST_SKIP() << error.message();
    Runtime runtime;
    int calls = 0;
    runtime.runProcess = [&](const ProcessSpec&) {
        ++calls;
        return 0;
    };
    Options options{input, output, true, false};
    std::ostringstream messages;
    std::ostringstream errors;
    EXPECT_NE(Execute(options, runtime, messages, errors), 0);
    EXPECT_EQ(calls, 0);

    fs::remove(output);
    fs::create_symlink(temporary.path / "missing-backup", input.string() + ".bak", error);
    options = {};
    options.input = input;
    options.inPlace = true;
    errors.str("");
    EXPECT_NE(Execute(options, runtime, messages, errors), 0);
    EXPECT_EQ(calls, 0);
}

TEST(LegacySignalRecoveryCLI, RejectsRemoteInputAndOutputProtocols) {
    Runtime runtime;
    std::ostringstream messages;
    std::ostringstream errors;

    Options options;
    options.input = "root://server/data.root";
    EXPECT_NE(Execute(options, runtime, messages, errors), 0);
    EXPECT_NE(errors.str().find("local filesystem"), std::string::npos);

    TemporaryDirectory temporary;
    const auto input = temporary.path / "input.root";
    WriteText(input, "legacy");
    options = {input, "https://server/fixed.root", true, false};
    errors.str("");
    EXPECT_NE(Execute(options, runtime, messages, errors), 0);
    EXPECT_NE(errors.str().find("local filesystem"), std::string::npos);
}

}  // namespace
