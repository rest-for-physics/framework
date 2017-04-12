///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestG4Track.cxx
///
///             Base class from which to inherit all other event classes in REST 
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 J. Galan
///_______________________________________________________________________________


#include "TRestG4Track.h"
using namespace std;

ClassImp(TRestG4Track)
//______________________________________________________________________________
    TRestG4Track::TRestG4Track()
{
   // TRestG4Track default constructor
}

//______________________________________________________________________________
TRestG4Track::~TRestG4Track()
{
   // TRestG4Track destructor
}

Int_t TRestG4Track::GetProcessID( TString pcsName )
{
    Int_t id = -1;

    // TODO We register the process manually. Not good if we add new processes to the physics list
    // There must be a way to get all the registered processes from the physics list.
    // Would be a better way to do it
    // Or at least we must make a mapping id number to processName on a header file
    if ( pcsName == "initStep" ) id = 0;
    else if ( pcsName == "Transportation" ) id = 1;
    else if ( pcsName == "ionIoni" ) id = 2;
    else if ( pcsName == "phot" ) id = 3;
    else if ( pcsName == "eIoni" ) id = 4;
    else if ( pcsName == "eBrem" ) id = 5;
    else if ( pcsName == "msc" ) id = 6;
    else if ( pcsName == "compt" ) id = 7;
    else if ( pcsName == "Rayl" ) id = 8;
    else if ( pcsName == "conv" ) id = 9;
    else if ( pcsName == "annihil" ) id = 10;
    else if ( pcsName == "RadioactiveDecay" ) id = 11;
    else if ( pcsName == "muIoni" ) id = 12;
    else if ( pcsName == "e-Step") id = 20;
    else if ( pcsName == "e+Step") id = 21;
    //else if ( pcsName == "neutronStep") id = 22;
    //else if ( pcsName == "alphaStep") id = 23;
    //else if ( pcsName == "He3Step") id = 24;
    else if ( pcsName == "muBrems" ) id = 30;
    else if ( pcsName == "muPairProd" ) id = 31;
    else if ( pcsName == "Decay" ) id = 32;
    else if ( pcsName == "hIoni" ) id = 33;
    else if ( pcsName == "hBrems" ) id = 34;
    else if ( pcsName == "hPairProd" ) id = 35;
    else if ( pcsName == "hadElastic" ) id = 36;
    else if ( pcsName == "neutronInelastic" ) id = 37;
    else if ( pcsName == "nCapture" ) id = 38;
    else if ( pcsName == "nKiller" ) id = 39;
    else if ( pcsName == "nuclearStopping" ) id = 40;
    else if ( pcsName == "CoulombScat" ) id = 41; 
    else if ( pcsName == "photonNuclear" ) id = 42;
    else if ( pcsName == "protonInelastic" ) id = 43;
    else if ( pcsName == "pi-Inelastic" ) id = 44;
    else if ( pcsName == "pi+Inelastic" ) id = 45;
    else if ( pcsName == "tInelastic" ) id = 46;
    else if ( pcsName == "dInelastic" ) id = 47;
    else if ( pcsName == "electronNuclear" ) id = 48;
    else if ( pcsName == "muonNuclear" ) id = 49;
    else if ( pcsName == "positronNuclear" ) id = 50;
    else
    {
        id = -1;
        cout << "WARNING : The process " << pcsName << " was not found. It must be added to TRestG4Track::GetProcessID()" << endl;
    }


    return id;
}

Double_t TRestG4Track::GetTrackLength( )
{
    Double_t length = 0;

    length = GetDistance( fHits.GetPosition(0), GetTrackOrigin() );

    for( int i = 1; i < GetNumberOfHits(); i++ )
    {
        TVector3 prevHit = fHits.GetPosition(i-1);
        TVector3 hit = fHits.GetPosition(i);
        length += GetDistance( hit, prevHit );
    }
    return length;


}

TString TRestG4Track::GetProcessName( Int_t id )
{
    if( id == 0 ) return "initStep";
    else if ( id == 1 ) return "TRansportation";
    else if ( id == 2 ) return "ionIoni";
    else if ( id == 3 ) return "phot";
    else if ( id == 4 ) return "eIoni";
    else if ( id == 5 ) return "eBrem";
    else if ( id == 6 ) return "msc";
    else if ( id == 7 ) return "compt";
    else if ( id == 8 ) return "Rayl";
    else if ( id == 9 ) return "conv";
    else if ( id == 10 ) return "annihil";
    else if ( id == 11 ) return "RadioactiveDecay";
    else if ( id == 12 ) return "muIoni";
    else if ( id == 20 ) return "e-Step";
    else if ( id == 21 ) return "e+Step";
    // else if ( id == 22 ) return "neutronStep";
    // else if ( id == 23 ) return "alphaStep";
    // else if ( id == 24 ) return "He3Step";
    else if ( id == 30 ) return "muBrems";
    else if ( id == 31 ) return "muPairProd";
    else if ( id == 32 ) return "Decay" ;
    else if ( id == 33 ) return "hIoni";
    else if ( id == 34 ) return "hBrems";
    else if ( id == 35 ) return "hPairProd";
    else if ( id == 36 ) return "hadElastic" ;
    else if ( id == 37 ) return "neutronInelastic";
    else if ( id == 38 ) return "nCapture";
    else if ( id == 39 ) return "nKiller" ;
    else if ( id == 40 ) return "nuclearStopping";
    else if ( id == 41 ) return "CoulombScat";
    else if ( id == 42 ) return "photonNuclear";
    else if ( id == 43 ) return "protonInelastic";
    else if ( id == 44 ) return "pi-Inelastic";
    else if ( id == 45 ) return "pi+Inelastic";
    else if ( id == 46 ) return "tInelastic";
    else if ( id == 47 ) return "dInelastic";
    else if ( id == 48 ) return "electronNuclear";
    else if ( id == 49 ) return "muonNuclear";
    else if ( id == 50 ) return "positronNuclear";
    else cout << "WARNING : The process ID : " << id << " could not be found" << endl;


    return "";
}

void TRestG4Track::PrintTrack()
{
    cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
    cout.precision(10);
    cout << " SubEvent ID : " << fSubEventId << " Global timestamp : " << GetGlobalTime() << " seconds" << endl;
    cout.precision(2);
    cout << " Track ID : " << GetTrackID() << " Parent ID : " << GetParentID();
    cout << " Particle : " << GetParticleName() << " Time track length : " << GetTrackTimeLength() << " us" << endl;
    cout << " Ekin : " << GetKineticEnergy() << " keV" << endl;
    cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
    TRestG4Hits *hits = GetHits();
    for( int i = 0; i < GetNumberOfHits(); i++ )
    {
        cout << "Hit " << i << " process : " << GetProcessName(hits->GetHitProcess(i)) << " volume : " << hits->GetHitVolume(i) << " X : " << hits->GetX(i) << " Y : " << hits->GetY(i) << " Z : " << hits->GetZ(i) << " mm   Edep : " << hits->GetEnergy(i) << " keV" << endl;

    }

}
