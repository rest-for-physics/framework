///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackEvent.h
///
///             Event class to store DAQ events either from simulation and
///             acquisition
///
///             oct 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier Gracia
///_______________________________________________________________________________

#ifndef RestDAQ_TRestTrackEvent
#define RestDAQ_TRestTrackEvent

#include <TAxis.h>
#include <TGraph.h>
#include <TGraph2D.h>
#include <TMultiGraph.h>
#include <TObject.h>
#include <TRestEvent.h>
#include <TRestTrack.h>
#include <TVirtualPad.h>

#include <iostream>

class TRestDetectorReadout;
class TRestTrackEvent : public TRestEvent {
   protected:
    Int_t fNtracks;
    Int_t fNtracksX;
    Int_t fNtracksY;
    Int_t fLevels;
    std::vector<TRestTrack> fTrack;  // Collection of tracks that define the event

#ifndef __CINT__
    // TODO These graphs should be placed in TRestTrack?
    // (following GetGraph implementation in TRestDetectorSignal)
    TGraph* fXYHit;       //!
    TGraph* fXZHit;       //!
    TGraph* fYZHit;       //!
    TGraph2D* fXYZHit;    //!
    TGraph* fXYTrack;     //!
    TGraph* fXZTrack;     //!
    TGraph* fYZTrack;     //!
    TGraph2D* fXYZTrack;  //!

    Bool_t fPrintHitsWarning;  //!
#endif

   public:
    TRestTrack* GetTrack(Int_t n) {
        if (fTrack.size() < n + 1) {
            std::cout << "-- Error : TRestTrackEvent::GetTrack. ERROR!" << std::endl;
            std::cout << "-- Error : GetTrack requested track with index n = " << n << std::endl;
            std::cout << "-- Error : However, only " << fTrack.size() << " where found inside TRestTrackEvent"
                      << std::endl;
            std::cout << "-- Error : This might be probably a crash ... !!" << std::endl;
            return NULL;
        }
        return &fTrack[n];
    }
    TRestTrack* GetTrackById(Int_t id);

    TRestTrack* GetOriginTrackById(Int_t tckId);
    TRestTrack* GetOriginTrack(Int_t tck);

    TRestTrack* GetMaxEnergyTrackInX();
    TRestTrack* GetMaxEnergyTrackInY();

    TRestTrack* GetMaxEnergyTrack(TString option = "");
    TRestTrack* GetSecondMaxEnergyTrack(TString option = "");

    Double_t GetMaxEnergyTrackVolume(TString option = "");
    Double_t GetMaxEnergyTrackLength(TString option = "");
    Double_t GetEnergy(TString option = "");

    Int_t GetLevel(Int_t tck);
    void SetLevels();
    Int_t GetLevels() { return fLevels; }

    TPad* DrawEvent(TString option = "");

    TPad* GetPad() { return fPad; }

    // Setters
    void AddTrack(TRestTrack* c);
    void RemoveTrack(int n);

    void RemoveTracks() { fTrack.clear(); }

    Bool_t isXYZ();
    Bool_t isTopLevel(Int_t tck);

    Int_t GetOriginTrackID(Int_t tck);

    void SetNumberOfXTracks(Int_t x) { fNtracksX = x; }
    void SetNumberOfYTracks(Int_t y) { fNtracksY = y; }

    // Getters
    Int_t GetNumberOfTracks(TString option = "");

    Int_t GetTotalHits();

    void Initialize();

    void PrintOnlyTracks();
    void PrintEvent(Bool_t fullInfo = false);

    // Construtor
    TRestTrackEvent();
    // Destructor
    virtual ~TRestTrackEvent();

    ClassDef(TRestTrackEvent, 1);  // REST event superclass
};
#endif
