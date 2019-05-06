///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestReadoutEventViewer.cxx
///
///             dec 2015:   First concept
///                 Viewer class for a TRestSignalEvent and TRestReadout
///                 JuanAn Garcia
///_______________________________________________________________________________

#include "TRestReadoutEventViewer.h"
using namespace std;

Int_t planeId = 0;

ClassImp(TRestReadoutEventViewer)

    TRestReadoutEventViewer::TRestReadoutEventViewer(char* cfgFilename) {
  fReadout = new TRestReadout(cfgFilename);
  // fDecoding = new TRestDecoding(cfgFilename);

  Initialize();
}

//______________________________________________________________________________
TRestReadoutEventViewer::~TRestReadoutEventViewer() {}

//______________________________________________________________________________
void TRestReadoutEventViewer::Initialize() {
  TRestGenericEventViewer::Initialize();

  fCanvasXY = new TCanvas("ReadoutMap", "ReadoutMap");
  fCanvasXZYZ = new TCanvas("XZYZ", "XZYZ");
  fCanvasXZYZ->Divide(2, 1);

  fSignalEvent = new TRestSignalEvent();
  SetEvent(fSignalEvent);

  cout << "WARNING : Only plane 0 is drawn. Implementation to draw several "
          "planes or to choose the plane must be implemented."
       << endl;
  fReadout->PrintMetadata();
  TRestReadoutPlane* plane = &(*fReadout)[planeId];
  fHistoXY = plane->GetReadoutHistogram();
  plane->GetBoundaries(xmin, xmax, ymin, ymax);
}

void TRestReadoutEventViewer::AddEvent(TRestEvent* ev) {
  TRestGenericEventViewer::AddEvent(ev);

  if (fPad == NULL) return;

  fHistoXY->Reset(0);
  delete fHistoXZ;
  delete fHistoYZ;

  DrawReadoutPulses();
  fCanvasXY->cd();
  fHistoXY->Draw("COLZ");
  fCanvasXY->Update();

  fCanvasXZYZ->cd(1);
  fHistoXZ->Draw("CONTZ");
  fCanvasXZYZ->cd(2);
  fHistoYZ->Draw("CONTZ");
  fCanvasXZYZ->Update();
}

void TRestReadoutEventViewer::DrawReadoutPulses() {
  int readoutChannel, daqChannel;
  double charge;

  Int_t modId;
  TRestReadoutModule* module = NULL;
  TRestReadoutChannel* channel;

  int maxIndex;

  zmin = 1E9;
  zmax = 1E-9;

  double z;
  for (int i = 0; i < fSignalEvent->GetNumberOfSignals(); i++)
    for (int j = 0; j < fSignalEvent->GetSignal(i)->GetNumberOfPoints(); j++) {
      z = fSignalEvent->GetSignal(i)->GetTime(j);
      if (z < zmin) zmin = z;
      if (z > zmax) zmax = z;
    }

  zmax++;
  zmin--;

  fHistoXZ = new TH2D("XZ", "XZ", 100, xmin, xmax, 100, zmin, zmax);
  fHistoYZ = new TH2D("YZ", "YZ", 100, ymin, ymax, 100, zmin, zmax);

  for (int i = 0; i < fSignalEvent->GetNumberOfSignals(); i++) {
    daqChannel = fSignalEvent->GetSignal(i)->GetSignalID();

    TRestReadoutPlane* plane = &(*fReadout)[planeId];
    for (int m = 0; m < plane->GetNumberOfModules(); m++) {
      module = &(*plane)[m];

      if (module->isDaqIDInside(daqChannel)) break;
    }
    modId = module->GetModuleID();

    readoutChannel = module->DaqToReadoutChannel(daqChannel);
    cout << "daqChannel " << daqChannel << " readoutChannel " << readoutChannel
         << endl;
    // if((module = GetModule(readoutChannel))==NULL)continue;
    if ((channel = GetChannel(readoutChannel)) == NULL) continue;

    int nPixels = channel->GetNumberOfPixels();

    Double_t xRead = plane->GetX(modId, readoutChannel);
    Double_t yRead = plane->GetY(modId, readoutChannel);

    // Pixel readout
    Int_t xStrip = 0;
    Int_t yStrip = 0;
    if (TMath::IsNaN(yRead)) xStrip = 1;
    if (TMath::IsNaN(xRead)) yStrip = 1;
    if (xStrip == 0 && yStrip == 0) {
      for (int j = 0; j < fSignalEvent->GetSignal(i)->GetNumberOfPoints(); j++)
        fHistoXZ->Fill(xRead, fSignalEvent->GetSignal(i)->GetTime(j),
                       fSignalEvent->GetSignal(i)->GetData(j));
      for (int j = 0; j < fSignalEvent->GetSignal(i)->GetNumberOfPoints(); j++)
        fHistoYZ->Fill(yRead, fSignalEvent->GetSignal(i)->GetTime(j),
                       fSignalEvent->GetSignal(i)->GetData(j));
    }
    // Strips readout
    else {
      if (yStrip == 0) {
        for (int j = 0; j < fSignalEvent->GetSignal(i)->GetNumberOfPoints();
             j++)
          fHistoXZ->Fill(xRead, fSignalEvent->GetSignal(i)->GetTime(j),
                         fSignalEvent->GetSignal(i)->GetData(j));
      } else if (xStrip == 0) {
        for (int j = 0; j < fSignalEvent->GetSignal(i)->GetNumberOfPoints();
             j++) {
          fHistoYZ->Fill(yRead, fSignalEvent->GetSignal(i)->GetTime(j),
                         fSignalEvent->GetSignal(i)->GetData(j));
        }
      }
    }

    // Only the maximum is plotted for the XY readout
    maxIndex = fSignalEvent->GetSignal(i)->GetMaxIndex();
    charge = fSignalEvent->GetSignal(i)->GetData(maxIndex);

    for (int px = 0; px < nPixels; px++) {
      // cout<< "X : " << module->GetPixelCenter( readoutChannel, px ).X()<<" Y
      // : "<<module->GetPixelCenter( readoutChannel, px ).Y()<<" Ch:
      // "<<charge<<endl;
      fHistoXY->Fill(module->GetPixelCenter(readoutChannel, px).X(),
                     module->GetPixelCenter(readoutChannel, px).Y(), charge);
    }
  }
}

TRestReadoutChannel* TRestReadoutEventViewer::GetChannel(int readoutChannel) {
  TRestReadoutPlane* plane = &(*fReadout)[0];
  for (int n = 0; n < plane->GetNumberOfModules(); n++) {
    if ((*plane)[n].GetChannel(readoutChannel) == NULL) continue;
    return (*plane)[n].GetChannel(readoutChannel);
  }

  cout << "Readout channel " << readoutChannel << " not found" << endl;
  return NULL;
}

TRestReadoutModule* TRestReadoutEventViewer::GetModule(int readoutChannel) {
  TRestReadoutPlane* plane = &(*fReadout)[0];
  for (int n = 0; n < fReadout->GetNumberOfModules(); n++) {
    if ((*plane)[n].GetChannel(readoutChannel) == NULL) continue;
    return &(*plane)[n];
  }

  cout << "Readout channel " << readoutChannel << " not found" << endl;
  return NULL;
}
