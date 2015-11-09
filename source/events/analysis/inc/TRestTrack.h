///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrack.h
///
///             Event class to store signals form simulation and acquisition events 
///
///             oct 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Gracia
///_______________________________________________________________________________


#ifndef RestCore_TRestTrack
#define RestCore_TRestTrack

#include <iostream>
using namespace std;

#include <TObject.h>
#include <TVector3.h>
#include <TRestVolumeHits.h>

class TRestTrack: public TObject {

    protected:
        
     Int_t fTrackID;		///< Track ID
     Int_t fTrackEnergy;		///< Total energy of the track
     Double_t fTrackLength;	///< Total length of the track

     //Hit volumes		
     TRestVolumeHits fVolumeHits;  	///< Hit volumes that define a track  				  

             				  
           
    public:

	void Initialize();
        
        //Setters
        void SetTrackID(Int_t sID){fTrackID=sID;}
        void SetTrackEnergy(Int_t energy){fTrackEnergy=energy;}
        void SetTrackLength(Int_t length){fTrackEnergy=length;}

        //Getters

        Int_t GetTrackID() { return fTrackID; }
        Int_t GetTrackEnergy() { return fTrackEnergy; }
        Double_t GetTrackLength() { return fTrackLength; }

        TRestVolumeHits GetVolumeHits() {return fVolumeHits;}
 

 
        //Construtor
        TRestTrack();
        //Destructor
        ~TRestTrack();
        
        ClassDef(TRestTrack, 1);


};
#endif



