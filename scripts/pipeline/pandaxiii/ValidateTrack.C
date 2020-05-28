
Int_t ValidateTrack(string fname) {
    TRestRun* run = new TRestRun(fname);

    TRestAnalysisTree* AnalysisTree = run->GetAnalysisTree();

    if (AnalysisTree->GetObservableValue<double>("tckBlob_2D_Qhigh_X_R20") != 404.79117548465728759766) {
        return -1;
    }

    if (AnalysisTree->GetObservableValue<double>("tckBlob_2D_Qhigh_Y_R20") != 301.09379857778549194336) {
        return -1;
    }

    if (AnalysisTree->GetObservableValue<double>("hitsAnaSmear_xMean") != 112.28328575433145886109) {
        return -1;
    }

    if (AnalysisTree->GetObservableValue<double>("hitsAnaSmear_yMean") != 567.69430957018153094396) {
        return -1;
    }

    cout << "Track analysis result is good" << endl;
    return 0;
}
