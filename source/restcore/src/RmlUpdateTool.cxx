#include "RmlUpdateTool.h"
#include "TRestStringHelper.h"
#include "tinyxml.h"

RmlUpdateTool::RmlUpdateTool() {}

RmlUpdateTool::RmlUpdateTool(string Filename, bool process) {
    Import(Filename);
    if (Is2_1() && process) {
        Process();
    } else {
        // TiXmlDocument* doc = new TiXmlDocument();
        // if (doc->LoadFile(Filename.c_str()))
        //{
        //	fStatus = true;
        //}
        // else
        //{
        //	fStatus = false;
        //	int result = system(("xmllint " + Filename + ">
        /// tmp/xmlerror.txt").c_str()); 	if (result == 256) { system("cat
        /// tmp/xmlerror.txt"); } 	else { 		error << "Something is wrong in
        /// the rml
        // file!" << endl; 		error << "To do syntax check for the file, please
        // install the package \"xmllint\"" << endl;
        //	}
        //	system("rm /tmp/xmlerror.txt");

        //}
    }
}

bool RmlUpdateTool::Process() {
    if (fFileContent != "") {
        cout << "-- begin processing old rml file, updating syntax!" << endl;
        if (UpdateSyntax()) {
            cout << "-- updating structure..." << endl;
            if (UpdateStructure()) {
                cout << "-- writing updated file..." << endl;
                fOutputFileName = WriteFile();
                cout << "--------------------" << endl;
                cout << "RmlUpdateTool: an equivelent version of rml file for V2.2 is "
                        "generated!"
                     << endl;
                cout << "Name: " << fOutputFileName << endl;
                cout << "--------------------" << endl;
                fStatus = true;
                return fStatus;
            }
        }
        cout << "error updating the old rml file!" << endl;
    } else {
        cout << "input file is blank!" << endl;
    }

    fStatus = false;
    return fStatus;
}

void RmlUpdateTool::Import(string Filename) {
    fFileName = Filename;

    std::ifstream t(fFileName);
    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    fFileContent = str;
    t.close();
}

bool RmlUpdateTool::Is2_1() {
    if (fFileContent.find("<section") != -1 && fFileContent.find("<TRest") == -1) {
        return true;
    }
    return false;
}

bool RmlUpdateTool::UpdateSyntax() {
    // we must use C++ to do the job in future
    map<string, int> index;
    index["restG4"] = fFileContent.find("TRestGeant4Metadata");
    index["restManager"] = fFileContent.find("TRestManager");
    index["processes"] = fFileContent.find("Process");
    index["readouts"] = fFileContent.find("TRestDetectorReadout");
    index["gases"] = fFileContent.find("TRestGas");

    string name = "restManager";
    int pos = 999999;
    auto iter = index.begin();
    while (iter != index.end()) {
        if (iter->second != -1 && iter->second < pos) {
            pos = iter->second;
            name = iter->first;
        }
        iter++;
    }

    int n = system(
        ("python ${REST_PATH}/scripts/restv2_1ToXml.py " + fFileName + " rest_rml_tmp.rml " + name).c_str());

    if (n == 0)
        return true;
    else
        return false;
}

