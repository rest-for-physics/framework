TRestRun* run = nullptr;
TRestAnalysisTree* ana_tree = nullptr;
TTree* ev_tree = nullptr;
TRestEvent* ev = nullptr;
TRestDataSet* dSet = nullptr;
std::map<std::string, TRestMetadata*> metadata;

void REST_OpenInputFile(const std::string& fileName) {
    if (TRestTools::isRunFile(fileName)) {
        printf("\n%s\n\n", "REST processed file identified. It contains a valid TRestRun.");
        run = new TRestRun(fileName);
        printf("\nAttaching TRestRun %s as run...\n", fileName.c_str());
        ana_tree = run->GetAnalysisTree();
        printf("Attaching TRestAnalysisTree as ana_tree...\n");
        ev_tree = run->GetEventTree();
        printf("Attaching event tree as ev_tree...\n");
        ev = run->GetInputEvent();
        run->GetEntry(0);
        printf("Attaching input event %s as ev...\n", ev->ClassName());
        for (auto& [name, meta] : metadata) delete meta;
        metadata.clear();
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
            metaName = Replace(metaName, " ", "");
            metaName = Replace(metaName, ".", "_");
            metadata[metaName] = md;
            printf("- %s as metadata[\"%s\"]...\n", md->ClassName(), metaName.c_str());
        }
        printf("\n");

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
