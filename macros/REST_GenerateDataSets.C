#include "TRestDataSet.h"
#include "TRestTask.h"

#ifndef RestTask_GenerateDataSet
#define RestTask_GenerateDataSet

//*******************************************************************************************************
//*** Description: This macro will launch the generation of datasets defined
//*** inside a particular RML file `datasets.rml` that contains the dataset
//*** definitions. The second argument will allow to specify the datasets
//*** to be generated from the existing ones inside `dataset.rml`.
//***
//*** --------------
//*** Usage: restManager GenerateDataSet datasets.rml set1,set2,set3
//***
//*******************************************************************************************************

Int_t REST_GenerateDataSets(std::string inputRML, std::string datasets) {
    std::vector<std::string> sets = REST_StringHelper::Split(datasets, ",");

    for (const auto& set : sets) {
        std::cout << "Set : " << set << std::endl;
        TRestDataSet d(inputRML.c_str(), set.c_str());
        d.GenerateDataSet();
        d.Export("Dataset_" + set + ".root");
    }
    return 0;
}
#endif
