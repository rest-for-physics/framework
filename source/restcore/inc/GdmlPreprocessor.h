
#ifndef restG4_GdmlPreprocessor
#define restG4_GdmlPreprocessor

#include "TGeoManager.h"
#include "TRestMetadata.h"
#include "unistd.h"

///////////////////////////////////////////
// we must preprocess gdml file because of a bug in TGDMLParse::Value() in ROOT6
//

class GdmlPreprocessor : public TRestMetadata {
   public:
    GdmlPreprocessor() {}
    ~GdmlPreprocessor() {
        // if(outfilename!=""){ system(("rm " + outfilename).c_str()); }
    }
    string filestr = "";
    string path = "";
    string outfilename = "";
    void Load(string file) {
        file = TRestTools::ToAbsoluteName(file);
        if (TRestTools::fileExists(file)) {
            fConfigFileName = file;
            path = TRestTools::SeparatePathAndName(file).first;

            // getchar();

            std::ifstream t(file);
            std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
            filestr = str;
            t.close();

            cout << "GDML: initializating variables" << endl;
            int pos = filestr.find("<gdml", 0);
            if (pos != -1) {
                string elementstr = filestr.substr(pos, -1);

                fElement = StringToElement(elementstr);
                fElementGlobal = GetElement("define", fElement);

                LoadSectionMetadata();
            }

            cout << "GDML: replacing expressions in GDML" << endl;
            ReplaceEntity();
            filestr = Replace(filestr, "= \"", "=\"");
            filestr = Replace(filestr, " =\"", "=\"");
            filestr = Replace(filestr, " = \"", "=\"");

            ReplaceAttributeWithKeyWord("cos(");
            ReplaceAttributeWithKeyWord("sin(");
            ReplaceAttributeWithKeyWord("tan(");
            ReplaceAttributeWithKeyWord("sqrt(");
            ReplaceAttributeWithKeyWord("log(");
            ReplaceAttributeWithKeyWord("exp(");

            cout << "GDML: creating temporary file" << endl;
            ofstream outf;
            outfilename = file + "_";
            outf.open(outfilename, ios::trunc);
            outf << filestr << endl;
            outf.close();
            // getchar();

        } else {
            ferr << "Filename : " << file << endl;
            ferr << "File does not exist. Right path/filename?" << endl;
            exit(0);
        }
    }

    TGeoManager* CreateGeoM() {
        // We must change to the gdml file directory, otherwise ROOT cannot load.
        if (outfilename != "") {
            char originDirectory[256];
            sprintf(originDirectory, "%s", getenv("PWD"));
            chdir(path.c_str());
            TGeoManager* geo2 = new TGeoManager();
            geo2->Import(outfilename.c_str());
            chdir(originDirectory);
            return geo2;
        }
        return NULL;
    }

    void InitFromConfigFile() {}

    void PrintContent() { cout << filestr << endl; }

    void ReplaceEntity() {
        int pos = 0;
        while ((pos = filestr.find("<!ENTITY", pos)) != -1) {
            int pos1 = filestr.find_first_not_of(" ", pos + 8);
            int pos2 = filestr.find("SYSTEM", pos1);
            string entityname = RemoveWhiteSpaces(filestr.substr(pos1, pos2 - pos1));
            // cout << entityname << endl;

            int pos3 = filestr.find("\"", pos2) + 1;
            int pos4 = filestr.find("\"", pos3);
            string entityfile = path + RemoveWhiteSpaces(filestr.substr(pos3, pos4 - pos3));
            // cout << entityfile << endl;

            int pos5 = 0;
            if ((pos5 = filestr.find("&" + entityname + ";")) != -1) {
                if (TRestTools::fileExists(entityfile)) {
                    std::ifstream t(entityfile);
                    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
                    t.close();

                    // cout << "replacing entity..." << endl;
                    filestr.replace(pos5, entityname.length() + 2, str);
                } else {
                    cout << "GDML ERROR! No matching reference file for entity: \"" << entityname << "\""
                         << endl;
                    cout << "file name: \"" << entityfile << "\"" << endl;
                    exit(0);
                }
            } else {
                cout << "GDML: Warning! redundant entity: \"" << entityname << "\"" << endl;
            }
            pos++;
        }
    }

    void ReplaceAttributeWithKeyWord(string keyword) {
        int n;
        while ((n = filestr.find(keyword, 0)) != -1) {
            int pos1 = 0, pos2 = 0;
            for (int i = n; i >= 0; i--) {
                if (filestr[i] == '"') {
                    pos1 = i + 1;
                    break;
                }
            }

            for (unsigned int i = n; i < filestr.size(); i++) {
                if (filestr[i] == '"') {
                    pos2 = i;
                    break;
                }
            }
            string target = filestr.substr(pos1, pos2 - pos1);
            // cout << target << endl;
            string replace = ReplaceMathematicalExpressions(ReplaceEnvironmentalVariables(target));
            // cout << replace << endl;
            // cout << target << " " << ReplaceEnvironmentalVariables(target) << " "
            // << replace << endl;

            if (replace == target) {
                cout << "Error! falied to replace mathematical expressions! check the "
                        "file!"
                     << endl;
                cout << replace << endl;
                exit(0);
            }
            filestr.replace(pos1, pos2 - pos1, replace);
        }
    }
};

#endif
