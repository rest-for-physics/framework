Int_t ValidateTrack(string fname) {
    TRestRun* run = new TRestRun(fname);

    TRestAnalysisTree* AnalysisTree = run->GetAnalysisTree();
    
    bool flag=true;
    if (AnalysisTree->GetObservableValue<double>("tckBlob_2D_Qhigh_X_R20") != 404.79117548465728759766) {
        cout<<"wrong tckBlob_2D_Qhigh_X_R20: "<< AnalysisTree->GetObservableValue<double>("tckBlob_2D_Qhigh_X_R20")<<endl;
        flag = false;
    }

    if (AnalysisTree->GetObservableValue<double>("tckBlob_2D_Qhigh_Y_R20") != 301.09379857778549194336) {
        cout<<"wrong tckBlob_2D_Qhigh_Y_R20: "<< AnalysisTree->GetObservableValue<double>("tckBlob_2D_Qhigh_Y_R20")<<endl;
        flag = false;
    }

    if (AnalysisTree->GetObservableValue<double>("hitsAnaSmear_xMean") != 112.28328575433145886109) {
        cout<<"wrong hitsAnaSmear_xMean: "<< AnalysisTree->GetObservableValue<double>("hitsAnaSmear_xMean")<<endl;
        flag = false;
    }

    if (AnalysisTree->GetObservableValue<double>("hitsAnaSmear_yMean") != 567.69430957018153094396) {
        cout<<"wrong hitsAnaSmear_yMean: "<< AnalysisTree->GetObservableValue<double>("hitsAnaSmear_yMean")<<endl;
        flag = false;
    }  

    if(flag){
        cout << "Track analysis result is good" << endl;
        return 0;
    }
    else{
        return -1;
    }
}
