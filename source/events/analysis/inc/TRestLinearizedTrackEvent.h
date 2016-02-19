///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestLinearizedTrackEvent.h
///
///             Event class to store linearized track events
///
///             Feb 2016:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Galan
///_______________________________________________________________________________


#ifndef RestDAQ_TRestLinearizedTrackEvent
#define RestDAQ_TRestLinearizedTrackEvent

#include <iostream>

#include <TObject.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TRestSignal.h>

#include <TRestEvent.h>
#include <TRestLinearTrack.h>

class TRestLinearizedTrackEvent: public TRestEvent {

    protected:
        Int_t fNTracks;
        std::vector <TRestLinearTrack> fLinearTrack;

        #ifndef __CINT__
        TPad *fPad; 
        Double_t fMinLength;
        Double_t fMaxLength;
        Double_t fMinWidth;
        Double_t fMaxWidth;
        Double_t fMinLinearDeposit;
        Double_t fMaxLinearDeposit;
        Double_t fMinTransversalDeposit;
        Double_t fMaxTransversalDeposit;
        #endif 
 
    public:

        Int_t GetNumberOfTracks() { return fNTracks; }

        void AddLinearTrack( TRestSignal l, TRestSignal t )
        {
            TRestLinearTrack lTrack;
            lTrack.SetID( fNTracks );
            lTrack.SetLinearTrack( l, t );
            fLinearTrack.push_back( lTrack );
            fNTracks++;
        }

        void AddLinearTrack( TRestLinearTrack lTck )
        {
            lTck.SetID( fNTracks );
            fLinearTrack.push_back( lTck );
            fNTracks++;
        }
      
        void Initialize();

        void PrintEvent( Bool_t fullInfo = false );

        void SetMaxAndMin( )
        {
            fMinLength = 1e10;
            fMaxLength = -1e10;
            fMinWidth = 1e10;
            fMaxWidth = -1e10;

            fMinLinearDeposit = 1e10;
            fMaxLinearDeposit = -1e10;
            fMinTransversalDeposit = 1e10;
            fMaxTransversalDeposit = -1e10;

            for( int tck = 0; tck < fNTracks; tck++ )
            {
                if( fMinLength > fLinearTrack[tck].GetMinLinearLength() ) 
                    fMinLength = fLinearTrack[tck].GetMinLinearLength();

                if( fMaxLength < fLinearTrack[tck].GetMaxLinearLength() ) 
                    fMaxLength = fLinearTrack[tck].GetMaxLinearLength();

                if( fMinLinearDeposit > fLinearTrack[tck].GetMinLinearDeposit() ) 
                    fMinLinearDeposit = fLinearTrack[tck].GetMinLinearDeposit();

                if( fMaxLinearDeposit < fLinearTrack[tck].GetMaxLinearDeposit() ) 
                    fMaxLinearDeposit = fLinearTrack[tck].GetMaxLinearDeposit();


                if( fMinWidth > fLinearTrack[tck].GetMinTransversalLength() ) 
                    fMinWidth = fLinearTrack[tck].GetMinTransversalLength();

                if( fMaxWidth < fLinearTrack[tck].GetMaxTransversalLength() ) 
                    fMaxWidth = fLinearTrack[tck].GetMaxTransversalLength();

                if( fMinTransversalDeposit > fLinearTrack[tck].GetMinTransversalDeposit() ) 
                    fMinTransversalDeposit = fLinearTrack[tck].GetMinTransversalDeposit();

                if( fMaxTransversalDeposit < fLinearTrack[tck].GetMaxTransversalDeposit() ) 
                    fMaxTransversalDeposit = fLinearTrack[tck].GetMaxTransversalDeposit();


            }
        }

        Double_t GetMinLinearLength() 
        {
            SetMaxAndMin();
            return fMinLength;
        }

        Double_t GetMaxLinearLength() 
        {
            SetMaxAndMin();
            return fMaxLength;
        }

        Double_t GetMinLinearDeposit() 
        {
            SetMaxAndMin();
            return fMinLinearDeposit;
        }

        Double_t GetMaxLinearDeposit() 
        {
            SetMaxAndMin();
            return fMaxLinearDeposit;
        }

        Double_t GetMinTransversalLength() 
        { 
            SetMaxAndMin();
            return fMinWidth;
        }

        Double_t GetMaxTransversalLength() 
        { 
            SetMaxAndMin();
            return fMaxWidth;
        }

        Double_t GetMinTransversalDeposit() 
        {
            SetMaxAndMin();
            return fMinTransversalDeposit;
        }

        Double_t GetMaxTransversalDeposit() 
        { 
            SetMaxAndMin();
            return fMaxTransversalDeposit;
        }

        TPad *DrawEvent();
                
        //Construtor
        TRestLinearizedTrackEvent();
        //Destructor
        virtual ~ TRestLinearizedTrackEvent();

        ClassDef(TRestLinearizedTrackEvent, 1);     // REST event superclass
};
#endif
