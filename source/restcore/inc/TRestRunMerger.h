///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRunMerger.h
///
///             A template class for copy/paste
///
///             jul 2015:   First concept
///                 J. Galan
///_______________________________________________________________________________


#ifndef RestCore_TRestRunMerger
#define RestCore_TRestRunMerger

#include <iostream>
using namespace std;

#include "TObject.h"
#include "TFile.h"
#include "TKey.h"
#include "TFileMerger.h"

class TRestRunMerger: public TFileMerger {
    private:
        TFile *fInputFile;

    protected:

        vector <TString> fInputFileList;

    public:

        //Construtor
        TRestRunMerger();
        TRestRunMerger( TString namePattern );

        //Destructor
        virtual ~TRestRunMerger();

        void MergeFiles( TString outputFilename );
        void SetFilelist( TString namePattern );
        void PrintFilelist();

        ClassDef(TRestRunMerger, 1);     // REST event superclass
};
#endif
