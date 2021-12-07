#include "TRestDataBase.h"
#include "TRestManager.h"
#include "TRestProcessRunner.h"
#include "TRestStringOutput.h"

#include "TRestCut.h"

ClassImp(TRestCut);

//______________________________________________________________________________
TRestCut::TRestCut() { 
    Initialize();
}

void TRestCut::Initialize() {
    fCuts.clear();
}

void TRestCut::InitFromConfigFile() {
    auto ele = GetElement("cut");
    while (ele != NULL) {
        string name = GetParameter("name", ele, "");
        string cutStr = GetParameter("value", ele, "");
        cutStr = Replace(cutStr, " AND ", " && ");
        cutStr = Replace(cutStr, " OR ", " || ");
        AddCut(TCut(name.c_str(), cutStr.c_str()));
        ele = GetNextElement(ele);
    }
}

void TRestCut::AddCut(TCut cut) {
    if ((string)cut.GetName() == "") {
        warning << "TRestCut::AddCut: cannot add cut without name!" << endl;
    }
    if ((string)cut.GetTitle() == "") {
        warning << "TRestCut::AddCut: cannot add empty cut!" << endl;
    }
    for (auto c : fCuts) {
        if ((string)c.GetName() == (string)cut.GetName()) {
            warning << "TRestCut::AddCut: cut with name \"" << c.GetName() << "\" already added!" << endl;
            return;
        }
    }
    fCuts.push_back(cut);
}

TCut TRestCut::GetCut(string name) {
    for (auto c : fCuts) {
        if ((string)c.GetName() == name) {
            return c;
        }
    }
    return TCut();
}

void TRestCut::PrintMetadata() {
	TRestMetadata::PrintMetadata();
	metadata << " " << endl;
	metadata << "Number of TCut objects added: " << fCuts.size() << endl;
	metadata << " " << endl;
	metadata << "+++" << endl;
}

Int_t TRestCut::Write(const char* name, Int_t option, Int_t bufsize) {
    // write cuts to file.

    for (auto c : fCuts) {
        c.Write();
    }

    return TRestMetadata::Write(name, option, bufsize);

}