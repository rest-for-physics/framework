#include "TRestStringHelper.h"

#include "Rtypes.h"

#if ROOT_VERSION_CODE < ROOT_VERSION(6, 0, 0)
#include "TFormula.h"
#else
#include "v5/TFormula.h"
#endif

#include <dirent.h>
#include <unistd.h>

using namespace std;

///////////////////////////////////////////////
/// \brief Returns 1 only if valid mathematical expression keywords (or numbers)
/// are found in the string **in**. If not it returns 0.
///
Int_t REST_StringHelper::isAExpression(string in) {
    string temp = in;
    vector<string> replace{"sqrt", "log", "exp", "gaus", "cos", "sin", "tan", "atan", "acos", "asin"};
    for (int i = 0; i < replace.size(); i++) {
        temp = Replace(temp, replace[i], "0", 0);
    }

    if (temp.length() == 0)
        return 0;
    else if (temp.length() == 1) {
        if (temp.find_first_not_of("0123456789") == std::string::npos) {
            return 1;
        } else {
            return 0;
        }
    } else {
        if (temp.find_first_not_of("-0123456789e+*/.,)( ^") == std::string::npos) {
            if (temp.find("/") == 0 || temp.find("./") == 0 || temp.find("../") == 0)
                return 0;  // identify path
            return 1;
        }
    }

    return 0;
}

///////////////////////////////////////////////
/// \brief Evaluates and replaces valid mathematical expressions found in the
/// input string **buffer**.
///
std::string REST_StringHelper::ReplaceMathematicalExpressions(std::string buffer) {
    // we spilt the unit part and the expresstion part
    int pos = buffer.find_last_of("1234567890().");

    string unit = buffer.substr(pos + 1, -1);
    string temp = buffer.substr(0, pos + 1);
    string result = "";

    bool erased = false;

    std::vector<std::string> Expressions = Split(temp, ",");

    if (Expressions.size() > 1 && Expressions[0][0] == '(' &&
        Expressions[Expressions.size() - 1][Expressions[Expressions.size() - 1].size() - 1] == ')') {
        Expressions[0].erase(0, 1);
        Expressions[Expressions.size() - 1].erase(Expressions[Expressions.size() - 1].size() - 1, 1);
        erased = true;
    }

    for (int i = 0; i < Expressions.size(); i++) {
        if (!isAExpression(Expressions[i])) {
            result += Expressions[i] + ",";
            continue;
        }
        result += EvaluateExpression(Expressions[i]) + ",";
    }
    if (Expressions.size() > 0) result.erase(result.size() - 1, 1);

    if (erased) {
        result = "(" + result + ")";
    }

    return result + unit;
}

///////////////////////////////////////////////
/// \brief Evaluates a complex numerical expression and returns the resulting
/// value using TFormula.
///
std::string REST_StringHelper::EvaluateExpression(std::string exp) {
    if (!isAExpression(exp)) {
        return exp;
    }

// NOTE!!! In root6 the expression like "1/2" will be computed using the input
// as int number, which will return 0, and cause problem. we roll back to
// TFormula of version 5
#if ROOT_VERSION_CODE < ROOT_VERSION(6, 0, 0)
    TFormula formula("tmp", exp.c_str());
#else
    ROOT::v5::TFormula formula("tmp", exp.c_str());
#endif

    ostringstream sss;
    Double_t number = formula.EvalPar(0);
    if (number > 0 && number < 1.e-300) {
        cout << "REST Warning! Expression not recognized --> " << exp << endl;
    }

    sss << number;
    string out = sss.str();

    return out;
}

///////////////////////////////////////////////
/// \brief Returns 1 only if a valid number is found in the string **in**. If
/// not it returns 0.
///
Int_t REST_StringHelper::isANumber(string in) {
    return (in.find_first_not_of("-+0123456789.eE") == std::string::npos && in.length() != 0);
}

///////////////////////////////////////////////
/// \brief Split the input string according to the given separator. Returning a
/// vector of fragments
///
/// e.g.
/// Input: "" and "", Output: {}
/// Input: ":" and ":", Output: {}
/// Input: "abc" and "", Output: { "a", "b", "c" }
/// Input: "abc:def" and ":", Output: { "abc", "def" }
/// Input: "abc:def" and ":def", Output: { "abc" }
std::vector<string> REST_StringHelper::Split(std::string in, string separator, bool allowBlankString,
                                             bool removeWhiteSpaces, int startPos) {
    std::vector<string> result;

    int pos = startPos;
    int front = 0;
    while (1) {
        pos = in.find(separator.c_str(), pos + 1);
        string sub = in.substr(front, pos - front);
        if (removeWhiteSpaces) sub = RemoveWhiteSpaces(sub);
        if (allowBlankString || sub != "") {
            result.push_back(sub);
        }
        front = pos + separator.size();
        if (pos == -1) break;
    }

    return result;
}

