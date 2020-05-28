
Int_t ValidatePlot(string fname) {

    string output = TRestTools::Execute("sha1sum " + fname);
    string sha1sum = Split(output, " ")[0];
    if (sha1sum != "0869177ec497f62967fd5aca3cba1856717e16ec") {
        cout << "incoherent sha1sum: " << sha1sum << endl;
        return -1;
    }
    cout << "Plot png is good" << endl;
    return 0;
}
