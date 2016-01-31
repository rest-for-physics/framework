///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDecoding inherited from TRestMetadata
///
///             Load metadata of the decoding and return a readout channel for a given DAQ channel.
///
///             now 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 JuanAn Garcia
///_______________________________________________________________________________


#ifndef RestCore_TRestDecoding
#define RestCore_TRestDecoding

#include <iostream>

#include "TRestMetadata.h"
#include "TRestReadout.h"

class TRestDecoding:public TRestMetadata{
    private:
        void InitFromConfigFile();

        virtual void Initialize();

        std::vector <Int_t> fReadoutChannelID;
        std::vector <Int_t> fDaqChannelID;

        Int_t fNChannels;

    protected:

    public:

        void PrintMetadata( );

        void LoadDefaultDecoding( );

        void AddChannel( int rC, int pCh){
            fReadoutChannelID.push_back(rC);
            fDaqChannelID.push_back(pCh);
        }

        Int_t GetNChannels(){return fNChannels;}
        Int_t GetReadoutChannel (int daqChannel);	

        Bool_t LoadDecodingFromFile(TString fName);

        //Construtor
        TRestDecoding();
        TRestDecoding( char *cfgFileName);
        //Destructor
        virtual ~ TRestDecoding();


        ClassDef(TRestDecoding, 1);     // REST run class
};
#endif
