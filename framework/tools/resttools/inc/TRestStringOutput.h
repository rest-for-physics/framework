#ifndef RestCore_TRestStringOutput
#define RestCore_TRestStringOutput

#include <RConfig.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

#ifndef WIN32
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#ifndef __APPLE__
#include <termio.h>
#endif

#endif  // WIN32

#define COLOR_RESET "\033[0m"
#define COLOR_BLACK "\033[30m"                    /* Black */
#define COLOR_RED "\033[31m"                      /* Red */
#define COLOR_GREEN "\033[32m"                    /* Green */
#define COLOR_YELLOW "\033[33m"                   /* Yellow */
#define COLOR_BLUE "\033[34m"                     /* Blue */
#define COLOR_MAGENTA "\033[35m"                  /* Magenta */
#define COLOR_CYAN "\033[36m"                     /* Cyan */
#define COLOR_WHITE "\033[37m"                    /* White */
#define COLOR_BOLDBLACK "\033[1m\033[30m"         /* Bold Black */
#define COLOR_BOLDRED "\033[1m\033[31m"           /* Bold Red */
#define COLOR_BOLDGREEN "\033[1m\033[32m"         /* Bold Green */
#define COLOR_BOLDYELLOW "\033[1m\033[33m"        /* Bold Yellow */
#define COLOR_BOLDBLUE "\033[1m\033[34m"          /* Bold Blue */
#define COLOR_BOLDMAGENTA "\033[1m\033[35m"       /* Bold Magenta */
#define COLOR_BOLDCYAN "\033[1m\033[36m"          /* Bold Cyan */
#define COLOR_BOLDWHITE "\033[1m\033[37m"         /* Bold White */
#define COLOR_BACKGROUNDBLACK "\033[1m\033[40m"   /* BACKGROUND Black */
#define COLOR_BACKGROUNDRED "\033[1m\033[41m"     /* BACKGROUND Red */
#define COLOR_BACKGROUNDGREEN "\033[1m\033[42m"   /* BACKGROUND Green */
#define COLOR_BACKGROUNDYELLOW "\033[1m\033[43m"  /* BACKGROUND Yellow */
#define COLOR_BACKGROUNDBLUE "\033[1m\033[44m"    /* BACKGROUND Blue */
#define COLOR_BACKGROUNDMAGENTA "\033[1m\033[45m" /* BACKGROUND Magenta */
#define COLOR_BACKGROUNDCYAN "\033[1m\033[46m"    /* BACKGROUND Cyan */
#define COLOR_BACKGROUNDWHITE "\033[1m\033[47m"   /* BACKGROUND White */

using namespace std;

//////////////////////////////////////////////////////////////////////////
/// Enumerate of verbose level, containing five levels
enum REST_Verbose_Level {
    REST_Silent,     //!< show minimum information of the software, as well as error
                     //!< messages
    REST_Essential,  //!< +show some essential information, as well as warnings
    REST_Info,       //!< +show most of the infomation for each steps
    REST_Debug,      //!< +show the defined debug messages
    REST_Extreme     //!< show everything
};

enum REST_Display_Format { kBorderedLeft, kBorderedMiddle, kHeaderedLeft, kHeaderedMiddle };

#define REST_Warning REST_Essential

