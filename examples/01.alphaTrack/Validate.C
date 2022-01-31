Int_t Validate(string fname) {
    TRestRun* run = new TRestRun(fname);
    cout << "Run entries : " << run->GetEntries() << endl;
    if (run->GetEntries() < 4350 || run->GetEntries() > 4500) {
        cout << "The number of entries is not in the range (4350,4500)!" << endl;
        return 1;
    }
    delete run;

    cout << "[\033[92m OK \x1b[0m]" << endl;
    return 0;
}
