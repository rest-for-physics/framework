Int_t ValidatePlot(string fname) {
    string output = TRestTools::Execute("sha1sum " + fname);
    string sha1sum = Split(output, " ")[0];
    if (sha1sum == "d0ad272769250daa47c37221dae2a759c278cb53" ||
        sha1sum == "6f7c62d923504e0cacfb3dd93e5b598c3277b573") {
        cout << "Plot png is good" << endl;
        return 0;
    }
    cout << "incoherent sha1sum: " << sha1sum << endl;
    return -1;
}
