#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <fstream>
#include <string>

#ifndef _WIN32
#include <sys/wait.h>
#include <unistd.h>
#endif

#ifndef _WIN32
volatile std::sig_atomic_t gForwardedSignal = 0;

void RecordSignal(int signal) { gForwardedSignal = signal; }
#endif

int main(int argc, char* argv[]) {
    if (argc == 2 && std::string(argv[1]) == "--terminate") {
        std::raise(SIGTERM);
        return 99;
    }
#ifndef _WIN32
    if (argc == 4 && std::string(argv[1]) == "--request-parent-signal-with-grandchild") {
        const pid_t grandchild = fork();
        if (grandchild < 0) return 4;
        if (grandchild == 0) {
            while (true) pause();
        }

        struct sigaction action {};
        action.sa_handler = RecordSignal;
        sigemptyset(&action.sa_mask);
        if (sigaction(SIGTERM, &action, nullptr) != 0) return 5;

        {
            std::ofstream pidFile(argv[2]);
            pidFile << grandchild;
            if (!pidFile) return 6;
        }
        if (kill(getppid(), SIGTERM) != 0) return 7;
        while (gForwardedSignal == 0) pause();

        int status = 0;
        pid_t waited;
        do {
            waited = waitpid(grandchild, &status, 0);
        } while (waited < 0 && errno == EINTR);
        if (waited != grandchild || !WIFSIGNALED(status) || WTERMSIG(status) != SIGTERM) return 8;

        std::ofstream reapedFile(argv[3]);
        reapedFile << "reaped";
        if (!reapedFile) return 9;
        return 128 + gForwardedSignal;
    }
#endif
    if (argc != 3) return 2;
    const char* value = std::getenv(argv[1]);
    return value != nullptr && value == std::string(argv[2]) ? 0 : 3;
}
