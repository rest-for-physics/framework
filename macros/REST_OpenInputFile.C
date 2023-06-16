TRestRun* run0 = nullptr;
TRestAnalysisTree* ana_tree0 = nullptr;
TTree* ev_tree0 = nullptr;
TRestDataSet* dSet0 = nullptr;
std::map<std::string, TRestMetadata*> metadata0;

void REST_OpenInputFile(const std::string& fileName) {
    if (TRestTools::isRunFile(fileName)) {
        printf("\n%s\n", "REST processed file identified. It contains a valid TRestRun.");
        run0 = new TRestRun(fileName);
        printf("\nAttaching TRestRun %s as run0...\n", fileName.c_str());
        ana_tree0 = run0->GetAnalysisTree();
        printf("\nAttaching TRestAnalysisTree as ana_tree0...\n");
        ev_tree0 = run0->GetEventTree();
        printf("\nAttaching event tree as ev_tree0...\n");
        std::map<std::string, int> metanames;
        for (auto& [name, meta] : metadata0) delete meta;
        metadata0.clear();
        for (int n = 0; n < run0->GetNumberOfMetadata(); n++) {
            std::string metaName = run0->GetMetadataNames()[n];
            if (metaName.find("Historic") != string::npos) continue;
            TRestMetadata* md = run0->GetMetadata(metaName);
            metadata0[metaName] = md;
            printf("\nAttaching Metadata class %s as metadata0[\"%s\"]...\n", md->ClassName(),
                   metaName.c_str());
        }

    } else if (TRestTools::isDataSet(fileName)) {
        printf("\n%s\n", "REST dataset file identified. It contains a valid TRestDataSet.");
        printf("\nImporting dataset %s as `dSet0`\n", fileName.c_str());
        printf("\n%s\n", "The dataset is ready. You may now access the dataset using:");
        printf("\n%s\n", " - dSet0->PrintMetadata()");
        printf("%s\n", " - dSet0->GetDataFrame().GetColumnNames()");
        printf("%s\n\n", " - dSet0->GetTree()->GetEntries()");
        if (dSet0) delete dSet0;
        dSet0 = new TRestDataSet();
        dSet0->Import(fileName);
    } else {
        printf("\n%s is not a valid TRestRun or TRestDataSet\n", fileName.c_str());
    }
}
