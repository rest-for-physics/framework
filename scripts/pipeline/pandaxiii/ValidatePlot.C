
Int_t ValidatePlot(string fname) {

    string output = TRestTools::Execute("sha1sum " + fname);
    string sha1sum = Split(output, " ")[0];
    if (sha1sum != "996d57a289c53a0d3490a85162982f84c2cb2a43") {
        cout << "incoherent sha1sum: " << sha1sum << endl;
        return -1;
    }
    cout << "Plot png is good" << endl;
    return 0;
}
