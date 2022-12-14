#include "TRestStringHelper.h"

#include <Rtypes.h>
#include <TApplication.h>
#include <TSystem.h>

#include <thread>

#if ROOT_VERSION_CODE < ROOT_VERSION(6, 0, 0)
#include <TFormula.h>
#else
#include <v5/TFormula.h>
#endif

using namespace std;

///////////////////////////////////////////////
/// \brief Returns 1 only if valid mathematical expression keywords (or numbers)
/// are found in the string **in**. If not it returns 0.
///
/// By logic, mathematical expressions must have: +-*/e^% in the middle, or % in the end, or math functions in
/// the beginning. despite those symbols, the string should be purely numeric. example: 1+1 --> expression
/// sin(1.5) --> expression
/// 123456789 --> not expression, It is a pure number that can be directly parsed.
/// ./123 --> not expression, it is a path
/// 333/555 --> is expression. But it may also be a path. We should avoid using paths like that
///
Int_t REST_StringHelper::isAExpression(const string& in) {
    bool symbol = false;

    if (in.length() < 2)  // minimum expression: 3%
        return 0;

    vector<string> funcs{"sqrt", "log", "exp", "gaus", "cos", "sin", "tan", "atan", "acos", "asin"};
    for (const auto& item : funcs) {
        if (in.find(item) != string::npos) {
            symbol = true;
            break;
        }
    }

    if (!symbol) {
        size_t pos = in.find_first_of("+-*/e^%");
        if (pos > 0 && pos < in.size() - 1) {
            symbol = true;
        }
    }

    if (!symbol) {
        size_t pos = in.find_first_of("%");
        if (pos == in.size() - 1) {
            symbol = true;
        }
    }

    if (symbol) {
        string temp = in;
        for (const auto& item : funcs) {
            temp = Replace(temp, item, "0", 0);
        }
        if (temp.find_first_not_of("-0123456789e+*/.,)( ^%") == string::npos) {
            if (temp.find("/") == 0 || temp.find("./") == 0 || temp.find("../") == 0)
                return 0;  // identify path
            return 1;
        }
    } else {
        return 0;
    }

    return 0;
}

///////////////////////////////////////////////
/// \brief It crops a floating number given inside the string `in` with the given precision.
/// I.e. CropWithPrecision("3.48604", 2) will return "3.48".
///
/// It will not round the number. Perhaps on the next update of this method.
///
string REST_StringHelper::CropWithPrecision(string in, Int_t precision) {
    if (precision == 0) return in;
    if (REST_StringHelper::isANumber(in) && in.find(".") != string::npos) {
        string rootStr;
        if (in.find("e") != string::npos) rootStr = in.substr(in.find("e"), -1);
        return in.substr(0, in.find(".") + precision + 1) + rootStr;
    }
    return in;
}

///////////////////////////////////////////////
/// \brief Evaluates and replaces valid mathematical expressions found in the
/// input string **buffer**.
///
/// The buffer string may define sub-expressions that will be evaluated by using single quotes.
///
/// I.e. The sentence "The following operation 3 x 4 is '3*4'" will be translated to
/// "The following operatin 3 x 4 is 12".
///
string REST_StringHelper::ReplaceMathematicalExpressions(string buffer, Int_t precision,
                                                         string errorMessage) {
    buffer = Replace(buffer, " AND ", " && ");
    buffer = Replace(buffer, " OR ", " || ");

    if (buffer.find("'") != string::npos && count(buffer.begin(), buffer.end(), '\'') % 2 == 0) {
        size_t pos1 = buffer.find("'");
        size_t pos2 = buffer.find("'", pos1 + 1);
        string expr = buffer.substr(pos1 + 1, pos2 - pos1 - 1);

        if (!isAExpression(expr)) return buffer;

        string evalExpr = ReplaceMathematicalExpressions(expr, precision);
        expr = "'" + expr + "'";
        string newbuff = Replace(buffer, expr, evalExpr);

        return ReplaceMathematicalExpressions(newbuff, precision, errorMessage);
    }

    // we spilt the unit part and the expresstion part
    int pos = buffer.find_last_of("1234567890().");

    string unit = buffer.substr(pos + 1, -1);
    string temp = buffer.substr(0, pos + 1);
    string result = "";

    bool erased = false;

    vector<string> Expressions = Split(temp, ",");

    if (Expressions.size() > 1 && Expressions[0][0] == '(' &&
        Expressions[Expressions.size() - 1][Expressions[Expressions.size() - 1].size() - 1] == ')') {
        Expressions[0].erase(0, 1);
        Expressions[Expressions.size() - 1].erase(Expressions[Expressions.size() - 1].size() - 1, 1);
        erased = true;
    }

    for (unsigned int i = 0; i < Expressions.size(); i++) {
        if (!isAExpression(Expressions[i])) {
            result += Expressions[i] + ",";
            continue;
        }
        string evaluated = EvaluateExpression(Expressions[i]);
        if (evaluated == "RESTerror") {
            result += Expressions[i] + ",";
            RESTError << "ReplaceMathematicalExpressions. Error on RML syntax!" << RESTendl;
            if (errorMessage != "") RESTError << errorMessage << RESTendl;
        } else
            result += evaluated + ",";
    }
    if (Expressions.size() > 0) result.erase(result.size() - 1, 1);

    if (erased) {
        result = "(" + result + ")";
    }
    result = CropWithPrecision(result, precision);

    return result + unit;
}

