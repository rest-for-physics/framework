///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestLinearTrackEvent.h
///
///             Event class to store linearized track events
///
///             Feb 2016:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier Galan
///
///_______________________________________________________________________________

#include "TRestLinearTrackEvent.h"
using namespace std;

ClassImp(TRestLinearTrackEvent)
    //______________________________________________________________________________
    TRestLinearTrackEvent::TRestLinearTrackEvent() {
    // TRestLinearTrackEvent default constructor
    Initialize();
}

//______________________________________________________________________________
TRestLinearTrackEvent::~TRestLinearTrackEvent() {
    // TRestLinearTrackEvent destructor
}

void TRestLinearTrackEvent::Initialize() {
    TRestEvent::Initialize();

    fPad = NULL;

    fLinearTrack.clear();
    fNTracks = 0;

    fMinLength = 1e10;
    fMaxLength = -1e10;
    fMinWidth = 1e10;
    fMaxWidth = -1e10;

    fMinLinearDeposit = 1e10;
    fMaxLinearDeposit = -1e10;
    fMinTransversalDeposit = 1e10;
    fMaxTransversalDeposit = -1e10;
}

void TRestLinearTrackEvent::PrintEvent(Bool_t fullInfo) {
    TRestEvent::PrintEvent();

    for (int tck = 0; tck < GetNumberOfTracks(); tck++) {
        cout << "+++++++++ Linear track : " << tck << " ++++++" << endl;
        fLinearTrack[tck].Print(fullInfo);
    }
}

// Draw current event in a Tpad
TPad* TRestLinearTrackEvent::DrawEvent(TString option) {
    if (fPad != NULL) {
        delete fPad;
        fPad = NULL;
    }

    int nTracks = this->GetNumberOfTracks();

    if (nTracks == 0) {
        cout << "Empty event " << endl;
        return NULL;
    }

    this->PrintEvent(true);

    fPad = new TPad(this->GetName(), " ", 0, 0, 1, 1);
    fPad->Divide(2, 2);
    fPad->cd();
    fPad->cd(1)->DrawFrame(GetMinLinearLength() - 10, 0, GetMaxLinearLength() + 10,
                           1.05 * GetMaxLinearDeposit());
    fPad->cd(2)->DrawFrame(0, 0, 1.05 * GetMaxTransversalLength(), 1.05 * GetMaxTransversalDeposit());
    fPad->cd(3)->DrawFrame(GetMinLinearLength() - 10, 0, GetMaxLinearLength() + 10,
                           1.05 * GetMaxLinearDeposit());
    fPad->cd(4)->DrawFrame(0, 0, 1.05 * GetMaxTransversalLength(), 1.05 * GetMaxTransversalDeposit());
    fPad->Draw();

    for (int tck = 0; tck < nTracks; tck++) {
        TGraph* linGr = fLinearTrack[tck].GetLinearGraph(tck + 2);
        TGraph* trGr = fLinearTrack[tck].GetTransversalGraph(tck + 2);

        if (fLinearTrack[tck].isXZ())
            fPad->cd(1);
        else
            fPad->cd(3);

        linGr->Draw("LP");

        if (fLinearTrack[tck].isXZ())
            fPad->cd(2);
        else
            fPad->cd(4);

        trGr->Draw("LP");
        fPad->Update();
    }

    fPad->Draw();

    return fPad;
}
