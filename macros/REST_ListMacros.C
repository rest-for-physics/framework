#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>

#include "TRestTask.h"
using namespace std;

#include <TString.h>

Int_t REST_ListMacros() {
    string macrosPath = (string)getenv("REST_PATH") + "/macros";
    vector<string> directories = TRestTools::GetSubdirectories(macrosPath);

    for (int n = 0; n < directories.size(); n++) {
        cout << "Directory : " << directories[n] << endl;
        if (directories[n].find("pipeline") != string::npos) continue;
        if (directories[n].find("/macros/mac/") != string::npos) continue;
        vector<string> files = TRestTools::GetFilesMatchingPattern(directories[n] + "/REST_*.C");

        for (int m = 0; m < files.size(); m++) {
            cout << "     ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
            cout << "     ++   "
                 << " Macro : " << TRestTools::SeparatePathAndName(files[m]).second << endl;
            std::ifstream t(files[m]);
            std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

            std::vector<string> lines = REST_StringHelper::Split(str, "\n");

            cout << "     ++   " << endl;
            for (int l = 0; l < lines.size(); l++)
                if (lines[l].find("//*** ") != string::npos)
                    cout << "     ++   " << lines[l].substr(6, -1) << endl;
            cout << "     ++   " << endl;
        }
        cout << "     ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
    }
    return 0;
}
