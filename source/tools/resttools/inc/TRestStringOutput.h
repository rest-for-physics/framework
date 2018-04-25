#ifndef RestCore_TRestStringOutput
#define RestCore_TRestStringOutput

#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream> 

#define COLOR_RESET   "\033[0m"
#define COLOR_BLACK   "\033[30m"      /* Black */
#define COLOR_RED     "\033[31m"      /* Red */
#define COLOR_GREEN   "\033[32m"      /* Green */
#define COLOR_YELLOW  "\033[33m"      /* Yellow */
#define COLOR_BLUE    "\033[34m"      /* Blue */
#define COLOR_MAGENTA "\033[35m"      /* Magenta */
#define COLOR_CYAN    "\033[36m"      /* Cyan */
#define COLOR_WHITE   "\033[37m"      /* White */
#define COLOR_BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define COLOR_BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define COLOR_BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define COLOR_BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define COLOR_BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define COLOR_BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define COLOR_BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define COLOR_BOLDWHITE   "\033[1m\033[37m"      /* Bold White */
#define COLOR_BACKGROUNDBLACK   "\033[1m\033[40m"      /* BACKGROUND Black */
#define COLOR_BACKGROUNDRED     "\033[1m\033[41m"      /* BACKGROUND Red */
#define COLOR_BACKGROUNDGREEN   "\033[1m\033[42m"      /* BACKGROUND Green */
#define COLOR_BACKGROUNDYELLOW  "\033[1m\033[43m"      /* BACKGROUND Yellow */
#define COLOR_BACKGROUNDBLUE    "\033[1m\033[44m"      /* BACKGROUND Blue */
#define COLOR_BACKGROUNDMAGENTA "\033[1m\033[45m"      /* BACKGROUND Magenta */
#define COLOR_BACKGROUNDCYAN    "\033[1m\033[46m"      /* BACKGROUND Cyan */
#define COLOR_BACKGROUNDWHITE   "\033[1m\033[47m"      /* BACKGROUND White */

using namespace std;

enum REST_Verbose_Level
{
	REST_Silent, //!< show minimum information of the software, as well as error messages
	REST_Essential, //!< +show some essential information, as well as warnings
	REST_Info, //!< +show most of the infomation for each steps
	REST_Debug, //!< +show the defined debug messages
	REST_Extreme //!< show everything
};
//////////////////////////////////////////////////////////////////////////
///
/// This class serves as an universal string output tool, aiming at leveling, 
/// arranging, and auto saving for the output message. 
///
/// Features of this class:
/// 1. overloaded operator "<<"
/// 2. inline method endl()
/// 3. pre-defined color constants
/// 4. output formatting methods
/// 
/// To use this tool class in the other classes, include its header file and instantiate
/// a TRestStringOutput object(suggested name: "fout"). The fout can therefore replace the
/// functionality of cout. The usage is the same: "fout<<"hello world"<<endl;". 
/// Setting output color, changing output border, length or orientation are all supported.
/// In future it is also possible to save output message in a log file.

class TRestStringOutput
{
public:

	string FormattingPrintString(string input)
	{
		if (input == "")return "";

		string output = string(length, ' ');
		for (int i = 0; i < border.size(); i++) {
			output[i] = border[i];
			output[length - i - 1] = border[border.size() - i - 1];
		}

		if (input == "=" || input == "-" || input == "*" || input == "+")
		{
			for (int i = border.size(); i < length - border.size(); i++)
			{
				output[i] = input[0];
			}
		}
		else
		{
			if (input[0] == input[input.size() - 1] && (input[0] == '=' || input[0] == '-' || input[0] == '*' || input[0] == '+'))
			{
				for (int i = border.size(); i < length - border.size(); i++)
				{
					output[i] = input[0];
				}
			}
			else
			{
				for (int i = border.size(); i < length - border.size(); i++)
				{
					output[i] = ' ';
				}
			}

			int l = input.size();
			if (l <= length - border.size() * 2)
			{
				int startblank;
				if (orientation == 0) {
					startblank = (length - border.size() * 2 - l) / 2;
				}
				else
				{
					startblank = border.size();
				}
				for (int i = 0; i < l; i++)
				{
					output[startblank + border.size() + i] = input[i];
				}
			}
			else
			{
				for (int i = 0; i < length - 3; i++)
				{
					output[i] = input[i];
				}
				output[length - 3] = '.';
				output[length - 2] = '.';
				output[length - 1] = '.';
			}
		}

		return output;
	}

