#include <iostream>
#include <string>
#include "TRestTask.h"
using namespace std;

#include <TString.h>
#include "TRestDetector.h"

#ifndef RestTask_Detector
#define RestTask_Detector

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro
//*** This macro might need update/revision.
//***
//*******************************************************************************************************
Int_t REST_Detector(Int_t run = -1) {
    TRestStringOutput fout;
    if (gDetector == NULL) {
        fout.setcolor(COLOR_BOLDRED);
        fout << "gDetector is NULL" << endl;
    } else {
        gDetector->Print();
    }

    return 1;
}
