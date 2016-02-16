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
#include <TH1D.h>
#include <TH2D.h>
#include <TRestSignal.h>

#include <TRestEvent.h>

class TRestLinearTrack: public TRestEvent {

    protected:
        Int_t fProjectionType;
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


        void SetXY() { fProjectionType = 1; }
        void SetYZ() { fProjectionType = 2; }
        void SetXZ() { fProjectionType = 3; }
        void SetXYZ() { fProjectionType = 4; }

        void SetID( Int_t id )
        {
            fLinearCharge.SetID( id );
            fTransversalCharge.SetID( id );
        }

        Int_t GetID( ) { return fLinearCharge.GetID(); }
            

        Bool_t isXY() { if( fProjectionType == 1 ) return true; return false; }
        Bool_t isYZ() { if( fProjectionType == 2 ) return true; return false; }
        Bool_t isXZ() { if( fProjectionType == 3 ) return true; return false; }
        Bool_t isXYZ() { if( fProjectionType == 4 ) return true; return false; }

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

        //Construtor
        TRestLinearTrack();
        //Destructor
        virtual ~ TRestLinearTrack();

        ClassDef(TRestLinearTrack, 1);     // REST event superclass
};
#endif
