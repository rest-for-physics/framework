///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRawSignalTo2DHitsProcess.h
///
///_______________________________________________________________________________

#ifndef RESTCore_TRestRawSignalTo2DHitsProcess
#define RESTCore_TRestRawSignalTo2DHitsProcess

#include "TRest2DHitsEvent.h"
#include "TRestEventProcess.h"
#include "TRestRawSignalEvent.h"
#include "TRestSignalEvent.h"

class TRestRawSignalTo2DHitsProcess : public TRestEventProcess {
   private:
    // We define specific input/output event data holders
    TRestRawSignalEvent* fInputSignalEvent;  //!
    TRest2DHitsEvent* fOutput2DHitsEvent;    //!

    TRestReadout* fReadout;  //!

    void InitFromConfigFile();

    void Initialize();

    // add here the metadata members of your event process
    // You can just remove fMyProcessParameter
    // int fNoiseReductionLevel;//0: no reduction, 1: subtract baseline, 2:
    // subtract baseline plus threshold

    string fSelection;  // 0: uses all, 1: muon, 2: strong electron, 3: weak
                        // electron, 4: firing, 5: abnormal, 6: pile up, 9: other

    TVector2 fBaseLineRange;       //!
    TVector2 fIntegralRange;       //!
    Double_t fPointThreshold;      //!
    Double_t fSignalThreshold;     //!
    Int_t fNPointsOverThreshold;   //!
    Double_t fHoughSigmaLimit;     //!
    Double_t fPeakPointRateLimit;  //!

    vector<TVector3> fHough_XZ;  // y=ax+b, vertical line angle ��, length ��,
                                 // [id][��,��,weight]
    vector<TVector3> fHough_YZ;  // y=ax+b, vertical line angle ��, length ��,
                                 // [id][��,��,weight]

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

    // observables
    double zlen;
    double xlen;
    double ylen;
    double firstx;
    double firsty;
    double firstz;
    double lastz;
    double ene;

    // double mutanthe;

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

        metadata << "Baseline range : ( " << fBaseLineRange.X() << " , " << fBaseLineRange.Y() << " ) "
                 << endl;
        metadata << "Integral range : ( " << fIntegralRange.X() << " , " << fIntegralRange.Y() << " ) "
                 << endl;
        metadata << "Point Threshold : " << fPointThreshold << " sigmas" << endl;
        metadata << "Signal threshold : " << fSignalThreshold << " sigmas" << endl;
        metadata << "Number of points over threshold : " << fNPointsOverThreshold << endl;
        metadata << "Hough sigma threshold for muon tracks: " << fHoughSigmaLimit << endl;
        metadata << endl;
        metadata << "Event selection: " << fSelection << endl;
        metadata << "0: uses all, 1: muon, 2: strong electron, 3: weak electron, "
                    "4: firing, 5: abnormal, 9: other"
                 << endl;

        EndPrintProcess();
    }

    // Constructor
    TRestRawSignalTo2DHitsProcess();
    // Destructor
    ~TRestRawSignalTo2DHitsProcess();

    ClassDef(TRestRawSignalTo2DHitsProcess,
             1);  // Template for a REST "event process" class inherited from
                  // TRestEventProcess
};
#endif
