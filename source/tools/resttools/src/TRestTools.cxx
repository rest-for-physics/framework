
#include "TRestTools.h"
#include <TSystem.h>
#include <iostream>
#include <limits>
#include "TRestStringHelper.h"
using namespace std;

#include <dirent.h>
#include "TClass.h"

struct _REST_STARTUP_CHECK {
   public:
    _REST_STARTUP_CHECK() {
        if (getenv("REST_PATH") == NULL) {
            cout << "REST ERROR!! Lacking system env \"REST_PATH\"! Cannot start!" << endl;
            cout << "You need to source \"thisREST.sh\" first" << endl;
            exit(1);
        }
        if (getenv("USER") == NULL) {
            cout << "REST ERROR!! Lacking system env \"USER\"! Cannot start!" << endl;
            cout << "You need to source \"thisREST.sh\" first" << endl;
            exit(1);
        }
    }
};
const _REST_STARTUP_CHECK __check;

ClassImp(TRestTools)

    std::vector<TString> TRestTools::GetListOfRESTLibraries() {
    vector<TString> libraryList;

    vector<TString> libraryPathList;

#ifdef WIN32
    libraryPathList.push_back(get_current_dir_name() + "/../");
#else
    libraryPathList = GetListOfPathsInEnvVariable("LD_LIBRARY_PATH");
#endif

    for (unsigned int n = 0; n < libraryPathList.size(); n++) {
        // cout << "Getting libraries in directory : " << libraryPathList[n] <<
        // endl;
        vector<TString> list = GetRESTLibrariesInDirectory(libraryPathList[n]);
        for (unsigned int i = 0; i < list.size(); i++) libraryList.push_back(list[i]);
    }

    return libraryList;
}

std::vector<TString> TRestTools::GetListOfPathsInEnvVariable(TString envVariable) {
    vector<TString> pathList;

    TString p(getenv(envVariable.Data()));

    while (p.Length() > 0) {
        TString path = GetFirstPath(p);

        if (path.Length() > 0) pathList.push_back(path);
    }

    return pathList;
}

std::vector<TString> TRestTools::GetOptions(TString optionsStr) {
    vector<TString> optionsList;

    TString p(optionsStr);

    while (p.Length() > 0) {
        TString option = GetFirstOption(p);

        if (option.Length() > 0) optionsList.push_back(option);
    }

    return optionsList;
}

TString TRestTools::GetFirstOption(TString& path) { return GetFirstPath(path); }

TString TRestTools::GetFirstPath(TString& path) {
    TString resultPath;

    if (path.First(":") >= 0) {
        resultPath = path(0, path.First(":"));

        path = path(path.First(":") + 1, path.Length());
    } else {
        resultPath = path;
        path = "";
    }

    return resultPath;
}

std::vector<TString> TRestTools::GetRESTLibrariesInDirectory(TString path) {
    vector<TString> fileList;
    DIR* dir;
    struct dirent* ent;
    if ((dir = opendir(path.Data())) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
            TString fName(ent->d_name);
            if ((fName.Contains("REST") || fName.Contains("Rest")))
                if (fName.Contains(".dylib") || fName.Contains(".so")) fileList.push_back(fName);
        }
        closedir(dir);
    } else {
        /* could not open directory */
        perror("");
    }

    return fileList;
}

void TRestTools::LoadRESTLibrary(bool silent) {
    vector<TString> list = TRestTools::GetListOfRESTLibraries();
    for (unsigned int n = 0; n < list.size(); n++) {
        if (!silent) cout << "Loading library : " << list[n] << endl;

        gSystem->Load(list[n]);
    }
}

int TRestTools::ReadASCIITable(TString fName, std::vector<std::vector<Double_t>>& data) {
    if (!REST_StringHelper::fileExists((string)fName)) {
        cout << "TRestTools::ReadASCIITable. Error" << endl;
        cout << "Cannot open file : " << fName << endl;
        return 0;
    }

    data.clear();

    std::ifstream fin(fName.Data());

    // First we create a table with string values
    std::vector<std::vector<std::string>> values;

    for (std::string line; std::getline(fin, line);) {
        std::istringstream in(line);
        values.push_back(std::vector<std::string>(std::istream_iterator<std::string>(in),
                                                  std::istream_iterator<std::string>()));
    }

    // Filling the double values table (TODO error handling in case ToDouble
    // conversion fails)
    for (int n = 0; n < values.size(); n++) {
        std::vector<Double_t> dblTmp;
        dblTmp.clear();

        for (int m = 0; m < values[n].size(); m++) dblTmp.push_back(StringToDouble(values[n][m]));

        data.push_back(dblTmp);
    }

    return 1;
}

std::string TRestTools::Execute(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

char* TRestTools::Assembly(TString typeName) {
    if (typeName == "double") {
        return (char*)new double(0);
    } else if (typeName == "float") {
        return (char*)new float(0);
    } else if (typeName == "long double") {
        return (char*)new long double(0);
    } else if (typeName == "bool") {
        return (char*)new bool(0);
    } else if (typeName == "char") {
        return (char*)new char(0);
    } else if (typeName == "int") {
        return (char*)new int(0);
    } else if (typeName == "short") {
        return (char*)new short(0);
    } else if (typeName == "long") {
        return (char*)new long(0);
    } else if (typeName == "long long") {
        return (char*)new long long(0);
    } else {
        TClass* c = new TClass(typeName);
        if (c != NULL) return (char*)c->New();
    }
    return NULL;
}
