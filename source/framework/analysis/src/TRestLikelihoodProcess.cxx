/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
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

//////////////////////////////////////////////////////////////////////////
///
/// The TRestLikelihoodProcess can extract probability density functions (pdf) for several observables
/// from a .root file and compute a new observable (some kind of likelihood) from them.
///
/// First observable added is **LogOdds** from:
///
/// GARCÍA PASCUAL, Juan Antonio. Solar Axion search with Micromegas Detectors in the CAST Experiment
/// with 3He as buffer gas. Tesis Doctoral. Universidad de Zaragoza, Prensas de la Universidad.
///
/// It is the sum for all observables of log(1.-p)-log(p) (p = porb of observable i)
///
/// Input parameters:
/// * **fFileWithPdfs* -> String with file name. Root file with pdf as histograms. Name of the histo
/// has to be the same as the observable.
/// * **fLikelihoodObservables** -> String with observables to compute LogOdds, separated by ','
///                               If value set to "all" or "" then all observables in fFileWithPdfs
///                               are considered.
///
///
/// Example for rml files:
///    <addProcess type="TRestLikelihoodProcess" name="likelihood" verboseLevel="info" value="ON" >
///        <parameter name="likelihoodObservables" value="rawAna_RiseTimeAvg,rawAna_BaseLineMean"/>
///        <parameter name="fileWithPdfs" value="R01876_Histos_8keV.root"/>
///    </addProcess>
///
/// <hr>
///
/// \warning **? REST is under continous development.** This documentation
/// is offered to you by the REST community. Your HELP is needed to keep this code
/// up to date. Your feedback will be worth to support this software, please report
/// any problems/suggestions you may find while using it at [The REST Framework
/// forum](http://ezpc10.unizar.es). You are welcome to contribute fixing typos,
/// updating information or adding/proposing new contributions. See also our
/// <a href="https://github.com/rest-for-physics/framework/blob/master/CONTRIBUTING.md">Contribution
/// Guide</a>.
///
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2022-Dec: First template: read probabilities and compute likelihood
///              David Diez
///
/// \class      TRestLikelihoodProcess
/// \author     David Diez
///
/// <hr>
///

#include "TRestLikelihoodProcess.h"

using namespace std;
ClassImp(TRestLikelihoodProcess);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestLikelihoodProcess::TRestLikelihoodProcess() { Initialize(); }

///////////////////////////////////////////////
/// \brief Constructor loading data from a config file
///
/// If no configuration path is defined using TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or
/// relative.
///
/// The default behaviour is that the config file must be specified with
/// full path, absolute or relative.
///
/// \param configFilename A const char* giving the path to an RML file.
///
TRestLikelihoodProcess::TRestLikelihoodProcess(const char* configFilename) {
    Initialize();

    if (LoadConfigFromFile(configFilename)) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name
///
/// In this case we re-use the initialization of TRestRawToSignalProcess
/// interface class.
///
void TRestLikelihoodProcess::Initialize() {
    SetSectionName(this->ClassName());

    fEvent = nullptr;
}

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
///
void TRestLikelihoodProcess::LoadDefaultConfig() { SetTitle("Default config"); }

///////////////////////////////////////////////
/// \brief Process initialization.
///
///
void TRestLikelihoodProcess::InitProcess() {
    RESTDebug << "Starting InitProcess()" << RESTendl;

    if (!fFileHistos) {
        RESTDebug << "First time file is opened" << RESTendl;
        fFileHistos = TFile::Open(fFileWithPdfs.c_str());
        if (!fFileHistos || fFileHistos->IsZombie()) {
            RESTDebug << "Zombie" << RESTendl;
            std::cerr << "Error opening file " << fFileWithPdfs << endl;
            exit(-1);
        } else {
            // String with observables to vector of strings
            std::string line;
            std::istringstream ss(fLikelihoodObservables);

            // Read all observables in pdf file
            if (fLikelihoodObservables == "all" || fLikelihoodObservables == "") {
                TList* list = fFileHistos->GetListOfKeys();
                if (!list) {
                    printf("<E> No keys found in file\n");
                    exit(-1);
                }
                TIter next(list);

                while (TObject* obj = next()) {
                    RESTDebug << obj->GetName() << RESTendl;
                    // Check it is a TH1
                    if (!(fFileHistos->Get(obj->GetName())->InheritsFrom("TH1"))) {
                        printf("Object %s is not 1D histogram : will not be converted\n", obj->GetName());
                    } else {
                        fVectorObservables.push_back(obj->GetName());
                        RESTDebug << "Reading all observables" << RESTendl;
                    }
                }
            }
            // Only selected observables
            else {
                while (std::getline(ss, line, ',')) {
                    fVectorObservables.push_back(line);
                    RESTDebug << "Reading observables" << RESTendl;
                }
            }
        }
    }
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestLikelihoodProcess::ProcessEvent(TRestEvent* inputEvent) {
    fEvent = inputEvent;

    // Look for prob of each observable on their pdfs and compute likelihood //

    // Loop over observable list fVectorObservables
    // Search its histogram
    // Compute the prob asociated to its value
    // Add log(1-p)-log(p)
    // Include the log observable in AnalysisTree

    int bin;
    double p, logOdds = 0;
    TH1* histo;

    for (vector<int>::size_type i = 0; i < fVectorObservables.size(); i++) {
        histo = fFileHistos->Get<TH1>(fVectorObservables[i].c_str());
        bin = histo->GetXaxis()->FindBin(fAnalysisTree->GetObservableValue<double>(fVectorObservables[i]));
        p = fFileHistos->Get<TH1>(fVectorObservables[i].c_str())->GetBinContent(bin);
        logOdds += log(1. - p) - log(p);
    }
    RESTDebug << logOdds << RESTendl;
    fAnalysisTree->SetObservableValue("LogOdds", logOdds);

    // If cut condition matches the event will be not registered.
    if (ApplyCut()) return nullptr;

    return fEvent;
}

///////////////////////////////////////////////
/// \brief Prints on screen the process data members
///
void TRestLikelihoodProcess::PrintMetadata() {
    BeginPrintProcess();

    RESTMetadata << "File with PDFs: " << fFileWithPdfs << RESTendl;
    RESTMetadata << "Observables used for log-odds: " << RESTendl;
    for (std::string i : fVectorObservables) {
        RESTMetadata << i << RESTendl;
    }

    EndPrintProcess();
}
