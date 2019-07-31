////////////////////////////////////////////////////////////////////////////////////////////
//
// Author : Gloria Luzon
// Date : 26/July/2019
// Description : It prints out on screen Geant4 events in the ROI between entries n1 and n2.
//
////////////////////////////////////////////////////////////////////////////////////////////
Int_t REST_ReadNEvents_inROI(TString fName, int n1, int n2, double en1, double en2) {
    TRestRun* run = new TRestRun();

    string fname = fName.Data();
    // if( !fileExists( fname ) ) { cout << "WARNING. Input file does not exist" << endl; exit(1); }

    // else run->OpenInputFile( fName );

    run->OpenInputFile(fName);

    run->PrintInfo();

    /////////////////////////////

    /////////////////////////////
    // Reading events
    int n = 0;
    double en = 0;
    TRestG4Event* ev = new TRestG4Event();

    run->SetInputEvent(ev);
    for (int i = n1; i < n2 + 1; i++) {
        run->GetEntry(i);
        en = ev->GetSensitiveVolumeEnergy();
        if ((en >= en1) && (en <= en2)) {
            ev->PrintEvent();
            n++;
            cout << n << " event with " << en << " energy in sensitive volume " << endl;
        }
    }

    delete ev;

    // Do what ever you want with g4Event

    /////////////////////////////

    delete run;
    return 0;
}
