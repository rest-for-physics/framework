#include "TRestStringHelper.h"
#include "v5/TFormula.h"

using namespace std;
TRestStringHelper::TRestStringHelper()
{

}


TRestStringHelper::~TRestStringHelper()
{
}


///////////////////////////////////////////////
/// \brief Returns 1 only if valid mathematical expression keywords (or numbers) are found in the string **in**. If not it returns 0.
///
Int_t TRestStringHelper::isAExpression(string in)
{
	string temp = in;
	vector<string> replace{ "sqrt","log","exp","gaus","cos","sin","tan","atan","acos","asin" };
	for (int i = 0; i < replace.size(); i++)
	{
		temp = Replace(temp, replace[i], "0", 0);
	}


	if (temp.length() != 0)
	{
		if (temp.find_first_not_of("-0123456789e+*/.,)( ^") == std::string::npos)
		{
			if (temp.find("/") == 0 || temp.find("./") == 0 || temp.find("../") == 0) return 0;//identify path 
			return 1;
		}
	}

	return 0;
}

///////////////////////////////////////////////
/// \brief Evaluates and replaces valid mathematical expressions found in the input string **buffer**.
///
std::string TRestStringHelper::ReplaceMathematicalExpressions(std::string buffer) {

	//we spilt the unit part and the expresstion part
	int pos = buffer.find_last_of("1234567890().");

	string unit = buffer.substr(pos + 1, -1);
	string temp = buffer.substr(0, pos + 1);
	string result = "";

	bool erased = false;

	std::vector<std::string> Expressions=Spilt(temp,",");

	if (Expressions.size() > 1 && Expressions[0][0] == '(' && Expressions[Expressions.size()-1][Expressions[Expressions.size() - 1].size()-1] == ')') {
		Expressions[0].erase(0, 1);
		Expressions[Expressions.size() - 1].erase(Expressions[Expressions.size() - 1].size() - 1, 1);
		erased = true;
	}

	for (int i = 0; i < Expressions.size(); i++)
	{
		if (!isAExpression(Expressions[i])) { result += Expressions[i] + ","; continue; }
		result += EvaluateExpression(Expressions[i]) + ",";
	}
	if(Expressions.size()>0)
		result.erase(result.size() - 1, 1);

	if (erased)
	{
		result = "(" + result + ")";
	}

	return result + unit;

}

///////////////////////////////////////////////
/// \brief Evaluates a complex numerical expression and returns the resulting value using TFormula.
///
std::string TRestStringHelper::EvaluateExpression(std::string exp) {
	if (!isAExpression(exp)) { return exp; }

	//NOTE!!! In root6 the expression like "1/2" will be computed using the input as int number,
	//which will return 0, and cause problem.
	//we roll back to TFormula of version 5
	ROOT::v5::TFormula formula("tmp", exp.c_str());

	ostringstream sss;
	Double_t number = formula.EvalPar(0);
	if (number > 0 && number < 1.e-300)
	{
		cout << "REST Warning! Expression not recognized --> " << exp << endl;  

	}

	sss << number;
	string out = sss.str();

	return out;
}


///////////////////////////////////////////////
/// \brief Returns 1 only if a valid number is found in the string **in**. If not it returns 0.
///
Int_t TRestStringHelper::isANumber(string in)
{
	return (in.find_first_not_of("-+0123456789.e") == std::string::npos && in.length() != 0);
}

///////////////////////////////////////////////
/// \brief Spilt the input string according to the given separator. Returning a vector of fragments
///
std::vector<string> TRestStringHelper::Spilt(std::string in, string separator)
{
	std::vector<string> result;

	int pos = -1;
	int front = 0;
	while (1)
	{
		pos = in.find(separator.c_str(), pos + 1);
		string sub = in.substr(front, pos - front);
		if (sub != "") {
			result.push_back(sub);
		}
		front = pos + separator.size();
		if (pos == -1)break;
	}

	return result;

}



///////////////////////////////////////////////
/// \brief Returns the input string removing white spaces.
///
string TRestStringHelper::RemoveWhiteSpaces(string in)
{
	string out = in;
	size_t pos = 0;
	while ((pos = out.find(" ", pos)) != string::npos)
	{
		out.erase(pos, 1);
		pos = pos + 1;
	}

	return out;
}

