TRestRun* run = nullptr;
TRestAnalysisTree* ana_tree = nullptr;
TTree* ev_tree = nullptr;
TRestEvent* ev = nullptr;
TRestDataSet* dSet = nullptr;
std::map<std::string, TRestMetadata*> metadata;

void REST_OpenInputFile(const std::string& fileName) {
    if (TRestTools::isRunFile(fileName)) {
        printf("\n%s\n", "REST processed file identified. It contains a valid TRestRun.");
        run = new TRestRun(fileName);
        printf("\nAttaching TRestRun %s as run...\n", fileName.c_str());
        ana_tree = run->GetAnalysisTree();
        printf("\nAttaching TRestAnalysisTree as ana_tree...\n");
        ev_tree = run->GetEventTree();
        printf("\nAttaching event tree as ev_tree...\n");
        ev = run->GetInputEvent();
        run->GetEntry(0);
        printf("\nAttaching input event %s as ev...\n", ev->ClassName());
        std::map<std::string, int> metanames;
        for (auto& [name, meta] : metadata) delete meta;
        metadata.clear();
        for (int n = 0; n < run->GetNumberOfMetadata(); n++) {
            std::string metaName = run->GetMetadataNames()[n];
            if (metaName.find("Historic") != string::npos) continue;
            TRestMetadata* md = run->GetMetadata(metaName);
            metadata[metaName] = md;
            printf("\nAttaching Metadata class %s as metadata[\"%s\"]...\n", md->ClassName(),
                   metaName.c_str());
        }

    } else if (TRestTools::isDataSet(fileName)) {
        printf("\n%s\n", "REST dataset file identified. It contains a valid TRestDataSet.");
        printf("\nImporting dataset %s as `dSet`\n", fileName.c_str());
        printf("\n%s\n", "The dataset is ready. You may now access the dataset using:");
        printf("\n%s\n", " - dSet->PrintMetadata()");
        printf("%s\n", " - dSet->GetDataFrame().GetColumnNames()");
        printf("%s\n\n", " - dSet->GetTree()->GetEntries()");
        if (dSet) delete dSet;
        dSet = new TRestDataSet();
        dSet->Import(fileName);
    } else {
        printf("\n%s is not a valid TRestRun or TRestDataSet\n", fileName.c_str());
    }
}
