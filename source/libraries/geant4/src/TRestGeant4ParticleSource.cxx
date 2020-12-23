///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4ParticleSource.cxx
///
///             Base class from which to inherit all other event classes in REST
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 J. Galan
///_______________________________________________________________________________

//#include "TFile.h"

#include "TRestGeant4ParticleSource.h"
#include "TRestStringOutput.h"
using namespace std;

// REST_Verbose_Level fLevel = REST_Essential;  //!
////	TRestLeveledOutput(REST_Verbose_Level& vref, string _color =
//// COLOR_RESET, string BorderOrHeader = "", REST_Display_Format style =
//// kBorderedLeft)
// TRestLeveledOutput<REST_Essential> metadata =
//    TRestLeveledOutput<REST_Essential>(fLevel, COLOR_BOLDGREEN, "||",
//                                       (REST_Display_Format)kBorderedMiddle);  //!

ClassImp(TRestGeant4ParticleSource)
    //______________________________________________________________________________
    TRestGeant4ParticleSource::TRestGeant4ParticleSource() {
    // TRestGeant4ParticleSource default constructor
}

//______________________________________________________________________________
TRestGeant4ParticleSource::~TRestGeant4ParticleSource() {
    // TRestGeant4ParticleSource destructor
}

void TRestGeant4ParticleSource::PrintParticleSource() {
    metadata << "---------------------------------------" << endl;
    metadata << "Particle : " << GetParticle() << endl;
    metadata << "Charge : " << GetParticleCharge() << endl;
    metadata << "Angular distribution type : " << GetAngularDistType() << endl;
    if (GetAngularDistType() == "TH1D") {
        metadata << "Angular distribution filename : " << GetAngularFilename() << endl;
        metadata << "Angular histogram name  : " << GetAngularName() << endl;
    }
    metadata << "Direction : (" << GetDirection().X() << "," << GetDirection().Y() << ","
             << GetDirection().Z() << ")" << endl;
    metadata << "Energy distribution : " << GetEnergyDistType() << endl;
    if (GetEnergyDistType() == "TH1D") {
        metadata << "Energy distribution filename : " << GetSpectrumFilename() << endl;
        metadata << "Energy histogram name  : " << GetSpectrumName() << endl;
    }
    if (GetEnergyRange().X() == GetEnergyRange().Y())
        metadata << "Energy : " << GetEnergy() << " keV" << endl;
    else
        metadata << "Energy range : (" << GetEnergyRange().X() << "," << GetEnergyRange().Y() << ") keV"
                 << endl;
}

/* {{{ LoadEnergyDistribution ( A better way would be to store TH1D
energySpectrum in particle source  (Commented) void
TRestGeant4ParticleSource::LoadEnergyDistribution( TString fName, TString spctName )
{
    TString fileFullPath = (TString ) getenv("REST_PATH") +
"/data/energySpectra/" + fName;

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
