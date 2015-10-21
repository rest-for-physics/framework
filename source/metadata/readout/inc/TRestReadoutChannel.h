///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestReadoutChannel.h
///
///             Base class from which to inherit all other event classes in REST 
///
///             apr 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Galan
///_______________________________________________________________________________


#ifndef RestCore_TRestReadoutChannel
#define RestCore_TRestReadoutChannel

#include <iostream>
using namespace std;

#include "TObject.h"

#include "TRestReadoutPixel.h"

class TRestReadoutChannel : public TObject {
    private:
        Int_t fChannelID;
        vector <TRestReadoutPixel> fReadoutPixel;

        void Initialize();

    protected:

    public:

        Int_t GetID() { return fChannelID; }

        Int_t GetNumberOfPixels( ) { return fReadoutPixel.size(); }

        TRestReadoutPixel *GetPixel( int n ) { return &fReadoutPixel[n]; }
        TRestReadoutPixel *GetPixelByID( int id );
        
        Bool_t isInside( Double_t x, Double_t y ) 
        {
            for( int i = 0; i < GetNumberOfPixels(); i++ )
                if( fReadoutPixel[i].isInside( x, y ) ) return true;
      
            return false;
        }

        void SetID( Int_t id ) { fChannelID = id; }
        void AddPixel( TRestReadoutPixel pix ) { fReadoutPixel.push_back( pix ); }
        
        void PrintReadoutChannel( );

        //Construtor
        TRestReadoutChannel();
        //Destructor
        virtual ~ TRestReadoutChannel();


        ClassDef(TRestReadoutChannel, 1);     // REST run class
};
#endif
