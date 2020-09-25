/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2020 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

#ifndef RestCore_TRestGeant4VetoAnalysisProcess
#define RestCore_TRestGeant4VetoAnalysisProcess

#include <TRestGeant4Event.h>
#include <TRestGeant4Metadata.h>

#include "TRestEventProcess.h"

class TRestGeant4VetoAnalysisProcess : public TRestEventProcess {
   private:
    /// A pointer to the specific TRestGeant4Event input
    TRestGeant4Event* fInputG4Event;  //!
    /// A pointer to the specific TRestGeant4Event output
    TRestGeant4Event* fOutputG4Event;  //!
    /// A pointer to the simulation metadata information accessible to TRestRun
    TRestGeant4Metadata* fG4Metadata;  //!

    std::vector<int> fVetoVolumeIds;                              //!
    string fVetoKeyword = "";                                     //!
    std::vector<string> fVetoGroupKeywords;                       //!
    std::map<string, std::vector<string>> fVetoGroupVolumeNames;  //!
    std::vector<Float_t> fQuenchingFactors;                       //!

    void InitFromConfigFile();
    void Initialize();
    void LoadDefaultConfig();

    // clean string (https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring)
    inline std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v") {
        s.erase(s.find_last_not_of(t) + 1);
        return s;
    }
    // trim from beginning of string (left)
    inline std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v") {
        s.erase(0, s.find_first_not_of(t));
        return s;
    }
    // trim from both ends of string (right then left)
    inline std::string& trim(std::string& s, const char* t = " \t\n\r\f\v") { return ltrim(rtrim(s, t), t); }

    // final clean string: trim and UPPER
    inline std::string& clean_string(std::string& s) {
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

        debug << "VETO KEYWORD: " << fVetoKeyword << endl;
        debug << endl;

        debug << "VETO GROUP KEYWORDS:" << endl;
        for (unsigned int i = 0; i < fVetoGroupKeywords.size(); i++) {
            debug << "\t" << fVetoGroupKeywords[i] << endl;
        }
        debug << endl;

        debug << "Found " << fVetoVolumeIds.size() << " veto volumes:" << endl;
        for (unsigned int i = 0; i < fVetoVolumeIds.size(); i++) {
            debug << "\t" << fG4Metadata->GetActiveVolumeName(fVetoVolumeIds[i]) << endl;
        }
        debug << endl;

        debug << "GROUPS:" << endl;
        for (const auto& pair : fVetoGroupVolumeNames) {
            debug << "GROUP " << pair.first << " (" << pair.second.size() << " volumes):\n";
            for (unsigned int i = 0; i < pair.second.size(); i++) {
                debug << "\t" << pair.second[i] << endl;
            }
        }
        debug << endl;

        debug << "QUENCHING FACTORS (" << fQuenchingFactors.size() << " Total)" << endl;
        for (unsigned int i = 0; i < fQuenchingFactors.size(); i++) {
            debug << "\t" << fQuenchingFactors[i] << endl;
        }
        debug << endl;

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
#endif  // RestCore_TRestGeant4VetoAnalysisProcess
