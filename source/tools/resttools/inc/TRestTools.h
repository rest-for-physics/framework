/*************************************************************************
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

#ifndef RestCore_TRestTools
#define RestCore_TRestTools

//#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "TObject.h"

using namespace std;

#define UNUSED(x) (void)x

const string PARAMETER_NOT_FOUND_STR = "NO_SUCH_PARA";
const double PARAMETER_NOT_FOUND_DBL = -99999999;

extern string REST_COMMIT;
extern string REST_PATH;
extern string REST_USER;
extern string REST_USER_PATH;
extern map<string, string> REST_ARGS;
/// A generic class with useful static methods.
class TRestTools {
   public:
    static std::vector<string> GetOptions(string optionsStr);

    static void LoadRESTLibrary(bool silent = false);

    static int ReadASCIITable(string fName, std::vector<std::vector<Double_t>>& data);
    static int ReadASCIITable(string fName, std::vector<std::vector<Float_t>>& data);

    template <typename T>
    static int ReadBinaryTable(string fName, std::vector<std::vector<T>>& data, Int_t columns);

    template <typename T>
    static T GetMaxValueFromTable(std::vector<std::vector<T>> data, Int_t column);

    template <typename T>
    static T GetMinValueFromTable(std::vector<std::vector<T>> data, Int_t column);

    template <typename T>
    static T GetLowestIncreaseFromTable(std::vector<std::vector<T>> data, Int_t column);

    template <typename T>
    static int PrintTable(std::vector<std::vector<T>> data, Int_t start = 0, Int_t end = 0);

    static Int_t isValidFile(const string& path);
    static bool fileExists(const std::string& filename);
    static bool isRootFile(const std::string& filename);
    static bool isURL(const std::string& filename);
    static bool isPathWritable(const std::string& path);
    static bool isAbsolutePath(const std::string& path);
    static string RemoveMultipleSlash(string);
    static string ToAbsoluteName(string filename);
    static vector<string> GetSubdirectories(const string& path, int recursion = -1);
    static std::pair<string, string> SeparatePathAndName(const std::string fullname);
    static std::string GetPureFileName(std::string fullpathFileName);
    static std::string SearchFileInPath(vector<string> path, string filename);
    static Int_t ChecktheFile(std::string cfgFileName);
    static std::vector<string> GetFilesMatchingPattern(string pattern);
    static int ConvertVersionCode(string in);
    static std::istream& GetLine(std::istream& is, std::string& t);

    static std::string Execute(string cmd);

    static std::string DownloadRemoteFile(string remoteFile);
    static int DownloadRemoteFile(string remoteFile, string localFile);
    static int UploadToServer(string localfile, string remotefile, string methodurl = "");

    /// Rest tools class
    ClassDef(TRestTools, 1);
};

namespace REST_InitTools {

template <class T>
struct GlobalVarInit {
    static int level;
};
template <class T>
int GlobalVarInit<T>::level = 0;

template <class T>
inline bool CanOverwrite(T* name, int level) {
    if (level > GlobalVarInit<T>::level) {
        return true;
    }
    return false;
}

template <class T>
inline void SetInitLevel(T* name, int level) {
    GlobalVarInit<T>::level = level;
}

#define MakeGlobal(classname, objname, level)                       \
    struct __##classname##_Init {                                   \
        __##classname##_Init() {                                    \
            if (objname != NULL) {                                  \
                if (REST_InitTools::CanOverwrite(objname, level)) { \
                    delete objname;                                 \
                    objname = new classname();                      \
                    REST_InitTools::SetInitLevel(objname, level);   \
                }                                                   \
            } else {                                                \
                objname = new classname();                          \
                REST_InitTools::SetInitLevel(objname, level);       \
            }                                                       \
            REST_ARGS[#objname] = #classname;                       \
        }                                                           \
    };                                                              \
    const __##classname##_Init classname##_Init;

}  // namespace REST_InitTools

#endif
