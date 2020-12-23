///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorTriggerAnalysisProcess.cxx
///
///
///             First implementation of Geant4 analysis process into REST_v2
///             Date : apr/2016
///             Author : J. Galan
///
///_______________________________________________________________________________

#include "TRestDetectorTriggerAnalysisProcess.h"
using namespace std;

ClassImp(TRestDetectorTriggerAnalysisProcess)
    //______________________________________________________________________________
    TRestDetectorTriggerAnalysisProcess::TRestDetectorTriggerAnalysisProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestDetectorTriggerAnalysisProcess::TRestDetectorTriggerAnalysisProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName)) LoadDefaultConfig();
}

//______________________________________________________________________________
TRestDetectorTriggerAnalysisProcess::~TRestDetectorTriggerAnalysisProcess() { delete fSignalEvent; }

void TRestDetectorTriggerAnalysisProcess::LoadDefaultConfig() { SetTitle("Default config"); }

//______________________________________________________________________________
void TRestDetectorTriggerAnalysisProcess::Initialize() {
    SetSectionName(this->ClassName());

    fSignalEvent = new TRestDetectorSignalEvent();

    fW = 0;
}

void TRestDetectorTriggerAnalysisProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name)) LoadDefaultConfig();
}

//______________________________________________________________________________
void TRestDetectorTriggerAnalysisProcess::InitProcess() {
    std::vector<string> fObservables;
    fObservables = TRestEventProcess::ReadObservables();

    for (unsigned int i = 0; i < fObservables.size(); i++)
        if (fObservables[i].find("Integral_Eth") != string::npos)
            fIntegralObservables.push_back(fObservables[i]);

    for (unsigned int i = 0; i < fIntegralObservables.size(); i++) {
        Double_t th = atof(fIntegralObservables[i].substr(12, fIntegralObservables[i].length() - 12).c_str());
        fThreshold.push_back(th);
    }

    TRestDetectorGas* gas = GetMetadata<TRestDetectorGas>();
    fW = gas->GetWvalue();
}

//______________________________________________________________________________
TRestEvent* TRestDetectorTriggerAnalysisProcess::ProcessEvent(TRestEvent* evInput) {
    TString obsName;

    TRestDetectorSignalEvent* fInputSignalEvent = (TRestDetectorSignalEvent*)evInput;

    /// Copying the signal event to the output event
    fSignalEvent->SetID(fInputSignalEvent->GetID());
    fSignalEvent->SetSubID(fInputSignalEvent->GetSubID());
    fSignalEvent->SetTimeStamp(fInputSignalEvent->GetTimeStamp());
    fSignalEvent->SetSubEventTag(fInputSignalEvent->GetSubEventTag());

    for (int sgnl = 0; sgnl < fInputSignalEvent->GetNumberOfSignals(); sgnl++)
        fSignalEvent->AddSignal(*fInputSignalEvent->GetSignal(sgnl));
    /////////////////////////////////////////////////

    vector<Double_t> integral;

    for (unsigned int i = 0; i < fIntegralObservables.size(); i++) integral.push_back(0);

    Int_t maxT = fSignalEvent->GetMaxTime() / fSampling;  // convert to time bin
    Int_t minT = fSignalEvent->GetMinTime() / fSampling;

    // cout << maxT << " " << minT << endl;

    Int_t triggerStarts = 0;

    unsigned int counter = 0;
    unsigned int nObs = fIntegralObservables.size();
    for (int i = minT - fADCLength / 2; i <= maxT && counter < nObs; i++) {
        Double_t en = fSignalEvent->GetIntegralWithTime(i * fSampling, (i + fADCLength / 2) * fSampling);

        for (unsigned int n = 0; n < nObs; n++)
            if (integral[n] == 0 && en > fThreshold[n]) {
                // We define the trigger start only for the first threshold definition
                if (n == 0) triggerStarts = i;
                integral[n] = fSignalEvent->GetIntegralWithTime(i * fSampling, (i + fADCLength) * fSampling);
            }

        // Break condition
        counter = 0;
        for (unsigned int n = 0; n < nObs; n++)
            if (integral[n] > 0) counter++;
    }

    for (unsigned int i = 0; i < nObs; i++) {
        SetObservableValue(fIntegralObservables[i], integral[i]);
    }

    Double_t full = fSignalEvent->GetIntegralWithTime(minT * fSampling - 1, maxT * fSampling + 1);

    SetObservableValue("RawIntegral", full);
    SetObservableValue("TriggerStarts", triggerStarts);

    if (fSignalEvent->GetNumberOfSignals() <= 0) return fSignalEvent;

    return fSignalEvent;
}

//______________________________________________________________________________
void TRestDetectorTriggerAnalysisProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestDetectorTriggerAnalysisProcess::InitFromConfigFile() {
    fSampling = GetDblParameterWithUnits("sampling", 0.2 * units("us"));
    fADCLength = StringToInteger(GetParameter("ADCLength", "512"));
}
