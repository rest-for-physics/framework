///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestMuonAnalysisProcess.h
///
///_______________________________________________________________________________

#ifndef _TRestMuonAnalysisProcess
#define _TRestMuonAnalysisProcess

#include "TRest2DHitsEvent.h"
#include "TRestEventProcess.h"

#define stripWidth 3

class TRestMuonAnalysisProcess : public TRestEventProcess {
   private:
    // We define specific input/output event data holders
    TRest2DHitsEvent* fAnaEvent;  //!

    TH1D* hxzt;  //!
    TH1D* hyzt;  //!
    TH1D* hxzr;  //!
    TH1D* hyzr;  //!

    TH1D* hdiffz;  //!
    TF1* fdiffz;   //!

    vector<TVector3> fHough_XZ;  // y=ax+b, vertical line angle 牟, length 老,
                                 // [id][老,牟,weight]
    vector<TVector3> fHough_YZ;  // y=ax+b, vertical line angle 牟, length 老,
                                 // [id][老,牟,weight]

    int X1;    //!
    int X2;    //!
    int Y1;    //!
    int Y2;    //!
    TF1* fxz;  //!
    TF1* fyz;  //!

    int nummudeposxz;  //!
    int nummudeposyz;  //!
    // int numsmearxy;//!
    // int numsmearz;//!
    TH1D* mudeposxz;  //!
    TH1D* mudeposyz;  //!

    TH1D* mudepos;   //!
    TH1D* mutanthe;  //!

    TH2D* muhitmap;  //!
    TH2D* muhitdir;  //!

    Double_t fHoughSigmaLimit;
    Double_t fPeakPointRateLimit;

    map<int, vector<double>> musmearxy;       // [z index][single strip ene]
    map<int, pair<double, double>> musmearz;  // [z index][sigma, chi2]

    void InitFromConfigFile();

    void Initialize();

    // add here the metadata members of your event process
    // You can just remove fMyProcessParameter
    Int_t fMyDummyParameter;

   protected:
   public:
    void InitProcess();

    TRestEvent* ProcessEvent(TRestEvent* eventInput);

    void EndProcess();

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << "Hough sigma threshold for muon tracks: " << fHoughSigmaLimit << endl;
        metadata << "Peak point rate limit: " << fPeakPointRateLimit << endl;

        EndPrintProcess();
    }

    TRest2DHitsEvent* MakeTag();

    void MuDepos(TRest2DHitsEvent* eve);

    double ProjectionToCenter(double x, double y, double xzthe, double yzthe);

    double DistanceToTrack(double x, double z, double x0, double z0, double theta);

    map<int, vector<double>> GetdiffXY() { return musmearxy; }
    map<int, pair<double, double>> GetdiffZ() { return musmearz; }

    // Constructor
    TRestMuonAnalysisProcess();
    // Destructor
    ~TRestMuonAnalysisProcess();

    ClassDef(TRestMuonAnalysisProcess, 2);  // Template for a REST "event process" class inherited from
                                            // TRestEventProcess
};
#endif
