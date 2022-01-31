Int_t Validate(string fname) {
    TRestRun* run = new TRestRun(fname);
    if (run->GetEntries() != 4424) {
        cout << "The number of entries is not 4424!" << endl;
        return 1;
    }
    delete run;

    cout << "[\033[92m OK \x1b[0m]" << endl;
    return 0;
}
