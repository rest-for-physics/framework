{
    auto f = new TFile("Hits_exprEval_01929.root");
    auto t = (TTree*) f->Get("AnalysisTree");

    // after our filtering we expect 53 remaining elements of the input 100
    if(t->GetEntries() != 53) return 1;

    // define variables for branch access
    int testExpr;
    int someBool;
    double milliEnergy;
    int rawAna_test;
    double rawAnaRisetime692;
    double hitsAna_energy;
    // set them
    t->SetBranchAddress("testExpr", &testExpr);
    t->SetBranchAddress("someBool", &someBool);
    t->SetBranchAddress("milliEnergy", &milliEnergy);
    t->SetBranchAddress("rawAna_test", &rawAna_test);
    t->SetBranchAddress("rawAnaRisetime692", &rawAnaRisetime692);
    t->SetBranchAddress("hitsAna_energy", &hitsAna_energy);
    // counts number of times these boolean variables are true
    int someBoolCountTrue = 0;
    int rawAnaTestCountTrue = 0;
    // we'll use this as a count table to check which entry appeared how often
    map<int, int> rawAnaHits692Vals;
    // walk through tree
    for(int i = 0; i < t->GetEntries(); i++){
        t->GetEntry(i);
        // break if milli energy does not match real energy
        if((milliEnergy * 1000.0 - hitsAna_energy) > 1e-6) return 2;
        // testExpr is just 5<10, so always true
        if(!testExpr) return 3;
        if(someBool) someBoolCountTrue++;
        // have a filter for energy to be < 5000
        if(hitsAna_energy >= 5000) return 4;
        if(rawAna_test) rawAnaTestCountTrue++;
        // increase count
        rawAnaHits692Vals[rawAnaRisetime692]++;
    }

    // expect someBool to be true 22 times out of 53
    if(someBoolCountTrue != 22) return 5;
    if(rawAnaHits692Vals[21] != 1) return 6;
    if(rawAnaHits692Vals[22] != 1) return 7;
    if(rawAnaHits692Vals[0] != 51) return 8;
    cout << "All tests passed" << endl;
}
