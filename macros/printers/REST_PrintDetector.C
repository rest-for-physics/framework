#include <iostream>
#include <string>
#include "TRestTask.h"
using namespace std;

#include <TString.h>
#include "TRestDetector.h"

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
