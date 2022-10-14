//*******************************************************************************************************
//*** Description: This macro receives as input two variable names that must be present inside the
//*** analysis tree. It creates a TH2D histogram using those variables. The histogram limits and range
//*** may be specified in the last argument, following the ROOT `Draw` method, range definition format.
//***
//*** This method is typically used to generate the response matrix of detector simulations using
//*** Geant4, although it could be used to generate a matrix in binary format of a TH2 object defined
//*** by any two analysis tree variables.
//*** --------------
//*** The output file will be a binary file containing the table, this table could be read later on
//*** using the method TRestTools::ReadBinaryTable.
//*** --------------
//*** Usage: restManager GenerateResponseMatrix /full/path/file.root [varX] [varY] [range]
//***
//*** Input arguments:
//*** - `varX` and `varY` : two variables inside the analysis tree.
//*** - `range` : If given, it defines the histogram limits and the binning. It is defined as:
//*** (nBinsX, Xlow, Xhigh, nBinsY, yLow, yHigh)
//***
//*** Remark: The input fname might be a filelist given with a glob pattern
//***
//**********************************************************************************************************
Int_t GenerateResponseMatrix(
    std::string fname, std::string varX = "g4Ana_energyPrimary", std::string varY = "g4Ana_totalEdep",
    std::string range = "(150,0,15,150,0,15)",
    std::string cutCondition = "g4Ana_boundingSize < 10 && g4Ana_containsProcessPhot > 0") {
    std::vector<string> files = TRestTools::GetFilesMatchingPattern(fname);
    for (const auto& f : files) {
        std::cout << "Reading file : " << f << std::endl;
        TRestRun run(f);

        TRestAnalysisTree* aTree = run.GetAnalysisTree();

        TRestGeant4Metadata* g4Md = (TRestGeant4Metadata*)run.GetMetadataClass("TRestGeant4Metadata");

        std::string drawCommand = varY + ":" + varX;
        if (range != "") drawCommand += ">>response" + range;

        aTree->Draw((TString)drawCommand, (TString)cutCondition);

        TH2D* h = (TH2D*)aTree->GetHistogram();

        /// We renormalize the values so that the values will be given
        /// on the units of X and Y axis.
        Double_t lowXValue = h->GetXaxis()->GetBinLowEdge(1);
        Double_t highXValue = h->GetXaxis()->GetBinUpEdge(h->GetNbinsX());
        Double_t normX = (highXValue - lowXValue) / h->GetNbinsX();

        Double_t lowYValue = h->GetYaxis()->GetBinLowEdge(1);
        Double_t highYValue = h->GetYaxis()->GetBinUpEdge(h->GetNbinsY());
        Double_t normY = (highYValue - lowYValue) / h->GetNbinsY();

        std::vector<std::vector<Float_t> > responseData;
        for (int n = 1; n <= h->GetNbinsX(); n++) {
            std::vector<Float_t> primaryResponse;
            for (int m = 1; m <= h->GetNbinsY(); m++) {
                Double_t value = h->GetBinContent(n, m) / normX / normY / g4Md->GetNumberOfEvents();
                primaryResponse.push_back(value);
            }
            responseData.push_back(primaryResponse);
        }

        std::string output_fname =
            (string)run.GetRunTag() + ".N" + REST_StringHelper::IntegerToString(responseData[0].size()) + "f";

        std::cout << "Writting output binary file: " << output_fname << std::endl;

        TRestTools::ExportBinaryTable(output_fname, responseData);

        Double_t efficiency = h->Integral() / g4Md->GetNumberOfEvents();

        std::cout << "Overall efficiency : " << efficiency << std::endl;
        std::cout << "Number of primaries: " << g4Md->GetNumberOfEvents() << std::endl;
    }

    return 0;
}
