
#ifndef RestCore_TRestStringHelper
#define RestCore_TRestStringHelper
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream> 
#include <algorithm> 
#include <sys/stat.h>
#include <iomanip>

#include <TFormula.h>
#include <TVector2.h>
#include <TVector3.h>

/// String helper classes. Declared static to be able to access them without having to instantiate TRestMetadata.

using namespace std;



class TRestStringHelper
{
public:
	TRestStringHelper();
	~TRestStringHelper();

	static Int_t isANumber(std::string in);
	static Int_t isAExpression(std::string in);
	static std::string ReplaceMathematicalExpressions(std::string buffer);
	static std::string EvaluateExpression(std::string exp);
	static Double_t StringToDouble(std::string in);
	static Int_t StringToInteger(std::string in);
	static Bool_t StringToBool(std::string in);
	static Long64_t StringToLong(std::string in);
	static  TVector3 StringTo3DVector(std::string in);
	static  TVector2 StringTo2DVector(std::string in);
	static std::vector<string> Spilt(std::string in, string separator);
	static std::string RemoveWhiteSpaces(std::string in);
	static  std::string Replace(std::string in, std::string thisString, std::string byThisString, size_t fromPosition);
	static string ToDateTimeString(time_t time);
	static time_t ToTime(string time);
	//template<class T> static string ToString(T source);
	static Int_t Count(std::string s, std::string sbstring);
	static bool fileExists(const std::string& filename);
	static  bool isRootFile(const std::string& filename);
	static bool isPathWritable(const std::string& path);
	static  bool isAbsolutePath(const std::string& path);
	static string ToAbsoluteName(string filename);
	static std::pair<string,string> SeparatePathAndName(const std::string fullname);
	static std::string SearchFileInPath(vector<string> path, string filename);
	static  Int_t ChecktheFile(std::string cfgFileName);
	static std::vector <string> GetFilesMatchingPattern(string pattern);
	static std::string ToUpper(std::string in);
	static std::string ExecuteShellCommand(string cmd);


};

inline Int_t isANumber(std::string in) { return TRestStringHelper::isANumber(in); }
inline Int_t isAExpression(std::string in) { return TRestStringHelper::isAExpression(in); }
inline std::string ReplaceMathematicalExpressions(std::string buffer) { return TRestStringHelper::ReplaceMathematicalExpressions(buffer); }
inline std::string EvaluateExpression(std::string exp) { return TRestStringHelper::EvaluateExpression(exp); }
inline Double_t StringToDouble(std::string in) { return TRestStringHelper::StringToDouble(in); }
inline Int_t StringToInteger(std::string in) { return TRestStringHelper::StringToInteger(in); }
inline Bool_t StringToBool(std::string in) { return TRestStringHelper::StringToBool(in); }
inline Long64_t StringToLong(std::string in) { return TRestStringHelper::StringToLong(in); }
inline TVector3 StringTo3DVector(std::string in) { return TRestStringHelper::StringTo3DVector(in); }
inline TVector2 StringTo2DVector(std::string in) { return TRestStringHelper::StringTo2DVector(in); }
inline std::string RemoveWhiteSpaces(std::string in) { return TRestStringHelper::RemoveWhiteSpaces(in); }
inline std::string Replace(std::string in, std::string thisString, std::string byThisString, size_t fromPosition)
{
	return TRestStringHelper::Replace(in, thisString, byThisString, fromPosition);
}
inline vector<string> Spilt(std::string in, string separator) { return TRestStringHelper::Spilt(in, separator); }
inline Int_t Count(std::string s, std::string sbstring) { return TRestStringHelper::Count(s, sbstring); }
inline string ToDateTimeString(time_t time) { return TRestStringHelper::ToDateTimeString(time); }
inline time_t ToTime(string time) { return TRestStringHelper::ToTime(time); }
template<class T> inline string ToString(T source) 
{
	stringstream s;
	s << source;
	return s.str();
}
inline bool fileExists(const std::string& filename) { return TRestStringHelper::fileExists(filename); }
inline bool isRootFile(const std::string& filename) { return TRestStringHelper::isRootFile(filename); }
inline bool isPathWritable(const std::string& path) { return TRestStringHelper::isPathWritable(path); }
inline bool isAbsolutePath(const std::string& path) { return TRestStringHelper::isAbsolutePath(path); }
inline std::pair<string, string> SeparatePathAndName(const std::string fullname) { return TRestStringHelper::SeparatePathAndName(fullname); }
inline string ToAbsoluteName(string filename) { return TRestStringHelper::ToAbsoluteName(filename); }
inline std::string SearchFileInPath(vector<string> path, string filename) { return TRestStringHelper::SearchFileInPath(path, filename); }
inline std::string SearchFileInPath(string paths, string filename) { return TRestStringHelper::SearchFileInPath(TRestStringHelper::Spilt(paths,":"), filename); }
inline Int_t ChecktheFile(std::string cfgFileName) { return TRestStringHelper::ChecktheFile(cfgFileName); }
inline std::vector <string> GetFilesMatchingPattern(string pattern) { return TRestStringHelper::GetFilesMatchingPattern(pattern); }
inline std::vector <TString> GetFilesMatchingPattern(TString pattern) { 
	vector<TString> result;
	auto a = TRestStringHelper::GetFilesMatchingPattern((string)pattern); 
	for (auto b : a)
		result.push_back((TString)b);
	return result;
}
inline std::string ToUpper(std::string in) { return TRestStringHelper::ToUpper(in); }
inline std::string ExecuteShellCommand(string cmd) {return TRestStringHelper::ExecuteShellCommand(cmd); }

#ifdef WIN32
inline void setenv(const char *__name, const char *__value, int __replace);
inline string get_current_dir_name();
#define __PRETTY_FUNCTION__ __FUNCTION__
#define M_PI 3.14159265358979323846
#endif


#endif

