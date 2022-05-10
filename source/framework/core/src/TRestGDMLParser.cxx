#include "TRestGDMLParser.h"

using namespace std;

string TRestGDMLParser::GetEntityVersion(string name) {
    for (auto& i : entityVersion) {
        if (i.first == name) {
            return i.second;
            break;
        }
    }
    return "0.0";
}

string TRestGDMLParser::GetGDMLVersion() { return gdmlVersion; }

string TRestGDMLParser::GetOutputGDMLFile() { return outfilename; }

void TRestGDMLParser::Load(string file) {
    file = TRestTools::ToAbsoluteName(file);
    if (TRestTools::fileExists(file)) {
        fConfigFileName = file;
        path = TRestTools::SeparatePathAndName(file).first;

        std::ifstream t(file);
        std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        filestr = str;
        t.close();

        int pp = filestr.find("##VERSION", 0);
        if (pp != string::npos) {
            int pp2 = filestr.find("##", pp + 4);
            if (pp2 != string::npos) gdmlVersion = filestr.substr(pp + 9, pp2 - pp - 9);
            gdmlVersion = ReplaceMathematicalExpressions(ReplaceConstants(ReplaceVariables(gdmlVersion)));
        }

        filestr = ReplaceConstants(ReplaceVariables(filestr));

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
        string fname = TRestTools::SeparatePathAndName(file).second;
        // we have to use a unique identifier on the file to prevent collision when launching multiple jobs
        outfilename = outPath + "PID" + std::to_string(getpid()) + "_" + fname;
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

TGeoManager* TRestGDMLParser::CreateGeoM() {
    // We must change to the gdml file directory, otherwise ROOT cannot load.
    if (outfilename != "") {
        char originDirectory[256];
        sprintf(originDirectory, "%s", getenv("PWD"));
        chdir(outPath.c_str());
        TGeoManager* geo2 = new TGeoManager();
        geo2->Import(outfilename.c_str());
        chdir(originDirectory);
        return geo2;
    }
    return nullptr;
}

void TRestGDMLParser::PrintContent() { cout << filestr << endl; }

void TRestGDMLParser::ReplaceEntity() {
    int pos = 0;
    while ((pos = filestr.find("<!ENTITY", pos)) != -1) {
        int pos1 = filestr.find_first_not_of(" ", pos + 8);
        int pos2 = filestr.find("SYSTEM", pos1);
        string entityname = RemoveWhiteSpaces(filestr.substr(pos1, pos2 - pos1));
        // cout << entityname << endl;

        int pos3 = filestr.find("\"", pos2) + 1;
        int pos4 = filestr.find("\"", pos3);
        string entityfile = RemoveWhiteSpaces(filestr.substr(pos3, pos4 - pos3));

        cout << "GDML: replacing entitiy: " << entityname << ", file: " << entityfile << endl;

        if ((int)entityfile.find("http") != -1) {
            string entityfiledl = outPath + "PID" + std::to_string(getpid()) + "_" + entityname + ".xml";
            int a = TRestTools::DownloadRemoteFile(entityfile, entityfiledl);
            if (a != 0) {
                cout << "GDML: Download failed!" << endl;
                exit(1);
            }
            entityfile = entityfiledl;
        } else {
            entityfile = path + entityfile;
        }

        int pos5 = 0;
        if ((pos5 = filestr.find("&" + entityname + ";")) != -1) {
            if (TRestTools::fileExists(entityfile)) {
                std::ifstream t(entityfile);
                std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
                t.close();

                str = ReplaceConstants(ReplaceVariables(str));

                entityVersion[entityname] = "";
                int pp = str.find("##VERSION", 0);
                if (pp != string::npos) {
                    int pp2 = str.find("##", pp + 4);
                    if (pp2 != string::npos) {
                        entityVersion[entityname] = str.substr(pp + 9, pp2 - pp - 9);
                    }
                }

                // cout << "replacing entity..." << endl;
                filestr.replace(pos5, entityname.length() + 2, str);
            } else {
                cout << "GDML ERROR! No matching reference file for entity: \"" << entityname << "\"" << endl;
                cout << "file name: \"" << entityfile << "\"" << endl;
                exit(0);
            }
        } else {
            cout << "GDML: Warning! redundant entity: \"" << entityname << "\"" << endl;
        }
        pos++;
    }
}

void TRestGDMLParser::ReplaceAttributeWithKeyWord(string keyword) {
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
        string replace = ReplaceMathematicalExpressions(ReplaceConstants(ReplaceVariables(target)));
        // cout << replace << endl;
        // cout << target << " " << ReplaceConstants(ReplaceVariables(target) << " "
        // << replace << endl;

        if (replace == target) {
            cout << "Error! failed to replace mathematical expressions! check the "
                    "file!"
                 << endl;
            cout << replace << endl;
            exit(0);
        }
        filestr.replace(pos1, pos2 - pos1, replace);
    }
}
