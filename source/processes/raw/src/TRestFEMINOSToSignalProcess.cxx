///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestFEMINOSToSignalProcess.cxx
///
///             Template to use to design "event process" classes inherited from 
///             TRestFEMINOSToSignalProcess
///             How to use: replace TRestFEMINOSToSignalProcess by your name, 
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             jun 2014:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Igor G. Irastorza
///_______________________________________________________________________________

//int counter = 0;

#include "TRestFEMINOSToSignalProcess.h"
using namespace std;
#include "TTimeStamp.h"

ClassImp(TRestFEMINOSToSignalProcess)
    //______________________________________________________________________________
TRestFEMINOSToSignalProcess::TRestFEMINOSToSignalProcess()
{
    Initialize();
}

TRestFEMINOSToSignalProcess::TRestFEMINOSToSignalProcess(char *cfgFileName):TRestRawToSignalProcess(cfgFileName)
{
    Initialize();
}


//______________________________________________________________________________
TRestFEMINOSToSignalProcess::~TRestFEMINOSToSignalProcess()
{
    // TRestFEMINOSToSignalProcess destructor
}

//______________________________________________________________________________
void TRestFEMINOSToSignalProcess::Initialize()
{

    //this->SetVerboseLevel(REST_Debug);

}

//______________________________________________________________________________
void TRestFEMINOSToSignalProcess::InitProcess()
{

    //Binary file header

    //The binary starts here
    char runUid[26],initTime[25];
    fread(runUid, 1, 26, fInputBinFile);

    sprintf(initTime,"%s",runUid);
    totalBytesReaded = sizeof(runUid);

    int year,day,month,hour,minute,second;
    sscanf(initTime,"%*c%*cR%d_%02d_%02d-%02d_%02d_%02d-%*d",&year,&month,&day,&hour,&minute,&second);
    TTimeStamp tS (year,month,day,hour,minute,second);  
    tStart = tS.AsDouble();
    //Timestamp of the run

    //NULL word
    unsigned short nullword;
    fread(&nullword, sizeof(nullword),1,fInputBinFile);
    if(this->GetVerboseLevel()==REST_Debug){
        cout<<"NULL word "<<nullword<<endl;
        printBits(nullword);
    }
    frameBits=0;
    totalBytesReaded+=sizeof(nullword);

    //Start of Data frame
    unsigned short startDF;
    fread(&startDF, sizeof(startDF),1,fInputBinFile);
    if(this->GetVerboseLevel()==REST_Debug){
        cout<<"PFX_START "<<startDF<<endl;
        printBits(startDF);
        cout<<" Version "<< ((startDF & 0x1E0) >> 5) <<endl;
        cout<<" FEC "<<(startDF & 0x1F)<<endl;
    }
    frameBits+=sizeof(startDF);
    totalBytesReaded+=sizeof(startDF);

    //Payload from start data frame to end of data frame (including both)
    fread(&pay, sizeof(pay),1,fInputBinFile);
    payload=pay;
    if(this->GetVerboseLevel()==REST_Debug){
        cout<<"Frame payload "<<payload<<endl;
        printBits(pay);
    }
    frameBits+=sizeof(pay);
    totalBytesReaded+=sizeof(pay);


}

