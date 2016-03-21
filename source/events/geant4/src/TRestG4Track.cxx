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
    else if ( pcsName == "neutronStep") id = 22;
    else if ( pcsName == "alphaStep") id = 23;
    else if ( pcsName == "He3Step") id = 24;
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
    //cout << GetTrackOrigin().X() << " " << GetTrackOrigin().Y() << " " << GetTrackOrigin().Z() << endl;
    //cout << "Length : " << length << endl;

    for( int i = 1; i < GetNumberOfHits(); i++ )
    {
        TVector3 prevHit = fHits.GetPosition(i-1);
        TVector3 hit = fHits.GetPosition(i);
        length += GetDistance( hit, prevHit );
        //cout << prevHit.X() << " " << prevHit.Y() << " " << prevHit.Z() << endl;
        //cout << "Length : " << length << endl;
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
    else if ( id == 22 ) return "neutronStep";
    else if ( id == 23 ) return "alphaStep";
    else if ( id == 24 ) return "He3Step";
    else cout << "WARNING : The process ID : " << id << " could not be found" << endl;

    return "";
}

void TRestG4Track::PrintTrack()
{
    cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
    cout.precision(10);
    cout << " SubEvent ID : " << fSubEventId << " Global timestamp : " << GetGlobalTrackTime() << " seconds" << endl;
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
