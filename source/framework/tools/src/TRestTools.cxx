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
#include "TRestTools.h"

#include <TClass.h>
#include <TSystem.h>
#include <TUrl.h>
#include <dirent.h>

#include <chrono>
#include <iostream>
#include <limits>
#include <memory>
#include <thread>

#include "TRestStringHelper.h"
#include "TRestStringOutput.h"

#ifdef USE_Curl
#include <curl/curl.h>
#endif

using namespace std;

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
/// REST library lies in $REST_PATH/lib and $HOME/.rest/userlib/
/// After this we can use reflection methods TClass::GetClass() and
/// TRestMetadata::GetDataMemberRef()
///
void TRestTools::LoadRESTLibrary(bool silent) {
    string ldpath = REST_PATH + "/lib/";
    ldpath += ":" + REST_USER_PATH + "/userlib/";
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
    if (!silent) cout << "\n= REST Version: " << Execute("rest-config --version") << endl;
    if (!silent) cout << "= Loading libraries ..." << endl;
    for (unsigned int n = 0; n < fileList.size(); n++) {
        if (!silent) cout << " - " << fileList[n] << endl;
        gSystem->Load(fileList[n].c_str());
    }
    if (!silent) cout << endl;
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
    return 0;
}

template int TRestTools::PrintTable<Int_t>(std::vector<std::vector<Int_t>> data, Int_t start, Int_t end);
template int TRestTools::PrintTable<Float_t>(std::vector<std::vector<Float_t>> data, Int_t start, Int_t end);
template int TRestTools::PrintTable<Double_t>(std::vector<std::vector<Double_t>> data, Int_t start,
                                              Int_t end);

///////////////////////////////////////////////
/// \brief Writes the contents of the vector table given as argument to `fname`.
/// Allowed types are Int_t, Float_t and Double_t.
///
template <typename T>
int TRestTools::ExportASCIITable(std::string fname, std::vector<std::vector<T>>& data) {
    ofstream file(fname);
    if (!file.is_open()) {
        ferr << "Unable to open file for writing : " << fname << endl;
        return 1;
    }

    for (int n = 0; n < data.size(); n++)
        for (int m = 0; m < data[n].size(); m++) {
            file << data[n][m];
            if (m + 1 < data[n].size()) file << "\t";
            if (m + 1 == data[n].size()) file << "\n";
        }
    file.close();

    return 0;
}

template int TRestTools::ExportASCIITable<Int_t>(std::string fname, std::vector<std::vector<Int_t>>& data);
template int TRestTools::ExportASCIITable<Float_t>(std::string fname,
                                                   std::vector<std::vector<Float_t>>& data);
template int TRestTools::ExportASCIITable<Double_t>(std::string fname,
                                                    std::vector<std::vector<Double_t>>& data);

///////////////////////////////////////////////
/// \brief Writes the contents of the vector table given as argument to `fname` as a binary file.
/// Allowed types are Int_t, Float_t and Double_t.
///
template <typename T>
int TRestTools::ExportBinaryTable(std::string fname, std::vector<std::vector<T>>& data) {
    ofstream file(fname, ios::out | ios::binary);
    if (!file.is_open()) {
        ferr << "Unable to open file for writing : " << fname << endl;
        return 1;
    }

    for (int n = 0; n < data.size(); n++)
        for (int m = 0; m < data[n].size(); m++) {
            file.write((char*)&data[n][m], sizeof(T));
        }
    file.close();

    return 0;
}

template int TRestTools::ExportBinaryTable<Int_t>(std::string fname, std::vector<std::vector<Int_t>>& data);
template int TRestTools::ExportBinaryTable<Float_t>(std::string fname,
                                                    std::vector<std::vector<Float_t>>& data);
template int TRestTools::ExportBinaryTable<Double_t>(std::string fname,
                                                     std::vector<std::vector<Double_t>>& data);

