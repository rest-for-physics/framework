
#ifndef RestTools_REST_StringHelper
#define RestTools_REST_StringHelper
#pragma once

#include <TF1.h>
#include <TFormula.h>
#include <TVector2.h>
#include <TVector3.h>
#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>

#include "TRestStringOutput.h"

/// String helper classes. Declared static to be able to have direct access to the methods
namespace REST_StringHelper {

Int_t GetChar(std::string hint = "Press a KEY to continue ...");
Int_t isANumber(std::string in);
Int_t isAExpression(const std::string& in);
std::string CropWithPrecision(std::string in, Int_t precision);
std::string ReplaceMathematicalExpressions(std::string buffer, Int_t precision = 0,
                                           std::string errorMessage = "");
std::string EvaluateExpression(std::string exp);
Float_t StringToFloat(std::string in);
Double_t StringToDouble(std::string in);
Int_t StringToInteger(std::string in);
std::string IntegerToString(Int_t n, std::string format = "%d");
std::string DoubleToString(Double_t d, std::string format = "%4.2lf");
Bool_t StringToBool(std::string booleanString);
Long64_t StringToLong(std::string in);
TVector3 StringTo3DVector(std::string in);
TVector2 StringTo2DVector(std::string in);
std::vector<std::string> Split(std::string in, std::string separator, bool allowBlankString = false,
                               bool removeWhiteSpaces = false, int startPos = -1);
std::vector<double> StringToElements(std::string in, std::string separator);
std::vector<double> StringToElements(std::string in, std::string headChar, std::string separator,
                                     std::string tailChar);
std::string RemoveDelimiters(std::string in);
std::string RemoveWhiteSpaces(std::string in);
std::string Replace(std::string in, std::string thisString, std::string byThisString, size_t fromPosition = 0,
                    Int_t N = 0);
std::string EscapeSpecialLetters(std::string in);
std::string ToDateTimeString(time_t time);
time_t StringToTimeStamp(std::string time);
TRestStringOutput::REST_Verbose_Level StringToVerboseLevel(std::string in);
ULong64_t ToHash(std::string str);
constexpr ULong64_t ToHash(const char* str, ULong64_t last_value = 0xCBF29CE484222325ull) {
    return *str ? ToHash(str + 1, (*str ^ last_value) * 0x100000001B3ull) : last_value;
}
Int_t Count(std::string s, std::string sbstring);
Int_t FindNthStringPosition(const std::string& in, size_t pos, const std::string& strToFind, size_t nth);
Bool_t MatchString(std::string str, std::string matcher);
Int_t DiffString(const std::string& source, const std::string& target);
template <class T>
std::string ToString(T source, int length = -1, char fill = ' ') {
    std::stringstream ss1;
    ss1 << source;
    std::string s = ss1.str();
    if (length < 0) {
        return s;
    } else if (s.size() < (unsigned int)length) {
        return s + std::string(length - s.size(), fill);
    } else {
        return s.substr(0, length);
    }
}

template <class T>
std::string StringWithPrecision(T& value, int precision) {
    std::stringstream ss;
    ss.precision(precision);
    ss << value;

    return ss.str();
}

template <class T1, class T2>
inline std::vector<T2> Vector_cast(std::vector<T1> vecstring) {
    std::vector<T2> result;
    for (T1 s : vecstring) {
        result.push_back((T2)s);
    }
    return result;
}

std::string ToUpper(std::string in);
std::string ToLower(std::string in);

std::string FirstToUpper(std::string s);

std::string RightTrim(std::string s, const char* t = " \t\n\r\f\v");
std::string LeftTrim(std::string s, const char* t = " \t\n\r\f\v");
std::string Trim(std::string s, const char* t = " \t\n\r\f\v");
std::string TrimAndUpper(std::string s);
std::string TrimAndLower(std::string s);

std::string DataMemberNameToParameterName(std::string name);
std::string ParameterNameToDataMemberName(std::string name);

TF1* CreateTF1FromString(std::string func, double init, double end);

};  // namespace REST_StringHelper
using namespace REST_StringHelper;

#ifdef WIN32
#include <process.h>
inline void setenv(const char* __name, const char* __value, int __replace) {
    _putenv(((std::string)__name + "=" + (std::string)__value).c_str());
}
inline int getpid() { return _getpid(); }
inline int fileno(FILE* fp) { return _fileno(fp); }

inline void usleep(int usec) {
    if (usec >= 1000) {
        _sleep(usec / 1000);
    } else {
        _sleep(1);  // sleep minimum 1ms on windows
    }
}
inline void sleep(int sec) { _sleep(1000 * sec); }
#define __PRETTY_FUNCTION__ __FUNCTION__
#define M_PI 3.14159265358979323846
#endif

#endif