///////////////////////////////////////////////
/// \brief Counts the number of occurences of **substring** inside the input string **in**. 
///
Int_t TRestStringHelper::Count(string in, string substring)
{
	int count = 0;
	size_t nPos = in.find(substring, 0); // First occurrence
	while (nPos != string::npos)
	{
		count++;
		nPos = in.find(substring, nPos + 1);
	}

	return count;
}

///////////////////////////////////////////////
/// \brief Replace every occurences of **thisSring** by **byThisString** inside string **in**.
///
string TRestStringHelper::Replace(string in, string thisString, string byThisString, size_t fromPosition = 0)
{
	string out = in;
	size_t pos = fromPosition;
	while ((pos = out.find(thisString, pos)) != string::npos)
	{
		out.replace(pos, thisString.length(), byThisString);
		pos = pos + 1;
	}

	return out;
}

string TRestStringHelper::ToDateTimeString(time_t time)
{
	tm *tm_ = localtime(&time);                // 将time_t格式转换为tm结构体
	int year, month, day, hour, minute, second;// 定义时间的各个int临时变量。
	year = tm_->tm_year + 1900;                // 临时变量，年，由于tm结构体存储的是从1900年开始的时间，所以临时变量int为tm_year加上1900。
	month = tm_->tm_mon + 1;                   // 临时变量，月，由于tm结构体的月份存储范围为0-11，所以临时变量int为tm_mon加上1。
	day = tm_->tm_mday;                        // 临时变量，日。
	hour = tm_->tm_hour;                       // 临时变量，时。
	minute = tm_->tm_min;                      // 临时变量，分。
	second = tm_->tm_sec;                      // 临时变量，秒。
	char yearStr[5], monthStr[3], dayStr[3], hourStr[3], minuteStr[3], secondStr[3];// 定义时间的各个char*变量。
	sprintf(yearStr, "%d", year);              // 年。
	sprintf(monthStr, "%d", month);            // 月。
	sprintf(dayStr, "%d", day);                // 日。
	sprintf(hourStr, "%d", hour);              // 时。
	sprintf(minuteStr, "%d", minute);          // 分。
	if (minuteStr[1] == '\0')                  // 如果分为一位，如5，则需要转换字符串为两位，如05。
	{
		minuteStr[2] = '\0';
		minuteStr[1] = minuteStr[0];
		minuteStr[0] = '0';
	}
	sprintf(secondStr, "%d", second);          // 秒。
	if (secondStr[1] == '\0')                  // 如果秒为一位，如5，则需要转换字符串为两位，如05。
	{
		secondStr[2] = '\0';
		secondStr[1] = secondStr[0];
		secondStr[0] = '0';
	}
	char s[20];                                // 定义总日期时间char*变量。
	sprintf(s, "%s-%s-%s %s:%s:%s", yearStr, monthStr, dayStr, hourStr, minuteStr, secondStr);// 将年月日时分秒合并。
	string str(s);                             // 定义string变量，并将总日期时间char*变量作为构造函数的参数传入。
	return str;                                // 返回转换日期时间后的string变量。
}


///////////////////////////////////////////////
/// \brief Gets a double from a string.
///
Double_t TRestStringHelper::StringToDouble(string in)
{
	if (isANumber(in))
	{
		return stod(in);
	}
	else
	{
		return -1;
	}
}


///////////////////////////////////////////////
/// \brief Gets an integer from a string.
///
Int_t TRestStringHelper::StringToInteger(string in)
{
	return (Int_t)StringToDouble(in);
}


