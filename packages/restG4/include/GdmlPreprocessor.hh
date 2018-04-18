
#ifndef restG4_GdmlPreprocessor
#define restG4_GdmlPreprocessor 

#include "TRestMetadata.h"

///////////////////////////////////////////
//we must preprocess gdml file because of a bug in TGDMLParse in ROOT6
//

class GdmlPreprocessor :public TRestMetadata {


public:
	string elementstr;
	void Load(string file) {
		if (fileExists(file)) {
			fConfigFileName = file;

			//getchar();

			std::ifstream t(file);
			std::string str((std::istreambuf_iterator<char>(t)),
				std::istreambuf_iterator<char>());
			elementstr = str;
			t.close();
			int pos = str.find("<gdml", 0);
			if (pos != -1) {
				string temp = str.substr(pos, -1);

				fElement = StringToElement(temp);
				fElementGlobal = GetElement("define", fElement);

				LoadSectionMetadata();

				ReplaceAttributeWithKeyWord("cos(");
				ReplaceAttributeWithKeyWord("sin(");
				ReplaceAttributeWithKeyWord("tan(");
				ReplaceAttributeWithKeyWord("sqrt(");
				ReplaceAttributeWithKeyWord("log(");
				ReplaceAttributeWithKeyWord("exp(");

				ofstream outf;
				outf.open(file, ios::trunc);
				outf << elementstr << endl;
				outf.close();
				//getchar();
			}			
		}
		else
		{
			error << "Filename : " << file << endl;
			error << "REST ERROR. File does not exist. Right path/filename?" << endl;
			exit(0);
		}
	}
	void InitFromConfigFile() {}

	void Print() {

		cout << elementstr << endl;
	}

	void ReplaceAttributeWithKeyWord(string s) 
	{
		int n;
		while ((n = elementstr.find(s, 0)) != -1)
		{
			int pos1, pos2;
			for (int i = n; i >= 0; i--) {
				if (elementstr[i] == '"') {
					pos1 = i + 1;
					break;
				}
			}

			for (int i = n; i < elementstr.size(); i++) {
				if (elementstr[i] == '"') {
					pos2 = i;
					break;
				}
			}

			string target = elementstr.substr(pos1, pos2 - pos1);
			//cout << target << endl;
			string replace = ReplaceMathematicalExpressions(ReplaceEnvironmentalVariables(target));
			//cout << replace << endl;
			elementstr.replace(pos1, pos2 - pos1, replace);

		}
	}


};

#endif