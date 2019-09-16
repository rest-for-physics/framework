
#ifndef RestTools_REST_StringHelper
#define RestTools_REST_StringHelper
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

using namespace std;

/// String helper classes. Declared static to be able to have direct access to the methods
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
std::vector<string> Split(std::string in, string separator, bool allowblank = false);
std::string RemoveWhiteSpaces(std::string in);
std::string Replace(std::string in, std::string thisString, std::string byThisString, size_t fromPosition = 0,
                    Int_t N = 0);
std::string EscapeSpecialLetters(string in);
string ToDateTimeString(time_t time);
time_t ToTime(string time);
Int_t Count(std::string s, std::string sbstring);
Int_t FindNthStringPosition(const string& in, size_t pos, const string& strToFind, size_t nth);
template <class T>
string ToString(T source, int length = -1, char fill = ' ') {
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
inline vector<TString> VectorTString_cast(vector<string> vecstring) {
	vector<TString> result;
	for (auto s : vecstring) {
		result.push_back((TString)s);
	}
	return result;
}
inline vector<string> Vectorstring_cast(vector<TString> vecstring) {
	vector<string> result;
	for (auto s : vecstring) {
		result.push_back((string)s);
	}
	return result;
}
std::string ToUpper(std::string in);

};  // namespace REST_StringHelper
using namespace REST_StringHelper;



#ifdef WIN32
inline void setenv(const char* __name, const char* __value, int __replace);
inline string get_current_dir_name();
#define __PRETTY_FUNCTION__ __FUNCTION__
#define M_PI 3.14159265358979323846
#endif

#endif
