Int_t ValidatePlot(string fname) {

    string output = TRestTools::Execute("sha1sum " + fname);
    string sha1sum = Split(output, " ")[0];
    if (sha1sum != "d0ad272769250daa47c37221dae2a759c278cb53") {
        cout << "incoherent sha1sum: " << sha1sum << endl;
        return -1;
    }
    cout << "Plot png is good" << endl;
    return 0;
}
