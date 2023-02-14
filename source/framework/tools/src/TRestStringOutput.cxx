#include "TRestStringOutput.h"

#include "TRestStringHelper.h"

using namespace std;

#ifdef WIN32
#include <Windows.h>
#include <conio.h>
#endif  // WIN32

#ifdef __APPLE__
#include <unistd.h>
#endif

int Console::GetWidth() {
#ifdef WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
#else
    if (isatty(fileno(stdout))) {
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        return w.ws_col;
    }
    return -1;
#endif  // WIN32
}

int Console::GetHeight() {
#ifdef WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
    if (isatty(fileno(stdout))) {
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        return w.ws_row;
    }
    return -1;
#endif  // WIN32
}

bool Console::kbhit() {
#ifdef WIN32
    return _kbhit();
#else
    struct termios oldt, newt;
    int ch;
    int oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if (ch != EOF) {
        ungetc(ch, stdin);
        return true;
    }
    return false;

// int byteswaiting;
//   ioctl(0, FIONREAD, &byteswaiting);
//   return byteswaiting > 0;
#endif
}

int Console::Read() { return getchar(); }

int Console::ReadKey() {
#ifdef WIN32
    return _getch();
#else
    struct termios tm, tm_old;
    int fd = 0, ch;

    if (tcgetattr(fd, &tm) < 0) {
        return -1;
    }

    tm_old = tm;
    cfmakeraw(&tm);
    if (tcsetattr(fd, TCSANOW, &tm) < 0) {
        return -1;
    }

    ch = getchar();
    if (tcsetattr(fd, TCSANOW, &tm_old) < 0) {
        return -1;
    }

    return ch;
#endif
}

string Console::ReadLine() {
    char a[500];
    cin.getline(a, 500);
    return string(a);
}

void Console::WriteLine(string content) {
    printf("%s", content.c_str());
    fflush(stdout);
}

void Console::CursorUp(int n) {
    printf("\033[%dA", n);
    fflush(stdout);
}

void Console::CursorDown(int n) {
    printf("\033[%dB", n);
    fflush(stdout);
}

void Console::CursorRight(int n) {
    printf("\033[%dC", n);
    fflush(stdout);
}

void Console::CursorLeft(int n) {
    printf("\033[%dD", n);
    fflush(stdout);
}

void Console::CursorToXY(int x, int y) {
    printf("\033[%d%dH", x, y);
    fflush(stdout);
}

void Console::ClearScreen() {
    printf("\033[2J");
    fflush(stdout);
}

void Console::ClearCurrentLine() {
#ifdef WIN32
    printf("\r");
    fflush(stdout);
#else
    printf("\033[K");
    fflush(stdout);
#endif  // WIN32
}

void Console::ClearLinesAfterCursor() {
    printf("\033[s");
    for (int i = 0; i < 50; i++) {
        printf("\033[K");
        CursorDown(1);
    }
    printf("\033[u");
    fflush(stdout);
}

char mirrorchar(char c) {
    switch (c) {
        default:
            return c;
        case '<':
            return '>';
        case '>':
            return '<';
        case '[':
            return ']';
        case ']':
            return '[';
        case '{':
            return '}';
        case '}':
            return '{';
        case '(':
            return ')';
        case ')':
            return '(';
        case '\\':
            return '/';
        case '/':
            return '\\';
        case 'd':
            return 'b';
        case 'b':
            return 'd';
        case 'q':
            return 'p';
        case 'p':
            return 'q';
    }
}

