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
/// TRestRealTimeDrawingProcess might be added at any stage of the data processing chain.
///
/// This process will draw and save analysis plots during processing, in case the process
/// chain takes long time to run while we want to see the result instantly. Supports
/// multithread.
///
/// For example, when the detector is taking data and we are using REST to process
/// data in real time. Then, for example, if we want to quickly know the baseline RMS,
/// we can add this process to the process chain. It will generate plot files every several
/// events, in which histogram will continuously grow.
///
/// *drawInterval*: How many events should passed when it starts next drawing, refreshing
/// the plots. =0 will draw only once at the end of the process.
///
/// *threadWaitTimeout*: In case the event triggering the draw operation is the last event
/// in the input file, processes of other threads may never get into ProcessEvent() and
/// flag themselves as paused, which will cause the process chian to never end. To prevent
/// this, we need to set a timeout.
///
/// *TRestAnalysisPlot*: sections to define TRestAnalysisPlot
///
/// The following code shows an example of implementation inside a
/// TRestProcessRunner RML section.
///
/// ```
///  <addProcess type="TRestRealTimeDrawingProcess" name="rD" value="ON" drawInterval="1000" >
///    <TRestAnalysisPlot name="BaselinePlot" title="Basic Plots" previewPlot="false">
///      <canvas size="(1000,800)" divide="(1,1)" save=""/>
///      <plot name="Baseline" title="Baseline average" xlabel="Baseline [ADC units]" ylabel="Counts"
///              logscale="false" value="ON" save="Baseline.png" >
///        <variable name="sAna_BaseLineMean" range="(0,1000)" nbins="100" />
///      </plot>
///    </TRestAnalysisPlot>
///    <TRestAnalysisPlot name="SpectrumPlot" title="Basic Plots" previewPlot="false">
///      <canvas size="(1000,800)" divide="(1,1)" save=""/>
///      <plot name="Spectrum" title="ADC energy spectrum" xlabel="Threshold integral energy [ADC units]"
///      ylabel="Counts"
///              logscale="false" value="ON" save="Spectrum.png" >
///        <variable name="sAna_ThresholdIntegral" range="(0,300000)" nbins="100" />
///      </plot>
///    </TRestAnalysisPlot>
///  </addProcess>
/// ```
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2020-Aug:  First implementation and concept
///             Ni Kaixiang
///
/// \class      TRestRealTimeDrawingProcess
/// \author     Ni Kaixiang
///
/// <hr>
///
#include "TRestRealTimeDrawingProcess.h"

#include "TRestMessenger.h"

#ifdef __APPLE__
#include <unistd.h>
#endif

using namespace std;

ClassImp(TRestRealTimeDrawingProcess);

Long64_t TRestRealTimeDrawingProcess::fLastDrawnEntry = 0;
map<TRestRealTimeDrawingProcess*, bool> TRestRealTimeDrawingProcess::fPauseResponse;
bool TRestRealTimeDrawingProcess::fPauseInvoke;
vector<string> TRestRealTimeDrawingProcess::fProcessesToDraw;
vector<TRestAnalysisPlot*> TRestRealTimeDrawingProcess::fPlots;

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestRealTimeDrawingProcess::TRestRealTimeDrawingProcess() { Initialize(); }

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestRealTimeDrawingProcess::~TRestRealTimeDrawingProcess() {}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name and library version
///
void TRestRealTimeDrawingProcess::Initialize() {
    fDrawInterval = 0;
    fThreadWaitTimeoutMs = 1000;
    fPlots.clear();
    fLastDrawnEntry = 0;
    fPauseResponse[this] = false;
    fPauseInvoke = false;
}