///////////////////////////////////////////////
/// \brief Evaluates a complex numerical expression and returns the resulting
/// value using TFormula.
///
string REST_StringHelper::EvaluateExpression(string exp) {
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
        RESTWarning << "REST_StringHelper::EvaluateExpresssion. Expression not recognized --> " << exp
                    << RESTendl;
        return (string) "RESTerror";
    }

    sss << number;
    string out = sss.str();

    return out;
}

///////////////////////////////////////////////////
/// \brief Helps to pause the program, printing a message before pausing.
///
/// ROOT GUI won't be jammed during this pause.
Int_t REST_StringHelper::GetChar(string hint) {
    if (gApplication != nullptr && !gApplication->IsRunning()) {
        thread t = thread(&TApplication::Run, gApplication, true);
        t.detach();

        cout << hint << endl;
        int result = REST_Display_CompatibilityMode ? 1 : getchar();
        gSystem->ExitLoop();
        return result;
    } else {
        cout << hint << endl;
        return REST_Display_CompatibilityMode ? 1 : getchar();
    }
    return -1;
}

///////////////////////////////////////////////
/// \brief Returns 1 only if a valid number is found in the string **in**. If
/// not it returns 0.
///
Int_t REST_StringHelper::isANumber(string in) {
    return (in.find_first_not_of("-+0123456789.eE") == string::npos && in.length() != 0);
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
vector<string> REST_StringHelper::Split(string in, string separator, bool allowBlankString,
                                        bool removeWhiteSpaces, int startPos) {
    vector<string> result;

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
/// \brief Convert the input string into a  vector of double elements
///
/// e.g. Input: "1,2,3,4", Output: {1.,2.,3.,4.}
///
vector<double> REST_StringHelper::StringToElements(string in, string separator) {
    vector<double> result;
    vector<string> vec_str = REST_StringHelper::Split(in, separator);
    for (unsigned int i = 0; i < vec_str.size(); i++) {
        double temp = REST_StringHelper::StringToDouble(vec_str[i]);
        result.push_back(temp);
    }

    return result;
}

///////////////////////////////////////////////
/// \brief Convert the input string `in` into a vector of double elements
///
/// Called as `StringToElements( in, "[", ",", "]" );` will get the
/// elements from a string with the following format "[a,b,c]" where a,b,c
/// are double numbers.
///
vector<double> REST_StringHelper::StringToElements(string in, string headChar, string separator,
                                                   string tailChar) {
    vector<double> result;
    size_t startPos = in.find(headChar);
    size_t endPos = in.find(tailChar);
    if (startPos == string::npos || endPos == string::npos) {
        return result;
    }
    vector<string> values = Split(in.substr(startPos + 1, endPos - startPos - 1), ",");

    for (unsigned int i = 0; i < values.size(); i++) {
        double temp = REST_StringHelper::StringToDouble(values[i]);
        result.push_back(temp);
    }

    return result;
}

///////////////////////////////////////////////
/// \brief Returns the input string removing all white spaces.
///
string REST_StringHelper::RemoveWhiteSpaces(string in) {
    string out = in;
    size_t pos = 0;
    while ((pos = out.find(" ", pos)) != string::npos) {
        out.erase(pos, 1);
    }

    return out;
}

ULong64_t REST_StringHelper::ToHash(string str) {
    ULong64_t prime = 0x100000001B3ull;
    ULong64_t basis = 0xCBF29CE484222325ull;
    ULong64_t ret{basis};

    for (unsigned int i = 0; i < str.size(); i++) {
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

/// \brief This method matches a string with certain matcher. Returns true if matched.
/// Supports wildcard characters.
///
/// Wildcard character includes "*" and "?". "*" means to replace any number of any characters
/// "?" means to replace a single arbitary character.
///
/// e.g. (string, matcher)
/// "abcddd", "abc?d" --> not matched
/// "abcddd", "abc??d" --> matched
/// "abcddd", "abc*d" --> matched
///
/// Note that this method is in equal-match logic. It is not matching substrings. So:
/// "abcddd", "abcddd" --> matched
/// "abcddd", "a?c" --> not matched
///
/// Source code from
/// https://blog.csdn.net/dalao_whs/article/details/110477705
///
Bool_t REST_StringHelper::MatchString(string str, string matcher) {
    if (str.size() > 256 || matcher.size() > 256) {
        RESTError << "REST_StringHelper::MatchString(): string size too large" << RESTendl;
        return false;
    }

    vector<vector<bool>> dp(256, vector<bool>(256));
    int n2 = str.size();
    int n1 = matcher.size();
    dp[0][0] = true;
    int i = 0, j = 0;
    for (int i = 0; i < n1; i++) {
        if (matcher[i] != '*') {
            break;
        }
        if (i == n1 - 1 && matcher[i] == '*') {
            return true;
        }
    }
    for (i = 1; matcher.at(i - 1) == '*'; i++) {
        dp[i][0] = true;
    }
    for (i = 1; i <= n1; i++) {
        for (j = 1; j <= n2; j++) {
            if (matcher.at(i - 1) == '*' && (dp[i - 1][j - 1] || dp[i][j - 1] || dp[i - 1][j])) {
                dp[i][j] = true;
            } else if (matcher.at(i - 1) == '?' && dp[i - 1][j - 1]) {
                dp[i][j] = true;
            } else if (matcher.at(i - 1) == str.at(j - 1) && dp[i - 1][j - 1]) {
                dp[i][j] = true;
            }
        }
    }
    if (dp[n1][n2]) {
        return true;
    } else {
        return false;
    }

    return false;
}

/// \brief Returns the number of different characters between two strings
///
/// This algorithm is case insensitive. It matches the two strings in pieces
/// after inserting proper blanks, then counts the unmatched part(just a guess).
/// e.g.
/// "woll "
/// "world"
///  00101    --> 2
///
/// " torgae"
/// "Storage"
///  1000110  --> 3
///
/// "fi le"
/// "title"
///  10100    --> 2
///
/// Source code from
/// https://blog.csdn.net/baidu_23086307/article/details/53020566
///
Int_t REST_StringHelper::DiffString(const string& source, const string& target) {
    int n = source.length();
    int m = target.length();
    if (m == 0) return n;
    if (n == 0) return m;
    // Construct a matrix
    typedef vector<vector<int>> Tmatrix;
    Tmatrix matrix(n + 1);
    for (int i = 0; i <= n; i++) matrix[i].resize(m + 1);

    // step 2 Initialize

    for (int i = 1; i <= n; i++) matrix[i][0] = i;
    for (int i = 1; i <= m; i++) matrix[0][i] = i;

    // step 3
    for (int i = 1; i <= n; i++) {
        const char si = source[i - 1];
        // step 4
        for (int j = 1; j <= m; j++) {
            const char dj = target[j - 1];
            // step 5
            int cost;
            if (si == dj) {
                cost = 0;
            } else {
                cost = 1;
            }
            // step 6
            const int above = matrix[i - 1][j] + 1;
            const int left = matrix[i][j - 1] + 1;
            const int diag = matrix[i - 1][j - 1] + cost;
            matrix[i][j] = min(above, min(left, diag));
        }
    }  // step7
    return matrix[n][m];
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

string REST_StringHelper::EscapeSpecialLetters(string in) {
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

    std::string yearStr = IntegerToString(year);
    std::string monthStr = IntegerToString(month, "%02d");
    std::string dayStr = IntegerToString(day, "%02d");
    std::string hourStr = IntegerToString(hour, "%02d");
    std::string minuteStr = IntegerToString(minute, "%02d");
    std::string secondStr = IntegerToString(second, "%02d");

    std::string date =
        yearStr + "/" + monthStr + "/" + dayStr + " " + hourStr + ":" + minuteStr + ":" + secondStr;
    return date;
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

TRestStringOutput::REST_Verbose_Level REST_StringHelper::StringToVerboseLevel(string in) {
    if (ToUpper(in) == "SILENT" || in == "0") return TRestStringOutput::REST_Verbose_Level::REST_Silent;
    if (ToUpper(in) == "ESSENTIAL" || ToUpper(in) == "WARNING" || in == "1")
        return TRestStringOutput::REST_Verbose_Level::REST_Essential;
    if (ToUpper(in) == "INFO" || in == "2") return TRestStringOutput::REST_Verbose_Level::REST_Info;
    if (ToUpper(in) == "DEBUG" || in == "3") return TRestStringOutput::REST_Verbose_Level::REST_Debug;
    if (ToUpper(in) == "EXTREME" || in == "4") return TRestStringOutput::REST_Verbose_Level::REST_Extreme;

    return TRestStringOutput::REST_Verbose_Level::REST_Essential;
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
Int_t REST_StringHelper::StringToInteger(string in) {
    // If we find a hexadecimal number
    if (in.find("0x") != string::npos) return (Int_t)stoul(in, nullptr, 16);

    return (Int_t)StringToDouble(in);
}

///////////////////////////////////////////////
/// \brief Gets a string from an integer.
///
string REST_StringHelper::IntegerToString(Int_t n, string format) { return Form(format.c_str(), n); }

///////////////////////////////////////////////
/// \brief Gets a string from a double
///
string REST_StringHelper::DoubleToString(Double_t d, string format) { return Form(format.c_str(), d); }

Bool_t REST_StringHelper::StringToBool(string booleanString) {
    const auto booleanStringUpper = ToUpper(booleanString);
    return booleanStringUpper == "TRUE" || booleanStringUpper == "ON" ||
           booleanStringUpper == to_string(true);
}

Long64_t REST_StringHelper::StringToLong(string in) {
    if (in.find_first_of("eE") != string::npos) {
        // in case for scientific numbers
        return (Long64_t)StringToDouble(in);
    }
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
string REST_StringHelper::ToUpper(string s) {
    transform(s.begin(), s.end(), s.begin(), (int (*)(int))toupper);
    return s;
}

///////////////////////////////////////////////
/// \brief Convert the first character of a string to upper case.
///
string REST_StringHelper::FirstToUpper(string s) {
    if (s.length() == 0) return s;
    s[0] = *REST_StringHelper::ToUpper(string(&s[0], 1)).c_str();
    return s;
}

///////////////////////////////////////////////
/// \brief Convert string to its lower case. Alternative of TString::ToLower
///
string REST_StringHelper::ToLower(string s) {
    transform(s.begin(), s.end(), s.begin(), (int (*)(int))tolower);
    return s;
}

///////////////////////////////////////////////
/// \brief Removes all white spaces found at the end of the string
/// (https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring)
///
string REST_StringHelper::RightTrim(string s, const char* t) {
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

///////////////////////////////////////////////
/// \brief Removes all white spaces found at the beginning of the string
/// (https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring)
///
string REST_StringHelper::LeftTrim(string s, const char* t) {
    s.erase(0, s.find_first_not_of(t));
    return s;
}

///////////////////////////////////////////////
/// \brief Removes all white spaces found at the beginning and the end of the string
/// (https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring)
///
string REST_StringHelper::Trim(string s, const char* t) { return LeftTrim(RightTrim(s, t), t); }

///////////////////////////////////////////////
/// \brief It trims and uppers the string
///
string REST_StringHelper::TrimAndUpper(string s) {
    s = Trim(s);
    s = ToUpper(s);
    return s;
}

///////////////////////////////////////////////
/// \brief It trims and lowers the string
///
string REST_StringHelper::TrimAndLower(string s) {
    s = Trim(s);
    s = ToLower(s);
    return s;
}

///////////////////////////////////////////////
/// \brief Convert data member name to parameter name, following REST parameter naming convention.
///
/// > The name of class data member, if starts from f and have the second character in
/// capital form, will be linked to a parameter. The linked parameter will strip the first
/// f and have the first letter in lowercase. For example, data member fTargetName is
/// linked to parameter targetName.
string REST_StringHelper::DataMemberNameToParameterName(string name) {
    if (name == "") {
        return "";
    }
    if (name[0] == 'f' && name.size() > 1 && (name[1] >= 65 && name[1] <= 90)) {
        return string(1, tolower(name[1])) + name.substr(2, -1);
    } else {
        return "";
    }
}

///////////////////////////////////////////////
/// \brief Convert parameter name to datamember name, following REST parameter naming convention.
///
/// > The name of class data member, if starts from f and have the second character in
/// capital form, will be linked to a parameter. The linked parameter will strip the first
/// f and have the first letter in lowercase. For example, data member fTargetName is
/// linked to parameter targetName.
string REST_StringHelper::ParameterNameToDataMemberName(string name) {
    if (name == "") {
        return "";
    }
    if (islower(name[0])) {
        return "f" + string(1, toupper(name[0])) + name.substr(1, -1);
    } else {
        return "";
    }
}

/////////////////////////////////////////////
/// \brief Reads a function with parameter options from string and returns it as TF1*.
///
/// > The function is defined as a string following ROOT::TFormula conventions for parameters.
/// Inside the square brackets we allow parameter initialization, constant parameter and ranges.
/// This has been created for fitting functions, where each parameter has its own restrictions.
///
/// Examples:
/// -- Initial value: [0=3.5]
/// -- Fixed value: [0==3.5]
/// -- Range: [0=3.5(1,5)] The parameter 0 begin at 3.5 and it can move between 1 and 5.
///
/// All parameters should be initialized.
///
/// Input arguments:
/// 	* String with function and parameter options.
///     * Two doubles with the range of the function.
///
/// Output: TF1* with the interpreted fuction. It contains all restrictions, ranges, etc.
///
/// \warning This object will create a new TF1 object in memory, so the code creating a
/// TF1 object using this method as a helper will be responsible to delete this object from
/// memory.
///
/// \code
/// TF1* ff = CreateTF1FromString(xxx);
/// ...
/// ff->DoSomething();
/// ...
/// delete ff;
/// \endcode
///
TF1* REST_StringHelper::CreateTF1FromString(string func, double init, double end) {
    string tf1 = func;
    // Number of parameters
    size_t n = count(func.begin(), func.end(), '[');
    // Reading options
    int a = 0;
    vector<int> optPos(n);
    vector<string> options(n);  // Vector of strings of any size.
    for (unsigned int i = 0; i < n; i++) {
        optPos[i] = func.find("[", a);
        options[i] =
            func.substr(func.find("[", a) + 1, func.find("]", func.find("[", a)) - func.find("[", a) - 1);
        a = func.find("[", a) + 1;
    }
    // Removing options from function string
    for (unsigned int i = 0; i < n; i++) {
        tf1.replace(optPos[n - 1 - i] + 1, (func.find("]", optPos[n - 1 - i]) - optPos[n - 1 - i] - 1),
                    string(1, func[optPos[n - 1 - i] + 1]));
    }

    // Function
    const char* tf1c = tf1.c_str();
    TF1* f = new TF1("f", tf1c, init, end);

    // Initial conditions
    for (unsigned int i = 0; i < n; i++) {
        if (options[i].find("=") != string::npos) {
            string op = options[i].substr(options[i].find_last_of("=") + 1,
                                          options[i].find("(") - options[i].find_last_of("=") - 1);
            if (isANumber(op)) {
                f->SetParameter(i, stod(op));
            } else {
                cout << "Initial condition for parameter " << i << " is not a number: " << op << endl;
            }
        } else {
            cout << "No initial condition given for parameter " << i << "!" << endl;
        }
    }

    // Fixed values
    for (unsigned int i = 0; i < n; i++) {
        if (count(options[i].begin(), options[i].end(), '=') == 2) {
            string op = options[i].substr(options[i].find_last_of("=") + 1,
                                          options[i].find("(") - options[i].find_last_of("=") - 1);
            if (isANumber(op)) {
                f->FixParameter(i, stod(op));
            }
            // cout << "Parameter " << i << " fixed with value " << op << endl;
        }
    }

    // Ranges
    for (unsigned int i = 0; i < n; i++) {
        if (options[i].find("(") != string::npos) {
            string op =
                options[i].substr(options[i].find("(") + 1, options[i].find(")") - options[i].find("(") - 1);
            f->SetParLimits(i, stod(op.substr(0, op.find(","))),
                            stod(op.substr(op.find(",") + 1, op.size() - 1 - op.find(","))));
            // cout << "Parameter " << i << " range " << "(" << stod(op.substr(0, op.find(","))) << "," <<
            // stod(op.substr(op.find(",")+1, op.size()-1-op.find(",") )) << ")" << endl;
        }
    }

    return f;
}
