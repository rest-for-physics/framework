#include "TRestGDMLParser.h"

#include <filesystem>

using namespace std;

string TRestGDMLParser::GetEntityVersion(const string& name) const {
    for (auto& [entityName, entityVersion] : fEntityVersionMap) {
        if (entityName == name) {
            return entityVersion;
        }
    }
    return "0.0";
}

void TRestGDMLParser::Load(const string& filename) {
    const auto filenameAbsolute = TRestTools::ToAbsoluteName(filename);
    if (TRestTools::fileExists(filenameAbsolute)) {
        fConfigFileName = filenameAbsolute;
        fPath = TRestTools::SeparatePathAndName(filenameAbsolute).first;

        std::ifstream t(filenameAbsolute);
        std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        fFileString = str;
        t.close();

        int pp = fFileString.find("##VERSION", 0);
        if (pp != string::npos) {
            int pp2 = fFileString.find("##", pp + 4);
            if (pp2 != string::npos) fGdmlVersion = fFileString.substr(pp + 9, pp2 - pp - 9);
            fGdmlVersion = ReplaceMathematicalExpressions(ReplaceConstants(ReplaceVariables(fGdmlVersion)));
        }

        fFileString = ReplaceConstants(ReplaceVariables(fFileString));

        cout << "TRestGDMLParser: Initializing variables" << endl;
        int pos = fFileString.find("<gdml", 0);
        if (pos != -1) {
            string elementString = fFileString.substr(pos, -1);

            fElement = StringToElement(elementString);
            fElementGlobal = GetElement("define", fElement);

            LoadSectionMetadata();
        }

        cout << "TRestGDMLParser: Replacing expressions in GDML" << endl;
        ReplaceEntity();
        fFileString = Replace(fFileString, "= \"", "=\"");
        fFileString = Replace(fFileString, " =\"", "=\"");
        fFileString = Replace(fFileString, " = \"", "=\"");

        ReplaceAttributeWithKeyWord("cos(");
        ReplaceAttributeWithKeyWord("sin(");
        ReplaceAttributeWithKeyWord("tan(");
        ReplaceAttributeWithKeyWord("sqrt(");
        ReplaceAttributeWithKeyWord("log(");
        ReplaceAttributeWithKeyWord("exp(");

        string filenameNoPath = TRestTools::SeparatePathAndName(filenameAbsolute).second;
        // we have to use a unique identifier on the file to prevent collision when launching multiple jobs
        fOutputGdmlFilename = fOutputGdmlDirectory + "PID" + std::to_string(getpid()) + "_" + filenameNoPath;
        cout << "TRestGDMLParser: Creating temporary file at: \"" << fOutputGdmlFilename << "\"" << endl;

        filesystem::create_directories(fOutputGdmlDirectory);

        ofstream outputFile;
        outputFile.open(fOutputGdmlFilename, ios::trunc);
        outputFile << fFileString << endl;
        outputFile.close();

        std::ifstream fileToCheckExistence(fOutputGdmlFilename);
        if (!fileToCheckExistence) {
            std::cout << "TRestGDMLParser: Problem writing temporary file." << std::endl;
            exit(1);
        }

    } else {
        ferr << "TRestGDMLParser: Input GDML file: \"" << filename
             << "\" does not exist. Please double check your current path and filename" << endl;
        exit(1);
    }
}

TGeoManager* TRestGDMLParser::CreateGeoManager() {
    // We must change to the gdml file directory, otherwise ROOT cannot load.
    if (!fOutputGdmlFilename.empty()) {
        char originDirectory[256];
        sprintf(originDirectory, "%s", getenv("PWD"));
        chdir(fOutputGdmlDirectory.c_str());
        auto geoManager = new TGeoManager();
        geoManager->Import(fOutputGdmlFilename.c_str());
        chdir(originDirectory);
        return geoManager;
    }
    return nullptr;
}

void TRestGDMLParser::PrintContent() { cout << fFileString << endl; }

void TRestGDMLParser::ReplaceEntity() {
    int pos = 0;
    while ((pos = fFileString.find("<!ENTITY", pos)) != -1) {
        int pos1 = fFileString.find_first_not_of(" ", pos + 8);
        int pos2 = fFileString.find("SYSTEM", pos1);
        string entityName = RemoveWhiteSpaces(fFileString.substr(pos1, pos2 - pos1));

        int pos3 = fFileString.find("\"", pos2) + 1;
        int pos4 = fFileString.find("\"", pos3);
        string entityFile = RemoveWhiteSpaces(fFileString.substr(pos3, pos4 - pos3));

        cout << "TRestGDMLParser: Replacing entity: " << entityName << ", file: " << entityFile << endl;

        if ((int)entityFile.find("http") != -1) {
            string entityField =
                fOutputGdmlDirectory + "PID" + std::to_string(getpid()) + "_" + entityName + ".xml";
            int a = TRestTools::DownloadRemoteFile(entityFile, entityField);
            if (a != 0) {
                cout << "TRestGDMLParser: Download failed!" << endl;
                exit(1);
            }
            entityFile = entityField;
        } else {
            entityFile = fPath + entityFile;
        }

        int pos5 = 0;
        if ((pos5 = fFileString.find("&" + entityName + ";")) != -1) {
            if (TRestTools::fileExists(entityFile)) {
                std::ifstream entityFileRead(entityFile);
                std::string str((std::istreambuf_iterator<char>(entityFileRead)),
                                std::istreambuf_iterator<char>());
                entityFileRead.close();

                str = ReplaceConstants(ReplaceVariables(str));

                fEntityVersionMap[entityName] = "";
                int pp = str.find("##VERSION", 0);
                if (pp != string::npos) {
                    int pp2 = str.find("##", pp + 4);
                    if (pp2 != string::npos) {
                        fEntityVersionMap[entityName] = str.substr(pp + 9, pp2 - pp - 9);
                    }
                }

                fFileString.replace(pos5, entityName.length() + 2, str);
            } else {
                cout << "GDML ERROR! No matching reference file for entity: \"" << entityName << "\"" << endl;
                cout << "file name: \"" << entityFile << "\"" << endl;
                exit(1);
            }
        } else {
            cout << "TRestGDMLParser: Warning! redundant entity: \"" << entityName << "\"" << endl;
        }
        pos++;
    }
}

void TRestGDMLParser::ReplaceAttributeWithKeyWord(const string& keyword) {
    int n;
    while ((n = fFileString.find(keyword, 0)) != -1) {
        int pos1 = 0, pos2 = 0;
        for (int i = n; i >= 0; i--) {
            if (fFileString[i] == '"') {
                pos1 = i + 1;
                break;
            }
        }

        for (unsigned int i = n; i < fFileString.size(); i++) {
            if (fFileString[i] == '"') {
                pos2 = i;
                break;
            }
        }
        string target = fFileString.substr(pos1, pos2 - pos1);
        string replace = ReplaceMathematicalExpressions(ReplaceConstants(ReplaceVariables(target)));

        if (replace == target) {
            cout << "Error! failed to replace mathematical expressions! check the file!" << endl;
            cout << replace << endl;
            exit(1);
        }
        fFileString.replace(pos1, pos2 - pos1, replace);
    }
}
