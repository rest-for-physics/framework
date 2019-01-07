

#include "TRestStringOutput.h"


string TRestStringOutput::FormattingPrintString(string input)
{
	if (input == "")return "";

	string output = string(length, ' ');
	for (unsigned int i = 0; i < border.size(); i++) {
		output[i] = border[i];
		output[length - i - 1] = border[border.size() - i - 1];
	}

	// input: "=", output "=========================="(length)
	if (input == "=" || input == "-" || input == "*" || input == "+")
	{
		for (unsigned int i = border.size(); i < length - border.size(); i++)
		{
			output[i] = input[0];
		}
	}
	else
	{
		// input: "=abc=", output "=============abc============="(length)
		if (input[0] == input[input.size() - 1] && (input[0] == '=' || input[0] == '-' || input[0] == '*' || input[0] == '+'))
		{
			for (unsigned int i = border.size(); i < length - border.size(); i++)
			{
				output[i] = input[0];
			}
		}

		int l = input.size();
		if (l <= length - (int)border.size() * 2)
		{
			int startblank;
			if (orientation == 0 || border.size() > 0) {
				startblank = (length - border.size() * 2 - l) / 2;
			}
			else
			{
				startblank = 0;
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



void TRestStringOutput::flushstring()
{
	if (length == -1)//this means we are using condor
	{
		cout << stringbuf << endl;
		stringbuf = "";
	}
	else
	{
		int consolewidth = ConsoleHelper::GetWidth() - 2;
		printf("\033[K");
		if (orientation == 0) {
			cout << color << string((consolewidth - length) / 2, ' ')
				<< FormattingPrintString(stringbuf)
				<< string((consolewidth - length) / 2, ' ') << COLOR_RESET << endl;
		}
		else
		{
			cout << color << FormattingPrintString(stringbuf) << COLOR_RESET << endl;
		}
		stringbuf = "";
	}
}