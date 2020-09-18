#include "TRestStringOutput.h"

bool Console::CompatibilityMode = false;

int Console::GetWidth() {
#ifdef WIN32
    return 100;
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
    return 100;
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
    return kbhit();
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
        return 1;
    }
    return 0;

// int byteswaiting;
//   ioctl(0, FIONREAD, &byteswaiting);
//   return byteswaiting > 0;
#endif
}

int Console::Read() { return getchar(); }

int Console::ReadKey() {
#ifdef WIN32
    return getch();
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
    printf(content.c_str());
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
    printf("\033[K");
    fflush(stdout);
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

TRestStringOutput::TRestStringOutput(string _color, string BorderOrHeader, REST_Display_Format style) {
    color = _color;
    formatstring = BorderOrHeader;

    if (style == kBorderedLeft) {
        orientation = 1;
        useborder = true;
    } else if (style == kBorderedMiddle) {
        orientation = 0;
        useborder = true;
    } else if (style == kHeaderedLeft) {
        orientation = 1;
        useborder = false;
    } else if (style == kHeaderedMiddle) {
        orientation = 0;
        useborder = false;
    }

    length = 100;
    if (length > Console::GetWidth() - 2) length = Console::GetWidth() - 2;

    resetstring();
    if (length > 500 || length < 20)  // unsupported console, we will fall back to compatibility modes
    {
        length = -1;
        Console::CompatibilityMode = true;
    }

    verbose = REST_Essential;
}

void TRestStringOutput::resetstring() {
    buf.clear();  //清空流
    buf.str("");  //清空流缓存
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
        if (useborder || orientation == 0) {
            startblank = (length - Lstr) / 2;
        } else {
            startblank = Lfmt;
        }
        if (startblank < 0) {
            startblank = 0;
        }

        string& border = formatstring;
        for (unsigned int i = 0; i < Lfmt && i < length; i++) {
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
    if (!Console::CompatibilityMode) {
        if (n < Console::GetWidth() - 2)
            length = n;
        else
            length = Console::GetWidth() - 2;
    }
}

void TRestStringOutput::flushstring() {
    if (Console::CompatibilityMode)  // this means we are using condor
    {
        std::cout << buf.str() << std::endl;
    } else {
        int consolewidth = Console::GetWidth() - 2;
        printf("\033[K");
        if (orientation == 0) {
            std::cout << color << string((consolewidth - length) / 2, ' ') << FormattingPrintString(buf.str())
                      << string((consolewidth - length) / 2, ' ') << COLOR_RESET << std::endl;
        } else {
            std::cout << color << FormattingPrintString(buf.str()) << COLOR_RESET << std::endl;
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

TRestStringOutput& TRestStringOutput::operator<<(endl_t et) {
    if (et.vref <= REST_Info) {
        et.sref += buf.str() + "\n";
        if (et.sref.size() > 1000) {
            et.sref.erase(0, et.sref.size() - 1000);
        }
    }

    if (et.vref >= verbose) {
        flushstring();
    } else {
        resetstring();
    }

    return *this;
}