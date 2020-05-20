/******************** REST disclaimer ***********************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
/// TRestTools is a class that defines static methods that might be handy
/// when accessing files or system utilities, or other basic methods that
/// do not fit in a specialized class, and are generic enought to be
/// considered a REST tool.
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2016-December: First concept.
///				   Javier Galan
///
/// \class      TRestTools
/// \author     Javier Galan <javier.galan@unizar.es>
/// \author     Kaixiang Ni
///
/// <hr>
///
#include <dirent.h>
#include <iostream>
#include <limits>
#include <memory>

#include "TClass.h"
#include "TSystem.h"
#include "TUrl.h"

#include "TRestStringHelper.h"
#include "TRestStringOutput.h"
#include "TRestTools.h"

string REST_PATH;
string REST_USER;
string REST_USER_PATH;
struct _REST_STARTUP_CHECK {
   public:
    _REST_STARTUP_CHECK() {
        char* _REST_PATH = getenv("REST_PATH");
        char* _REST_USER = getenv("USER");
        char* _REST_USERHOME = getenv("HOME");

        if (_REST_PATH == nullptr) {
            cout << "REST ERROR!! Lacking system env \"REST_PATH\"! Cannot start!" << endl;
            cout << "You need to source \"thisREST.sh\" first" << endl;
            abort();
        }
        REST_PATH = _REST_PATH;

        if (_REST_USER == nullptr) {
            cout << "REST WARNING!! Lacking system env \"USER\"!" << endl;
            cout << "Setting default user" << endl;
            REST_USER = "defaultUser";
            setenv("USER", REST_USER.c_str(), true);

        } else {
            REST_USER = _REST_USER;
        }

        if (_REST_USERHOME == nullptr) {
            cout << "REST WARNING!! Lacking system env \"HOME\"!" << endl;
            cout << "Setting REST temp path to $REST_PATH/data" << endl;
            REST_USER_PATH = REST_PATH + "/data";
        } else {
            string restUserPath = (string)_REST_USERHOME + "/.rest";
            // check the directory exists
            if (!TRestTools::fileExists(restUserPath)) {
                mkdir(restUserPath.c_str(), S_IRWXU);
            }
            // check the runNumber file
            if (!TRestTools::fileExists(restUserPath+"/runNumber")) {
                TRestTools::Execute("echo 1 > " + restUserPath + "/runNumber");
            }
            // check the dataURL file
            if (!TRestTools::fileExists(restUserPath+"/dataURL")) {
                TRestTools::Execute("cp " + REST_PATH + "/data/dataURL " + restUserPath);
            }

            // now we don't need to check write accessibility in other methods in REST
            REST_USER_PATH = restUserPath;
        }


    }
};
const _REST_STARTUP_CHECK __check;

ClassImp(TRestTools);

///////////////////////////////////////////////
/// \brief Returns all the options in an option string
///
/// This method gives string to the method GetFirstOption().
/// And then adds the result to the list.
///
std::vector<string> TRestTools::GetOptions(string optionsStr) { return Split(optionsStr, ":"); }

///////////////////////////////////////////////
/// \brief Calls gSystem to load REST library.
///
/// After this we can use reflection methods TClass::GetClass() and
/// TRestMetadata::GetDataMemberRef()
///
void TRestTools::LoadRESTLibrary(bool silent) {
    char* _ldpath = getenv("LD_LIBRARY_PATH");
    if (_ldpath == nullptr) {
        _ldpath = Form("%s/lib/", REST_PATH.c_str());
    }
    string ldpath = _ldpath;
    ldpath += ":" + REST_USER_PATH + "/AddonDict/";
    vector<string> ldpaths = Split(ldpath, ":");

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

    // load the found REST libraries
    for (unsigned int n = 0; n < fileList.size(); n++) {
        if (!silent) cout << "Loading library : " << fileList[n] << endl;
        gSystem->Load(fileList[n].c_str());
    }
}

///////////////////////////////////////////////
/// \brief Prints the contents of the vector table given as argument in screen.
/// Allowed types are Int_t, Float_t and Double_t.
///
/// The printed out data can be restricted to the row lines between `start`
/// and `end` parameters given by argument.
///
template <typename T>
int TRestTools::PrintTable(std::vector<std::vector<T>> data, Int_t start, Int_t end) {
    Int_t size = data.size();
    if (end > 0 && size > end) size = end;
    for (int n = start; n < size; n++) {
        for (int m = 0; m < data[n].size(); m++) cout << data[n][m] << "\t";
        cout << endl;
    }
}

template int TRestTools::PrintTable<Int_t>(std::vector<std::vector<Int_t>> data, Int_t start, Int_t end);
template int TRestTools::PrintTable<Float_t>(std::vector<std::vector<Float_t>> data, Int_t start, Int_t end);
template int TRestTools::PrintTable<Double_t>(std::vector<std::vector<Double_t>> data, Int_t start,
                                              Int_t end);

///////////////////////////////////////////////
/// \brief Reads a binary file containning a fixed-columns table with values
///
/// This method will open the file fName. This file should contain a
/// table with numeric values of the type specified inside the syntax < >.
///
/// For example, a float number table with 6-columns would be read as follows:
///
/// \code
/// std::vector<std::vector <Float_t> > fvec;
/// ReadBinaryFile( "myfile.bin", fvec, 6);
/// \endcode
///
/// The values on the table will be loaded in the matrix provided through the
/// argument `data`. The content of `data` will be cleared in this method.
///
template <typename T>
int TRestTools::ReadBinaryTable(string fName, std::vector<std::vector<T>>& data, Int_t columns) {
    if (!TRestTools::isValidFile((string)fName)) {
        cout << "TRestTools::ReadBinaryTable. Error." << endl;
        cout << "Cannot open file : " << fName << endl;
        return 0;
    }

    std::ifstream fin(fName, std::ios::binary);
    fin.seekg(0, std::ios::end);
    const size_t num_elements = fin.tellg() / sizeof(T);
    fin.seekg(0, std::ios::beg);

    if (num_elements % columns != 0) {
        cout << "TRestTools::ReadBinaryTable. Error." << endl;
        cout << "Number of elements : " << num_elements
             << " is not compatible with the number of columns : " << columns << endl;
        fin.close();
        return 0;
    }

    std::vector<T> dataArray(columns);
    while (fin.good()) {
        fin.read(reinterpret_cast<char*>(&dataArray[0]), columns * sizeof(T));
        data.push_back(dataArray);
    }
    return 1;
}

template int TRestTools::ReadBinaryTable<Int_t>(string fName, std::vector<std::vector<Int_t>>& data,
                                                Int_t columns);
template int TRestTools::ReadBinaryTable<Float_t>(string fName, std::vector<std::vector<Float_t>>& data,
                                                  Int_t columns);
template int TRestTools::ReadBinaryTable<Double_t>(string fName, std::vector<std::vector<Double_t>>& data,
                                                   Int_t columns);

///////////////////////////////////////////////
/// \brief It returns the maximum value for a particular `column` from the table given by argument.
///
/// This method is available for tables of type Float_t, Double_t and Int_t.
///
template <typename T>
T TRestTools::GetMaxValueFromTable(std::vector<std::vector<T>> data, Int_t column) {
    if (data.size() == 0 || data[0].size() <= column) return 0;
    T maxValue = data[0][column];
    for (int n = 0; n < data.size(); n++)
        if (maxValue < data[n][column]) maxValue = data[n][column];
    return maxValue;
}

template Int_t TRestTools::GetMaxValueFromTable<Int_t>(std::vector<std::vector<Int_t>> data, Int_t column);

template Float_t TRestTools::GetMaxValueFromTable<Float_t>(std::vector<std::vector<Float_t>> data,
                                                           Int_t column);

template Double_t TRestTools::GetMaxValueFromTable<Double_t>(std::vector<std::vector<Double_t>> data,
                                                             Int_t column);

///////////////////////////////////////////////
/// \brief It returns the minimum value for a particular `column` from the table given by argument.
///
/// This method is available for tables of type Float_t, Double_t and Int_t.
///
template <typename T>
T TRestTools::GetMinValueFromTable(std::vector<std::vector<T>> data, Int_t column) {
    if (data.size() == 0 || data[0].size() <= column) return 0;
    T minValue = data[0][column];
    for (int n = 0; n < data.size(); n++)
        if (minValue > data[n][column]) minValue = data[n][column];
    return minValue;
}

template Int_t TRestTools::GetMinValueFromTable<Int_t>(std::vector<std::vector<Int_t>> data, Int_t column);

template Float_t TRestTools::GetMinValueFromTable<Float_t>(std::vector<std::vector<Float_t>> data,
                                                           Int_t column);

template Double_t TRestTools::GetMinValueFromTable<Double_t>(std::vector<std::vector<Double_t>> data,
                                                             Int_t column);

///////////////////////////////////////////////
/// \brief It returns the lowest increase, different from zero, between the elements of a particular `column`
/// from the table given by argument.
///
/// This method is available for tables of type Float_t, Double_t and Int_t.
///
/// \warning This method will not check every possible column element difference. It will only look for
/// consecutive elements steps.
///
template <typename T>
T TRestTools::GetLowestIncreaseFromTable(std::vector<std::vector<T>> data, Int_t column) {
    if (data.size() == 0 || data[0].size() <= column) return 0;
    T lowestIncrease = abs(data[0][column] - data[1][column]);
    for (int n = 1; n < data.size(); n++) {
        T value = abs(data[n - 1][column] - data[n][column]);
        if (lowestIncrease == 0) lowestIncrease = value;
        if (value > 0 && value < lowestIncrease) lowestIncrease = value;
    }
    return lowestIncrease;
}

template Int_t TRestTools::GetLowestIncreaseFromTable<Int_t>(std::vector<std::vector<Int_t>> data,
                                                             Int_t column);

template Float_t TRestTools::GetLowestIncreaseFromTable<Float_t>(std::vector<std::vector<Float_t>> data,
                                                                 Int_t column);

template Double_t TRestTools::GetLowestIncreaseFromTable<Double_t>(std::vector<std::vector<Double_t>> data,
                                                                   Int_t column);

///////////////////////////////////////////////
/// \brief Reads an ASCII file containning a table with values
///
/// This method will open the file fName. This file should contain a tabulated
/// ASCII table containning numeric values. The values on the table will be
/// loaded in the matrix provided through the argument `data`. The content of
/// `data` will be cleared in this method.
///
/// Only works with Double_t vector since we use StringToDouble method.
///
int TRestTools::ReadASCIITable(string fName, std::vector<std::vector<Double_t>>& data) {
    if (!TRestTools::isValidFile((string)fName)) {
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
/// \brief Reads an ASCII file containning a table with values
///
/// This method will open the file fName. This file should contain a tabulated
/// ASCII table containning numeric values. The values on the table will be
/// loaded in the matrix provided through the argument `data`. The content of
/// `data` will be cleared in this method.
///
/// This version works with Float_t vector since we use StringToFloat method.
///
int TRestTools::ReadASCIITable(string fName, std::vector<std::vector<Float_t>>& data) {
    if (!TRestTools::isValidFile((string)fName)) {
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

    // Filling the float values table (TODO error handling in case ToFloat
    // conversion fails)
    for (int n = 0; n < values.size(); n++) {
        std::vector<Float_t> dblTmp;
        dblTmp.clear();

        for (int m = 0; m < values[n].size(); m++) dblTmp.push_back(StringToFloat(values[n][m]));

        data.push_back(dblTmp);
    }

    return 1;
}

///////////////////////////////////////////////
/// \brief Returns true if the file with path filename exists.
///
Int_t TRestTools::isValidFile(const string& path) {
    struct stat buffer;
    stat(path.c_str(), &buffer);
    return S_ISREG(buffer.st_mode);
}

///////////////////////////////////////////////
/// \brief Returns true if the file (or directory) with path filename exists.
///
/// This method will return true even if it is a pure path.
/// We should call `isValidFile` to check if we will be able to open it without
/// problems.
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
///
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
///
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
///
string TRestTools::GetPureFileName(string fullpathFileName) {
    fullpathFileName = RemoveMultipleSlash(fullpathFileName);
    return SeparatePathAndName(fullpathFileName).second;
}

///////////////////////////////////////////////
/// \brief It takes a filename and adds it a full path based on
/// the directory the system is at the moment of the method call,
/// through the PWD system variable.
///
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
///
/// If recursion is 0, then list only the subdirectory of this directory
/// If recursion is < 0, then list subdirectories recursively
///
/// Otherwise recurse only certain times.
///
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
        vector<string> items = Split(pattern, "\n");

        for (auto item : items) {
            if (item.find_first_of("*") >= 0 || item.find_first_of("?") >= 0) {
                string a = Execute("find " + item);
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
                if (fileExists(item)) outputFileNames.push_back(item);
            }
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

///////////////////////////////////////////////
/// \brief Executes a shell command and returns its output in a string
///
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

///////////////////////////////////////////////
/// \brief It reads the next line from the incoming istream and puts it
/// in the string argument `t`. The remaining istream is returned.
///
std::istream& TRestTools::GetLine(std::istream& is, std::string& t) {
    t.clear();

    // The characters in the stream are read one-by-one using a std::streambuf.
    // That is faster than reading them one-by-one using the std::istream.
    // Code that uses streambuf this way must be guarded by a sentry object.
    // The sentry object performs various tasks,
    // such as thread synchronization and updating the stream state.

    std::istream::sentry se(is, true);
    std::streambuf* sb = is.rdbuf();

    for (;;) {
        int c = sb->sbumpc();
        switch (c) {
            case '\n':
                return is;
            case '\r':
                if (sb->sgetc() == '\n') sb->sbumpc();
                return is;
            case std::streambuf::traits_type::eof():
                // Also handle the case when the last line has no line ending
                if (t.empty()) is.setstate(std::ios::eofbit);
                return is;
            default:
                t += (char)c;
        }
    }
}

///////////////////////////////////////////////
/// \brief download the remote file to the given local address.
///
/// The file name is given in url format, and is parsed by TUrl. Various methods
/// will be used, including scp, wget. returns 0 if succeed.
int TRestTools::DownloadRemoteFile(string remoteFile, string localFile) {
    TUrl url(remoteFile.c_str());

    info << "Downloading remote file : " << remoteFile << endl;
    info << "To local file : " << localFile << endl;

    string localFiletmp = localFile + ".tmp";
    if ((string)url.GetProtocol() == "https" || (string)url.GetProtocol() == "http") {
        string path = TRestTools::SeparatePathAndName(localFiletmp).first;
        if (!TRestTools::fileExists(path)) {
            system(("mkdir " + path).c_str());
        }

        string cmd = "wget --no-check-certificate " + EscapeSpecialLetters(remoteFile) + " -O " +
                     EscapeSpecialLetters(localFiletmp) + " -q";
        debug << cmd << endl;
        int a = system(cmd.c_str());

        if (a == 0) {
            rename(localFiletmp.c_str(), localFile.c_str());
            return 0;
        } else {
            ferr << "download failed! (" << remoteFile << ")" << endl;
            if (a == 1024) ferr << "Network connection problem?" << endl;
            if (a == 2048) ferr << "File does NOT exist in remotely?" << endl;
        }
    } else if ((string)url.GetProtocol() == "ssh") {
        string cmd = "scp -P " + ToString(url.GetPort() == 0 ? 22 : url.GetPort()) + " " + url.GetUser() +
                     "@" + url.GetHost() + ":" + EscapeSpecialLetters(url.GetFile()) + " " + localFiletmp;
        cout << cmd << endl;
        int a = system(cmd.c_str());
        if (a == 0) {
            rename(localFiletmp.c_str(), localFile.c_str());
            return 0;
        }
    } else {
        ferr << "unknown protocol!" << endl;
    }

    return -1;
}

///////////////////////////////////////////////
/// Upload the local file to remote file, method url will overwrite the login information
/// inside remotefile.
///
/// Example: UploadToServer("/home/nkx/abc.txt", "https://sultan.unizar.es/gasFiles/gases.rml",
/// "ssh://nkx:M123456@:8322") Then, the local file abc.txt will be uploaded to the server, renamed to
/// gases.rml and overwrite it
int TRestTools::UploadToServer(string filelocal, string remotefile, string methodurl) {
    if (!TRestTools::fileExists(filelocal)) {
        cout << "error! local file not exist!" << endl;
        return -1;
    }
    // construct path
    // [proto://][user[:passwd]@]host[:port]/file.ext[#anchor][?options]
    TUrl url(remotefile.c_str());
    TUrl method(methodurl.c_str());
    if (method.GetProtocol() != "") url.SetProtocol(method.GetProtocol());
    if (method.GetPort() != 0) url.SetPort(method.GetPort());
    if (method.GetUser() != "") url.SetUser(method.GetUser());
    if (method.GetPasswd() != "") url.SetPasswd(method.GetPasswd());

    if ((string)url.GetProtocol() == "https" || (string)url.GetProtocol() == "http") {
        // maybe we use curl to upload to http in future
    } else if ((string)url.GetProtocol() == "ssh") {
        string cmd = "scp -P " + ToString(url.GetPort() == 0 ? 22 : url.GetPort()) + " " + filelocal + " " +
                     url.GetUser() + "@" + url.GetHost() + ":" + EscapeSpecialLetters(url.GetFile());
        cout << cmd << endl;
        int a = system(cmd.c_str());

        if (a != 0) {
            ferr << __PRETTY_FUNCTION__ << endl;
            ferr << "problem copying gases definitions to remote server" << endl;
            ferr << "Please report this problem at "
                    "http://gifna.unizar.es/rest-forum/"
                 << endl;
            return -1;
        }

        return 0;
    }
    return 0;
}
