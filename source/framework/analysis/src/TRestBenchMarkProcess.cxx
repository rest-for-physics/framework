#include "TRestBenchMarkProcess.h"

#include "TRestManager.h"
#include "TRestProcessRunner.h"
#ifndef __APPLE__
#include "sys/sysinfo.h"
#endif
#include "sys/wait.h"

using namespace std;

ClassImp(TRestBenchMarkProcess);

thread* TRestBenchMarkProcess::fMonitorThread = nullptr;
int TRestBenchMarkProcess::fMonitorFlag = 0;
float TRestBenchMarkProcess::fCPUUsageInPct = 0;
float TRestBenchMarkProcess::fMemUsageInMB = 0;
float TRestBenchMarkProcess::fReadingInMBs = 0;
float TRestBenchMarkProcess::fProcessSpeedInHz = 0;
int TRestBenchMarkProcess::fLastEventNumber = 0;
ULong64_t TRestBenchMarkProcess::fStartTime = 0;

TRestBenchMarkProcess::TRestBenchMarkProcess() { Initialize(); }

void TRestBenchMarkProcess::Initialize() {
    fEvent = nullptr;
#ifndef __APPLE__
    fCPUNumber = get_nprocs_conf();
    fMemNumber = get_phys_pages();
#else
    fCPUNumber = 0;
    fMemNumber = 0;
    RESTError << "TRestBenchMarkProcess is not available yet on MACOS!!" << RESTendl;
#endif
    fPid = getpid();
    fRefreshRate = 10;
}

void TRestBenchMarkProcess::SysMonitorFunc(int pid, double refreshRate) {
    while (fMonitorFlag == 1) {
        string topOutput = TRestTools::Execute(Form("top -bn 1 -p %i | grep %i", pid, pid));
        auto topItems = Split(topOutput, " ", false, true);

        fCPUUsageInPct = StringToDouble(topItems[8]);
        fMemUsageInMB =
            StringToInteger(topItems[4]) + StringToInteger(topItems[5]) + StringToInteger(topItems[6]);
        fMemUsageInMB /= 1000;                                                          // convert kB to MB
        fReadingInMBs = fHostmgr->GetProcessRunner()->GetReadingSpeed() / 1024 / 1024;  // convert byte to MB
        int Neve = fHostmgr->GetProcessRunner()->GetNProcessedEvents();
        fProcessSpeedInHz = (Neve - fLastEventNumber) * refreshRate;

        usleep(1e6 / refreshRate);
    }
}

void TRestBenchMarkProcess::InitProcess() {
    if (fHostmgr == nullptr || fHostmgr->GetProcessRunner() == nullptr) {
        RESTError << "TRestBenchMarkProcess: the process is not hosted by TRestProcessRunner!" << RESTendl;
        exit(1);
    }

    // init external thread for system info service
    if (fMonitorThread == nullptr) {
        fMonitorFlag = 1;
        // SysMonitorFunc(fPid, fRefreshRate);
        fMonitorThread = new thread(&TRestBenchMarkProcess::SysMonitorFunc, this, fPid, fRefreshRate);
        fMonitorThread->detach();
    }

    fStartTime = chrono::high_resolution_clock::now().time_since_epoch().count();
}

TRestEvent* TRestBenchMarkProcess::ProcessEvent(TRestEvent* inputEvent) {
    fEvent = inputEvent;

    ULong64_t time = chrono::high_resolution_clock::now().time_since_epoch().count();
    SetObservableValue("RunningTime", (time - fStartTime) / 1e9);
    SetObservableValue("EventPerSecond", fProcessSpeedInHz);
    SetObservableValue("ReadingSpeedMBs", fReadingInMBs);
    SetObservableValue("CPUPrecentage", fCPUUsageInPct);
    SetObservableValue("MemoryUsedMB", fMemUsageInMB);

    return fEvent;
}

void TRestBenchMarkProcess::EndProcess() {
    if (fMonitorThread != nullptr) {
        fMonitorFlag = 0;
        usleep(1e6 / fRefreshRate * 2);
        // fMonitorThread->join();
        delete fMonitorThread;
        fMonitorThread = nullptr;
    }
}

void TRestBenchMarkProcess::PrintMetadata() {
    BeginPrintProcess();

    RESTMetadata << "REST pid: " << fPid << RESTendl;
    RESTMetadata << "Number of CPUs: " << fCPUNumber << RESTendl;
    RESTMetadata << "Total Memory: " << round((double)fMemNumber / 1024 / 1024 * 10) / 10 << " GB"
                 << RESTendl;
    RESTMetadata << "System information refresh rate: " << fRefreshRate << " Hz" << RESTendl;
    RESTMetadata << "Monitoring thread: " << fMonitorThread
                 << ", status: " << (fMonitorFlag == 0 ? "stopped" : "running") << RESTendl;

    EndPrintProcess();
}
