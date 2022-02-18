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

using namespace std;

//////////////////////////////////////////////////////////////////////////
/// String identifiers for terminal colors
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
#define REST_Warning REST_Essential

//////////////////////////////////////////////////////////////////////////
/// Enumerate of TRestStringOutput display orientation
enum REST_Display_Orientation { kLeft = 1, kMiddle = 0 };

//////////////////////////////////////////////////////////////////////////
/// Console helper class, providing several static methods dealing with terminal
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
class Console {
   public:
    /// returns the width of the window
    static int GetWidth();
    /// returns the height of the window
    static int GetHeight();
    /// returns directly. true if keyboard is hit. be used together with while loop
    static bool kbhit();
    /// get one char from keyboard. need to press enter. (same as getchar())
    static int Read();
    /// get one char from keyboard. don't need to press enter.
    static int ReadKey();
    /// returns the whole input line in string. need to press enter.
    static string ReadLine();
    /// write the string to the console. doesn't append line ending mark.
    static void WriteLine(string content);
    /// move up cursor by n lines.
    static void CursorUp(int n);
    /// move down cursor by n lines. If hits buttom of the console, it won't keep moving
    static void CursorDown(int n);
    /// move right cursor by n characters.
    static void CursorRight(int n);
    /// move left cursor by n characters.
    static void CursorLeft(int n);
    /// move cursor to coordinate x,y. the origin is at topleft of the console
    static void CursorToXY(int x, int y);
    /// clear screen, and move cursor to the topleft of the console.
    static void ClearScreen();
    /// clear current line.
    static void ClearCurrentLine();
    /// clear lines after the cursor.
    static void ClearLinesAfterCursor();
    /// indicates whether the output tool should work under compatibility mode for nonatty
    static bool CompatibilityMode;
};

//////////////////////////////////////////////////////////////////////////
/// This class serves as an end-line mark for TRestStringOutput in TRestMetadata class.
///
/// It keeps a reference of the metadata class's verbose level and string buffer.
/// When calling `fout<<"hello world"<<endl;` inside metadata class, this class is
/// passed to TRestStringOutput, who compares the verbose level to dicide whether to
/// print, and saves the printed string to metadata class's string buffer.
class TRestMetadata;
struct endl_t {
    endl_t(TRestMetadata* ptr) { TRestMetadataPtr = ptr; }
    TRestMetadata* TRestMetadataPtr = 0;
    friend ostream& operator<<(ostream& a, endl_t& et) { return (a << std::endl); }
};

//////////////////////////////////////////////////////////////////////////
/// This class serves as an universal string output tool, aiming at leveling,
/// rendering, and auto saving for the output message.
///
/// To use this tool class in the other classes, include this header file.
/// You will get several global output objects: fout, info, essential, debug, etc.
/// they works similarly as cout: `fout<<"hello world"<<endl;`. It is also possible
/// to initialize a local TRestStringOutput object. Then one can costomize output color,
/// border and orientation on that.
class TRestStringOutput {
   protected:
    string color;
    string formatstring;
    bool useborder;
    bool iserror;
    int orientation;  // 0->middle, 1->left

    stringstream buf;
    int length;

    REST_Verbose_Level verbose;

    void lock();
    void unlock();

   public:
    string FormattingPrintString(string input);
    void resetstring();
    void flushstring();
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

    // If formatter is in mirror form(e.g., "|| ||","< >"), it will use such border
    // to wrap the string to be displayed. otherwise the formatter is used as
    // prefix(e.g., "-- Warning: ")
    TRestStringOutput(string color = COLOR_RESET, string formatter = "",
                      REST_Display_Orientation orientation = kLeft);

    TRestStringOutput(REST_Verbose_Level v, string _color = COLOR_RESET, string formatter = "",
                      REST_Display_Orientation orientation = kLeft, bool _iserror = false)
        : TRestStringOutput(_color, formatter, orientation) {
        verbose = v;
        iserror = _iserror;
    }

    template <class T>
    TRestStringOutput& operator<<(T content) {
        buf << content;
        return *this;
    }

    TRestStringOutput& operator<<(void (*pfunc)(TRestStringOutput&));

    TRestStringOutput& operator<<(endl_t et);
};

namespace noClass {
//////////////////////////////////////////////////////////////////////////
/// \relates TRestStringOutput
/// This method serves as an end-line mark for TRestStringOutput.
///
/// Calls TRestStringOutput to flushstring().
///
/// When calling `fout<<"hello world"<<endl;` outside metadata class, the pointer of
/// method is passed to TRestStringOutput, who calls back to this method giving its
/// reference. This logic is same as std::endl.
inline void endl(TRestStringOutput& input) { input.flushstring(); }
};  // namespace noClass
using namespace noClass;

/// \relates TRestStringOutput
/// print a welcome message by calling shell script "rest-config"
inline void PrintWelcome() { system("rest-config --welcome"); }

/// formatted message output, used for print metadata
extern TRestStringOutput fout;
extern TRestStringOutput ferr;
extern TRestStringOutput warning;
extern TRestStringOutput essential;
extern TRestStringOutput metadata;
extern TRestStringOutput info;
extern TRestStringOutput success;
extern TRestStringOutput debug;
extern TRestStringOutput extreme;

extern REST_Verbose_Level gVerbose;
#endif