///////////////////////////////////////////////
/// \brief Function to use in initialization of process members before starting
/// to process the event
///
void TRestRealTimeDrawingProcess::InitProcess() {
    // For example, try to initialize a pointer to existing metadata
    // accessible from TRestRun

    if (fPlots.size() == 0) {
        TiXmlElement* ele = GetElement("TRestAnalysisPlot");
        while (ele != nullptr) {
            TRestAnalysisPlot* plt = new TRestAnalysisPlot();
            plt->SetHostmgr(this->fHostmgr);
            plt->LoadConfigFromElement(ele, fElementGlobal, fVariables);
            plt->SetName(plt->GetName() +
                         (TString)ToString(this));  // to prevent deleting canvas with same name
            fPlots.push_back(plt);

            ele = GetNextElement(ele);
        }
    }

    if (fProcessesToDraw.size() == 0) {
        TiXmlElement* ele = GetElement("ProcessDrawing");
        while (ele != nullptr) {
            string proc = GetParameter("processName", ele);
            if (GetFriendLive(proc) == nullptr) {
                RESTError << "TRestRealTimeDrawingProcess: cannot find process \"" << proc
                          << "\" to call drawing!" << RESTendl;
                exit(1);
            }
            fProcessesToDraw.push_back(proc);

            ele = GetNextElement(ele);
        }
    }
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestRealTimeDrawingProcess::ProcessEvent(TRestEvent* inputEvent) {
    fEvent = inputEvent;
    if (fDrawInterval == 0) {
        return fEvent;
    }
    if (GetFullAnalysisTree() == nullptr) {
        return fEvent;
    }

    // check the pause flag from other thread's TRestRealTimeDrawingProcess
    // need to pause during drawing, to avoid writing to AnalysisTree
    while (fPauseInvoke == true) {
        fPauseResponse[this] = true;
        usleep(1000);  // sleep 1 ms
    }
    fPauseResponse[this] = false;

    if (GetFullAnalysisTree()->GetEntries() >= fDrawInterval + fLastDrawnEntry) {
        fPauseInvoke = true;

        RESTInfo << "TRestRealTimeDrawingProcess: reached drawing flag. Waiting for other processes to pause"
                 << RESTendl;
        // wait while all other TRestRealTimeDrawingProcess is paused
        for (auto iter = fPauseResponse.begin(); iter != fPauseResponse.end(); iter++) {
            if (iter->first == this) continue;
            int i = 0;
            while (iter->second == false) {
                usleep(1000);  // sleep 1 ms
                i++;
                if (i > fThreadWaitTimeoutMs) {
                    // to prevent last event in the process chain
                    RESTWarning
                        << "TRestRealTimeDrawingProcess: waiting time reaches maximum, plotting job aborted"
                        << RESTendl;
                    fPauseInvoke = false;
                    return fEvent;
                }
            }
        }

        // starts drawing
        RESTInfo << "TRestRealTimeDrawingProcess: drawing..." << RESTendl;
        DrawOnce();

        fLastDrawnEntry = GetFullAnalysisTree()->GetEntries();
        fPauseInvoke = false;
    }

    return fEvent;
}

///////////////////////////////////////////////
/// \brief Function to use when all events have been processed
///
void TRestRealTimeDrawingProcess::EndProcess() {
    if (fPauseInvoke == false) {
        RESTInfo << "TRestRealTimeDrawingProcess: end drawing..." << RESTendl;
        DrawOnce();

        fPauseInvoke = true;
    }
}

void TRestRealTimeDrawingProcess::DrawOnce() {
    Long64_t totalentries = GetFullAnalysisTree()->GetEntries();
    for (unsigned int i = 0; i < fPlots.size(); i++) {
        fPlots[i]->SetTreeEntryRange(totalentries - fLastDrawnEntry, fLastDrawnEntry);
        fPlots[i]->PlotCombinedCanvas();
    }
    for (unsigned int i = 0; i < fProcessesToDraw.size(); i++) {
        GetFriendLive(fProcessesToDraw[i])->Draw();
    }
}

void TRestRealTimeDrawingProcess::DrawWithNotification() {
    auto messager = GetMetadata<TRestMessenger>();
    int runNumber = StringToInteger(GetParameter("runNumber"));
    if (runNumber == -1) {
        RESTError << "TRestRealTimeDrawingProcess::DrawWithNotification: runNumber must be given!"
                  << RESTendl;
        RESTError << "consider adding \"--d xx\" in restManager command" << RESTendl;
        abort();
    }
    while (true) {
        // consmue the message, take out from the message pool
        string message = messager->ConsumeMessage();
        if (message != "") {
            RESTInfo << "Recieveing message: " << message << RESTendl;
            if (TRestTools::fileExists(message) && TRestTools::isRootFile(message)) {
                TRestRun* run = new TRestRun(message);
                int _runNumber = run->GetRunNumber();
                delete run;
                if (_runNumber == runNumber) {
                    for (auto& plot: fPlots) {
                        plot->SetFile(message);
                        plot->PlotCombinedCanvas();
                    }
                } else {
                    // if the runnumber does not match, we put this message back to pool
                    // maybe other processes need it
                    RESTWarning << "file: " << message << RESTendl;
                    RESTWarning << "It is not the file we wanted! runNumber in file: " << _runNumber
                                << ", run we are processing: " << runNumber << RESTendl;
                    messager->SendMessage(message);
                }
            }
        }
        usleep(1000);
    }
}

///////////////////////////////////////////////
/// \brief It prints out the process parameters stored in the
/// metadata structure
///
void TRestRealTimeDrawingProcess::PrintMetadata() {
    BeginPrintProcess();

    RESTMetadata << "Number of AnalysisPlots added: " << fPlots.size() << RESTendl;
    for (auto p : fPlots) {
        RESTMetadata << p->GetName();
    }
    RESTMetadata << RESTendl;
    RESTMetadata << "Number of process plots added: " << fProcessesToDraw.size() << RESTendl;
    for (auto p : fProcessesToDraw) {
        RESTMetadata << p;
    }
    RESTMetadata << RESTendl;
    RESTMetadata << "Draw interval" << fDrawInterval << RESTendl;
    RESTMetadata << "Waiting time for other thread to stop " << fThreadWaitTimeoutMs << RESTendl;

    EndPrintProcess();
}
