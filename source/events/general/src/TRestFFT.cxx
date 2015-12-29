///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestFFT.cxx
///
///             Event class to help for using TFFT 
///
///             dec 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Galan
///_______________________________________________________________________________

#include <TVirtualFFT.h>
#include <TComplex.h>

#include "TRestFFT.h"

ClassImp(TRestFFT)
//______________________________________________________________________________
    TRestFFT::TRestFFT()
{
   // TRestFFT default constructor
}

//______________________________________________________________________________
TRestFFT::~TRestFFT()
{
   // TRestFFT destructor
}

void TRestFFT::SetNfft( Int_t n )
{

    fNfft = n;

    fTimeReal.Set( fNfft );
    fTimeImg.Set( fNfft );
    fFrequencyReal.Set( fNfft );
    fFrequencyImg.Set( fNfft );

}

void TRestFFT::ForwardSignalFFT( TRestSignal *sgnl, Int_t fNStart, Int_t fNEnd )
{
    
    Int_t n = sgnl->GetNumberOfPoints()-fNStart-fNEnd;
    SetNfft( n );

    for( int i = fNStart; i < sgnl->GetNumberOfPoints()-fNEnd; i++ )
    {
        fTimeReal[i-fNStart] = sgnl->GetData(i);
        fTimeImg[i-fNStart] = 0;
    }

    TVirtualFFT *forward = TVirtualFFT::FFT(1, &fNfft, "R2C");
    forward->SetPointsComplex( fTimeReal.GetArray(), fTimeImg.GetArray() );
    forward->Transform();

    for( int i = 0; i < fNfft; i++ )
        forward->GetPointComplex( i, fFrequencyReal.GetArray()[i], fFrequencyImg.GetArray()[i] );

    delete forward;
}

void TRestFFT::GetSignal( TRestSignal *sgnl )
{

    sgnl->Reset();
    for( int i = 0; i < fNfft; i++ )
        sgnl->AddPoint( i, fTimeReal.GetArray()[i] );
}

void TRestFFT::MultiplyBy( TRestFFT *fftInput, Int_t from, Int_t to )
{
    if( fftInput->GetNfft() != this->GetNfft() ) { cout << "Not the same N FFT" << endl; return; }

    for( int i = from; i < to; i++ )
    {
        TComplex top( this->GetFrequencyAmplitudeReal(i), this->GetFrequencyAmplitudeImg(i) );
        TComplex bottom( fftInput->GetFrequencyAmplitudeReal(i), fftInput->GetFrequencyAmplitudeImg(i) );
        TComplex product = top*bottom;
        fFrequencyReal.GetArray()[i] = product.Re();
        fFrequencyImg.GetArray()[i] = product.Im();
        fFrequencyReal.GetArray()[fNfft-i-1] = fFrequencyReal.GetArray()[i];
        fFrequencyImg.GetArray()[fNfft-i-1] = fFrequencyImg.GetArray()[i];
    }
}

void TRestFFT::DivideBy( TRestFFT *fftInput )
{
    if( fftInput->GetNfft() != this->GetNfft() ) { cout << "Not the same N FFT" << endl; return; }

    for( int i = 0; i < fNfft; i++ )
    {
        TComplex top( this->GetFrequencyAmplitudeReal(i), this->GetFrequencyAmplitudeImg(i) );
        TComplex bottom( fftInput->GetFrequencyAmplitudeReal(i), fftInput->GetFrequencyAmplitudeImg(i) );
        TComplex cocient = top/bottom;
        fFrequencyReal.GetArray()[i] = cocient.Re();
        fFrequencyImg.GetArray()[i] = cocient.Im();
    }
}

void TRestFFT::KillFrequencies( Int_t from, Int_t to )
{
    for( int i = from; i < to; i++ )
    {
        fFrequencyReal.GetArray()[i] = 0.;
        fFrequencyImg.GetArray()[i] = 0.;

        fFrequencyReal.GetArray()[fNfft-i-1] = fFrequencyReal.GetArray()[i];
        fFrequencyImg.GetArray()[fNfft-i-1] = fFrequencyImg.GetArray()[i];
    }


}

