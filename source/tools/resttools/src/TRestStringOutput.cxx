

#include "TRestStringOutput.h"

TRestStringOutput::TRestStringOutput(string _color, string BorderOrHeader,
                                     REST_Display_Format style) {
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
  if (length > ConsoleHelper::GetWidth() - 2)
    length = ConsoleHelper::GetWidth() - 2;

  stringbuf = "";
  if (length > 500 ||
      length <
          20)  // unsupported console, we will fall back to compatibility modes
  {
    length = -1;
  }
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
      (input[0] == '=' || input[0] == '-' || input[0] == '*' ||
       input[0] == '+')) {
    return string(length, input[0]);
  }

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

  if (useborder) {
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
  } else {
    string& header = formatstring;
    for (unsigned int i = 0; i < Lfmt && i < length; i++) {
      output[i] = header[i];
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
  }

  return output;
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
    cout << stringbuf << endl;
    stringbuf = "";
  } else {
    int consolewidth = ConsoleHelper::GetWidth() - 2;
    printf("\033[K");
    if (orientation == 0) {
      cout << color << string((consolewidth - length) / 2, ' ')
           << FormattingPrintString(stringbuf)
           << string((consolewidth - length) / 2, ' ') << COLOR_RESET << endl;
    } else {
      cout << color << FormattingPrintString(stringbuf) << COLOR_RESET << endl;
    }
    stringbuf = "";
  }
}
