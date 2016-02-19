///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestLinearTrack.h
///
///             Event class to store linearized track events
///
///             Feb 2016:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Galan
///_______________________________________________________________________________


#ifndef RestDAQ_TRestLinearTrack
#define RestDAQ_TRestLinearTrack

#include <iostream>

#include <TObject.h>
#include <TMath.h>
#include <TVector3.h>

#include <TRestEvent.h>
#include <TRestSignal.h>

class TRestLinearTrack: public TRestEvent {

    protected:
        TVector3 fMeanPosition;
        Double_t fTrackEnergy;
        TRestSignal fLinearCharge;
        TRestSignal fTransversalCharge;

    public:
      
        void Initialize();

        Double_t GetMinLinearLength() { return fLinearCharge.GetMinTime(); }
        Double_t GetMaxLinearLength() { return fLinearCharge.GetMaxTime(); }

        Double_t GetMinLinearDeposit() { return fLinearCharge.GetMinValue(); }
        Double_t GetMaxLinearDeposit() { return fLinearCharge.GetMaxValue(); }

        Double_t GetMinTransversalLength() { return fTransversalCharge.GetMinTime(); }
        Double_t GetMaxTransversalLength() { return fTransversalCharge.GetMaxTime(); }

        Double_t GetMinTransversalDeposit() { return fTransversalCharge.GetMinValue(); }
        Double_t GetMaxTransversalDeposit() { return fTransversalCharge.GetMaxValue(); }

        TRestSignal *GetLongitudinalTrack() { return &fLinearCharge; }
        TRestSignal *GetTransversalTrack() { return &fTransversalCharge; }

        // Setters
        void SetID( Int_t id )
        {
            fLinearCharge.SetID( id );
            fTransversalCharge.SetID( id );
        }

        void SetMeanPosition( TVector3 pos ) { fMeanPosition = pos; }
        void SetMeanPosition( Double_t x, Double_t y, Double_t z ) { fMeanPosition = TVector3( x, y, z ); }

        void SetEnergy( Double_t en ) { fTrackEnergy = en; }


        // Getters
        Int_t GetID( ) { return fLinearCharge.GetID(); }

        TVector3 GetMeanPosition( ) { return fMeanPosition; }

        Double_t GetEnergy( ) { return fTrackEnergy; }

        Bool_t isXY()
        {
            if( TMath::IsNaN( fMeanPosition.Z() ) )
                return true;
            return false;
        }

        Bool_t isYZ()
        {
            if( TMath::IsNaN( fMeanPosition.X() ) )
                return true;
            return false;
        }

        Bool_t isXZ()
        {
            if( TMath::IsNaN( fMeanPosition.Y() ) )
                return true;
            return false;
        }

        Bool_t isXYZ()
        {
            Double_t x = fMeanPosition.X();
            Double_t y = fMeanPosition.Y();
            Double_t z = fMeanPosition.Z();
            
            if( !TMath::IsNaN( x ) && !TMath::IsNaN( y ) && !TMath::IsNaN( z ) )
                return true;

            return false;
        }

        void AddChargeToLinearTrack( Double_t lin, Double_t trans, Double_t charge )
        {
            fLinearCharge.AddCharge( lin, charge );
            fTransversalCharge.AddCharge( trans, charge );
        }

        void SetLinearTrack( TRestSignal l, TRestSignal t )
        {
            fLinearCharge = l;
            fTransversalCharge = t;
        }

        void SortTracks( ) { fLinearCharge.Sort(); fTransversalCharge.Sort(); }

        TGraph *GetLinearGraph( Int_t color = 1 )
        {
            return fLinearCharge.GetGraph( color );
        }

        TGraph *GetTransversalGraph( Int_t color = 1 )
        {
            return fTransversalCharge.GetGraph( color );
        }

        void Print( Bool_t fullInfo = false );

        //Construtor
        TRestLinearTrack();
        //Destructor
        virtual ~ TRestLinearTrack();

        ClassDef(TRestLinearTrack, 1);     // REST event superclass
};
#endif
