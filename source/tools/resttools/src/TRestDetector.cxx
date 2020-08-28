#include "TRestDetector.h"
#include "TClass.h"

TRestDetector* TRestDetector::instantiate(string name) {
    TRestDetector* detector = NULL;
    if (name != "") {
        TClass* c = TClass::GetClass(("TRestDetector" + name).c_str());
        if (c != NULL)  // this means we have the package installed
        {
            detector = (TRestDetector*)c->New();
        } else {
            warning << "unrecognized TRestDataBase implementation: \"" << name << "\"" << endl;
            detector = new TRestDetector();
        }
    } else {
        detector = new TRestDetector();
    }
    if (gDetector != NULL) delete gDetector;
    gDetector = detector;
    return detector;
}