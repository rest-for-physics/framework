/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

#ifndef RestCore_TRestDetectorReadoutPlane
#define RestCore_TRestDetectorReadoutPlane

#include <iostream>

#include "TObject.h"

#include "TRestDetectorReadoutChannel.h"
#include "TRestDetectorReadoutModule.h"
#include "TRestMetadata.h"

#include <TGraph.h>
#include <TH2Poly.h>

/// A class to store the readout plane definition used in TRestDetectorReadout. It
/// allows to integrate any number of independent readout modules.
class TRestDetectorReadoutPlane : public TObject {
   private:
    Int_t fPlaneID;  ///< The readout plane id. The id number is imposed by the
                     ///< order of creation. Being the first id=0.

    TVector3 fPosition;            ///< The position of the readout plane. The relative position
                                   ///< of the modules will be shifted by this value.
    TVector3 fPlaneVector;         ///< The plane vector definning the plane orientation
                                   ///< and the side of the active volume.
    TVector3 fCathodePosition;     ///< The cathode position which delimites the active
                                   ///< volume together with the readout plane.
    Double_t fChargeCollection;    ///< A parameter between 0 and 1 definning how
                                   ///< much charge should be collected from a
                                   ///< charge hit. It might be used to distribute
                                   ///< the charge between different readout planes.
    Double_t fTotalDriftDistance;  ///< A parameter storing the total drift distance,
                                   ///< defined between cathode and readout plane.

    Int_t fNModules;  ///< The number of modules that have been added to the
                      ///< readout plane
    std::vector<TRestDetectorReadoutModule>
        fReadoutModules;  ///< A vector of the instances of TRestDetectorReadoutModule
                          ///< containned in the readout plane.

    void Initialize();

   protected:
   public:
    // Setters
    /// Sets the planeId. This is done by TRestDetectorReadout during initialization
    void SetID(int id) { fPlaneID = id; }

    /// Sets the readout plane position
    void SetPosition(TVector3 pos) { fPosition = pos; }

    /// Sets the cathode plane position. By default is parallel to the readout
    /// plane.
    void SetCathodePosition(TVector3 pos) { fCathodePosition = pos; }

    /// Sets the orientation of the readout plane, and defines the side of the
    /// active volume.
    void SetPlaneVector(TVector3 vect) { fPlaneVector = vect.Unit(); }

    /// Sets the value for the charge collection.
    void SetChargeCollection(Double_t charge) { fChargeCollection = charge; }

    /// Sets the value for the total drift distance
    void SetTotalDriftDistance(Double_t d) { fTotalDriftDistance = d; }

    // Getters
    /// Returns an integer with the plane id number.
    Int_t GetID() { return fPlaneID; }

    /// Returns a TVector3 with the readout plane position
    TVector3 GetPosition() { return fPosition; }

    /// Returns a TVector3 with the cathode position
    TVector3 GetCathodePosition() { return fCathodePosition; }

    /// Returns a TVector3 with a vector normal to the readout plane
    TVector3 GetPlaneVector() { return fPlaneVector; }

    /// Returns the charge collection ratio at this readout plane
    Double_t GetChargeCollection() { return fChargeCollection; }

    /// Returns the total drift distance
    Double_t GetTotalDriftDistance() { return fTotalDriftDistance; }

    /// Returns the perpendicular distance to the readout plane from a given
    /// position *pos*.
    Double_t GetDistanceTo(TVector3 pos);

    /// Returns the perpendicular distance to the readout plane from a given
    /// position *x*, *y*, *z*.
    Double_t GetDistanceTo(Double_t x, Double_t y, Double_t z);

    /// Returns a TVector2 oriented as the shortest distance of a given position
    /// *pos* on the plane to a specific module with id *mod*
    TVector2 GetDistanceToModule(Int_t mod, TVector2 pos) {
        return GetModuleByID(mod)->GetDistanceToModule(pos);
    }

    TRestDetectorReadoutModule& operator[](int mod) { return fReadoutModules[mod]; }

    /// Returns a pointer to a readout module using its vector index
    TRestDetectorReadoutModule* GetModule(int mod) {
        if (mod >= GetNumberOfModules()) return NULL;
        return &fReadoutModules[mod];
    }

    /// Returns the total number of modules in the readout plane
    Int_t GetNumberOfModules() { return fReadoutModules.size(); }

    /// Adds a new module to the readout plane
    void AddModule(TRestDetectorReadoutModule& rModule) {
        fReadoutModules.push_back(rModule);
        fNModules++;
    }

    /// Prints the readout plane description
    void PrintMetadata() { Print(); }

    Int_t GetNumberOfChannels();

    TRestDetectorReadoutModule* GetModuleByID(Int_t modID);

    Int_t isZInsideDriftVolume(Double_t z);

    Int_t isZInsideDriftVolume(TVector3 pos);

    Bool_t isDaqIDInside(Int_t daqId);

    Int_t GetModuleIDFromPosition(TVector3 pos);

    Int_t GetModuleIDFromPosition(Double_t x, Double_t y, Double_t z);

    void SetDriftDistance();

    void Draw();

    void Print(Int_t DetailLevel = 0);

    Int_t FindChannel(Int_t module, Double_t absX, Double_t absY);

    Double_t GetX(Int_t modID, Int_t chID);
    Double_t GetY(Int_t modID, Int_t chID);

    TH2Poly* GetReadoutHistogram();
    void GetBoundaries(double& xmin, double& xmax, double& ymin, double& ymax);

    // Construtor
    TRestDetectorReadoutPlane();
    // Destructor
    virtual ~TRestDetectorReadoutPlane();

    ClassDef(TRestDetectorReadoutPlane, 1);
};
#endif
