#include <dirent.h>
#include <iostream>
#include <limits>
#include <memory>

#include "TClass.h"
#include "TSystem.h"

#include "TRestStringHelper.h"
#include "TRestTools.h"

struct _REST_STARTUP_CHECK {
   public:
    _REST_STARTUP_CHECK() {
        if (getenv("REST_PATH") == nullptr) {
            cout << "REST ERROR!! Lacking system env \"REST_PATH\"! Cannot start!" << endl;
            cout << "You need to source \"thisREST.sh\" first" << endl;
            exit(1);
        }
        if (getenv("USER") == nullptr) {
            cout << "REST ERROR!! Lacking system env \"USER\"! Cannot start!" << endl;
            cout << "You need to source \"thisREST.sh\" first" << endl;
            exit(1);
        }
    }
};
const _REST_STARTUP_CHECK __check;

ClassImp(TRestTools);
//
// std::vector<string> TRestTools::GetListOfRESTLibraries() {
//    vector<string> libraryList;
//
//    vector<string> libraryPathList;
//
//#ifdef WIN32
//    libraryPathList.push_back(get_current_dir_name() + "/../");
//#else
//	char* _env = getenv("LD_LIBRARY_PATH");
//	string env = _env == nullptr ? "" : _env;
//	libraryPathList = Split(env, ":");
//#endif
//
//    for (unsigned int n = 0; n < libraryPathList.size(); n++) {
//        // cout << "Getting libraries in directory : " << libraryPathList[n] <<
//        // endl;
//        vector<string> list = GetRESTLibrariesInDirectory(libraryPathList[n]);
//        for (unsigned int i = 0; i < list.size(); i++) libraryList.push_back(list[i]);
//    }
//
//    return libraryList;
//}
//
// std::vector<string> TRestTools::GetListOfPathsInEnvVariable(string envVariable) {
//    vector<string> pathList;
//
//    string p(getenv(envVariable.Data()));
//
//    while (p.Length() > 0) {
//        string path = GetFirstPath(p);
//
//        if (path.Length() > 0) pathList.push_back(path);
//    }
//
//    return pathList;
//}

std::vector<string> TRestTools::GetOptions(string optionsStr) { return Split(optionsStr, ":"); }

// string TRestTools::GetFirstOption(string& path) { return GetFirstPath(path); }
//
// string TRestTools::GetFirstPath(string& path) {
//    string resultPath;
//
//    if (path.First(":") >= 0) {
//        resultPath = path(0, path.First(":"));
//
//        path = path(path.First(":") + 1, path.Length());
//    } else {
//        resultPath = path;
//        path = "";
//    }
//
//    return resultPath;
//}

// std::vector<string> TRestTools::GetRESTLibrariesInDirectory(string path) {
//    vector<string> fileList;
//    DIR* dir;
//    struct dirent* ent;
//    if ((dir = opendir(path.c_str())) != nullptr) {
//        /* print all the files and directories within directory */
//        while ((ent = readdir(dir)) != nullptr) {
//            string fName(ent->d_name);
//            if ((fName.find("REST") != -1 || fName.find("Rest") != -1))
//                if (fName.find(".dylib") != -1 || fName.find(".so") != -1) fileList.push_back(fName);
//        }
//        closedir(dir);
//    } else {
//        /* could not open directory */
//        perror("");
//    }
//
//    return fileList;
//}

void TRestTools::LoadRESTLibrary(bool silent) {
    char* _ldpath = getenv("LD_LIBRARY_PATH");
    if (_ldpath == nullptr) {
        _ldpath = Form("%s/lib/", getenv("REST_PATH"));
    }
    vector<string> ldpaths = Split(_ldpath, ":");

    vector<string> fileList;
    for (string path : ldpaths) {
        DIR* dir;
        struct dirent* ent;
        if ((dir = opendir(path.c_str())) != nullptr) {
            /* print all the files and directories within directory */
            while ((ent = readdir(dir)) != nullptr) {
                string fName(ent->d_name);
                if ((fName.find("REST") != -1 || fName.find("Rest") != -1))
                    if (fName.find(".dylib") != -1 || fName.find(".so") != -1) fileList.push_back(fName);
            }
            closedir(dir);
        }
    }

    // load the finded REST libraries
    for (unsigned int n = 0; n < fileList.size(); n++) {
        if (!silent) cout << "Loading library : " << fileList[n] << endl;
        gSystem->Load(fileList[n].c_str());
    }
}

