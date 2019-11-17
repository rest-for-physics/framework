#include "TRestBenchMarkProcess.h"
#include "TRestManager.h"
#include "TRestProcessRunner.h"
#include "sys/sysinfo.h"
#include "sys/wait.h"

ClassImp(TRestBenchMarkProcess);

thread* TRestBenchMarkProcess::fMonitorThread = NULL;
int TRestBenchMarkProcess::fMonitorFlag = 0;
float TRestBenchMarkProcess::fCPUUsageInPct = 0;
float TRestBenchMarkProcess::fMemUsageInMB = 0;
float TRestBenchMarkProcess::fReadingInMBs = 0;
float TRestBenchMarkProcess::fProcessSpeedInHz = 0;
int TRestBenchMarkProcess::fLastEventNumber = 0;
ULong64_t TRestBenchMarkProcess::fStartTime = 0;

//______________________________________________________________________________
TRestBenchMarkProcess::TRestBenchMarkProcess() { Initialize(); }

void TRestBenchMarkProcess::InitFromConfigFile() {
    fRefreshRate = StringToDouble(GetParameter("RefreshRate", "10"));
}

void TRestBenchMarkProcess::Initialize() {
    fCPUNumber = get_nprocs_conf();
    fMemNumber = get_phys_pages();
    fPid = getpid();
    fRefreshRate = 10;
}

void TRestBenchMarkProcess::SysMonitorFunc(int pid, double refreshrate) {
    while (fMonitorFlag == 1) {
        string topOutput = TRestTools::Execute(Form("top -bn 1 -p %i | grep %i", pid, pid));
        auto topItems = Split(topOutput, " ", false, true);

        fCPUUsageInPct = StringToDouble(topItems[8]);
        fMemUsageInMB =
            StringToInteger(topItems[4]) + StringToInteger(topItems[5]) + StringToInteger(topItems[6]);
        fMemUsageInMB /= 1000;                                                          // convert kB to MB
        fReadingInMBs = fHostmgr->GetProcessRunner()->GetReadingSpeed() / 1024 / 1024;  // convert byte to MB
        int Neve = fHostmgr->GetProcessRunner()->GetNProcessedEvents();
        fProcessSpeedInHz = (Neve - fLastEventNumber) * refreshrate;

        usleep(1e6 / refreshrate);
    }
}

void TRestBenchMarkProcess::InitProcess() {
    if (fHostmgr == NULL || fHostmgr->GetProcessRunner() == NULL) {
        ferr << "TRestBenchMarkProcess: the process is not hosted by TRestProcessRunner!" << endl;
        exit(1);
    }

    // init external thread for system info service
    if (fMonitorThread == NULL) {
        fMonitorFlag = 1;
        // SysMonitorFunc(fPid, fRefreshRate);
        fMonitorThread = new thread(&TRestBenchMarkProcess::SysMonitorFunc, this, fPid, fRefreshRate);
        fMonitorThread->detach();
    }

    fStartTime = chrono::high_resolution_clock::now().time_since_epoch().count();
}

TRestEvent* TRestBenchMarkProcess::ProcessEvent(TRestEvent* eventInput) {
    fOutputEvent = eventInput;

    ULong64_t time = chrono::high_resolution_clock::now().time_since_epoch().count();
    SetObservableValue("RunningTime", (time - fStartTime) / 1e9);
    SetObservableValue("EventPerSecond", fProcessSpeedInHz);
    SetObservableValue("ReadingSpeedMBs", fReadingInMBs);
    SetObservableValue("CPUPrecentage", fCPUUsageInPct);
    SetObservableValue("MemoryUsedMB", fMemUsageInMB);

    return fOutputEvent;
}

void TRestBenchMarkProcess::EndProcess() {
    if (fMonitorThread != NULL) {
        fMonitorFlag = 0;
        // fMonitorThread->join();
        delete fMonitorThread;
        fMonitorThread = NULL;
    }
}

void TRestBenchMarkProcess::PrintMetadata() {
    BeginPrintProcess();

    metadata << "REST pid: " << fPid << endl;
    metadata << "Number of CPUs: " << fCPUNumber << endl;
    metadata << "Total Memory: " << round((double)fMemNumber / 1024 / 1024 * 10) / 10 << " GB" << endl;
    metadata << "System information refresh rate: " << fRefreshRate << " Hz" << endl;
    metadata << "Monitoring thread: " << fMonitorThread
             << ", status: " << (fMonitorFlag == 0 ? "stopped" : "running") << endl;

    EndPrintProcess();
}