///////////////////////////////////////////////
/// \brief Returns the input string removing white spaces.
///
string REST_StringHelper::RemoveWhiteSpaces(string in) {
    string out = in;
    size_t pos = 0;
    while ((pos = out.find(" ", pos)) != string::npos) {
        out.erase(pos, 1);
        // pos = pos + 1;
    }

    return out;
}

ULong64_t REST_StringHelper::ToHash(string str) {
    ULong64_t prime = 0x100000001B3ull;
    ULong64_t basis = 0xCBF29CE484222325ull;
    ULong64_t ret{basis};

    for (int i = 0; i < str.size(); i++) {
        ret ^= str[i];
        ret *= prime;
    }

    return ret;
}

///////////////////////////////////////////////
/// \brief Counts the number of occurences of **substring** inside the input
/// string **in**.
///
Int_t REST_StringHelper::Count(string in, string substring) {
    int count = 0;
    size_t nPos = in.find(substring, 0);  // First occurrence
    while (nPos != string::npos) {
        count++;
        nPos = in.find(substring, nPos + 1);
    }

    return count;
}

/// \brief Returns the position of the **nth** occurence of the string
/// **strToFind** inside the string **in**.
///
Int_t REST_StringHelper::FindNthStringPosition(const string& in, size_t pos, const string& strToFind,
                                               size_t nth) {
    size_t found_pos = in.find(strToFind, pos);
    if (nth == 0 || string::npos == found_pos) return found_pos;
    return FindNthStringPosition(in, found_pos + 1, strToFind, nth - 1);
}

///////////////////////////////////////////////
/// \brief Replace every occurences of **thisSring** by **byThisString** inside
/// string **in**.
///
string REST_StringHelper::Replace(string in, string thisString, string byThisString, size_t fromPosition,
                                  Int_t N) {
    string out = in;
    size_t pos = fromPosition;
    Int_t cont = 0;
    while ((pos = out.find(thisString, pos)) != string::npos) {
        out.replace(pos, thisString.length(), byThisString);
        pos = pos + byThisString.length();
        cont++;

        if (N > 0 && cont == N) return out;
    }

    return out;
}

std::string REST_StringHelper::EscapeSpecialLetters(string in) {
    string result = Replace(in, "(", "\\(", 0);
    result = Replace(result, ")", "\\)", 0);
    result = Replace(result, "$", "\\$", 0);
    result = Replace(result, "#", "\\#", 0);
    result = Replace(result, "{", "\\{", 0);
    result = Replace(result, "}", "\\}", 0);
    result = Replace(result, "<", "\\<", 0);
    result = Replace(result, ">", "\\>", 0);
    return result;
}

///////////////////////////////////////////////
/// \brief Format time_t into string
///
/// The output datatime format is "Y-M-D H:M:S". e.g.
/// \code
/// REST_StringHelper::ToDateTimeString(0)
/// (return) 1970-1-1 8:00:00
/// \endcode
/// here the type "time_t" is actually the type "long long", which indicates the
/// elapsed time in second from 1970-1-1 8:00:00
string REST_StringHelper::ToDateTimeString(time_t time) {
    tm* tm_ = localtime(&time);
    int year, month, day, hour, minute, second;
    year = tm_->tm_year + 1900;
    month = tm_->tm_mon + 1;
    day = tm_->tm_mday;
    hour = tm_->tm_hour;
    minute = tm_->tm_min;
    second = tm_->tm_sec;
    char yearStr[5], monthStr[3], dayStr[3], hourStr[3], minuteStr[3], secondStr[3];
    sprintf(yearStr, "%d", year);
    sprintf(monthStr, "%d", month);
    sprintf(dayStr, "%d", day);
    sprintf(hourStr, "%d", hour);
    sprintf(minuteStr, "%d", minute);
    if (minuteStr[1] == '\0') {
        minuteStr[2] = '\0';
        minuteStr[1] = minuteStr[0];
        minuteStr[0] = '0';
    }
    sprintf(secondStr, "%d", second);
    if (secondStr[1] == '\0') {
        secondStr[2] = '\0';
        secondStr[1] = secondStr[0];
        secondStr[0] = '0';
    }
    char s[20];
    sprintf(s, "%s-%s-%s %s:%s:%s", yearStr, monthStr, dayStr, hourStr, minuteStr, secondStr);
    string str(s);
    return str;
}

