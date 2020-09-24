///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4Track.cxx
///
///             Base class from which to inherit all other event classes in REST
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 J. Galan
///_______________________________________________________________________________

#include "TRestGeant4Track.h"
using namespace std;

ClassImp(TRestGeant4Track);
//______________________________________________________________________________
TRestGeant4Track::TRestGeant4Track() {
    // TRestGeant4Track default constructor
}

//______________________________________________________________________________
TRestGeant4Track::~TRestGeant4Track() {
    // TRestGeant4Track destructor
}

Int_t TRestGeant4Track::GetProcessID(TString pcsName) {
    Int_t id = -1;

    // TODO We register the process manually. Not good if we add new processes to
    // the physics list There must be a way to get all the registered processes
    // from the physics list. Would be a better way to do it Or at least we must
    // make a mapping id number to processName on a header file
    if (pcsName == "initStep")
        id = 0;
    else if (pcsName == "Transportation")
        id = 1;
    else if (pcsName == "ionIoni")
        id = 2;
    else if (pcsName == "phot")
        id = 3;
    else if (pcsName == "eIoni")
        id = 4;
    else if (pcsName == "eBrem")
        id = 5;
    else if (pcsName == "msc")
        id = 6;
    else if (pcsName == "compt")
        id = 7;
    else if (pcsName == "Rayl")
        id = 8;
    else if (pcsName == "conv")
        id = 9;
    else if (pcsName == "annihil")
        id = 10;
    else if (pcsName == "RadioactiveDecay")
        id = 11;
    else if (pcsName == "muIoni")
        id = 12;
    else if (pcsName == "e-Step")
        id = 20;
    else if (pcsName == "e+Step")
        id = 21;
    // else if ( pcsName == "neutronStep") id = 22;
    // else if ( pcsName == "alphaStep") id = 23;
    // else if ( pcsName == "He3Step") id = 24;
    else if (pcsName == "muBrems")
        id = 30;
    else if (pcsName == "muPairProd")
        id = 31;
    else if (pcsName == "Decay")
        id = 32;
    else if (pcsName == "hIoni")
        id = 33;
    else if (pcsName == "hBrems")
        id = 34;
    else if (pcsName == "hPairProd")
        id = 35;
    else if (pcsName == "hadElastic")
        id = 36;
    else if (pcsName == "neutronInelastic")
        id = 37;
    else if (pcsName == "nCapture")
        id = 38;
    else if (pcsName == "nKiller")
        id = 39;
    else if (pcsName == "nuclearStopping")
        id = 40;
    else if (pcsName == "CoulombScat")
        id = 41;
    else if (pcsName == "photonNuclear")
        id = 42;
    else if (pcsName == "protonInelastic")
        id = 43;
    else if (pcsName == "pi-Inelastic")
        id = 44;
    else if (pcsName == "pi+Inelastic")
        id = 45;
    else if (pcsName == "tInelastic")
        id = 46;
    else if (pcsName == "dInelastic")
        id = 47;
    else if (pcsName == "electronNuclear")
        id = 48;
    else if (pcsName == "muonNuclear")
        id = 49;
    else if (pcsName == "positronNuclear")
        id = 50;
    else if (pcsName == "mu-Step")
        id = 51;
    else if (pcsName == "mu+Step")
        id = 52;
    else if (pcsName == "ePairProd")
        id = 53;
    else if (pcsName == "alphaInelastic")
        id = 54;
    else if (pcsName == "H3Inelastic")
        id = 55;
    else if (pcsName == "He3Inelastic")
        id = 56;
    else if (pcsName == "kaon+Inelastic")
        id = 57;
    else if (pcsName == "kaon-Inelastic")
        id = 58;
    else if (pcsName == "kaon0LInelastic")
        id = 59;
    else if (pcsName == "kaon0SInelastic")
        id = 60;
    else if (pcsName == "lambdaInelastic")
        id = 61;
    else {
        id = -1;
        cout << "WARNING : The process " << pcsName
             << " was not found. It must be added to TRestGeant4Track::GetProcessID()" << endl;
    }

    return id;
}

EColor TRestGeant4Track::GetParticleColor() {
    EColor color = kGray;

    if (GetParticleName() == "e-")
        color = kRed;
    else if (GetParticleName() == "e+")
        color = kBlue;
    else if (GetParticleName() == "alpha")
        color = kOrange;
    else if (GetParticleName() == "mu-")
        color = kViolet;
    else if (GetParticleName() == "gamma")
        color = kGreen;
    else
        cout << "TRestGeant4Track::GetParticleColor. Particle NOT found! Returning "
                "gray color."
             << endl;

    return color;
}

