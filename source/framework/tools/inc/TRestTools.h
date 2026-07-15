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

#include <TObject.h>
#include <TString.h>
#include <TVector3.h>

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#define UNUSED(x) (void)x

#ifdef WIN32
#define EXTERN_DEF __declspec(dllimport)
#define EXTERN_IMP __declspec(dllexport)
#else
#define EXTERN_DEF extern
#define EXTERN_IMP
#endif

#ifndef WIN32
#include <unistd.h>
#endif

const std::string PARAMETER_NOT_FOUND_STR = "NO_SUCH_PARA";
const double PARAMETER_NOT_FOUND_DBL = -99999999;

EXTERN_DEF std::string REST_COMMIT;
EXTERN_DEF std::string REST_PATH;
EXTERN_DEF std::string REST_USER;
EXTERN_DEF std::string REST_USER_PATH;
EXTERN_DEF std::string REST_TMP_PATH;
EXTERN_DEF std::map<std::string, std::string> REST_ARGS;

/// A generic class with useful static methods.
class TRestTools {
   public:
    static std::vector<std::string> GetOptions(std::string optionsStr);

    static void LoadRESTLibrary(bool silent = false);

    static int ReadASCIITable(std::string fName, std::vector<std::vector<Double_t>>& data,
                              Int_t skipLines = 0, std::string separator = "\t");
    static int ReadASCIITable(std::string fName, std::vector<std::vector<Float_t>>& data, Int_t skipLines = 0,
                              std::string separator = "\t");
    static int ReadASCIITable(std::string fName, std::vector<std::vector<std::string>>& data,
                              Int_t skipLines = 0, std::string separator = "\t");

    static int ReadCSVFile(std::string fName, std::vector<std::vector<Double_t>>& data, Int_t skipLines = 0);
    static int ReadCSVFile(std::string fName, std::vector<std::vector<Float_t>>& data, Int_t skipLines = 0);

    template <typename T>
    static void TransposeTable(std::vector<std::vector<T>>& data);

    template <typename T>
    static int ReadBinaryTable(std::string fName, std::vector<std::vector<T>>& data, Int_t columns = -1);

    static Bool_t IsBinaryFile(std::string fname);

    static std::string GetFileNameExtension(const std::string& fullname);
    static std::string GetFileNameRoot(const std::string& fullname);
    static std::vector<std::string> GetObservablesInString(const std::string& observablesStr,
                                                           bool removeDuplicates = true);
    static std::set<std::string> GetMatchingStrings(const std::vector<std::string>& stack,
                                                    const std::vector<std::string>& wantedStrings);

    static int GetBinaryFileColumns(std::string fname);

    template <typename T>
    static T GetMaxValueFromTable(const std::vector<std::vector<T>>& data, Int_t column = -1);

    template <typename T>
    static T GetMinValueFromTable(const std::vector<std::vector<T>>& data, Int_t column = -1);

    template <typename T>
    static T GetLowestIncreaseFromTable(std::vector<std::vector<T>> data, Int_t column);

    template <typename T>
    static T GetIntegralFromTable(const std::vector<std::vector<T>>& data);

    template <typename T>
    static std::vector<T> GetColumnFromTable(const std::vector<std::vector<T>>& data, unsigned int column);

    template <typename T>
    static int PrintTable(std::vector<std::vector<T>> data, Int_t start = 0, Int_t end = 0);

    template <typename T>
    static int ExportASCIITable(std::string fname, std::vector<std::vector<T>>& data);

    template <typename T>
    static int ExportBinaryTable(std::string fname, std::vector<std::vector<T>>& data);

    static Int_t isValidFile(const std::string& path);
    static bool fileExists(const std::string& filename);
    static bool isRootFile(const std::string& filename);
    static bool isRunFile(const std::string& filename);
    static bool isDataSet(const std::string& filename);
    static bool isURL(const std::string& filename);
    static bool isPathWritable(const std::string& path);
    static bool isAbsolutePath(const std::string& path);
    static std::string RemoveMultipleSlash(std::string);
    static std::string ToAbsoluteName(const std::string& filename);
    static std::vector<std::string> GetSubdirectories(const std::string& path, int recursion = -1);
    static std::pair<std::string, std::string> SeparatePathAndName(const std::string& fullname);
    static std::string GetPureFileName(const std::string& fullPathFileName);
    static std::string SearchFileInPath(std::vector<std::string> path, std::string filename);
    static bool CheckFileIsAccessible(const std::string&);
    static std::vector<std::string> GetFilesMatchingPattern(std::string pattern, bool unlimited = false);
    static int ConvertVersionCode(std::string in);
    static std::istream& GetLine(std::istream& is, std::string& t);

    static std::string Execute(std::string cmd);

    static std::string DownloadRemoteFile(const std::string& remoteFile, bool pidPrefix = false);
    static int DownloadRemoteFile(std::string remoteFile, std::string localFile);
    static int UploadToServer(std::string localFile, std::string remoteFile, std::string methodUrl = "");

    static std::string POSTRequest(const std::string& url, const std::map<std::string, std::string>& keys);
    static void ChangeDirectory(const std::string& toDirectory);

    static std::vector<int> CanvasDivisions(int n);
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

// Initialize global variable with certain class, overwriting the dummy variable of its base class.
// For example, we initialize gDataBase as TRestDataBase in Framework
// library. When we load restP3DB library, this object will be overwritten
// by a new TRestDataBaseP3DB class object, by calling this macro
#define MakeGlobal(classname, objName, level)                       \
    struct __##classname##_Init {                                   \
        __##classname##_Init() {                                    \
            REST_ARGS[#objName] = #classname;                       \
            if ((objName) != nullptr) {                             \
                if (REST_InitTools::CanOverwrite(objName, level)) { \
                    delete (objName);                               \
                    (objName) = new classname();                    \
                    REST_InitTools::SetInitLevel(objName, level);   \
                }                                                   \
            } else {                                                \
                (objName) = new classname();                        \
                REST_InitTools::SetInitLevel(objName, level);       \
            }                                                       \
        }                                                           \
    };                                                              \
    const __##classname##_Init classname##_Init;

}  // namespace REST_InitTools

enum Quantities { ENERGY, LENGTH, TIME };
class ValueWithQuantity {
   public:
    ValueWithQuantity(double value, Quantities quantity) : fValue(value), fQuantity(quantity){};
    double GetValue() const { return fValue; }
    std::string ToString() const;

    inline operator std::string() const { return ToString(); }

   private:
    const double fValue;
    const Quantities fQuantity;
};

inline std::string ToEnergyString(double value) { return (std::string)ValueWithQuantity(value, ENERGY); }
inline std::string ToTimeString(double value) { return (std::string)ValueWithQuantity(value, TIME); }
inline std::string ToLengthString(double value) { return (std::string)ValueWithQuantity(value, LENGTH); }

std::string ToTimeStringLong(double value);

inline std::string VectorToString(const TVector3& v) {
    return TString::Format("(%0.3f, %0.3f, %0.3f)", v.X(), v.Y(), v.Z()).Data();
}

inline std::string VectorToString(const TVector2& v) {
    return TString::Format("(%0.3f, %0.3f)", v.X(), v.Y()).Data();
}

#endif
