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
    TRestStringOutput RESTfout;
    if (gDataBase == NULL) {
        RESTfout.setcolor(COLOR_BOLDRED);
        RESTfout << "gDataBase is NULL" << RESTendl;
    } else {
        if (run < 0) run = gDataBase->get_lastrun();
        gDataBase->print(Form("select * from rest_runs where run_id=%i", run));
    }

    return 1;
}
#endif
