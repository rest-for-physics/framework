
#ifndef RestTools_REST_StringHelper
#define RestTools_REST_StringHelper
#pragma once

#include <TF1.h>
#include <TFormula.h>
#include <TVector2.h>
#include <TVector3.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>

#include "TRestStringOutput.h"

using namespace std;

/// String helper classes. Declared static to be able to have direct access to the methods
namespace REST_StringHelper {

Int_t GetChar(string hint = "Press a KEY to continue ...");
Int_t isANumber(std::string in);
Int_t isAExpression(std::string in);
std::string ReplaceMathematicalExpressions(std::string buffer, std::string errorMessage = "");
std::string EvaluateExpression(std::string exp);
Float_t StringToFloat(std::string in);
Double_t StringToDouble(std::string in);
Int_t StringToInteger(std::string in);
std::string IntegerToString(Int_t n);
std::string DoubleToString(Double_t d);
Bool_t StringToBool(std::string in);
Long64_t StringToLong(std::string in);
TVector3 StringTo3DVector(std::string in);
TVector2 StringTo2DVector(std::string in);
std::vector<string> Split(std::string in, string separator, bool allowBlankString = false,
                          bool removeWhiteSpaces = false, int startPos = -1);
std::vector<double> StringToElements(std::string in, string separator);
std::vector<double> StringToElements(std::string in, string headChar, string separator, string tailChar);
std::string RemoveWhiteSpaces(std::string in);
std::string Replace(std::string in, std::string thisString, std::string byThisString, size_t fromPosition = 0,
                    Int_t N = 0);
std::string EscapeSpecialLetters(string in);
string ToDateTimeString(time_t time);
time_t StringToTimeStamp(string time);
REST_Verbose_Level StringToVerboseLevel(string in);
ULong64_t ToHash(string str);
constexpr ULong64_t ToHash(const char* str, ULong64_t last_value = 0xCBF29CE484222325ull) {
    return *str ? ToHash(str + 1, (*str ^ last_value) * 0x100000001B3ull) : last_value;
}
Int_t Count(std::string s, std::string sbstring);
Int_t FindNthStringPosition(const string& in, size_t pos, const string& strToFind, size_t nth);
Int_t DiffString(const string& source, const string& target);
template <class T>
string ToString(T source, int length = -1, char fill = ' ') {
    stringstream ss1;
    ss1 << source;
    string s = ss1.str();
    if (length < 0) {
        return s;
    } else if (s.size() < length) {
        return s + string(length - s.size(), fill);
    } else {
        return s.substr(0, length);
    }
}
template <class T1, class T2>
inline vector<T2> Vector_cast(vector<T1> vecstring) {
    vector<T2> result;
    for (T1 s : vecstring) {
        result.push_back((T2)s);
    }
    return result;
}

std::string ToUpper(std::string in);
std::string ToLower(std::string in);

std::string RightTrim(std::string s, const char* t = " \t\n\r\f\v");
std::string LeftTrim(std::string s, const char* t = " \t\n\r\f\v");
std::string Trim(std::string s, const char* t = " \t\n\r\f\v");
std::string TrimAndUpper(std::string s);
std::string TrimAndLower(std::string s);

string DataMemberNameToParameterName(string name);
string ParameterNameToDataMemberName(string name);

TF1* CreateTF1FromString(std::string func, double init, double end);

};  // namespace REST_StringHelper
using namespace REST_StringHelper;

#ifdef WIN32
inline void setenv(const char* __name, const char* __value, int __replace);
inline string get_current_dir_name();
#define __PRETTY_FUNCTION__ __FUNCTION__
#define M_PI 3.14159265358979323846
#endif

#endif
