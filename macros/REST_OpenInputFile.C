bool gSchemaError = false;

void SchemaErrorHandler(int level, Bool_t abort, const char* location, const char* msg) {
    if (level >= kError) {
        std::string loc(location);
        if (loc.find("TBufferFile") != std::string::npos || loc.find("TStreamerInfo") != std::string::npos) {
            gSchemaError = true;
        }
    }
    DefaultErrorHandler(level, abort, location, msg);
}

TRestRun* run = nullptr;
TRestAnalysisTree* ana_tree = nullptr;
TTree* ev_tree = nullptr;
TRestDataSet* dSet = nullptr;

void REST_OpenInputFile(const std::string& fileName) {
    if (TRestTools::isRunFile(fileName)) {
        printf("\n%s\n\n", "REST processed file identified. It contains a valid TRestRun.");
        gSchemaError = false;
        auto oldHandler = SetErrorHandler(SchemaErrorHandler);
        run = new TRestRun(fileName);
        SetErrorHandler(oldHandler);
        // print number of entries in the run
        printf("\nThe run has %lld entries.\n", run->GetEntries());
        printf("\nAttaching TRestRun %s as run...\n", fileName.c_str());
        ana_tree = run->GetAnalysisTree();
        printf("Attaching TRestAnalysisTree as ana_tree...\n");
        ev_tree = run->GetEventTree();
        printf("Attaching event tree as ev_tree...\n");
        std::string eventType = run->GetInputEvent()->ClassName();
        std::string evcmd = Form("%s* ev = (%s*)run->GetInputEvent();", eventType.c_str(), eventType.c_str());
        gROOT->ProcessLine(evcmd.c_str());
        if (gSchemaError) {
            printf(
                "\nWARNING: Schema errors were detected. This file was produced with an older REST "
                "version.\n");
            printf(
                "Event browsing is disabled to prevent hanging. You can still use ana_tree and "
                "metadata.\n\n");
        } else {
            run->GetEntry(0);
        }
        printf("Attaching input event %s as ev...\n", eventType.c_str());
        for (int n = 0; n < run->GetNumberOfMetadata(); n++) {
            if (n == 0) printf("Attaching Metadata classes:\n");
            std::string metaName = run->GetMetadataNames()[n];
            if (metaName.find("Historic") != string::npos) continue;
            TRestMetadata* md = run->GetMetadata(metaName);
            if (md == nullptr) {
                printf("\nERROR Cannot get metadata pointer for class %s and name%s\n", md->ClassName(),
                       metaName.c_str());
                continue;
            }
            std::string mName = Replace(metaName, " ", "");
            mName = Replace(mName, ".", "_");
            mName = Replace(mName, "-", "_");
            std::string mdcmd = Form("%s* %s = (%s*)run->GetMetadata(\"%s\");", md->ClassName(),
                                     mName.c_str(), md->ClassName(), metaName.c_str());
            gROOT->ProcessLine(mdcmd.c_str());
            printf("- %s as %s\n", md->ClassName(), mName.c_str());
        }
        printf("\n");

    } else if (TRestTools::isDataSet(fileName)) {
        printf("\n%s\n", "REST dataset file identified. It contains a valid TRestDataSet.");
        printf("\nImporting dataset %s as `dSet`\n", fileName.c_str());
        printf("\n%s\n", "The dataset is ready. You may now access the dataset using:");
        printf("\n%s\n", " - dSet->PrintMetadata()");
        printf("%s\n", " - dSet->GetDataFrame().GetColumnNames()");
        printf("%s\n\n", " - dSet->GetTree()->GetEntries()");
        printf("%s\n", " - dSet->GetDataFrame().Display(\"\")->Print()");
        printf("%s\n\n", " - dSet->GetDataFrame().Display({\"colName1\", \"colName2\"})->Print()");
        if (dSet) delete dSet;
        dSet = new TRestDataSet();
        dSet->EnableMultiThreading(false);
        dSet->Import(fileName);
    } else {
        printf("\n%s is not a valid TRestRun or TRestDataSet\n", fileName.c_str());
    }
}