///////////////////////////////////////////////
/// \brief A method to convert a date/time formatted string to a timestamp.
///
/// The input datatime format should match any of the following patterns:
/// "YYYY-mm-DD HH:MM:SS", "YYYY/mm/DD HH:MM:SS", "YYYY-mm-DD", or "YYYY/mm/DD".
/// If no time is given it will be assumed to be 00:00:00.
///
/// \code
/// REST_StringHelper::ToTime("2018-1-1 8:00:00")
/// (return) 1514764800
/// \endcode
///
/// here the type "time_t" is actually the type "long long", which indicates the
/// elapsed time in second from 1970-1-1 8:00:00
///
time_t REST_StringHelper::StringToTimeStamp(string time) {
    struct tm tm1;
    tm1.tm_hour = 0;
    tm1.tm_min = 0;
    tm1.tm_sec = 0;
    time_t time1;
    if (time.find(":") != string::npos) {
        if (time.find("/") != string::npos)
            sscanf(time.c_str(), "%d/%d/%d %d:%d:%d", &(tm1.tm_year), &(tm1.tm_mon), &(tm1.tm_mday),
                   &(tm1.tm_hour), &(tm1.tm_min), &(tm1.tm_sec));
        else
            sscanf(time.c_str(), "%d-%d-%d %d:%d:%d", &(tm1.tm_year), &(tm1.tm_mon), &(tm1.tm_mday),
                   &(tm1.tm_hour), &(tm1.tm_min), &(tm1.tm_sec));
    } else {
        if (time.find("/") != string::npos)
            sscanf(time.c_str(), "%d/%d/%d", &(tm1.tm_year), &(tm1.tm_mon), &(tm1.tm_mday));
        else
            sscanf(time.c_str(), "%d-%d-%d", &(tm1.tm_year), &(tm1.tm_mon), &(tm1.tm_mday));
    }

    tm1.tm_year -= 1900;
    tm1.tm_mon--;
    tm1.tm_isdst = -1;
    time1 = mktime(&tm1);

    return time1;
}

REST_Verbose_Level REST_StringHelper::StringToVerboseLevel(string in) {
    if (ToUpper(in) == "SILENT" || in == "0") return REST_Silent;
    if (ToUpper(in) == "ESSENTIAL" || ToUpper(in) == "WARNING" || in == "1") return REST_Essential;
    if (ToUpper(in) == "INFO" || in == "2") return REST_Info;
    if (ToUpper(in) == "DEBUG" || in == "3") return REST_Debug;
    if (ToUpper(in) == "EXTREME" || in == "4") return REST_Extreme;

    return REST_Essential;
}

///////////////////////////////////////////////
/// \brief Gets a double from a string.
///
Double_t REST_StringHelper::StringToDouble(string in) {
    if (isANumber(in)) {
        return stod(in);
    } else {
        return -1;
    }
}

///////////////////////////////////////////////
/// \brief Gets a float from a string.
///
Float_t REST_StringHelper::StringToFloat(string in) {
    if (isANumber(in)) {
        return stof(in);
    } else {
        return -1;
    }
}

///////////////////////////////////////////////
/// \brief Gets an integer from a string.
///
Int_t REST_StringHelper::StringToInteger(string in) { return (Int_t)StringToDouble(in); }

Bool_t REST_StringHelper::StringToBool(std::string in) {
    return (ToUpper(in) == "TRUE" || ToUpper(in) == "ON");
}

Long64_t REST_StringHelper::StringToLong(std::string in) {
    stringstream strIn;
    strIn << in;
    long long llNum;
    strIn >> llNum;
    return llNum;
}

///////////////////////////////////////////////
/// \brief Gets a 3D-vector from a string. Format should be : (X,Y,Z).
///
TVector3 REST_StringHelper::StringTo3DVector(string in) {
    TVector3 a;

    size_t startVector = in.find_first_of("(");
    if (startVector == string::npos) return a;

    size_t endVector = in.find_first_of(")");
    if (endVector == string::npos) return a;

    size_t n = count(in.begin(), in.end(), ',');
    if (n != 2) return a;

    size_t firstComma = in.find_first_of(",");
    size_t secondComma = in.find(",", firstComma + 1);

    if (firstComma >= endVector || firstComma <= startVector) return a;
    if (secondComma >= endVector || secondComma <= startVector) return a;

    string X = in.substr(startVector + 1, firstComma - startVector - 1);
    string Y = in.substr(firstComma + 1, secondComma - firstComma - 1);
    string Z = in.substr(secondComma + 1, endVector - secondComma - 1);

    a.SetXYZ(StringToDouble(X), StringToDouble(Y), StringToDouble(Z));

    return a;
}

///////////////////////////////////////////////
/// \brief Gets a 2D-vector from a string.
///
TVector2 REST_StringHelper::StringTo2DVector(string in) {
    TVector2 a(-1, -1);

    size_t startVector = in.find_first_of("(");
    if (startVector == string::npos) return a;

    size_t endVector = in.find_first_of(")");
    if (endVector == string::npos) return a;

    size_t n = count(in.begin(), in.end(), ',');
    if (n != 1) return a;

    size_t firstComma = in.find_first_of(",");

    if (firstComma >= endVector || firstComma <= startVector) return a;

    string X = in.substr(startVector + 1, firstComma - startVector - 1);
    string Y = in.substr(firstComma + 1, endVector - firstComma - 1);

    a.Set(StringToDouble(X), StringToDouble(Y));

    return a;
}

///////////////////////////////////////////////
/// \brief Convert string to its upper case. Alternative of TString::ToUpper
///
std::string REST_StringHelper::ToUpper(std::string str) {
    transform(str.begin(), str.end(), str.begin(), (int (*)(int))toupper);
    return str;
}

#ifdef WIN32
string get_current_dir_name() {
    char pBuf[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, pBuf);
    return string(pBuf);
}
#endif
