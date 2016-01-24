///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestReadout.h
///
///             Base class from which to inherit all other event classes in REST 
///
///             sep 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Galan
///_______________________________________________________________________________


#ifndef RestCore_TRestReadout
#define RestCore_TRestReadout

#include <iostream>
using namespace std;

#include "TObject.h"

#include "TRestMetadata.h"
#include "TRestReadoutModule.h"
#include "TRestReadoutChannel.h"

#include <TGraph.h>
#include <TH2Poly.h>

class TRestReadout:public TRestMetadata {
    private:
        void InitFromConfigFile();

        virtual void Initialize();

        vector <TRestReadoutModule> fReadoutModule;

    protected:

    public:

        void Draw();

        void PrintReadout( );

        void PrintMetadata( ) { PrintReadout( ); }

        void AddModule( TRestReadoutModule &rModule ) { fReadoutModule.push_back( rModule ); }

        TRestReadoutModule *GetModuleByID( Int_t modID );
        TRestReadoutChannel *GetChannelByID( Int_t modID, Int_t chID );

        TRestReadoutModule *GetModule( int mod ) { return &fReadoutModule[mod]; }
        TRestReadoutChannel *GetChannel( Int_t mod, Int_t ch ) { return fReadoutModule[mod].GetChannel( ch ); }

        TRestReadoutModule *GetReadoutModule( int mod ) { return GetModule( mod ); }
        TRestReadoutChannel *GetReadoutChannel( Int_t mod, Int_t ch ) { return GetChannel( mod, ch ); }

        Int_t GetNumberOfModules( ) { return fReadoutModule.size(); }
        Int_t GetNumberOfChannels( );

        Double_t GetX( Int_t modID, Int_t chID );
        Double_t GetY( Int_t modID, Int_t chID );
        
        TH2Poly *GetReadoutHistogram( );
        void GetBoundaries(double &xmin,double &xmax,double &ymin,double &ymax);
        
        //Construtor
        TRestReadout();
        TRestReadout( const char *cfgFileName);
        //Destructor
        virtual ~ TRestReadout();


        ClassDef(TRestReadout, 1);     // REST run class
};
#endif