///////////////////////////////////////////////
/// \brief Reads a binary file containing a fixed-columns table with values
///
/// This method will open the file fName. This file should contain a
/// table with numeric values of the type specified inside the syntax < >.
///
/// For example, a float number table with 6-columns would be read as follows:
///
/// \code
/// std::vector<std::vector <Float_t> > fvec;
/// ReadBinaryTable( "myfile.bin", fvec, 6);
/// \endcode
///
/// The values on the table will be loaded in the matrix provided through the
/// argument `data`. The content of `data` will be cleared in this method.
///
template <typename T>
int TRestTools::ReadBinaryTable(string fName, std::vector<std::vector<T>>& data, Int_t columns) {
    if (!TRestTools::isValidFile((string)fName)) {
        ferr << "TRestTools::ReadBinaryTable. Error." << endl;
        ferr << "Cannot open file : " << fName << endl;
        return 0;
    }

    if (columns == -1) {
        columns = GetBinaryFileColumns(fName);
        if (columns == -1) {
            ferr << "TRestTools::ReadBinaryTable. Format extension error." << endl;
            ferr << "Please, specify the number of columns at the method 3rd argument" << endl;
            return 0;
        }
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
    fin.read(reinterpret_cast<char*>(&dataArray[0]), columns * sizeof(T));
    while (fin.good()) {
        data.push_back(dataArray);
        fin.read(reinterpret_cast<char*>(&dataArray[0]), columns * sizeof(T));
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
/// \brief It identifies if the filename extension follows the formatting ".Nxyzf", where the
/// number of columns is `xyz`, and the last character is the type of data (f/d/i), float,
/// double and integer respectively.
///
Bool_t TRestTools::IsBinaryFile(string fname) {
    if (GetBinaryFileColumns(fname) > 0) return true;
    return false;
}

///////////////////////////////////////////////
/// \brief It extracts the number of columns from the filename extension given by argument.
/// The file will containing a binary formatted table with a fixed number of rows and columns.
///
/// The filename extension will be : ".Nxyzf", where the number of columns is `xyz`, and the
/// last character is the type of data (f/d/i), float, double and integer respectively.
///
int TRestTools::GetBinaryFileColumns(string fname) {
    string extension = GetFileNameExtension(fname);
    if (extension.find("N") != 0) {
        ferr << "Wrong filename extension." << endl;
        ferr << "Cannot guess the number of columns" << endl;
        return -1;
    }

    size_t pos = extension.find("i");
    if (pos != string::npos) {
        return StringToInteger(extension.substr(1, pos - 1));
    }

    pos = extension.find("f");
    if (pos != string::npos) {
        return StringToInteger(extension.substr(1, pos - 1));
    }

    pos = extension.find("d");
    if (pos != string::npos) {
        return StringToInteger(extension.substr(1, pos - 1));
    }

    ferr << "Format " << ToUpper(extension) << " not recognized" << endl;
    return -1;
}

///////////////////////////////////////////////
/// \brief It returns the maximum value for a particular `column` from the table given by
/// argument. If no column is specified in the arguments, then it gets the maximum from the
/// full table.
///
/// This method is available for tables of type Float_t, Double_t and Int_t.
///
template <typename T>
T TRestTools::GetMaxValueFromTable(const std::vector<std::vector<T>>& data, Int_t column) {
    if (data.size() == 0) return 0;
    if (column != -1 && data[0].size() <= column) return 0;

    T maxValue = data[0][0];
    if (column == -1) {
        for (int n = 0; n < data.size(); n++)
            for (int c = 0; c < data[n].size(); c++)
                if (maxValue < data[n][c]) maxValue = data[n][c];
    } else {
        maxValue = data[0][column];
        for (int n = 0; n < data.size(); n++)
            if (maxValue < data[n][column]) maxValue = data[n][column];
    }

    return maxValue;
}

template Int_t TRestTools::GetMaxValueFromTable<Int_t>(const std::vector<std::vector<Int_t>>& data,
                                                       Int_t column);

template Float_t TRestTools::GetMaxValueFromTable<Float_t>(const std::vector<std::vector<Float_t>>& data,
                                                           Int_t column);

template Double_t TRestTools::GetMaxValueFromTable<Double_t>(const std::vector<std::vector<Double_t>>& data,
                                                             Int_t column);

///////////////////////////////////////////////
/// \brief It returns the minimum value for a particular `column` from the table given by
/// argument. If no column is specified in the arguments, then it gets the minimum from the
/// full table.
///
/// This method is available for tables of type Float_t, Double_t and Int_t.
///
template <typename T>
T TRestTools::GetMinValueFromTable(const std::vector<std::vector<T>>& data, Int_t column) {
    if (data.empty()) return 0;
    if (column != -1 && data[0].size() <= column) return 0;

    T minValue = data[0][0];
    if (column == -1) {
        for (int n = 0; n < data.size(); n++)
            for (int c = 0; c < data[n].size(); c++)
                if (minValue > data[n][c]) minValue = data[n][c];
    } else {
        minValue = data[0][column];
        for (int n = 0; n < data.size(); n++)
            if (minValue > data[n][column]) minValue = data[n][column];
    }

    return minValue;
}

template Int_t TRestTools::GetMinValueFromTable<Int_t>(const std::vector<std::vector<Int_t>>& data,
                                                       Int_t column);

template Float_t TRestTools::GetMinValueFromTable<Float_t>(const std::vector<std::vector<Float_t>>& data,
                                                           Int_t column);

template Double_t TRestTools::GetMinValueFromTable<Double_t>(const std::vector<std::vector<Double_t>>& data,
                                                             Int_t column);

///////////////////////////////////////////////
/// \brief It returns the lowest increase, different from zero, between the elements of a
/// particular `column` from the table given by argument.
///
/// This method is available for tables of type Float_t, Double_t and Int_t.
///
/// \warning This method will not check every possible column element difference. It will only
/// look for consecutive elements steps.
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
/// \brief Reads an ASCII file containing a table with values
///
/// This method will open the file fName. This file should contain a tabulated
/// ASCII table containing numeric values. The values on the table will be
/// loaded in the matrix provided through the argument `data`. The content of
/// `data` will be cleared in this method.
///
/// Only works with Double_t vector since we use StringToDouble method.
///
int TRestTools::ReadASCIITable(string fName, std::vector<std::vector<Double_t>>& data, Int_t skipLines) {
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
        if (skipLines > 0) {
            skipLines--;
            continue;
        }

        if (line.find("#") == string::npos) {
            std::istringstream in(line);
            values.push_back(
                std::vector<string>(std::istream_iterator<string>(in), std::istream_iterator<string>()));
        }
    }

    // Filling the double values table (TODO error handling in case ToDouble conversion fails)
    for (int n = 0; n < values.size(); n++) {
        std::vector<Double_t> dblTmp;
        dblTmp.clear();

        for (int m = 0; m < values[n].size(); m++) dblTmp.push_back(StringToDouble(values[n][m]));

        data.push_back(dblTmp);
    }

    return 1;
}

///////////////////////////////////////////////
/// \brief Reads an ASCII file containing a table with values
///
/// This method will open the file fName. This file should contain a tabulated
/// ASCII table containing numeric values. The values on the table will be
/// loaded in the matrix provided through the argument `data`. The content of
/// `data` will be cleared in this method.
///
/// This version works with Float_t vector since we use StringToFloat method.
///
int TRestTools::ReadASCIITable(string fName, std::vector<std::vector<Float_t>>& data, Int_t skipLines) {
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
        if (skipLines > 0) {
            skipLines--;
            continue;
        }

        if (line.find("#") == string::npos) {
            std::istringstream in(line);
            values.push_back(
                std::vector<string>(std::istream_iterator<string>(in), std::istream_iterator<string>()));
        }
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
    if (filename.find("http") == 0) {
        return true;
    }
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
/// \brief Gets the file extension as the substring found after the lastest "."
///
/// Input: "/home/jgalan/abc.txt" Output: "txt"
///
string TRestTools::GetFileNameExtension(string fullname) {
    int pos = fullname.find_last_of('.', -1);

    if (pos != -1) {
        return fullname.substr(pos + 1, fullname.size() - pos - 1);
    }
    return fullname;
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
string TRestTools::GetPureFileName(string fullPathFileName) {
    fullPathFileName = RemoveMultipleSlash(fullPathFileName);
    return SeparatePathAndName(fullPathFileName).second;
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
Int_t TRestTools::CheckTheFile(std::string configFilename) {
    ifstream ifs;
    ifs.open(configFilename.c_str());

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

    if (result.size() > 0 && result[result.size() - 1] == '\n')
        result = result.substr(0, result.size() - 1);  // remove last "\n"

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
/// \brief download the remote file automatically, returns the downloaded file name.
///
/// The file name is given in url format, and is parsed by TUrl. Various methods
/// will be used, including scp, wget. Downloads to REST_USER_PATH + "/download/" + filename
/// by default.
///
std::string TRestTools::DownloadRemoteFile(string url) {
    string purename = TRestTools::GetPureFileName(url);
    if (purename == "") {
        cout << "error! (TRestTools::DownloadRemoteFile): url is not a file!" << endl;
        cout << "please specify a concrete file name in this url" << endl;
        cout << "url: " << url << endl;
        return "";
    }

    if (url.find("local:") == 0) {
        return Replace(url, "local:", "");
    } else {
        string fullpath = REST_USER_PATH + "/download/" + purename;
        int out;
        int attempts = 10;
        do {
            out = TRestTools::DownloadRemoteFile(url, fullpath);
            if (out == 1024) {
                warning << "Retrying download in 5 seconds" << endl;
                std::this_thread::sleep_for(std::chrono::seconds(5));
            } else if (attempts < 10) {
                success << "Download suceeded after " << 10 - attempts << " attempts" << endl;
            }
            attempts--;
        } while (out == 1024 && attempts > 0);

        if (out == 0) {
            return fullpath;
        } else if (TRestTools::fileExists(fullpath)) {
            return fullpath;
        } else {
            return "";
        }
    }
    return "";
}

///////////////////////////////////////////////
/// \brief download the remote file to the given local address.
///
/// The file name is given in url format, and is parsed by TUrl. Various methods
/// will be used, including scp, wget. returns 0 if succeed.
///
int TRestTools::DownloadRemoteFile(string remoteFile, string localFile) {
    TUrl url(remoteFile.c_str());

    info << "Downloading remote file : " << remoteFile << endl;
    info << "To local file : " << localFile << endl;

    string localFiletmp = localFile + ".restdownload";
    if ((string)url.GetProtocol() == "https" || (string)url.GetProtocol() == "http") {
        string path = TRestTools::SeparatePathAndName(localFiletmp).first;
        if (!TRestTools::fileExists(path)) {
            system(("mkdir -p " + path).c_str());
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
            if (a == 1024) {
                ferr << "Network connection problem?" << endl;
                return 1024;
            }
            if (a == 2048) {
                ferr << "File does NOT exist in remotely?" << endl;
                return 2048;
            }
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
/// \brief It performs a POST web protocol request using a set of keys and values given
/// by argument, and returns the result as a string.
///
std::string TRestTools::POSTRequest(const std::string& url, const std::map<std::string, std::string>& keys) {
    std::string file_content = "";
#ifdef USE_Curl
    CURL* curl;
    CURLcode res;

    string filename = REST_USER_PATH + "/download/curl.out";

    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    FILE* f = fopen(filename.c_str(), "wt");

    std::string request = "";
    int n = 0;
    for (auto const& x : keys) {
        if (n > 0) request += "&";
        request += x.first + "=" + x.second;
        n++;
    }
    /* get a curl handle */
    curl = curl_easy_init();
    if (curl) {
        /* First set the URL that is about to receive our POST. This URL can
           just as well be a https:// URL if that is what should receive the
           data. */
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)f);
        /* Now specify the POST data */
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.c_str());

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK) ferr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    fclose(f);
    curl_global_cleanup();

    std::getline(std::ifstream(filename), file_content, '\0');
#else
    ferr << "TRestTools::POSTRequest. REST framework was compiled without CURL support" << endl;
    ferr << "Please recompile REST after installing curl development libraries." << endl;
    ferr << "Depending on your system this might be: curl-dev, curl-devel or libcurl-openssl-dev. " << endl;
    ferr << "No file will be downloaded" << endl;
#endif

    return file_content;
}

///////////////////////////////////////////////
/// Upload the local file to remote file, method url will overwrite the login information
/// inside remotefile.
///
/// Example: UploadToServer("/home/nkx/abc.txt", "https://sultan.unizar.es/gasFiles/gases.rml",
/// "ssh://nkx:M123456@:8322") Then, the local file abc.txt will be uploaded to the server,
/// renamed to gases.rml and overwrite it
int TRestTools::UploadToServer(string localFile, string remoteFile, string methodUrl) {
    if (!TRestTools::fileExists(localFile)) {
        cout << "error! local file not exist!" << endl;
        return -1;
    }
    // construct path
    // [proto://][user[:passwd]@]host[:port]/file.ext[#anchor][?options]
    TUrl url(remoteFile.c_str());
    TUrl method(methodUrl.c_str());
    if (method.GetProtocol() != (string) "") url.SetProtocol(method.GetProtocol());
    if (method.GetPort() != 0) url.SetPort(method.GetPort());
    if (method.GetUser() != (string) "") url.SetUser(method.GetUser());
    if (method.GetPasswd() != (string) "") url.SetPasswd(method.GetPasswd());

    if ((string)url.GetProtocol() == "https" || (string)url.GetProtocol() == "http") {
        // maybe we use curl to upload to http in future
    } else if ((string)url.GetProtocol() == "ssh") {
        string cmd = "scp -P " + ToString(url.GetPort() == 0 ? 22 : url.GetPort()) + " " +
                     EscapeSpecialLetters(localFile) + " " + url.GetUser() + "@" + url.GetHost() + ":" +
                     EscapeSpecialLetters(url.GetFile());
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

void TRestTools::ChangeDirectory(string toDirectory) {
    char originDirectory[256];
    sprintf(originDirectory, "%s", getenv("PWD"));
    chdir(toDirectory.c_str());

    string fullPath = "";
    if (toDirectory[0] == '/')
        fullPath = toDirectory;
    else
        fullPath = (string)originDirectory + "/" + toDirectory;

    setenv("PWD", fullPath.c_str(), 1);
    setenv("OLDPWD", originDirectory, 1);
}

void TRestTools::ReturnToPreviousDirectory() {
    char originDirectory[256];
    sprintf(originDirectory, "%s", getenv("PWD"));

    char newDirectory[256];
    sprintf(newDirectory, "%s", getenv("OLDPWD"));

    setenv("PWD", newDirectory, 1);
    setenv("OLDPWD", originDirectory, 1);

    chdir(newDirectory);
}
