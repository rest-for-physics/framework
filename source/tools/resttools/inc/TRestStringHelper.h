
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

#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
/// String helper classes. Declared static to be able to access them without having to instantiate TRestMetadata.

using namespace std;



class TRestStringHelper
{
public:
	TRestStringHelper();
	~TRestStringHelper();

	static Int_t isANumber(std::string in);
	static Int_t isAExpression(std::string in);
	static Double_t StringToDouble(std::string in);
	static Int_t StringToInteger(std::string in);
	static  TVector3 StringTo3DVector(std::string in);
	static  TVector2 StringTo2DVector(std::string in);
	static std::vector<string> Spilt(std::string in, string separator);
	static std::string RemoveWhiteSpaces(std::string in);
	static  std::string Replace(std::string in, std::string thisString, std::string byThisString, size_t fromPosition);
	static string ToDateTimeString(time_t time);
	//template<class T> static string ToString(T source);
	static Int_t Count(std::string s, std::string sbstring);
	static bool fileExists(const std::string& filename);
	static  bool isRootFile(const std::string& filename);
	static bool isPathWritable(const std::string& path);
	static  bool isAbsolutePath(const std::string& path);
	static  std::string typeidToClassName(std::string typeidstr);
	static  Int_t ChecktheFile(std::string cfgFileName);
	static std::vector <TString> GetFilesMatchingPattern(TString pattern);
	static std::string ToUpper(std::string in);

};

inline Int_t isANumber(std::string in) { return TRestStringHelper::isANumber(in); }
inline Int_t isAExpression(std::string in) { return TRestStringHelper::isAExpression(in); }
inline Double_t StringToDouble(std::string in) { return TRestStringHelper::StringToDouble(in); }
inline Int_t StringToInteger(std::string in) { return TRestStringHelper::StringToInteger(in); }
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
inline std::string typeidToClassName(std::string typeidstr) { return TRestStringHelper::typeidToClassName(typeidstr); }
inline Int_t ChecktheFile(std::string cfgFileName) { return TRestStringHelper::ChecktheFile(cfgFileName); }
inline std::vector <TString> GetFilesMatchingPattern(TString pattern) { return TRestStringHelper::GetFilesMatchingPattern(pattern); }
inline std::string ToUpper(std::string in) { return TRestStringHelper::ToUpper(in); }




#endif

