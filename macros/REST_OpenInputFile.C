
TRestRun* run0 = nullptr;
TRestAnalysisTree* ana_tree0 = nullptr;
TTree* ev_tree0 = nullptr;
TRestDataSet* dSet0 = nullptr;

void REST_OpenInputFile(const std::string& fileName) {
    if (TRestTools::isRunFile(fileName)) {
        printf("\n%s\n", "REST processed file identified. It contains a valid TRestRun.");
        run0 = new TRestRun(fileName);
        printf("\nAttaching TRestRun %s as run0...\n", fileName.c_str());
        ana_tree0 = run0->GetAnalysisTree();
        printf("\nAttaching TRestAnalysisTree as ana_tree0...\n");
        ev_tree0 = run0->GetEventTree();
        printf("\nAttaching event tree as ev_tree0...\n");

    } else if (TRestTools::isDataSet(fileName)) {
        printf("\n%s\n", "REST dataset file identified. It contains a valid TRestDataSet.");
        printf("\nImporting dataset %s as `dSet0`\n", fileName.c_str());
        printf("\n%s\n", "The dataset is ready. You may now access the dataset using:");
        printf("\n%s\n", " - dSet0->PrintMetadata()");
        printf("%s\n", " - dSet0->GetDataFrame().GetColumnNames()");
        printf("%s\n\n", " - dSet0->GetTree()->GetEntries()");
        dSet0 = new TRestDataSet();
        dSet0->Import(fileName);
    } else {
        printf("\n%s is not a valid TRestRun or TRestDataSet\n", fileName.c_str());
    }
}
