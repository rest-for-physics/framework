///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestParticleSource.cxx
///
///             Base class from which to inherit all other event classes in REST 
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 J. Galan
///_______________________________________________________________________________

#include "TFile.h"

#include "TRestParticleSource.h"

ClassImp(TRestParticleSource)
//______________________________________________________________________________
    TRestParticleSource::TRestParticleSource()
{
   // TRestParticleSource default constructor
}

//______________________________________________________________________________
TRestParticleSource::~TRestParticleSource()
{
   // TRestParticleSource destructor
}

void TRestParticleSource::PrintParticleSource()
{
    cout << "Particle : " << GetParticle() << endl;
    cout << "Angular distribution type : " << GetAngularDistType() << endl;
    if (GetAngularDistType() == "TH1D" )
    {
        cout << "Angular distribution filename : " << GetAngularFilename() << endl;
        cout << "Angular histogram name  : " << GetAngularName() << endl;
    }
    cout << "Direction : (" << GetDirection().X() << "," << GetDirection().Y() << "," << GetDirection().Z() << ")" << endl;
    cout << "Energy distribution : " << GetEnergyDistType() << endl;
    if (GetEnergyDistType() == "TH1D" )
    {
        cout << "Energy distribution filename : " << GetSpectrumFilename() << endl;
        cout << "Energy histogram name  : " << GetSpectrumName() << endl;
    }
    if( GetEnergyRange().X() ==  GetEnergyRange().Y() )
        cout << "Energy : " << GetEnergy() << " keV" << endl;
    else
        cout << "Energy range : (" << GetEnergyRange().X() << "," << GetEnergyRange().Y() << ") keV" << endl;
    cout << "---------------------------------------" << endl;

}

/* {{{ LoadEnergyDistribution ( A better way would be to store TH1D energySpectrum in particle source  (Commented)
void TRestParticleSource::LoadEnergyDistribution( TString fName, TString spctName )
{
    TString fileFullPath = (TString ) getenv("REST_PATH") + "/inputData/energySpectra/" + fName;

    cout << "Loading energy distribution : " << endl;
    cout << "Full path file : " << fileFullPath << endl;

    //TFile fin( fileFullPath );

    //fEnergySpectrum = (TH1D *)  fin.Get( spctName );
   // TH1D *h = (TH1D *) fin.Get( spctName );

    //fEnergySpectrum = *h;
  //  fTest = h->GetBinContent( 40 );

 //   cout << fTest << endl;

 //   fin.Close();

    //cout << "Total events loaded : " << fEnergySpectrum.Integral() << endl;
    cout << "X" << endl;

}
 }}} */
