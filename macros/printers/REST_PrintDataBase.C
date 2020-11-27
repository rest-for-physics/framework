#include <iostream>
#include <string>
#include "TRestTask.h"
using namespace std;

#include <TString.h>
#include "TRestDataBase.h"

#ifndef RestTask_DataBase
#define RestTask_DataBase

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro
//*** This macro might need update/revision.
//***
//*******************************************************************************************************
Int_t REST_DataBase(Int_t run = -1) {
    TRestStringOutput fout;
    if (gDataBase == NULL) {
        fout.setcolor(COLOR_BOLDRED);
        fout << "gDataBase is NULL" << endl;
    } else {
        if (run < 0) run = gDataBase->get_lastrun();
        gDataBase->print(Form("select * from rest_runs where run_id=%i", run));
    }

    return 1;
}
#endif
