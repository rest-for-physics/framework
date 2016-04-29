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

#include "TObject.h"

#include "TRestMetadata.h"
#include "TRestReadoutPlane.h"

class TRestReadout:public TRestMetadata {
    private:
        void InitFromConfigFile();

        virtual void Initialize();

        Bool_t fDecoding;

        Int_t fNReadoutPlanes;
        std::vector <TRestReadoutPlane> fReadoutPlanes;

        void ValidateReadout( );

    protected:

    public:

        void Draw();

        void PrintMetadata( ) { PrintMetadata( 0 ); }
        void PrintMetadata( Int_t fullDetail );

        TRestReadoutPlane *GetReadoutPlane( int p ) { return &fReadoutPlanes[p]; }

        void AddReadoutPlane( TRestReadoutPlane plane ) { fReadoutPlanes.push_back( plane ); fNReadoutPlanes++; }


        Int_t GetNumberOfReadoutPlanes( );
        Int_t GetNumberOfModules( );
        Int_t GetNumberOfChannels( );

        Double_t GetX( Int_t plane, Int_t modID, Int_t chID );
        Double_t GetY( Int_t plane, Int_t modID, Int_t chID );
        
        //Construtor
        TRestReadout();
        TRestReadout( const char *cfgFileName);
        TRestReadout( const char *cfgFileName, std::string name);
        //Destructor
        virtual ~ TRestReadout();


        ClassDef(TRestReadout, 1);     // REST readout class
};
#endif
