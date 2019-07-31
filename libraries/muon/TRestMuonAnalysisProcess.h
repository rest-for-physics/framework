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

    int X1;  //!
    int X2;  //!
    int Y1;  //!
    int Y2;  //!

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

        std::cout << "A dummy Process parameter : " << fMyDummyParameter << std::endl;

        EndPrintProcess();
    }

    double ProjectionToCenter(double x, double y, double xzthe, double yzthe);

    double DistanceToTrack(double x, double z, double x0, double z0, double theta);

    map<int, vector<double>> GetdiffXY() { return musmearxy; }
    map<int, pair<double, double>> GetdiffZ() { return musmearz; }

    // Constructor
    TRestMuonAnalysisProcess();
    // Destructor
    ~TRestMuonAnalysisProcess();

    ClassDef(TRestMuonAnalysisProcess,
             1);  // Template for a REST "event process" class inherited from
                  // TRestEventProcess
};
#endif
