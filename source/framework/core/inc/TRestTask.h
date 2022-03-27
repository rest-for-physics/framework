#ifndef RestCore_TRestTask
#define RestCore_TRestTask

#include <TCanvas.h>
#include <TF1.h>
#include <TGeoManager.h>
#include <TH2D.h>
#include <TInterpreter.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TObject.h>
#include <TROOT.h>
#include <TRandom.h>
#include <TStyle.h>

#include <iostream>
#include <string>

#include "TRestAnalysisTree.h"
#include "TRestRun.h"

class TRestManager;



enum REST_TASKMODE { TASK_ERROR = -1, TASK_MACRO = 0, TASK_CPPCMD = 1, TASK_CLASS = 2, TASK_SHELLCMD = 3 };

/// Wrapping REST macros into tasks
class TRestTask : public TRestMetadata {
   protected:
    TRestTask(TString TaskString, REST_TASKMODE mode = TASK_MACRO);

    int fNRequiredArgument;
    REST_TASKMODE fMode;
    std::string fInvokeObject = "";
    std::string fInvokeMethod = "";
    // indicates whether the argument is std::string/TString/const char *. If so, the value would be 1. We need to
    // add "" mark when constructing command. Otherwise the value is 0.
    std::vector<int> fArgumentTypes;     //!
    std::vector<std::string> fArgumentNames;  //!
    std::vector<std::string> fArgumentValues;
    std::string fConstructedCommand = "";

   public:
    // define default values here
    void InitFromConfigFile();

    void SetArgumentValue(std::vector<std::string> arg);

    static TRestTask* GetTaskFromMacro(TString Name);
    static TRestTask* GetTaskFromCommand(TString cmd);

    virtual void RunTask(TRestManager*);
    virtual void PrintArgumentHelp();

    void SetMode(REST_TASKMODE mod) { fMode = mod; }
    REST_TASKMODE GetMode() { return fMode; }

    // Constructor & Destructor
    TRestTask();
    ~TRestTask(){};

    ClassDef(TRestTask, 1);
};

#endif