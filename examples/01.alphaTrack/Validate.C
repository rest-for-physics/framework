Int_t Validate(const char* filename) {
    TRestRun run(filename);
    cout << "Run entries : " << run.GetEntries() << endl;

    constexpr double numberOfEntriesRef = 4551;
    constexpr double tolerance = 0.05;

    if (TMath::Abs(run.GetEntries() - numberOfEntriesRef) / numberOfEntriesRef > tolerance) {
        cout << "The number of entries does not match the reference value of " << numberOfEntriesRef << endl;
        return 1;
    }

    cout << "[\033[92m OK \x1b[0m]" << endl;
    return 0;
}
