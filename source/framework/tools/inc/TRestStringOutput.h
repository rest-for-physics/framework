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

//////////////////////////////////////////////////////////////////////////
/// String identifiers for terminal colors
constexpr const char* const COLOR_RESET = "\033[0m";
constexpr const char* const COLOR_BLACK = "\033[30m";                    /* Black */
constexpr const char* const COLOR_RED = "\033[31m";                      /* Red */
constexpr const char* const COLOR_GREEN = "\033[32m";                    /* Green */
constexpr const char* const COLOR_YELLOW = "\033[33m";                   /* Yellow */
constexpr const char* const COLOR_BLUE = "\033[34m";                     /* Blue */
constexpr const char* const COLOR_MAGENTA = "\033[35m";                  /* Magenta */
constexpr const char* const COLOR_CYAN = "\033[36m";                     /* Cyan */
constexpr const char* const COLOR_WHITE = "\033[37m";                    /* White */
constexpr const char* const COLOR_BOLDBLACK = "\033[1m\033[30m";         /* Bold Black */
constexpr const char* const COLOR_BOLDRED = "\033[1m\033[31m";           /* Bold Red */
constexpr const char* const COLOR_BOLDGREEN = "\033[1m\033[32m";         /* Bold Green */
constexpr const char* const COLOR_BOLDYELLOW = "\033[1m\033[33m";        /* Bold Yellow */
constexpr const char* const COLOR_BOLDBLUE = "\033[1m\033[34m";          /* Bold Blue */
constexpr const char* const COLOR_BOLDMAGENTA = "\033[1m\033[35m";       /* Bold Magenta */
constexpr const char* const COLOR_BOLDCYAN = "\033[1m\033[36m";          /* Bold Cyan */
constexpr const char* const COLOR_BOLDWHITE = "\033[1m\033[37m";         /* Bold White */
constexpr const char* const COLOR_BACKGROUNDBLACK = "\033[1m\033[40m";   /* BACKGROUND Black */
constexpr const char* const COLOR_BACKGROUNDRED = "\033[1m\033[41m";     /* BACKGROUND Red */
constexpr const char* const COLOR_BACKGROUNDGREEN = "\033[1m\033[42m";   /* BACKGROUND Green */
constexpr const char* const COLOR_BACKGROUNDYELLOW = "\033[1m\033[43m";  /* BACKGROUND Yellow */
constexpr const char* const COLOR_BACKGROUNDBLUE = "\033[1m\033[44m";    /* BACKGROUND Blue */
constexpr const char* const COLOR_BACKGROUNDMAGENTA = "\033[1m\033[45m"; /* BACKGROUND Magenta */
constexpr const char* const COLOR_BACKGROUNDCYAN = "\033[1m\033[46m";    /* BACKGROUND Cyan */
constexpr const char* const COLOR_BACKGROUNDWHITE = "\033[1m\033[47m";   /* BACKGROUND White */

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
    /// returns the whole input line in std::string. need to press enter.
    static std::string ReadLine();
    /// write the std::string to the console. doesn't append line ending mark.
    static void WriteLine(std::string content);
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
/// It keeps a reference of the metadata class's verbose level and std::string buffer.
/// When calling `fout<<"hello world"<<endl;` inside metadata class, this class is
/// passed to TRestStringOutput, who compares the verbose level to dicide whether to
/// print, and saves the printed std::string to metadata class's std::string buffer.
class TRestMetadata;
struct endl_t {
    endl_t(TRestMetadata* ptr) { TRestMetadataPtr = ptr; }
    TRestMetadata* TRestMetadataPtr = nullptr;
    friend std::ostream& operator<<(std::ostream& a, endl_t& et) { return (a << std::endl); }
};

//////////////////////////////////////////////////////////////////////////
/// This class serves as an universal std::string output tool, aiming at leveling,
/// rendering, and auto saving for the output message.
///
/// To use this tool class in the other classes, include this header file.
/// You will get several global output objects: fout, info, essential, debug, etc.
/// they works similarly as std::cout: `fout<<"hello world"<<endl;`. It is also possible
/// to initialize a local TRestStringOutput object. Then one can costomize output color,
/// border and orientation on that.
class TRestStringOutput {
   public:
    //////////////////////////////////////////////////////////////////////////
    /// Enumerate of verbose level, containing five levels
    enum class REST_Verbose_Level {
        REST_Silent = 0,     //!< show minimum information of the software, as well as error
                             //!< messages
        REST_Essential = 1,  //!< +show some essential information, as well as warnings
        REST_Warning = REST_Essential,
        REST_Info = 2,    //!< +show most of the infomation for each steps
        REST_Debug = 3,   //!< +show the defined debug messages
        REST_Extreme = 4  //!< show everything
    };

    //////////////////////////////////////////////////////////////////////////
    /// Enumerate of TRestStringOutput display orientation
    enum class REST_Display_Orientation { kLeft = 1, kMiddle = 0 };

