///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalTo2DHitsProcess.h
///
///_______________________________________________________________________________

#ifndef RESTCore_TRestSignalTo2DHitsProcess
#define RESTCore_TRestSignalTo2DHitsProcess

#include "TRest2DHitsEvent.h"
#include "TRestEventProcess.h"
#include "TRestSignalEvent.h"

class TRestSignalTo2DHitsProcess : public TRestEventProcess {
   private:
    // We define specific input/output event data holders
    TRestSignalEvent* fInputSignalEvent;   //!
    TRest2DHitsEvent* fOutput2DHitsEvent;  //!

    TRestReadout* fReadout;  //!

    void InitFromConfigFile();

    void Initialize();

    // add here the metadata members of your event process
    // You can just remove fMyProcessParameter
    // int fNoiseReductionLevel;//0: no reduction, 1: subtract baseline, 2:
    // subtract baseline plus threshold

    string fSelection;  // 0: uses all, 1: muon, 2: strong electron, 3: weak
                        // electron, 4: firing, 5: abnormal, 6: pile up, 9: other

    Double_t fHoughSigmaLimit;     
    Double_t fPeakPointRateLimit;  

    vector<TVector3> fHough_XZ;  // y=ax+b, vertical line angle 牟, length 老,
                                 // [id][老,牟,weight]
    vector<TVector3> fHough_YZ;  // y=ax+b, vertical line angle 牟, length 老,
                                 // [id][老,牟,weight]

    // int longmunumxz;//!
    // int longmunumyz;//!
    // TH1D* mudeposxz;//!
    // TH1D* mudeposyz;//!

    TH1D* hxzt;  //!
    TH1D* hyzt;  //!
    TH1D* hxzr;  //!
    TH1D* hyzr;  //!
    TF1* fxz;    //!
    TF1* fyz;    //!

    int X1;  //!
    int X2;  //!
    int Y1;  //!
    int Y2;  //!

   protected:
   public:
    void InitProcess();

    TRestEvent* ProcessEvent(TRestEvent* eventInput);

    void MakeCluster();

    TRest2DHitsEvent* SelectTag();

    void MuDepos(TRest2DHitsEvent* eve);

    void EndProcess();

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << "Hough sigma threshold for muon tracks: " << fHoughSigmaLimit << endl;
        metadata << endl;
        metadata << "Event selection: " << fSelection << endl;
        metadata << "0: uses all, 1: muon, 2: strong electron, 3: weak electron, "
                    "4: firing, 5: abnormal, 9: other"
                 << endl;

        EndPrintProcess();
    }

    // Constructor
    TRestSignalTo2DHitsProcess();
    // Destructor
    ~TRestSignalTo2DHitsProcess();

    ClassDef(TRestSignalTo2DHitsProcess,
             1);  // Template for a REST "event process" class inherited from
                  // TRestEventProcess
};
#endif
