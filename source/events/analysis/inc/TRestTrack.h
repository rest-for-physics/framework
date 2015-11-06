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

class TRestTrack: public TObject {

    protected:
        
     Int_t fTrackID;		///< Track ID
     Int_t fTrackCharge;		///< Total charge of the track
     Int_t fTrackEnergy;		///< Total energy of the track
     Double_t fTrackLength;	///< Total length of the track

     Int_t fNtrackPoints;		///< Number of points that define the track
     vector <TVector3> fTrackPointPosition;  //[fNtrackPoints]  Position of the track points 

     //Clusters
     Int_t fNClusters;   			///< Number of clusters or charge accumulations within the track
     vector <TVector3> fClusterPosition;  	//[fNClusters] Position of the clusters
     vector <TVector3> fClusterWidth; 	//[fNClusters] Width of the clusters
             				  
           
    public:
        
        //Setters
        void SetTrackID(Int_t sID){fTrackID=sID;}
        void SetTrackChage(Int_t charge){fTrackCharge=charge;}
        void SetTrackEnergy(Int_t energy){fTrackEnergy=energy;}
        void SetTrackLength(Int_t length){fTrackEnergy=length;}

        //Getters

        Int_t GetTrackID() { return fTrackID; }
        Int_t GetTrackCharge() { return fTrackCharge; }
        Int_t GetTrackEnergy() { return fTrackEnergy; }
        Double_t GetTrackLength() { return fTrackLength; }
        Int_t GetNTrackPoints() { return fNtrackPoints; }
        TVector3 GetTrackPointPosition(int n) {return  fTrackPointPosition[n];}

        Int_t GetNClusters() { return fNClusters; }
        TVector3 GetClusterPosition(int n) {return  fClusterPosition[n];}
        TVector3 GetClusterWidth(int n) {return  fClusterWidth[n];}
        Double_t GetClusterSize(int n) { return fClusterWidth[n].Mag(); }
        Double_t GetXYSize(int n) { return fClusterWidth[n].Perp(); }


        //Construtor
        TRestTrack();
        //Destructor
        ~TRestTrack();
        
        ClassDef(TRestTrack, 1);


};
#endif