//////////////////////////////////////////////////////////////////////////
/// ConsoleHelper class, providing several static methods dealing with terminal
///
/// GetWidth: returns the width of the current treminal(how many characterics
/// can be contained in one line)
///
/// GetHeight: returns the height of the current treminal(how many lines)
///
/// kbhit: return true if a key is pressed. won't jam the program like getchar()
///
/// getch: return the keyvalue if a key is pressed. won't jam the program like
/// getchar()
class ConsoleHelper {
   public:
    static int GetWidth() {
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

    static int GetHeight() {
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

#ifdef WIN32
    static int kbhit(void) { return kbhit(); }
    static int getch(void) { return getch(); }
    static void enable_raw_mode() {}
    static void disable_raw_mode() {}
#else
    static void enable_raw_mode() {
        termios term;
        tcgetattr(0, &term);
        term.c_lflag &= ~(ICANON | ECHO);  // Disable echo as well
        tcsetattr(0, TCSANOW, &term);
    }

    static void disable_raw_mode() {
        termios term;
        tcgetattr(0, &term);
        term.c_lflag |= ICANON | ECHO;
        tcsetattr(0, TCSANOW, &term);
    }

    static bool kbhit() {
        int byteswaiting;
        ioctl(0, FIONREAD, &byteswaiting);
        return byteswaiting > 0;
    }

    static int getch(void) {
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
    }

#endif  // WIN32
};
//////////////////////////////////////////////////////////////////////////
///
/// This class serves as an universal string output tool, aiming at leveling,
/// arranging, and auto saving for the output message.
///
/// Features of this class:
/// 1. overloaded operator "<<"
/// 2. inline method endl()
/// 3. pre-defined color constants
/// 4. output formatting methods
///
/// To use this tool class in the other classes, include its header file and
/// instantiate a TRestStringOutput object(suggested name: "fout"). The fout can
/// therefore replace the functionality of cout. The usage is the same:
/// "fout<<"hello world"<<endl;". Setting output color, changing output border,
/// length or orientation are all supported. In future it is also possible to
/// save output message in a log file.

class TRestStringOutput {
   protected:
    string color;
    string formatstring;
    bool useborder;
    int orientation;  // 0->middle, 1->left, 2->right

    string stringbuf;
    int length;

   public:
    string FormattingPrintString(string input);
    void resetstring() { stringbuf = ""; }
    void flushstring();
    void insertfront(string str) { stringbuf = str + stringbuf; }
    void insertback(string str) { stringbuf = stringbuf + str; }
    void setcolor(string colordef) { color = colordef; }
    void setheader(string headerdef) {
        formatstring = headerdef;
        useborder = false;
    }
    void resetcolor() { color = COLOR_RESET; }
    void resetheader() { formatstring = ""; }
    void setborder(string b) {
        formatstring = b;
        useborder = true;
    }
    void resetborder() { formatstring = ""; }
    void setlength(int n);
    void setorientation(int o) { orientation = o; }
    void resetorientation() { orientation = 0; }
    bool CompatibilityMode() { return length == -1; }

    // style options: < : orientation left, ^ : orientation middle, > :
    // orientation right, | : use border, - : use header
    TRestStringOutput(string _color = COLOR_RESET, string BorderOrHeader = "",
                      REST_Display_Format style = kBorderedLeft);

    template <class T>
    TRestStringOutput& operator<<(T content) {
        stringstream tmp;
        tmp << content;
        stringbuf += tmp.str();
        return *this;
    }

    TRestStringOutput& operator<<(void (*pfunc)(TRestStringOutput&)) {
        ((*pfunc)(*this));
        return *this;
    }
};

//////////////////////////////////////////////////////////////////////////
/// Leveled string output class, won't print message if verbose level is smaller
/// than required.
///
/// Usage:
/// \code
/// TRestLeveledOutput<REST_Debug> debug =
/// TRestLeveledOutput<REST_Debug>(fVerboseLevel, COLOR_RESET, "", 1); \endcode
template <REST_Verbose_Level v>
class TRestLeveledOutput : public TRestStringOutput {
   public:
    TRestLeveledOutput(){};
    TRestLeveledOutput(REST_Verbose_Level& vref, string _color = COLOR_RESET, string BorderOrHeader = "",
                       REST_Display_Format style = kBorderedLeft)
        : TRestStringOutput(_color, BorderOrHeader, style), verboselvlref(vref) {}

    REST_Verbose_Level verbose = v;
    REST_Verbose_Level& verboselvlref;

    template <class T>
    TRestLeveledOutput& operator<<(T content) {
        stringstream tmp;
        tmp << content;
        stringbuf += tmp.str();
        return *this;
    }

    TRestLeveledOutput& operator<<(void (*pfunc)(TRestLeveledOutput&)) {
        ((*pfunc)(*this));
        return *this;
    }
};

/// \relates ConsoleHelper
/// move up cursor by n lines. take effect immediately.
inline void cursorUp(int n) {
    printf("\033[%dA", n);
    fflush(stdout);
}

/// \relates ConsoleHelper
/// move down cursor by n lines. take effect immediately.
///
/// If hits buttom of the console, it won't keep moving
inline void cursorDown(int n) {
    printf("\033[%dB", n);
    fflush(stdout);
}

/// \relates ConsoleHelper
/// move right cursor by n characters. take effect immediately.
inline void cursorRight(int n) {
    printf("\033[%dC", n);
    fflush(stdout);
}

/// \relates ConsoleHelper
/// move left cursor by n characters. take effect immediately.
inline void cursorLeft(int n) {
    printf("\033[%dD", n);
    fflush(stdout);
}

/// \relates ConsoleHelper
/// move cursor to coordinate x,y. take effect immediately.
///
/// the origin in at topleft of the console
inline void cursorToXY(int x, int y) {
    printf("\033[%d%dH", x, y);
    fflush(stdout);
}

/// \relates ConsoleHelper
/// clear screen, and move cursor to the topleft of the console. take effect
/// immediately.
inline void clearScreen() {
    printf("\033[2J");
    fflush(stdout);
}

/// \relates ConsoleHelper
/// clear current line. take effect immediately.
inline void clearCurrentLine() {
    printf("\033[K");

    fflush(stdout);
}

/// \relates ConsoleHelper
/// clear lines after the cursor. take effect immediately.
inline void clearLinesAfterCursor() {
    printf("\033[s");

    for (int i = 0; i < 50; i++) {
        printf("\033[K");
        cursorDown(1);
    }

    printf("\033[u");

    fflush(stdout);
}

/// \relates TRestStringOutput
/// calls TRestStringOutput to flush string
inline void endl(TRestStringOutput& input) { input.flushstring(); }

/// \relates TRestLeveledOutput
/// calls TRestLeveledOutput to flush string when the referred verbose level
/// meets the required
template <REST_Verbose_Level v>
inline void endl(TRestLeveledOutput<v>& input) {
    if (input.verboselvlref >= input.verbose)
        input.flushstring();
    else
        input.resetstring();
}

/// \relates TRestStringOutput
/// print a welcome message by calling shell script "rest-config"
inline void PrintWelcome() { system("rest-config --welcome"); }

#endif