//______________________________________________________________________________
TRestEvent* TRestFEMINOSToSignalProcess::ProcessEvent( TRestEvent *evInput )
{

    //Now we start reading the events

    unsigned short startEv;

    //Event header
    if(fread(&startEv, sizeof(startEv),1,fInputBinFile) !=1){
        fclose(fInputBinFile);
        cout<<"File END"<<endl;
        fOutputEvent = NULL;
        return NULL;//File end
    }
    if(this->GetVerboseLevel()==REST_Debug){
        cout<<"Start of Event "<<startEv<<endl;
        printBits(startEv);
        cout<<" Event type "<<(startEv & 0xF)<<endl;
    }
    frameBits+=sizeof(startEv);
    totalBytesReaded+=sizeof(startEv);

    /*
    counter++;
    if( counter % 100 == 0 ) cout << "Events read : " << counter << endl;
    */

    //TimeStamp of the event 48 bit word
    unsigned short tsh,tsm,tsl;
    fread(&tsh, sizeof(tsh),1,fInputBinFile);
    fread(&tsm, sizeof(tsm),1,fInputBinFile);
    fread(&tsl, sizeof(tsl),1,fInputBinFile);
    if(this->GetVerboseLevel()==REST_Debug){
        printBits(tsh);
        printBits(tsm);
        printBits(tsl);
        cout<<" TimeStamp "<<2147483648*tsl+32768*tsm+tsh<<endl;
    }

    frameBits+=sizeof(tsh)*3;
    totalBytesReaded+=sizeof(tsh)*3;

    //Event ID 32 bit word
    unsigned int evID;
    fread(&evID, sizeof(evID),1,fInputBinFile);
    if(this->GetVerboseLevel()==REST_Debug){
        printBits(evID);
        cout<<"Event ID "<<evID<<endl;
        getchar();
    }
    frameBits+=sizeof(evID);
    totalBytesReaded+=sizeof(evID);

    //Timestamp number of clocks since the start of the run (1 clock ~20 ns)
    double timestamp = (double) (2147483648*tsl+32768*tsm+tsh);

    //Set timestamp and event ID
    fSignalEvent->SetTime(tStart+timestamp*2.E-8);
    fSignalEvent->SetID(evID);
    fSignalEvent->SetRunOrigin( fRunOrigin );
    fSignalEvent->SetSubRunOrigin( fSubRunOrigin );

    int timeBin = 0;

    int fecN;
    int channel;
    int asicN;

    int adc;

    int physChannel;

    bool skip=false;

    unsigned short dat, startDF;;
    TRestSignal sgnl;
    sgnl.SetSignalID( -1 );


    //Bucle till it finds the data end
    while (((dat & 0xFFF0)>>4) !=14) {

        if(fread(&dat, sizeof(dat), 1, fInputBinFile) !=1){
            fclose(fInputBinFile);
            cout<<"File END 2"<<endl;
            fOutputEvent = NULL;
            return NULL;//File end
        }
        frameBits+=sizeof(dat);
        totalBytesReaded +=sizeof(dat);

        //New Event
        if((dat & 0xC000) >> 14 == 3 ){

            //Storing previous event, skipping if is the first one

            fecN = (dat & 0x3E00) >> 9;
            asicN = (dat & 0x180) >> 7;
            channel = (dat & 0x7F);

            timeBin =0;
            if(this->GetVerboseLevel()==REST_Debug)cout<<"Reset"<<endl;


            physChannel = GetPhysChannel(channel);


            //Skipping non physical channels
            if(physChannel<0){
                skip=true;

                if(this->GetVerboseLevel()==REST_Debug){cout<<"Skipping channel "<<channel<<endl;}
            }
            else skip=false;


            //FECN not included so far....
            if( physChannel >= 0 ) physChannel += asicN*72;
            //physChannel += fecN*4*72+asicN*72;

            if(this->GetVerboseLevel()==REST_Debug)
            {
                cout<<"----------------------"<<endl;
                cout<<"FEC "<<fecN<<endl;
                cout<<"asic "<<asicN<<endl;
                cout<<"Channel "<<channel<<endl;
                cout<<"PhysChannel "<<physChannel<<endl;
            }

            if( sgnl.GetSignalID( ) >= 0 && sgnl.GetNumberOfPoints() >= fMinPoints )
	    {
		    if ( fRejectNoise && sgnl.GetIntegralWithThreshold( 10, 490, 5, 90, 2.2, 9, 3.5 ) > 0 )
			    fSignalEvent->AddSignal( sgnl );
		    else
			    fSignalEvent->AddSignal( sgnl );
	    }

            sgnl.Initialize();
            sgnl.SetSignalID( physChannel );
        }
        //Timebin, may be not present if zero-suppresion is not active
        else if((dat & 0xFE00) >> 9 == 7 ){
            timeBin = (dat & 0x1FF);
            if(this->GetVerboseLevel()==REST_Debug)cout<<"TimeBin "<<timeBin<<endl;
        }
        //ADC values
        else if((dat & 0xF000) >> 12 == 3 ){

            adc = (dat & 0xFFF);

            if(this->GetVerboseLevel()==REST_Debug)cout<<"Time bin "<<timeBin<<"\tADC "<<adc<<endl;
            if(!skip) sgnl.NewPoint( timeBin, adc );
            timeBin++;
        }
        //End of Frame, reading frame header and payload
        else if(dat==15){
            if(this->GetVerboseLevel()==REST_Debug)cout<<" End of frame "<<"Bits readed "<<frameBits<<" /"<<payload<<endl;
            frameBits=0;

            fread(&startDF, sizeof(startDF),1,fInputBinFile);
            if(this->GetVerboseLevel()==REST_Debug){
                cout<<"PFX_START "<<startDF<<endl;
                printBits(startDF);
                cout<<" Version "<< ((startDF & 0x1E0) >> 5) <<endl;
                cout<<" FEC "<<(startDF & 0x1F)<<endl;
            }
            frameBits+=sizeof(startDF);
            totalBytesReaded+=sizeof(startDF);

            fread(&pay, sizeof(pay),1,fInputBinFile);
            payload = pay;
            if(this->GetVerboseLevel()==REST_Debug){
                cout<<"Frame payload "<<payload<<endl;
                printBits(pay);
            }
            frameBits+=sizeof(pay);
            totalBytesReaded+=sizeof(pay);
        }

    }//while

    //Storing last event 
    if( sgnl.GetSignalID( ) >= 0 && sgnl.GetNumberOfPoints() >= fMinPoints )
    {
	    if ( fRejectNoise && sgnl.GetIntegralWithThreshold( 10, 490, 5, 90, 2.2, 9, 3.5 ) > 0 )
		    fSignalEvent->AddSignal( sgnl );
	    else
		    fSignalEvent->AddSignal( sgnl );
    }

    if(this->GetVerboseLevel()==REST_Debug)cout<<" End of event "<< dat<<endl;
    //End of event footer
    fread(&dat, sizeof(dat),1,fInputBinFile);
    frameBits+=sizeof(dat);
    totalBytesReaded+=sizeof(dat);

    //If data frame is being finish, the data frame header is reader in order to avoid a misalignement reading the file
    if((unsigned short)payload<=frameBits+2){
        while( dat !=15 ){
            fread(&dat, sizeof(dat),1,fInputBinFile);
            frameBits+=sizeof(dat);
            totalBytesReaded+=sizeof(dat);
        }

        if(this->GetVerboseLevel()==REST_Debug)cout<<" End of frame "<<"Bits readed "<<frameBits<<" /"<<payload<<endl;

        frameBits=0;
        fread(&startDF, sizeof(startDF),1,fInputBinFile);
        if(this->GetVerboseLevel()==REST_Debug){
            printBits(startDF);
            cout<<"PFX_START "<<startDF<<endl;
            cout<<" Version "<< ((startDF & 0x1E0) >> 5) <<endl;
            cout<<" FEC "<<(startDF & 0x1F)<<endl;
        }
        frameBits+=sizeof(startDF);
        totalBytesReaded+=sizeof(startDF);

        fread(&pay, sizeof(pay),1,fInputBinFile);
        payload=pay;
        frameBits+=sizeof(pay);
        totalBytesReaded+=sizeof(pay);
        if(this->GetVerboseLevel()==REST_Debug){
            cout<<"Frame payload "<<payload<<endl;
            printBits(pay);
        }

    }

    if( fSignalEvent->GetNumberOfSignals() == 0 ) return NULL;

    return fSignalEvent;
}

int TRestFEMINOSToSignalProcess::GetPhysChannel(int channel){

    int physChannel=-10;

    //AFTER
    if(GetElectronicsType( )=="AFTER"){
        if (channel> 2 && channel < 15 ) {
            physChannel= channel -3; 
        } else if (channel> 15 && channel < 28 ) {
            physChannel= channel -4; 
        } else if (channel> 28 && channel < 53 ) {
            physChannel= channel -5; 
        } else if (channel> 53 && channel < 66 ) {
            physChannel= channel -6; 
        } else if (channel> 66  ) {
            physChannel= channel -7; 
        }
    }
    //AGET Short seq
    else if(GetElectronicsType( )=="AGET"){
        if (channel> 1 && channel < 13 ) {
            physChannel= channel -2; 
        } else if (channel> 13 && channel < 24 ) {
            physChannel= channel -3; 
        } else if (channel> 24 && channel < 47 ) {
            physChannel= channel -4; 
        } else if (channel> 47 && channel < 58 ) {
            physChannel= channel -5; 
        } else if (channel> 58 ) {
            physChannel= channel -6; 
        }
    }

    else return -1;


    return physChannel;

}