void TRestFFT::ButterWorthFilter( Int_t cutOff, Int_t order )//, Double_t amp, Double_t decay )
{
    double cOffDouble = (double) cutOff;
    for( int i = 0; i < fNfft/2; i++ )
    {
        double iDouble = (double) i;
        if( i > cutOff )
        {

        fFrequencyReal.GetArray()[i] /= sqrt( 1 + pow(iDouble/cOffDouble, 2*order)  );
        fFrequencyImg.GetArray()[i] /= sqrt( 1 + pow(iDouble/cOffDouble, 2*order)  );

        fFrequencyReal.GetArray()[fNfft-i-1] = fFrequencyReal.GetArray()[i];
        fFrequencyImg.GetArray()[fNfft-i-1] = fFrequencyImg.GetArray()[i];

        }

    }
}

void TRestFFT::BackwardFFT( )
{

    TVirtualFFT *backward = TVirtualFFT::FFT(1, &fNfft, "C2R");
    backward->SetPointsComplex( fFrequencyReal.GetArray(), fFrequencyImg.GetArray() );
    backward->Transform();

    for ( int i = 0; i < fNfft; i++ )
    {
        backward->GetPointComplex(i, fTimeReal.GetArray()[i], fTimeImg.GetArray()[i]);
        fTimeReal.GetArray()[i] /= fNfft;
        fTimeImg.GetArray()[i] /= fNfft;
    }

    delete backward;
}

void TRestFFT::ApplyLowPassFilter( Int_t cutFrequency )
{
    for( int i = 0; i < fNfft; i++ )
    {
        if( i >= cutFrequency && i < fNfft-cutFrequency )
        {
            fFrequencyReal.GetArray()[i] = 0.;
            fFrequencyImg.GetArray()[i] = 0.;
        }
    }

}

void TRestFFT::SetSecondOrderAnalyticalResponse( Double_t f1, Double_t f2, Double_t to )
{
    for( int i = 0; i < fNfft/2; i++ )
    {
        Double_t w = 2.59 * (double) i;

        TComplex *cmplx1 = new TComplex( f1*w, (f1*f2-w*w) );

        TComplex *cmplx2 = new TComplex( f1, 0);

        *cmplx2 /= *cmplx1;

        TComplex phase( TComplex::Exp( TComplex( 0, -w*to ) ) );

        *cmplx2 *= phase;

        fFrequencyReal.GetArray()[i] = cmplx2->Re();
        fFrequencyImg.GetArray()[i] = cmplx2->Im();
    }
    for( int i = fNfft/2; i < fNfft; i++ )
    {
        fFrequencyReal.GetArray()[i] = fFrequencyReal.GetArray()[fNfft-i-1];
        fFrequencyImg.GetArray()[i] = fFrequencyImg.GetArray()[fNfft-i-1];
    }

    WriteFrequencyToTextFile( "/home/javier/tmp/frequencyResponse" );

    BackwardFFT();

    WriteTimeSignalToTextFile ( "/home/javier/tmp/timeSignal" );

}

void TRestFFT::RemoveBaseline( )
{
    Double_t real = this->GetFrequencyAmplitudeReal(1);
    Double_t img = this->GetFrequencyAmplitudeImg(1);
    Double_t module = sqrt( real*real + img*img );
    this->SetNode( 0, module );
}

void TRestFFT::RenormalizeNode( Int_t n, Double_t factor )
{
    fFrequencyReal.GetArray()[fNfft-n-1] = fFrequencyReal.GetArray()[n]/factor;
    fFrequencyImg.GetArray()[fNfft-n-1] = fFrequencyImg.GetArray()[n]/factor;

    fFrequencyReal.GetArray()[n] = fFrequencyReal.GetArray()[n]/factor;
    fFrequencyImg.GetArray()[n] = fFrequencyImg.GetArray()[n]/factor;
}

void TRestFFT::WriteFrequencyToTextFile ( TString filename )
{

    FILE *fff = fopen( filename.Data(), "w" );
    for ( int i = 0; i < fNfft; i++ )
        fprintf( fff, "%d\t%17.14e\t%17.14e\n", i, fFrequencyReal.GetArray()[i], fFrequencyImg.GetArray()[i] );
    fclose(fff);
}

void TRestFFT::WriteTimeSignalToTextFile ( TString filename )
{

    FILE *fff = fopen( filename.Data(), "w" );
    for ( int i = 0; i < fNfft; i++ )
        fprintf( fff, "%d\t%e\t%e\n", i, fTimeReal.GetArray()[i], fTimeImg.GetArray()[i] );
    fclose(fff);
}
