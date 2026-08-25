#ifndef REST_LEGACY_SIGNAL_RECOVERY_CLI_H
#define REST_LEGACY_SIGNAL_RECOVERY_CLI_H

#include <filesystem>
#include <functional>
#include <iosfwd>
#include <string>
#include <utility>
#include <vector>

namespace REST_LegacySignalRecoveryCLI {

enum class ParseAction { kNotRequested, kRun, kHelp, kError };

struct Options {
    std::filesystem::path input;
    std::filesystem::path output;
    bool outputWasSpecified = false;
    bool inPlace = false;
    bool requireComplete = false;
};

struct ParseResult {
    ParseAction action = ParseAction::kNotRequested;
    Options options;
    std::string error;
};

using Arguments = std::vector<std::string>;
struct ProcessSpec {
    Arguments arguments;
    std::vector<std::pair<std::string, std::string>> environment;
};

using ProcessRunner = std::function<int(const ProcessSpec&)>;
using WorkDirectoryFactory =
    std::function<bool(const std::filesystem::path&, std::filesystem::path&, std::string&)>;

struct Runtime {
    std::filesystem::path restPath;
    std::string rootExecutable = "root";
    std::string restRootExecutable = "restRoot";
    ProcessRunner runProcess;
    WorkDirectoryFactory createWorkDirectory;
};

ParseResult ParseArguments(const Arguments& arguments);
void PrintHelp(std::ostream& output);

ProcessSpec BuildStage1Process(const Runtime& runtime, const std::filesystem::path& wrapper,
                               const std::filesystem::path& workDirectory, const std::filesystem::path& input,
                               const std::filesystem::path& intermediate);
ProcessSpec BuildStage2Process(const Runtime& runtime, const std::filesystem::path& wrapper,
                               const std::filesystem::path& input, const std::filesystem::path& intermediate,
                               const std::filesystem::path& output, bool inPlace, bool requireComplete);

int RunChildProcess(const ProcessSpec& process);
std::string ResolveExecutableFromSearchPath(const std::string& executable, const std::string& searchPath);
std::string ResolveExecutablePath(const std::string& executable);
bool CreateUniqueWorkDirectory(const std::filesystem::path& parent, std::filesystem::path& result,
                               std::string& error);
int Execute(const Options& options, Runtime runtime, std::ostream& output, std::ostream& errors);

}  // namespace REST_LegacySignalRecoveryCLI

#endif