	template<class T> TRestStringOutput& operator << (T content)
	{
		stringstream tmp;
		tmp << content;
		tmpstring += tmp.str();
		return*this;
	}

	TRestStringOutput& operator <<(void(*pfunc)(TRestStringOutput&))
	{
		((*pfunc)(*this));
		return *this;
	}


	void resetstring()
	{
		tmpstring = "";
	}

	void flushstring()
	{
		cout << color << FormattingPrintString(tmpstring) << COLOR_RESET << endl;
		tmpstring = "";
	}


	void insertfront(string s) {
		tmpstring = s + tmpstring;
	}

	void insertback(string s) {
		tmpstring = tmpstring + s;
	}

	void setcolor(string colordef)
	{
		color = colordef;
	}

	void resetcolor()
	{
		color = COLOR_RESET;
	}

	void setborder(string b) {
		border = b;
	}

	void resetborder() {
		border = "";
	}

	void setlength(int n) {
		length = n;
	}

	void resetlength() {
		length = 100;
	}

	void setorientation(int o) {
		orientation = o;
	}

	void resetorientation() {
		orientation = 0;
	}

protected:
	int length = 100;
	int orientation = 0;//0->middle, else->left
	string border = "";
	string tmpstring = "";
	string color = COLOR_RESET;

	//stringstream tmpstring;
};

template<REST_Verbose_Level v> class TRestLeveledOutput :public TRestStringOutput {
public:
	TRestLeveledOutput() {};
	TRestLeveledOutput(REST_Verbose_Level& vref, string c = COLOR_RESET, string b = "", int orientation = 0)
		:verboselvlref(vref)
	{
		this->orientation = orientation;
		color = c;
		border = b;
	}

	REST_Verbose_Level verbose = v;
	REST_Verbose_Level& verboselvlref;

	template<class T> TRestLeveledOutput& operator << (T content)
	{
		stringstream tmp;
		tmp << content;
		tmpstring += tmp.str();
		return*this;
	}


	TRestLeveledOutput& operator <<(void(*pfunc)(TRestLeveledOutput&))
	{
		((*pfunc)(*this));
		return *this;
	}

};


inline void cursorUp(int n) {
	printf("\033[%dA",n);
	fflush(stdout);
}

inline void cursorDown(int n) {
	printf("\033[%dB", n);
	fflush(stdout);
}

inline void cursorRight(int n) {
	printf("\033[%dC", n);
	fflush(stdout);
}

inline void cursorLeft(int n) {
	printf("\033[%dD", n);
	fflush(stdout);
}

inline void cursorToXY(int x, int y) {
	printf("\033[%d%dH", x,y);
	fflush(stdout);
}

inline void clearScreen() {
	printf("\033[2J");
	fflush(stdout);
}

inline void clearCurrentLine() {

	printf("\033[K");

	fflush(stdout);
}

inline void clearLinesAfterCursor() {
	printf("\033[s");

	for (int i = 0; i < 50; i++) {
		printf("\033[K");
		cursorDown(1);
	}

	printf("\033[u");

	fflush(stdout);
}

inline void endl(TRestStringOutput& input)
{
	input.flushstring();
}

template<REST_Verbose_Level v> inline void endl(TRestLeveledOutput<v>& input)
{
	if (input.verboselvlref >= input.verbose)
		input.flushstring();
	else
		input.resetstring();
}

inline void PrintWelcome()
{
	system("rest-config --welcome");
}

#endif