///////////////////////////////////////////////
/// \brief Gets a 3D-vector from a string. Format should be : (X,Y,Z).
///
TVector3 TRestStringHelper::StringTo3DVector(string in)
{
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
TVector2 TRestStringHelper::StringTo2DVector(string in)
{
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
/// \brief Returns true if the filename exists.
///
bool TRestStringHelper::fileExists(const std::string& filename)
{
	struct stat buf;
	if (stat(filename.c_str(), &buf) != -1)
	{
		return true;
	}
	return false;
}

///////////////////////////////////////////////
/// \brief Returns true if the **filename** has *.root* extension.
///
bool TRestStringHelper::isRootFile(const std::string& filename)
{
	if (filename.find(".root") == string::npos) return false;

	return true;
}

///////////////////////////////////////////////
/// \brief Returns true if the **path** given by argument is writable 
///
bool TRestStringHelper::isPathWritable(const std::string& path)
{
	int result = 0;
#ifdef WIN32
	result = _access(path.c_str(), 2);
#endif
#ifdef linux
	result = access(path.c_str(), 2);
#endif
	
	if (result == 0) return true;
	else return false;
}

///////////////////////////////////////////////
/// \brief Check if the path is absolute path or not
///
bool TRestStringHelper::isAbsolutePath(const std::string & path)
{
	if (path[0] == '/' ||path[0]=='~'|| path.find(':') != -1) { return true; }
	return false;
}

std::vector <string> TRestStringHelper::SeparatePathAndName(const std::string fullname)
{
	vector <string>result;
	int pos = fullname.find_last_of('/', -1);

	if (pos == -1) {
		result.push_back(".");
		result.push_back(fullname);
	}
	else if(pos==0)
	{
		result.push_back("/");
		result.push_back(fullname.substr(1,fullname.size()-1));
	}
	else if(pos==fullname.size())
	{
		result.push_back(fullname);
		result.push_back("");
	}
	else
	{
		result.push_back(fullname.substr(0, pos));
		result.push_back(fullname.substr(pos + 1, fullname.size() - pos - 1));
	}
	return result;
}

std::string TRestStringHelper::SearchFileInPath(vector<string> paths, string filename) {
	if (fileExists(filename)) {
		return filename;
	}
	else
	{
		for (int i = 0; i < paths.size(); i++)
		{
			if (fileExists(paths[i] + filename)) {
				return paths[i] + filename;
				break;
			}
		}
	}
	return "";

}


///////////////////////////////////////////////
/// \brief Windows returns the class name in format of typeid. This method extracts the real class name from that.
///
std::string TRestStringHelper::typeidToClassName(std::string typeidstr)
{
	cout << typeidstr << endl;
	string temp = Replace(typeidstr, "class", "");
	temp = Replace(temp, "*", "");
	temp = Replace(temp, " ", "");
	return temp;

}


///////////////////////////////////////////////
/// \brief Checks if the config file can be openned. It returns OK in case of success, ERROR otherwise.
///
Int_t TRestStringHelper::ChecktheFile(std::string FileName)
{

	ifstream ifs;
	ifs.open(FileName.c_str());

	if (!ifs)
	{
		return -1;
	}
	else ifs.close();

	return 0;
}


///////////////////////////////////////////////
/// \brief Returns a list of files whose name match the pattern string. Key word is "*". e.g. abc00*.root
///
vector <string> TRestStringHelper::GetFilesMatchingPattern(string pattern)
{
	std::vector <string> outputFileNames;

	if (pattern != "") {
		if (pattern.find_first_of("*") >= 0 || pattern.find_first_of("?") >= 0)
		{
			string a = ExecuteShellCommand("find " + pattern);

			auto b = Spilt(a, "\n");

			for (int i = 0; i < b.size(); i++) {
				outputFileNames.push_back(b[i]);
			}

			//char command[256];
			//sprintf(command, "find %s > /tmp/RESTTools_fileList.tmp", pattern.Data());

			//system(command);

			//FILE *fin = fopen("/tmp/RESTTools_fileList.tmp", "r");
			//char str[256];
			//while (fscanf(fin, "%s\n", str) != EOF)
			//{
			//	TString newFile = str;
			//	outputFileNames.push_back(newFile);
			//}
			//fclose(fin);

			//system("rm /tmp/RESTTools_fileList.tmp");
		}
		else
		{
			if (fileExists(pattern))
				outputFileNames.push_back(pattern);
		}
	}
	return outputFileNames;
}

std::string TRestStringHelper::ToUpper(std::string str)
{
	transform(str.begin(), str.end(), str.begin(), (int(*)(int))toupper);
	return str;
}


std::string TRestStringHelper::ExecuteShellCommand(string cmd)
{
#ifdef WIN32
	system(cmd.c_str());
#else
	char buf[1024];
	string result = "";
	FILE *ptr;
	if ((ptr = popen(cmd.c_str(), "r")) != NULL)
	{
		while (fgets(buf, 1024, ptr) != NULL)
		{
			result += (string)buf;
		}
		pclose(ptr);
		ptr = NULL;
		return result;
	}
	else
	{
		printf("popen %s error\n", cmd.c_str());
	}
#endif // WIN32
	return "";

}
