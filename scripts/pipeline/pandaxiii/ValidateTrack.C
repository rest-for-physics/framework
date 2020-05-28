
Int_t ValidateTrack(string fname) {
    TRestRun* run = new TRestRun(fname);

    TRestAnalysisTree* AnalysisTree = run->GetAnalysisTree();

    if (AnalysisTree->GetObservableValue<double>("tckBlob_2D_Qhigh_X_R20") != 552.65964439511299133301) {
        return -1;
    }

    if (AnalysisTree->GetObservableValue<double>("tckBlob_2D_Qhigh_Y_R20") != 638.12474718689918518066) {
        return -1;
    }

    if (AnalysisTree->GetObservableValue<double>("hitsAnaSmear_xMean") != 190.11300330629373434022) {
        return -1;
    }

    if (AnalysisTree->GetObservableValue<double>("hitsAnaSmear_yMean") != 672.01805915711759098485) {
        return -1;
    }

    cout << "Track analysis result is good" << endl;
    return 0;
}
