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
/// The TRestRawSignalViewerProcess allows to display on screen the pulses
/// registered inside a TRestRawSignalEvent.
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2017-February: First implementation of Geant4 analysis process into REST_v2.
///             Javier Galan
///
/// \class      TRestRawSignalViewerProcess
/// \author     Javier Galan
///
/// <hr>
///
#include "TRestRawSignalViewerProcess.h"

#include <TLegend.h>
#include <TPaveText.h>

using namespace std;

ClassImp(TRestRawSignalViewerProcess);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestRawSignalViewerProcess::TRestRawSignalViewerProcess() { Initialize(); }

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
/// \param cfgFileName A const char* giving the path to an RML file.
///
TRestRawSignalViewerProcess::TRestRawSignalViewerProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName)) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestRawSignalViewerProcess::~TRestRawSignalViewerProcess() {}

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
///
void TRestRawSignalViewerProcess::LoadDefaultConfig() { SetTitle("Default config"); }

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name
///
void TRestRawSignalViewerProcess::Initialize() {
    SetSectionName(this->ClassName());

    fSignalEvent = NULL;

    fDrawRefresh = 0;

    fSingleThreadOnly = true;
}

///////////////////////////////////////////////
/// \brief Function to load the configuration from an external configuration
/// file.
///
/// If no configuration path is defined in TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or
/// relative.
///
/// \param cfgFileName A const char* giving the path to an RML file.
/// \param name The name of the specific metadata. It will be used to find the
/// correspondig TRestGeant4AnalysisProcess section inside the RML.
///
void TRestRawSignalViewerProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name)) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Process initialization. It creates the canvas available in TRestEventProcess
///
void TRestRawSignalViewerProcess::InitProcess() { this->CreateCanvas(); }

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestRawSignalViewerProcess::ProcessEvent(TRestEvent* evInput) {
    TString obsName;

    // no need for verbose copy now
    fSignalEvent = (TRestRawSignalEvent*)evInput;

    fCanvas->cd();
    eveCounter++;
    if (eveCounter >= fDrawRefresh) {
        eveCounter = 0;
        sgnCounter = 0;
        if (GetVerboseLevel() >= REST_Debug) {
            GetAnalysisTree()->PrintObservables();
        }
        for (unsigned int i = 0; i < fDrawingObjects.size(); i++) delete fDrawingObjects[i];
        fDrawingObjects.clear();

        TPad* pad2 = DrawSignal(sgnCounter);

        fCanvas->cd();
        pad2->Draw();
        fCanvas->Update();

        fout.setborder("");
        fout.setorientation(1);
        fout << "Press Enter to continue\nPress Esc to stop viewing\nPress n/p to "
                "switch signals"
             << endl;

        while (1) {
            int a = GetChar("");
            if (a == 10)  // enter
            {
                break;
            } else if (a == 27)  // esc
            {
                fDrawRefresh = 1e99;
                while (getchar() != '\n')
                    ;
                break;
            } else if (a == 110 || a == 78)  // n
            {
                sgnCounter++;
                if (sgnCounter >= 0 && sgnCounter < fSignalEvent->GetNumberOfSignals()) {
                    TPad* pad2 = DrawSignal(sgnCounter);
                    fCanvas->cd();
                    pad2->Draw();
                    fCanvas->Update();
                } else {
                    warning << "cannot plot signal with id " << sgnCounter << endl;
                }
            } else if (a == 112 || a == 80)  // p
            {
                sgnCounter--;
                if (sgnCounter >= 0 && sgnCounter < fSignalEvent->GetNumberOfSignals()) {
                    TPad* pad2 = DrawSignal(sgnCounter);
                    fCanvas->cd();
                    pad2->Draw();
                    fCanvas->Update();
                } else {
                    warning << "cannot plot signal with id " << sgnCounter << endl;
                }
            }
            while (getchar() != '\n')
                ;
        }
    }

    return fSignalEvent;
}

///////////////////////////////////////////////
/// \brief Function to include required actions after all events have been
/// processed.
///
void TRestRawSignalViewerProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

///////////////////////////////////////////////
/// \brief A helper method to draw signals in a pad
///
TPad* TRestRawSignalViewerProcess::DrawSignal(Int_t signal) {
    TPad* pad = new TPad(this->GetName(), this->GetTitle(), 0, 0, 1, 1);

    pad->cd();

    fDrawingObjects.push_back((TObject*)pad);

    TGraph* gr = new TGraph();
    fDrawingObjects.push_back((TObject*)gr);

    TRestRawSignal* sgnl = fSignalEvent->GetSignal(signal);

    info << "Drawing signal. Event ID : " << fSignalEvent->GetID() << " Signal ID : " << sgnl->GetID()
         << endl;

    for (int n = 0; n < sgnl->GetNumberOfPoints(); n++) gr->SetPoint(n, n, sgnl->GetData(n));

    gr->Draw("AC*");

    TGraph* gr2 = new TGraph();
    fDrawingObjects.push_back((TObject*)gr2);

    gr2->SetLineWidth(2);
    gr2->SetLineColor(2);

    for (int n = fBaseLineRange.X(); n < fBaseLineRange.Y(); n++)
        gr2->SetPoint(n - fBaseLineRange.X(), n, sgnl->GetData(n));

    gr2->Draw("CP");

    vector<Int_t> pOver = sgnl->GetPointsOverThreshold();

    TGraph* gr3[5];
    Int_t nGraphs = 0;
    gr3[nGraphs] = new TGraph();
    fDrawingObjects.push_back((TObject*)gr3[nGraphs]);
    gr3[nGraphs]->SetLineWidth(2);
    gr3[nGraphs]->SetLineColor(3);
    Int_t point = 0;
    Int_t nPoints = pOver.size();
    for (int n = 0; n < nPoints; n++) {
        gr3[nGraphs]->SetPoint(point, pOver[n], sgnl->GetData(pOver[n]));
        point++;
        if (n + 1 < nPoints && pOver[n + 1] - pOver[n] > 1) {
            gr3[nGraphs]->Draw("CP");
            nGraphs++;
            if (nGraphs > 4) cout << "Ngraphs : " << nGraphs << endl;
            point = 0;
            gr3[nGraphs] = new TGraph();
            fDrawingObjects.push_back((TObject*)gr3[nGraphs]);
            gr3[nGraphs]->SetLineWidth(2);
            gr3[nGraphs]->SetLineColor(3);
        }
    }

    if (nPoints > 0) gr3[nGraphs]->Draw("CP");

    return pad;
}

///////////////////////////////////////////////
/// \brief Function to read input parameters from the RML
/// TRestRawSignalViewerProcess metadata section
///
void TRestRawSignalViewerProcess::InitFromConfigFile() {
    fDrawRefresh = StringToDouble(GetParameter("refreshEvery", "0"));

    fBaseLineRange = StringTo2DVector(GetParameter("baseLineRange", "(5,55)"));

    fCanvasSize = StringTo2DVector(GetParameter("canvasSize", "(800,600)"));
}
