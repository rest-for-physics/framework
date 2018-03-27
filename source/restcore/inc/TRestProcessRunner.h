#ifndef RestCore_TRestProcessRunner
#define RestCore_TRestProcessRunner

#include "TRestEvent.h"
#include "TRestMetadata.h"
#include "TRestEventProcess.h"
#include "TRestRun.h"
#include "TRestAnalysisTree.h"
#include <mutex>
#include <thread>
#ifdef WIN32
#include <conio.h>
#else

#include <stdio.h>  
#include <termios.h>  
#include <unistd.h>  
#include <fcntl.h> 
#include <termio.h>

inline int kbhit(void)
{
	struct termios oldt, newt;
	int ch;
	int oldf;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);
	if (ch != EOF)
	{
		ungetc(ch, stdin);
		return 1;
	}
	return 0;
}

inline int getch(void)
{
	struct termios tm, tm_old;
	int fd = 0, ch;

	if (tcgetattr(fd, &tm) < 0)
	{
		return -1;
	}

	tm_old = tm;
	cfmakeraw(&tm);
	if (tcsetattr(fd, TCSANOW, &tm) < 0)
	{
		return -1;
	}

	ch = getchar();
	if (tcsetattr(fd, TCSANOW, &tm_old) < 0)
	{
		return -1;
	}

	return ch;
}

#endif // WIN32

class TRestThread;
class TRestManager;

enum ProcStatus {
	kNormal,//!< normal processing
	kPause,//!< pausing the process, showing a menu
	kStep, //!< step to next event. this state will then fall back to kPause
	kStop,//!< stopping the process, files will be saved
	kIgnore,//!< do not response to keyboard orders
};

class TRestProcessRunner :public TRestMetadata {
public:
	/// REST run class
	ClassDef(TRestProcessRunner, 1);

	TRestProcessRunner();
	~TRestProcessRunner();
	void Initialize();
	void BeginOfInit();
	Int_t ReadConfig(string keydeclare, TiXmlElement* e);
	void EndOfInit();

	//core functionality
	void ReadProcInfo();
	void RunProcess();
	void PauseMenu();
	Int_t GetNextevtFunc(TRestEvent* targetevt, TRestAnalysisTree* targettree);
	void FillThreadEventFunc(TRestThread* t);
	void WriteThreadFileFunc(TRestThread* t);
	void ConfigOutputFile();


	//tools
	void ResetRunTimes();
	TRestEventProcess* InstantiateProcess(TString type, TiXmlElement* ele);
	void PrintProcessedEvents(Int_t rateE);
	void SetProcStatus(ProcStatus s) { fProcStatus = s; }


	//getters and setters
	TRestEvent* GetInputEvent();
	TFile* GetTempOutputDataFile() { return fTempOutputDataFile; }
	string GetProcInfo(string infoname) { return ProcessInfo[infoname] == "" ? infoname : ProcessInfo[infoname]; }
	TRestAnalysisTree* GetAnalysisTree();
private:
	//global variable
	TRestRun *fRunInfo;//!


	//event variables
	TRestEvent *fInputEvent;//!
	TRestEvent *fOutputEvent;//!

	//self variables for processing
	vector<TRestThread*> fThreads;//!
	TFile* fTempOutputDataFile;//!
	TRestAnalysisTree* fEventTree;//!
	TRestAnalysisTree* fAnalysisTree;//!
	ProcStatus fProcStatus;
	Int_t nBranches;
	Int_t fThreadNumber;
	Int_t fProcessNumber;
	Int_t firstEntry;
	Int_t eventsToProcess;
	Int_t fProcessedEvents;
	map<string, string> ProcessInfo;
	vector<string> fOutputItem;





};

#endif