int TRestTools::ReadASCIITable(string fName, std::vector<std::vector<Double_t>>& data) {
    if (!TRestTools::fileExists((string)fName)) {
        cout << "TRestTools::ReadASCIITable. Error" << endl;
        cout << "Cannot open file : " << fName << endl;
        return 0;
    }

    data.clear();

    std::ifstream fin(fName);

    // First we create a table with string values
    std::vector<std::vector<string>> values;

    for (string line; std::getline(fin, line);) {
        std::istringstream in(line);
        values.push_back(
            std::vector<string>(std::istream_iterator<string>(in), std::istream_iterator<string>()));
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

///////////////////////////////////////////////
/// \brief Returns true if the file with path filename exists.
///
bool TRestTools::fileExists(const string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

///////////////////////////////////////////////
/// \brief Returns true if the **filename** has *.root* extension.
///
bool TRestTools::isRootFile(const string& filename) {
    if (filename.find(".root") == string::npos) return false;

    return true;
}

///////////////////////////////////////////////
/// \brief Returns true if **filename** is an *http* address.
///
bool TRestTools::isURL(const string& filename) {
    if (filename.find("http") == 0) return true;

    return false;
}

///////////////////////////////////////////////
/// \brief Returns true if the **path** given by argument is writable
///
bool TRestTools::isPathWritable(const string& path) {
    int result = 0;
#ifdef WIN32
    result = _access(path.c_str(), 2);
#else
    result = access(path.c_str(), 2);
#endif

    if (result == 0)
        return true;
    else
        return false;
}

///////////////////////////////////////////////
/// \brief Check if the path is absolute path or not
///
bool TRestTools::isAbsolutePath(const string& path) {
    if (path[0] == '/' || path[0] == '~' || path.find(':') != -1) {
        return true;
    }
    return false;
}

///////////////////////////////////////////////
/// \brief Separate path and filename in a full path+filename string, returns a
/// pair of string
///
/// if input file name contains no directory, the returned directory is set to
/// "." if input file name contains no file, the returned filename is set to ""
/// e.g.
/// Input: "/home/nkx/abc.txt" and ":def", Output: { "/home/nkx/", "abc.txt" }
/// Input: "abc.txt" and ":", Output: { ".", "abc.txt" }
/// Input: "/home/nkx/" and ":", Output: { "/home/nkx/", "" }
std::pair<string, string> TRestTools::SeparatePathAndName(string fullname) {
    fullname = RemoveMultipleSlash(fullname);
    pair<string, string> result;
    int pos = fullname.find_last_of('/', -1);

    if (pos == -1) {
        result.first = ".";
        result.second = fullname;
    } else if (pos == 0) {
        result.first = "/";
        result.second = fullname.substr(1, fullname.size() - 1);
    } else if (pos == fullname.size() - 1) {
        result.first = fullname;
        result.second = "";
    } else {
        result.first = fullname.substr(0, pos + 1);
        result.second = fullname.substr(pos + 1, fullname.size() - pos - 1);
    }
    return result;
}

///////////////////////////////////////////////
/// \brief Returns the input string but without multiple slashes ("/")
///
/// \param str: input path string (e.g. "///home///test/")
/// \return path string without multiple slashes (e.g. "/home/test/")
string TRestTools::RemoveMultipleSlash(string str) {
    // we replace multiple appearances of "/" (slash) by a single "/"
    string to_replace = "//";
    size_t start_pos = str.find(to_replace);
    while (start_pos != string::npos) {
        str.replace(start_pos, to_replace.length(), "/");
        start_pos = str.find(to_replace);
    }
    return str;
}

///////////////////////////////////////////////
/// \brief Removes all directories in the full path filename description
/// given in the argument.
///
/// e.g.
/// Input: "/home/nkx/abc.txt", Returns: "abc.txt"
/// Input: "/home/nkx/", Output: ""
string TRestTools::RemoveAbsolutePath(string fullpathFileName) {
    fullpathFileName = RemoveMultipleSlash(fullpathFileName);
    return SeparatePathAndName(fullpathFileName).second;
}

string TRestTools::ToAbsoluteName(string filename) {
    string result = filename;
    if (filename[0] == '~') {
        result = (string)getenv("HOME") + filename.substr(1, -1);
    } else if (filename[0] != '/') {
        result = (string)getenv("PWD") + "/" + filename;
    }
    result = RemoveMultipleSlash(result);
    return result;
}

///////////////////////////////////////////////
/// \brief It lists all the subdirectories inside path and adds
/// them to the result vector.
/// if recursion is 0, then list only the subdirectory of this directory
/// if recursion is < 0, then list subdirectories recursively
/// Otherwise recurse only certain times.
vector<string> TRestTools::GetSubdirectories(const string& path, int recursion) {
    vector<string> result;
    if (auto dir = opendir(path.c_str())) {
        while (1) {
            auto f = readdir(dir);
            if (f == nullptr) {
                break;
            }
            if (f->d_name[0] == '.') continue;

            string ipath;
            if (path[path.size() - 1] != '/') {
                ipath = path + "/" + f->d_name + "/";
            } else {
                ipath = path + f->d_name + "/";
            }

            // if (f->d_type == DT_DIR)
            if (opendir(ipath.c_str()))  // to make sure it is a directory
            {
                result.push_back(ipath);

                if (recursion != 0) {
                    vector<string> subD = GetSubdirectories(ipath, recursion - 1);
                    result.insert(result.begin(), subD.begin(), subD.end());
                    //, cb);
                }
            }
        }
        closedir(dir);
    }
    return result;
}

///////////////////////////////////////////////
/// \brief Search file in the given vector of path strings, return a full name
/// if found, return "" if not
///
string TRestTools::SearchFileInPath(vector<string> paths, string filename) {
    if (fileExists(filename)) {
        return filename;
    } else {
        for (int i = 0; i < paths.size(); i++) {
            string path = paths[i];
            if (path[path.size() - 1] != '/') {
                path = path + "/";
            }

            if (fileExists(path + filename)) {
                return path + filename;
            }

            // search also in subdirectory, but only 5 times of recursion
            vector<string> pathsExpanded = GetSubdirectories(paths[i], 5);
            for (int j = 0; j < pathsExpanded.size(); j++)
                if (fileExists(pathsExpanded[j] + filename)) return pathsExpanded[j] + filename;
        }
    }
    return "";
}

///////////////////////////////////////////////
/// \brief Checks if the config file can be openned. It returns OK in case of
/// success, ERROR otherwise.
///
Int_t TRestTools::ChecktheFile(string FileName) {
    ifstream ifs;
    ifs.open(FileName.c_str());

    if (!ifs) {
        return -1;
    } else
        ifs.close();

    return 0;
}

///////////////////////////////////////////////
/// \brief Returns a list of files whose name match the pattern string. Key word
/// is "*". e.g. abc00*.root
///
vector<string> TRestTools::GetFilesMatchingPattern(string pattern) {
    std::vector<string> outputFileNames;

    if (pattern != "") {
        if (pattern.find_first_of("*") >= 0 || pattern.find_first_of("?") >= 0) {
            string a = Execute("find " + pattern);
            auto b = Split(a, "\n");

            for (int i = 0; i < b.size(); i++) {
                outputFileNames.push_back(b[i]);
            }

            // char command[256];
            // sprintf(command, "find %s > /tmp/RESTTools_fileList.tmp",
            // pattern.Data());

            // system(command);

            // FILE *fin = fopen("/tmp/RESTTools_fileList.tmp", "r");
            // char str[256];
            // while (fscanf(fin, "%s\n", str) != EOF)
            //{
            //	TString newFile = str;
            //	outputFileNames.push_back(newFile);
            //}
            // fclose(fin);

            // system("rm /tmp/RESTTools_fileList.tmp");
        } else {
            if (fileExists(pattern)) outputFileNames.push_back(pattern);
        }
    }
    return outputFileNames;
}

///////////////////////////////////////////////
/// \brief Convert version to a unique string
///
int TRestTools::ConvertVersionCode(string in) {
#ifndef REST_Version
#define REST_VERSION(a, b, c) (((a) << 16) + ((b) << 8) + (c))
#endif
    vector<string> ver = Split(in, ".");
    if (ver.size() == 3) {
        vector<int> verint;
        for (auto v : ver) {
            int n = StringToInteger(v.substr(0, v.find_first_not_of("0123456789")));
            if (n != -1) {
                verint.push_back(n);
            } else {
                return -1;
            }
        }
        return REST_VERSION(verint[0], verint[1], verint[2]);
    }
    return -1;
}

string TRestTools::Execute(string cmd) {
    std::array<char, 128> buffer;
    string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    if (result[result.size() - 1] == '\n') result = result.substr(0, result.size() - 1);  // remove last "\n"

    return result;
}
