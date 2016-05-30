///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalShapingProcess.cxx
///
///             March 2016: Xinglong Li
///_______________________________________________________________________________


#include "TRestSignalShapingProcess.h"
using namespace std;

#include <TRestFFT.h>

#include <TFile.h>


ClassImp(TRestSignalShapingProcess)
    //______________________________________________________________________________
TRestSignalShapingProcess::TRestSignalShapingProcess()
{
    Initialize();
}

//______________________________________________________________________________
TRestSignalShapingProcess::TRestSignalShapingProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) == -1 )  LoadDefaultConfig( );

    PrintMetadata();
    // TRestSignalShapingProcess default constructor
}

//______________________________________________________________________________
TRestSignalShapingProcess::~TRestSignalShapingProcess()
{
    delete fOutputSignalEvent;
    delete fInputSignalEvent;
    // TRestSignalShapingProcess destructor

}



void TRestSignalShapingProcess::LoadDefaultConfig( )
{
    SetName( "signalShapingProcess-Default" );
    SetTitle( "Default config" );
}

//______________________________________________________________________________
void TRestSignalShapingProcess::Initialize()
{
    SetName( "signalShapingProcess" );

    fInputSignalEvent = new TRestSignalEvent();
    fOutputSignalEvent = new TRestSignalEvent();

    fInputEvent = fInputSignalEvent;
    fOutputEvent = fOutputSignalEvent;

}

void TRestSignalShapingProcess::LoadConfig( string cfgFilename, string name )
{
    if( LoadConfigFromFile( cfgFilename, name ) == -1 ) LoadDefaultConfig( );
}

//______________________________________________________________________________
void TRestSignalShapingProcess::InitProcess()
{
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    //Start by calling the InitProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::InitProcess();
   cout << __PRETTY_FUNCTION__ << endl;

   responseSignal = new TRestSignal();
   TString fullPathName = (TString) getenv("REST_PATH") + "/inputData/signal/" + fResponseFilename;
   
   TFile *f = new TFile(fullPathName);
   responseSignal = (TRestSignal *) f->Get("signal Response");
   f->Close();
}

//______________________________________________________________________________
void TRestSignalShapingProcess::BeginOfEventProcess() 
{
    fOutputSignalEvent->Initialize(); 
}

//______________________________________________________________________________
TRestEvent* TRestSignalShapingProcess::ProcessEvent( TRestEvent *evInput )
{

    fInputSignalEvent = (TRestSignalEvent *) evInput;

	//cout<<"Number of signals "<< fInputSignalEvent->GetNumberOfSignals()<< endl;

    if( fInputSignalEvent->GetNumberOfSignals() <= 0 ) return NULL;

    int i,j;
    int Tmin,Tmax,Ttmp;
    int Np,Nin,Nout;
    int Nr=responseSignal->GetNumberOfPoints();
    double *rsp=new double[Nr];

    for(i=0;i<Nr;i++) rsp[i]=responseSignal->GetData(i);

    for( int n = 0; n < fInputSignalEvent->GetNumberOfSignals(); n++ ) 
    {
        TRestSignal* inSignal;
        TRestSignal* shapingSignal=new TRestSignal();
        
        // Asign ID and shaping signals   
        inSignal = fInputSignalEvent->GetSignal(n);

        //read data of points
        //cout<<"number of points: "<<inSignal->GetNumberOfPoints()<<endl;
        Tmin=inSignal->GetTime(0);
        Np=inSignal->GetNumberOfPoints();
        Tmax=inSignal->GetTime(Np-1);

        Nin=Tmax-Tmin+1;
        Nout=Nin+Nr-1;

        double *in=new double[Nin];
        double *out=new double[Nout];

        for(i=0;i<Nin;i++) in[i]=0;
        for(i=0;i<Nout;i++) out[i]=0;

       // /*
        //read data
        for( int m=0; m < Np; m++)
        {
           Ttmp=inSignal->GetTime(m);
           for(j=0;j<Nin;j++)
           {
              if(j==Ttmp-Tmin) 
              {
                 in[j]=inSignal->GetData(m);
                 break;
              }
           }
        //   if(j==Nin) cout<<"======error======"<<endl;
        }
        //convolution
        for(i=0;i<Nout;i++)
           for(j=0; j<=i && j<Nin ;j++)
              if((i-j)<Nr) out[i] += in[j]*rsp[i-j];
            // if((i-j)<Nr) out[i] += in[j]*rsp[i-j]*chargeToAdcConst;//input from config file?

        for(i=0;i<Nout;i++) shapingSignal->AddPoint((double)i+Tmin,out[i]);

        shapingSignal->SetSignalID( inSignal->GetSignalID() );
//*/

        fOutputSignalEvent->AddSignal(*shapingSignal );

        delete shapingSignal;
        delete in;
        delete out;
    }

    delete rsp;

    return fOutputSignalEvent;
}

//______________________________________________________________________________
void TRestSignalShapingProcess::EndOfEventProcess() 
{

}

//______________________________________________________________________________
void TRestSignalShapingProcess::EndProcess()
{
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestSignalShapingProcess::InitFromConfigFile( )
{
   fResponseFilename = GetParameter("responseFile");
  // cout<<fResponseFilename<<endl;
}