#define TRestStringOutput_BestLength 100
TRestStringOutput::TRestStringOutput(COLORCODE_TYPE _color, string formatter,
                                     REST_Display_Orientation _orientation) {
    iserror = false;
    color = _color;
    orientation = _orientation;
    // check if is border expression
    useborder = true;
    if (formatter.size() < 2) {
        useborder = false;
    }
    for (unsigned int i = 0; i < formatter.size() / 2; i++) {
        if (mirrorchar(formatter[i]) != formatter[formatter.size() - i - 1]) {
            useborder = false;
            break;
        }
    }
    if (formatter[formatter.size() / 2] != ' ') {
        useborder = false;
    }

    if (useborder) {
        formatstring = formatter.substr(0, formatter.size() / 2);
        formatstring = Replace(formatstring, " ", "");
    } else {
        formatstring = formatter;
    }

    setlength(TRestStringOutput_BestLength);
    resetstring();
    if (length > 500 || length < 20)  // unsupported console, we will fall back to compatibility modes
    {
        length = -1;
        REST_Display_CompatibilityMode = true;
    }

    verbose = REST_Verbose_Level::REST_Essential;
}

void TRestStringOutput::resetstring() {
    buf.clear();
    buf.str("");
}

string TRestStringOutput::FormattingPrintString(string input) {
    if (input == "") return "";

    // input: "=abc=", output "=============abc============="(length)
    if (input[0] == input[input.size() - 1] &&
        (input[0] == '=' || input[0] == '-' || input[0] == '*' || input[0] == '+')) {
        return string(length, input[0]);
    }

    if (useborder) {
        string output(length, ' ');

        int Lstr = input.size();
        int Lfmt = formatstring.size();

        int startblank;
        if (useborder || orientation == TRestStringOutput::REST_Display_Orientation::kMiddle) {
            startblank = (length - Lstr) / 2;
        } else {
            startblank = Lfmt;
        }
        if (startblank < 0) {
            startblank = 0;
        }

        string& border = formatstring;
        for (int i = 0; i < Lfmt && i < length; i++) {
            output[i] = border[i];
            output[length - i - 1] = mirrorchar(border[i]);
        }

        for (int i = 0; i < Lstr; i++) {
            if (startblank + i > length - 1) {
                output[length - 3] = '.';
                output[length - 2] = '.';
                output[length - 1] = '.';
                return output;
            }
            output[startblank + i] = input[i];
        }

        return output;
    } else {
        return formatstring + input;
    }
}

void TRestStringOutput::setlength(int n) {
    if (!REST_Display_CompatibilityMode) {
        if (n >= Console::GetWidth() - 2) {
            length = Console::GetWidth() - 2;
        } else if (n <= 0) {
            length = Console::GetWidth() - 2;
        } else {
            length = n;
        }
    } else {
        length = n;
    }
}

#ifdef WIN32
// use >> 4 << 4 to extract first 4 bytes from COLOR_RESET, which means the background color
#define SET_COLOR(color) \
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (COLOR_RESET >> 4 << 4) + color);
#define RESET_COLOR() SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), COLOR_RESET);
#else
#define SET_COLOR(color) std::cout << color;
#define RESET_COLOR() std::cout << COLOR_RESET;
#endif  // WIN32

void TRestStringOutput::flushstring() {
    if (REST_Display_CompatibilityMode)  // this means we are using condor
    {
        std::cout << buf.str() << std::endl;
    } else {
        Console::ClearCurrentLine();
        if (orientation == TRestStringOutput::REST_Display_Orientation::kMiddle) {
            // we always reset the length of TRestStringOutput in case the console is resized
            setlength(TRestStringOutput_BestLength);
            int blankwidth = (Console::GetWidth() - 2 - length) / 2;

            SET_COLOR(color);
            std::cout << string(blankwidth, ' ') << FormattingPrintString(buf.str())
                      << string(blankwidth, ' ');
            RESET_COLOR()
            std::cout << std::endl;
        } else {
            SET_COLOR(color);
            std::cout << FormattingPrintString(buf.str());
            RESET_COLOR()
            std::cout << std::endl;
        }
    }
    resetstring();
}

TRestStringOutput& TRestStringOutput::operator<<(void (*pfunc)(TRestStringOutput&)) {
    if (gVerbose >= verbose) {
        ((*pfunc)(*this));
    }
    return *this;
}
