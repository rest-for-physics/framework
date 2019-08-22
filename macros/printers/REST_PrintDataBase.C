#include <iostream>
#include <string>
#include "TRestTask.h"
using namespace std;

#include <TString.h>
#include "TRestDataBase.h"

Int_t REST_DataBase(Int_t run = -1) {
    TRestStringOutput fout;
    TRestDataBase* db = TRestDataBase::instantiate();
    if (db == NULL) {
        fout.setcolor(COLOR_BOLDRED);
        fout << "REST ERROR!! package: restDataBaseImpl not installed!" << endl;
        fout << "cannot print database record!" << endl;
    } else {
        if (run < 0) run = db->get_lastrun();
		db->print(run);
    }

    return 0;
}
