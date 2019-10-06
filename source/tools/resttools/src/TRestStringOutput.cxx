
#include "TRestStringOutput.h"
#include <mutex>

std::mutex mutex_stringoutput;

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
    if (length > ConsoleHelper::GetWidth() - 2) length = ConsoleHelper::GetWidth() - 2;

    resetstring();
    if (length > 500 || length < 20)  // unsupported console, we will fall back to compatibility modes
    {
        length = -1;
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
    if (length != -1) {
        if (n < ConsoleHelper::GetWidth() - 2)
            length = n;
        else
            length = ConsoleHelper::GetWidth() - 2;
    }
}

void TRestStringOutput::flushstring() {
    if (length == -1)  // this means we are using condor
    {
        std::cout << buf << std::endl;
    } else {
        int consolewidth = ConsoleHelper::GetWidth() - 2;
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

/// formatted message output, used for print metadata
TRestStringOutput fout(REST_Silent, COLOR_BOLDBLUE, "[==", kBorderedMiddle);
TRestStringOutput error(REST_Silent, COLOR_BOLDRED, "-- Error : ", kHeaderedLeft);
TRestStringOutput warning(REST_Warning, COLOR_BOLDYELLOW, "-- Warning : ", kHeaderedLeft);
TRestStringOutput essential(REST_Essential, COLOR_BOLDGREEN, "", kHeaderedMiddle);
TRestStringOutput metadata(REST_Essential, COLOR_BOLDGREEN, "||", kBorderedMiddle);
TRestStringOutput info(REST_Info, COLOR_BLUE, "-- Info : ", kHeaderedLeft);
TRestStringOutput success(REST_Info, COLOR_GREEN, "-- Success : ", kHeaderedLeft);
TRestStringOutput debug(REST_Debug, COLOR_RESET, "-- Debug : ", kHeaderedLeft);
TRestStringOutput extreme(REST_Extreme, COLOR_RESET, "-- Extreme : ", kHeaderedLeft);