Double_t TRestGeant4Track::GetTrackLength() {
    Double_t length = 0;

    length = GetDistance(fHits.GetPosition(0), GetTrackOrigin());

    for (int i = 1; i < GetNumberOfHits(); i++) {
        TVector3 prevHit = fHits.GetPosition(i - 1);
        TVector3 hit = fHits.GetPosition(i);
        length += GetDistance(hit, prevHit);
    }
    return length;
}

TString TRestGeant4Track::GetProcessName(Int_t id) {
    if (id == 0)
        return "initStep";
    else if (id == 1)
        return "Transportation";
    else if (id == 2)
        return "ionIoni";
    else if (id == 3)
        return "phot";
    else if (id == 4)
        return "eIoni";
    else if (id == 5)
        return "eBrem";
    else if (id == 6)
        return "msc";
    else if (id == 7)
        return "compt";
    else if (id == 8)
        return "Rayl";
    else if (id == 9)
        return "conv";
    else if (id == 10)
        return "annihil";
    else if (id == 11)
        return "RadioactiveDecay";
    else if (id == 12)
        return "muIoni";
    else if (id == 20)
        return "e-Step";
    else if (id == 21)
        return "e+Step";
    // else if ( id == 22 ) return "neutronStep";
    // else if ( id == 23 ) return "alphaStep";
    // else if ( id == 24 ) return "He3Step";
    else if (id == 30)
        return "muBrems";
    else if (id == 31)
        return "muPairProd";
    else if (id == 32)
        return "Decay";
    else if (id == 33)
        return "hIoni";
    else if (id == 34)
        return "hBrems";
    else if (id == 35)
        return "hPairProd";
    else if (id == 36)
        return "hadElastic";
    else if (id == 37)
        return "neutronInelastic";
    else if (id == 38)
        return "nCapture";
    else if (id == 39)
        return "nKiller";
    else if (id == 40)
        return "nuclearStopping";
    else if (id == 41)
        return "CoulombScat";
    else if (id == 42)
        return "photonNuclear";
    else if (id == 43)
        return "protonInelastic";
    else if (id == 44)
        return "pi-Inelastic";
    else if (id == 45)
        return "pi+Inelastic";
    else if (id == 46)
        return "tInelastic";
    else if (id == 47)
        return "dInelastic";
    else if (id == 48)
        return "electronNuclear";
    else if (id == 49)
        return "muonNuclear";
    else if (id == 50)
        return "positronNuclear";
    else if (id == 51)
        return "mu-Step";
    else if (id == 52)
        return "mu+Step";
    else if (id == 53)
        return "ePairProd";
    else if (id == 54)
        return "alphaInelastic";
    else if (id == 55)
        return "H3Inelastic";
    else if (id == 56)
        return "He3Inelastic";
    else if (id == 57)
        return "kaon+Inelastic";
    else if (id == 58)
        return "kaon-Inelastic";
    else if (id == 59)
        return "kaon0LInelastic";
    else if (id == 60)
        return "kaon0SInelastic";
    else if (id == 61)
        return "lambdaInelastic";

    else
        cout << "WARNING : The process ID : " << id << " could not be found" << endl;

    return "";
}

void TRestGeant4Track::PrintTrack(int maxHits) {
    cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
            "++++++++++++"
         << endl;
    cout.precision(10);
    cout << " SubEvent ID : " << fSubEventId << " Global timestamp : " << GetGlobalTime() << " seconds"
         << endl;
    cout.precision(5);
    cout << " Track ID : " << GetTrackID() << " Parent ID : " << GetParentID();
    cout << " Particle : " << GetParticleName() << " Time track length : " << GetTrackTimeLength() << " us"
         << endl;
    cout << " Origin : X = " << GetTrackOrigin().X() << "mm Y = " << GetTrackOrigin().Y()
         << "mm Z = " << GetTrackOrigin().Z() << "mm  Ekin : " << GetKineticEnergy() << " keV" << endl;
    cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
            "++++++++++++"
         << endl;

    int nHits = GetNumberOfHits();
    if (maxHits > 0) {
        nHits = min(maxHits, GetNumberOfHits());
        cout << " Printing only the first " << nHits << " hits of the track" << endl;
    }

    TRestGeant4Hits* hits = GetHits();
    for (int i = 0; i < nHits; i++) {
        cout << " # Hit " << i << " # process : " << GetProcessName(hits->GetHitProcess(i))
             << " volume : " << hits->GetHitVolume(i) << " X : " << hits->GetX(i) << " Y : " << hits->GetY(i)
             << " Z : " << hits->GetZ(i) << " mm" << endl;
        cout << " Edep : " << hits->GetEnergy(i) << " keV Ekin : " << hits->GetKineticEnergy(i) << " keV"
             << " Global time : " << hits->GetTime(i) << " us" << endl;
    }
    cout << endl;
    cout.precision(2);
}