   protected:
    std::string color;
    std::string formatstring;
    bool useborder;
    bool iserror;
    REST_Display_Orientation orientation;  // 0->middle, 1->left

    std::stringstream buf;
    int length;

    REST_Verbose_Level verbose;

    void lock();
    void unlock();

   public:
    REST_Verbose_Level GetVerboseLevel() { return verbose; }
    std::string GetBuffer() { return buf.str(); }
    bool isError() { return iserror; }
    std::string FormattingPrintString(std::string input);
    void resetstring();
    void flushstring();
    void setcolor(std::string colordef) { color = colordef; }
    void setheader(std::string headerdef) {
        formatstring = headerdef;
        useborder = false;
    }
    void resetcolor() { color = COLOR_RESET; }
    void resetheader() { formatstring = ""; }
    void setborder(std::string b) {
        formatstring = b;
        useborder = true;
    }
    void resetborder() { formatstring = ""; }
    void setlength(int n);
    void setorientation(REST_Display_Orientation o) { orientation = o; }
    void resetorientation() { orientation = REST_Display_Orientation::kMiddle; }
    static void RESTendl(TRestStringOutput& input) { input.flushstring(); }

    // If formatter is in mirror form(e.g., "|| ||","< >"), it will use such border
    // to wrap the std::string to be displayed. otherwise the formatter is used as
    // prefix(e.g., "-- Warning: ")
    TRestStringOutput(
        std::string color = COLOR_RESET, std::string formatter = "",
        REST_Display_Orientation orientation = TRestStringOutput::REST_Display_Orientation::kLeft);

    TRestStringOutput(
        REST_Verbose_Level v, std::string _color = COLOR_RESET, std::string formatter = "",
        REST_Display_Orientation orientation = TRestStringOutput::REST_Display_Orientation::kLeft,
        bool _iserror = false)
        : TRestStringOutput(_color, formatter, orientation) {
        verbose = v;
        iserror = _iserror;
    }

    template <class T>
    TRestStringOutput& operator<<(T const& content) {
        buf << content;
        return *this;
    }

    TRestStringOutput& operator<<(void (*pfunc)(TRestStringOutput&));
    friend TRestStringOutput& operator<<(TRestMetadata& mt, TRestStringOutput& so);
    TRestStringOutput& operator<<(endl_t et);
};

/// \relates TRestStringOutput
/// print a welcome message by calling shell script "rest-config"
inline void PrintWelcome() { system("rest-config --welcome"); }

/// formatted message output, used for print metadata
// initialize formatted message output tool
// initialize formatted message output tool
static TRestStringOutput RESTFout(TRestStringOutput::REST_Verbose_Level::REST_Silent, COLOR_BOLDBLUE,
                                  "[== ==]", TRestStringOutput::REST_Display_Orientation::kMiddle);
static TRestStringOutput RESTFerr(TRestStringOutput::REST_Verbose_Level::REST_Silent, COLOR_BOLDRED,
                                  "-- Error : ", TRestStringOutput::REST_Display_Orientation::kLeft, true);
static TRestStringOutput RESTWarning(TRestStringOutput::REST_Verbose_Level::REST_Warning, COLOR_BOLDYELLOW,
                                     "-- Warning : ", TRestStringOutput::REST_Display_Orientation::kLeft,
                                     true);
static TRestStringOutput RESTEssential(TRestStringOutput::REST_Verbose_Level::REST_Essential, COLOR_BOLDGREEN,
                                       "", TRestStringOutput::REST_Display_Orientation::kMiddle);
static TRestStringOutput RESTMetadata(TRestStringOutput::REST_Verbose_Level::REST_Essential, COLOR_BOLDGREEN,
                                      "|| ||", TRestStringOutput::REST_Display_Orientation::kMiddle);
static TRestStringOutput RESTInfo(TRestStringOutput::REST_Verbose_Level::REST_Info, COLOR_BLUE,
                                  "-- Info : ", TRestStringOutput::REST_Display_Orientation::kLeft);
static TRestStringOutput RESTSuccess(TRestStringOutput::REST_Verbose_Level::REST_Info, COLOR_GREEN,
                                     "-- Success : ", TRestStringOutput::REST_Display_Orientation::kLeft);
static TRestStringOutput RESTDebug(TRestStringOutput::REST_Verbose_Level::REST_Debug, COLOR_RESET,
                                   "-- Debug : ", TRestStringOutput::REST_Display_Orientation::kLeft);
static TRestStringOutput RESTExtreme(TRestStringOutput::REST_Verbose_Level::REST_Extreme, COLOR_RESET,
                                     "-- Extreme : ", TRestStringOutput::REST_Display_Orientation::kLeft);

static void RESTendl(TRestStringOutput& input) { input.flushstring(); }

extern TRestStringOutput::REST_Verbose_Level gVerbose;

#endif
