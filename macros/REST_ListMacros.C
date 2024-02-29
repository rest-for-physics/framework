#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>

#include "TRestTask.h"
using namespace std;

#include <TString.h>

//*******************************************************************************************************
//***
//*** Lists all the official macros together with its documentation
//***
//*******************************************************************************************************
Int_t REST_ListMacros(int details = 0, std::string onlyThisMacro = "") {
    string macrosPath = (string)getenv("REST_PATH") + "/macros";
    vector<string> directories = TRestTools::GetSubdirectories(macrosPath);

    cout << endl;
    cout << "Entering directory : " << macrosPath << endl;
    vector<string> main_files = TRestTools::GetFilesMatchingPattern(macrosPath + "/REST_*.C");
    if (details) {
        for (int m = 0; m < main_files.size(); m++) {
            std::string macro = TRestTools::SeparatePathAndName(main_files[m]).second;
            if (!onlyThisMacro.empty() && onlyThisMacro != macro) continue;
            cout << "     ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
            cout << "     ++   "
                 << " Macro : " << macro << endl;
            std::ifstream t(main_files[m]);
            std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

            std::vector<string> lines = REST_StringHelper::Split(str, "\n");

            cout << "     ++   " << endl;
            for (int l = 0; l < lines.size(); l++)
                if (lines[l].find("//*** ") != string::npos)
                    cout << "     ++   " << lines[l].substr(6, -1) << endl;
            cout << "     ++   " << endl;
        }
        cout << "     ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
    } else {
        for (int m = 0; m < main_files.size(); m++) {
            cout << " + " << TRestTools::SeparatePathAndName(main_files[m]).second << endl;
        }
    }

    for (int n = 0; n < directories.size(); n++) {
        cout << endl;
        cout << "Entering directory : " << directories[n] << endl;
        if (directories[n].find("pipeline") != string::npos) continue;
        if (directories[n].find("/macros/mac/") != string::npos) continue;
        vector<string> files = TRestTools::GetFilesMatchingPattern(directories[n] + "/REST_*.C");

        if (details) {
            for (int m = 0; m < files.size(); m++) {
                std::string macro = TRestTools::SeparatePathAndName(files[m]).second;
                if (!onlyThisMacro.empty() && onlyThisMacro != macro) continue;
                cout << "     ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
                     << endl;
                cout << "     ++    Macro : " << macro << endl;
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
        } else {
            for (int m = 0; m < files.size(); m++) {
                cout << " + " << TRestTools::SeparatePathAndName(files[m]).second << endl;
            }
            std::cout << std::endl;
            std::cout << " ------- " << std::endl;
            std::cout << "IMPORTANT. To get a more detailed macro documentation enable the details argument:"
                      << std::endl;
            std::cout << "Execute: restListMacros 1" << std::endl;
            std::cout << "OR if you want to show only the documentation of a given macro, add the macro.C "
                         "filename as argument"
                      << std::endl;
            std::cout << "Execute: restListMacros 1 REST_CheckValidRuns.C" << std::endl;
            std::cout << " ------- " << std::endl;
        }
    }
    return 0;
}
