///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestReadoutEventViewer.h inherited from TRestEventViewer
///
///             dec 2015:   First concept
///                 Viewer class for a TRestSignalEvent and TRestReadout
///                 JuanAn Garcia
///_______________________________________________________________________________

#ifndef RestCore_TRestReadoutEventViewer
#define RestCore_TRestReadoutEventViewer

#include <TCanvas.h>
#include <TPad.h>

#include "TRestEventViewer.h"
#include "TRestSignalEvent.h"
//#include "TRestDecoding.h"
#include "TH2Poly.h"
#include "TRestReadout.h"

class TRestReadoutEventViewer : public TRestEventViewer {
   protected:
    TCanvas* fCanvasXY;
    TCanvas* fCanvasXZYZ;

    // TRestDecoding *fDecoding;
    TRestReadout* fReadout;

    TRestSignalEvent* fSignalEvent;

    TH2Poly* fHistoXY;
    TH2D* fHistoXZ;
    TH2D* fHistoYZ;

    bool isfirstEvent;

    double xmin, xmax, ymin, ymax, zmin, zmax;

   public:
    void Initialize();
    // Finalize initialization based on arg. TRestReadout
    void SetReadout(TRestReadout* readout);

    void AddEvent(TRestEvent* ev);
    void DrawReadoutPulses();
    TRestReadoutChannel* GetChannel(int readoutChannel);
    TRestReadoutModule* GetModule(int readoutChannel);

    // Constructor
    TRestReadoutEventViewer();
    // Destructor
    ~TRestReadoutEventViewer();

    ClassDef(TRestReadoutEventViewer, 1);  // class inherited from
                                           // TRestEventViewer
};
#endif
