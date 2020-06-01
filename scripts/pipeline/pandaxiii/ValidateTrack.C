Int_t ValidateTrack(string fname) {
    TRestRun* run = new TRestRun(fname);

    TRestAnalysisTree* AnalysisTree = run->GetAnalysisTree();
    
    bool flag=true;
    if (round(1000 * AnalysisTree->GetObservableValue<double>("tckBlob_2D_Qhigh_X_R20")) != 328099) {
        cout<<"wrong tckBlob_2D_Qhigh_X_R20: "<< AnalysisTree->GetObservableValue<double>("tckBlob_2D_Qhigh_X_R20")<<endl;
        flag = false;
    }

    if (round(1000 * AnalysisTree->GetObservableValue<double>("tckBlob_2D_Qhigh_Y_R20")) != 361361) {
        cout<<"wrong tckBlob_2D_Qhigh_Y_R20: "<< AnalysisTree->GetObservableValue<double>("tckBlob_2D_Qhigh_Y_R20")<<endl;
        flag = false;
    }

    if (round(1000 * AnalysisTree->GetObservableValue<double>("hitsAnaSmear_xMean")) != 102933) {
        cout<<"wrong hitsAnaSmear_xMean: "<< AnalysisTree->GetObservableValue<double>("hitsAnaSmear_xMean")<<endl;
        flag = false;
    }

    if (round(1000 * AnalysisTree->GetObservableValue<double>("hitsAnaSmear_yMean")) != -636089) {
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
