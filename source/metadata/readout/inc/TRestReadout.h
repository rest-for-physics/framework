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

#include <TGraph.h>

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

        TRestReadoutModule *GetReadoutModule( int n ) { return &fReadoutModule[n]; }

        Int_t GetNumberOfModules( ) { return fReadoutModule.size(); }

        //Construtor
        TRestReadout();
        TRestReadout( char *cfgFileName);
        //Destructor
        virtual ~ TRestReadout();


        ClassDef(TRestReadout, 1);     // REST run class
};
#endif
