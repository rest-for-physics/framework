// We run a couple validations to see if everything in the dataset is ok

Int_t DataSetTests() {
    TRestDataSet d("dataset.rml");
    d.GenerateDataSet();

    std::cout << "RunTimeStart: " << d.GetFilterStartTime() << std::endl;
    std::cout << "RunTimeEnd: " << d.GetFilterEndTime() << std::endl;

    if (d.GetFilterStartTime() != "2021/04/28 00:00") {
        return 1;
    }

    if (d.GetTree()->GetEntries() != 3816) {
        return 2;
    }

    if (d.GetFileSelection().size() != 1) {
        return 3;
    }

    if (d.GetDataFrame().GetColumnNames().size() != 13) {
        return 4;
    }
    return 0;
}
