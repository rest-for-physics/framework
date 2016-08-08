///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             myManager.h
///
///             apr 2016    Javier Galan
///_______________________________________________________________________________


#ifndef _myManager
#define _myManager

#include <TRestManager.h>

class myManager:public TRestManager {
    private:

        virtual void Initialize();

    protected:

        virtual void LoadExternalProcess( TString processType, std::string processesCfgFile, std::string processName );


    public:

        //Construtor
        myManager();
        myManager( const char *cfgFileName, const char *name = "" );
        //Destructor
        virtual ~ myManager();


        ClassDef(myManager, 1);     // REST readout class
};
#endif