/// we reorganize the element order
/// before (v2.1.6 style):
/// <restManager>
///   <globals/>
///   <TRestManager>
///      <addProcess/>
///      <addMetadata/>
///      <addTask/>
///   </TRestManager>
///   <TRestRun/>
/// </restManager>
/// after (v2.2 style):
/// <TRestManager>
///   <globals/>
///   <TRestRun/>
///      <addMetadata/>
///   </TRestRun>
///   <TRestProcessRunner>
///      <addProcess/>
///   </TRestProcessRunner>
///   <addTask/>
/// </TRestManager>
bool RmlUpdateTool::UpdateStructure() {
    TiXmlDocument* doc = new TiXmlDocument();
    if (doc->LoadFile("rest_rml_tmp.rml")) {
        fElement = (TiXmlElement*)doc->RootElement()->Clone();

        delete doc;
        system("rm rest_rml_tmp.rml");

        // get debug level
        string debugStr = GetParameter("verboseLevel", "essential");
        if (debugStr == "silent" || debugStr == "0") fVerboseLevel = REST_Silent;
        if (debugStr == "essential" || debugStr == "warning" || debugStr == "1")
            fVerboseLevel = REST_Essential;
        if (debugStr == "info" || debugStr == "2") fVerboseLevel = REST_Info;
        if (debugStr == "debug" || debugStr == "3") fVerboseLevel = REST_Debug;
        if (debugStr == "extreme" || debugStr == "4") fVerboseLevel = REST_Extreme;

        if (fElement->FirstChildElement("TRestManager") != NULL) {
            cout << "old style config file of restManager is detected!" << endl;
            cout << "trying to re-arranage the xml element..." << endl;
            cout << endl;

            debug << "switch position of the elements" << endl;
            // fElement = (TiXmlElement*)fElementGlobal->Parent();
            // fElement->SetValue("TRestManager");
            TiXmlElement* TRestProcessRunnerElement = fElement->FirstChildElement("TRestManager");
            TRestProcessRunnerElement->SetValue("TRestProcessRunner");
            TiXmlElement* TRestRunElement_old = fElement->FirstChildElement("TRestRun");
            fElement->InsertBeforeChild(TRestProcessRunnerElement, *TRestRunElement_old);
            fElement->RemoveChild(TRestRunElement_old);
            TiXmlElement* TRestRunElement = fElement->FirstChildElement("TRestRun");

            debug << "handle \"addProcess\"" << endl;
            TString processFile = GetParameter("processesFile", TRestProcessRunnerElement, "");
            TRestProcessRunnerElement->RemoveChild(
                GetElementWithName("parameter", "processesFile", TRestProcessRunnerElement));
            TiXmlElement* addProcessElement = TRestProcessRunnerElement->FirstChildElement("addProcess");
            while (addProcessElement != NULL) {
                addProcessElement->SetAttribute("file", processFile);
                addProcessElement = addProcessElement->NextSiblingElement("addProcess");
            }

            debug << "handle \"addMetadata\"" << endl;
            TiXmlElement* addMetadataElement = TRestProcessRunnerElement->FirstChildElement("addMetadata");
            while (addMetadataElement != NULL) {
                if (addMetadataElement->Attribute("type") != NULL &&
                    (string)addMetadataElement->Attribute("type") == "TRestDetectorReadout") {
                    TString readoutFile = GetParameter("readoutFile", TRestProcessRunnerElement, "");
                    if (readoutFile != "") {
                        addMetadataElement->SetAttribute("file", readoutFile);
                        TRestProcessRunnerElement->RemoveChild(
                            GetElementWithName("parameter", "readoutFile", TRestProcessRunnerElement));
                    }
                }
                if (addMetadataElement->Attribute("type") != NULL &&
                    (string)addMetadataElement->Attribute("type") == "TRestGas") {
                    if (addMetadataElement->Attribute("name") != NULL) {
                        addMetadataElement->SetAttribute(
                            "name", Replace(addMetadataElement->Attribute("name"), "V/cm", "Vcm", 0));
                    }

                    TString gasFile = GetParameter("gasFile", TRestProcessRunnerElement, "");
                    if (gasFile != "") {
                        addMetadataElement->SetAttribute("file", gasFile);
                        if (Count((string)gasFile, ".rml") > 0) {
                            addMetadataElement->SetValue("TRestGas");
                        }
                        TRestProcessRunnerElement->RemoveChild(
                            GetElementWithName("parameter", "gasFile", TRestProcessRunnerElement));
                    }
                }
                TRestRunElement->InsertEndChild(*addMetadataElement);
                TiXmlElement* tempelementptr = addMetadataElement;
                addMetadataElement = addMetadataElement->NextSiblingElement("addMetadata");
                TRestProcessRunnerElement->RemoveChild(tempelementptr);
            }

            debug << "handle \"addTask\"" << endl;
            TiXmlElement* addTaskElement = TRestProcessRunnerElement->FirstChildElement("addTask");
            while (addTaskElement != NULL) {
                fElement->InsertEndChild(*addTaskElement);
                TiXmlElement* tempelementptr = addTaskElement;
                addTaskElement = addTaskElement->NextSiblingElement("addTask");
                TRestProcessRunnerElement->RemoveChild(tempelementptr);
            }

            debug << "update parameters" << endl;
            // add parameter "inputAnalysis" for the TRestProcessRunnerElement
            TiXmlElement* parele = new TiXmlElement("parameter");
            parele->SetAttribute("name", "inputAnalysis");
            parele->SetAttribute("value", "on");
            TRestProcessRunnerElement->InsertBeforeChild(TRestProcessRunnerElement->FirstChildElement(),
                                                         *parele);

            // some parameters should be translated
            TString analysisString = GetParameter("pureAnalysisOutput", TRestProcessRunnerElement, "OFF");
            if (analysisString == "ON" || analysisString == "On" || analysisString == "on") {
                parele->SetAttribute("name", "inputEvent");
                parele->SetAttribute("value", "off");
                TRestProcessRunnerElement->InsertBeforeChild(TRestProcessRunnerElement->FirstChildElement(),
                                                             *parele);

                parele->SetAttribute("name", "outputEvent");
                parele->SetAttribute("value", "off");
                TRestProcessRunnerElement->InsertBeforeChild(TRestProcessRunnerElement->FirstChildElement(),
                                                             *parele);
            } else {
                parele->SetAttribute("name", "inputEvent");
                parele->SetAttribute("value", "on");
                TRestProcessRunnerElement->InsertBeforeChild(TRestProcessRunnerElement->FirstChildElement(),
                                                             *parele);

                parele->SetAttribute("name", "outputEvent");
                parele->SetAttribute("value", "on");
                TRestProcessRunnerElement->InsertBeforeChild(TRestProcessRunnerElement->FirstChildElement(),
                                                             *parele);
            }
            TiXmlElement* inputFileElement =
                GetElementWithName("parameter", "inputFile", TRestProcessRunnerElement);
            if (inputFileElement != NULL) {
                TRestRunElement->InsertBeforeChild(TRestRunElement->FirstChildElement(), *inputFileElement);
                TRestProcessRunnerElement->RemoveChild(inputFileElement);
            }

            if (TRestProcessRunnerElement->FirstChildElement("readoutPlane") != NULL) {
                warning << "REST WARNING. TRestManager. Readout plane re-definition is "
                           "not supported currently!"
                        << endl;
            }

            if (fVerboseLevel >= REST_Debug) {
                cout << "updated TRestManager section:" << endl;
                fElement->Print(stdout, 0);
                cout << endl;
                GetChar();
            }
        }

        if (fElement->FirstChildElement("TRestAnalysisPlot") != NULL &&
            fElement->FirstChildElement("TRestRun") == NULL &&
            fElement->FirstChildElement("addTask") == NULL) {
            cout << "old style config file of restPlots is detected!" << endl;
            cout << "trying to update the xml element..." << endl;
            cout << endl;

            char* name;
            name = getenv("plotsectionname");
            if (name == NULL) {
                name = const_cast<char*>(fElement->FirstChildElement("TRestAnalysisPlot")->Attribute("name"));
            }
            if (name == NULL) {
                fElement->FirstChildElement("TRestAnalysisPlot")->SetAttribute("name", "myplot");
                name = (char*)"myplot";
            }

            TiXmlElement* ele = new TiXmlElement("addTask");
            ele->SetAttribute("command", (string)name + "->PlotCombinedCanvas()");
            ele->SetAttribute("value", "ON");

            fElement->LinkEndChild(ele);

            if (fVerboseLevel >= REST_Debug) {
                cout << "updated TRestManager section:" << endl;
                fElement->Print(stdout, 0);
                cout << endl;
                GetChar();
            }
        }

        return true;
    }

    return false;
}

string RmlUpdateTool::WriteFile() {
    string name = Replace(fFileName, ".rml", "");

    TiXmlDocument doc;

    TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");

    doc.LinkEndChild(decl);

    doc.LinkEndChild(fElement);

    doc.SaveFile(name + "_updated.rml");

    return name + "_updated.rml";
}
