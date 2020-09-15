//
// Created by Luis Obis on 15/09/2020.
//

#ifndef REST_TRESTGEANT4VETOANALYSISPROCESS_H
#define REST_TRESTGEANT4VETOANALYSISPROCESS_H

#include <TRestG4Event.h>
#include <TRestG4Metadata.h>

#include "TRestEventProcess.h"

class TRestGeant4VetoAnalysisProcess : public TRestEventProcess {
   private:
    /// A pointer to the specific TRestG4Event input
    TRestG4Event* fInputG4Event;  //!
    /// A pointer to the specific TRestG4Event output
    TRestG4Event* fOutputG4Event;  //!
    /// A pointer to the simulation metadata information accessible to TRestRun
    TRestG4Metadata* fG4Metadata;  //!

    std::vector<int> fVetoVolumeIds; //!
    string fVetoKeyword = ""; //!
    std::vector<string> fVetoGroupKeywords; //!
    std::map<string, std::vector<string>> fVetoGroupVolumeNames; //!
    std::vector<Float_t> fQuenchingFactors; //!

    void InitFromConfigFile();
    void Initialize();
    void LoadDefaultConfig();

    // clean string (https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring)
    inline std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v")
    {
        s.erase(s.find_last_not_of(t) + 1);
        return s;
    }
    // trim from beginning of string (left)
    inline std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v")
    {
        s.erase(0, s.find_first_not_of(t));
        return s;
    }
    // trim from both ends of string (right then left)
    inline std::string& trim(std::string& s, const char* t = " \t\n\r\f\v")
    {
        return ltrim(rtrim(s, t), t);
    }

    // final clean string: trim and UPPER
    inline std::string& clean_string(std::string& s){
        s = trim(s);
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
    }
   protected:
    // add here the members of your event process

   public:
    any GetInputEvent() { return fInputG4Event; }
    any GetOutputEvent() { return fOutputG4Event; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    /// It prints out the process parameters stored in the metadata structure
    void PrintMetadata() {
        BeginPrintProcess();

        cout << "VETO KEYWORD: " << fVetoKeyword << endl;
        cout << endl;

        cout << "VETO GROUP KEYWORDS:" << endl;
        for (unsigned int i = 0; i < fVetoGroupKeywords.size(); i++) {
            cout << "\t" << fVetoGroupKeywords[i] << endl;
        }
        cout << endl;

        cout << "Found " << fVetoVolumeIds.size() << " veto volumes:" << endl;
        for (unsigned int i = 0; i < fVetoVolumeIds.size(); i++) {
            cout << "\t" << fG4Metadata->GetActiveVolumeName(fVetoVolumeIds[i]) << endl;
        }
        cout << endl;

        cout << "GROUPS:" << endl;
        for ( const auto &pair : fVetoGroupVolumeNames ) {
            cout << "GROUP " << pair.first << " (" << pair.second.size() << " volumes):\n";
            for (unsigned int i = 0; i < pair.second.size(); i++) {
                cout << "\t" << pair.second[i] << endl;
            }
        }
        cout << endl;

        cout << "QUENCHING FACTORS (" << fQuenchingFactors.size() << " Total)" << endl;
        for (unsigned int i = 0; i < fQuenchingFactors.size(); i++) {
            cout << "\t" << fQuenchingFactors[i] << endl;
        }
        cout << endl;

        EndPrintProcess();
    }

    /// Returns a new instance of this class
    TRestEventProcess* Maker() { return new TRestGeant4VetoAnalysisProcess; }
    /// Returns the name of this process
    TString GetProcessName() { return (TString) "geant4VetoAnalysis"; }

    TRestGeant4VetoAnalysisProcess();
    TRestGeant4VetoAnalysisProcess(char* cfgFileName);
    ~TRestGeant4VetoAnalysisProcess();

   ClassDef(TRestGeant4VetoAnalysisProcess, 1);
};
#endif  // REST_TRESTGEANT4VETOANALYSISPROCESS_H
