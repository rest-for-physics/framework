
#ifndef RestCore_REST_StringHelper
#define RestCore_REST_StringHelper
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <TFormula.h>
#include <TVector2.h>
#include <TVector3.h>

/// String helper classes. Declared static to be able to access them without
/// having to instantiate TRestMetadata.

using namespace std;

namespace REST_StringHelper {

Int_t isANumber(std::string in);
Int_t isAExpression(std::string in);
std::string ReplaceMathematicalExpressions(std::string buffer);
std::string EvaluateExpression(std::string exp);
Double_t StringToDouble(std::string in);
Int_t StringToInteger(std::string in);
Bool_t StringToBool(std::string in);
Long64_t StringToLong(std::string in);
TVector3 StringTo3DVector(std::string in);
TVector2 StringTo2DVector(std::string in);
std::vector<string> Spilt(std::string in, string separator);
std::string RemoveWhiteSpaces(std::string in);
std::string Replace(std::string in, std::string thisString, std::string byThisString, size_t fromPosition = 0,
                    Int_t N = 0);
std::string EscapeSpecialLetters(string in);
string ToDateTimeString(time_t time);
time_t ToTime(string time);
// template<class T> string ToString(T source);
Int_t Count(std::string s, std::string sbstring);
Int_t FindNthStringPosition(const string& in, size_t pos, const string& strToFind, size_t nth);
bool fileExists(const std::string& filename);
bool isRootFile(const std::string& filename);
bool isURL(const std::string& filename);
bool isPathWritable(const std::string& path);
bool isAbsolutePath(const std::string& path);
string ToAbsoluteName(string filename);
vector<string> GetSubdirectories(const string& path, int recursion = -1);
std::pair<string, string> SeparatePathAndName(const std::string fullname);
std::string RemoveAbsolutePath(std::string fullpathFileName);
std::string SearchFileInPath(vector<string> path, string filename);
Int_t ChecktheFile(std::string cfgFileName);
std::vector<string> GetFilesMatchingPattern(string pattern);
std::string ToUpper(std::string in);
std::string ExecuteShellCommand(string cmd);
int ConvertVersionCode(string in);

};  // namespace REST_StringHelper

inline vector<TString> VectorTString_cast(vector<string> vecstring) {
    vector<TString> result;
    for (auto s : vecstring) {
        result.push_back((TString)s);
    }
    return result;
}
inline Int_t isANumber(std::string in) { return REST_StringHelper::isANumber(in); }
inline Int_t isAExpression(std::string in) { return REST_StringHelper::isAExpression(in); }
inline std::string ReplaceMathematicalExpressions(std::string buffer) {
    return REST_StringHelper::ReplaceMathematicalExpressions(buffer);
}
inline std::string EvaluateExpression(std::string exp) { return REST_StringHelper::EvaluateExpression(exp); }
inline Double_t StringToDouble(std::string in) { return REST_StringHelper::StringToDouble(in); }
inline Int_t StringToInteger(std::string in) { return REST_StringHelper::StringToInteger(in); }
inline Bool_t StringToBool(std::string in) { return REST_StringHelper::StringToBool(in); }
inline Long64_t StringToLong(std::string in) { return REST_StringHelper::StringToLong(in); }
inline TVector3 StringTo3DVector(std::string in) { return REST_StringHelper::StringTo3DVector(in); }
inline TVector2 StringTo2DVector(std::string in) { return REST_StringHelper::StringTo2DVector(in); }
inline std::string RemoveWhiteSpaces(std::string in) { return REST_StringHelper::RemoveWhiteSpaces(in); }
inline std::string Replace(std::string in, std::string thisString, std::string byThisString,
                           size_t fromPosition = 0, Int_t N = 0) {
    return REST_StringHelper::Replace(in, thisString, byThisString, fromPosition, N);
}
inline std::string EscapeSpecialLetters(string in) { return REST_StringHelper::EscapeSpecialLetters(in); }
inline vector<string> Spilt(std::string in, string separator) {
    return REST_StringHelper::Spilt(in, separator);
}
inline Int_t Count(std::string s, std::string sbstring) { return REST_StringHelper::Count(s, sbstring); }
inline Int_t FindNthStringPosition(const string& in, size_t pos, const string& strToFind, size_t nth) {
    return REST_StringHelper::FindNthStringPosition(in, pos, strToFind, nth);
}
inline string ToDateTimeString(time_t time) { return REST_StringHelper::ToDateTimeString(time); }
inline time_t ToTime(string time) { return REST_StringHelper::ToTime(time); }
template <class T>
inline string ToString(T source, int length = -1, char fill = ' ') {
    stringstream ss1;
    ss1 << source;
    string s = ss1.str();
    if (length == -1) {
        return s;
    } else if (s.size() < length) {
        return s + string(length - s.size(), fill);
    } else {
        return s.substr(0, length);
    }
}
inline bool fileExists(const std::string& filename) { return REST_StringHelper::fileExists(filename); }
inline bool isRootFile(const std::string& filename) { return REST_StringHelper::isRootFile(filename); }
inline bool isPathWritable(const std::string& path) { return REST_StringHelper::isPathWritable(path); }
inline bool isAbsolutePath(const std::string& path) { return REST_StringHelper::isAbsolutePath(path); }
inline vector<string> GetSubdirectories(const string& path, int recursion = -1) {
    return REST_StringHelper::GetSubdirectories(path, recursion);
}
inline std::pair<string, string> SeparatePathAndName(const std::string fullname) {
    return REST_StringHelper::SeparatePathAndName(fullname);
}
inline string ToAbsoluteName(string filename) { return REST_StringHelper::ToAbsoluteName(filename); }
inline std::string SearchFileInPath(vector<string> path, string filename) {
    return REST_StringHelper::SearchFileInPath(path, filename);
}
inline std::string SearchFileInPath(string paths, string filename) {
    return REST_StringHelper::SearchFileInPath(REST_StringHelper::Spilt(paths, ":"), filename);
}
inline Int_t ChecktheFile(std::string cfgFileName) { return REST_StringHelper::ChecktheFile(cfgFileName); }
inline std::vector<string> GetFilesMatchingPattern(string pattern) {
    return REST_StringHelper::GetFilesMatchingPattern(pattern);
}
inline std::vector<TString> GetFilesMatchingPattern(TString pattern) {
    return VectorTString_cast(REST_StringHelper::GetFilesMatchingPattern((string)pattern));
}
inline std::string ToUpper(std::string in) { return REST_StringHelper::ToUpper(in); }
inline std::string ExecuteShellCommand(string cmd) { return REST_StringHelper::ExecuteShellCommand(cmd); }
inline int ConvertVersionCode(string in) { return REST_StringHelper::ConvertVersionCode(in); }

#ifdef WIN32
inline void setenv(const char* __name, const char* __value, int __replace);
inline string get_current_dir_name();
#define __PRETTY_FUNCTION__ __FUNCTION__
#define M_PI 3.14159265358979323846
#endif

#endif
