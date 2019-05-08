///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             Some useful tools in system level. It is obslete now.
///
///             Dec 2016:   First concept
///                 author: Javier Galan
///_______________________________________________________________________________

#ifndef RestCore_TRestTools
#define RestCore_TRestTools

#include <TClass.h>

#include <TList.h>
#include <TString.h>
#include <iostream>
#define UNUSED(x) (void)x

#include "TRestStringHelper.h"

class TRestTools {
   public:
    ///////////////////////////////////////////////
    /// \brief Returns the list of rest libraries.
    ///
    /// This method finds the rest library in system env "LD_LIBRARY_PATH",
    /// calling the method GetRESTLibrariesInDirectory(). This requests rest being
    /// installed correctly.
    ///
    static std::vector<TString> GetListOfRESTLibraries();

    ///////////////////////////////////////////////
    /// \brief Returns all paths in an env variable.
    ///
    /// This method gives the env variable string to the method GetFirstPath().
    /// And then adds the result to the list.
    ///
    static std::vector<TString> GetListOfPathsInEnvVariable(TString envVariable);

    ///////////////////////////////////////////////
    /// \brief Returns the first sub string spilt by ":" in a string
    ///
    /// The sub string will be removed from the input string. As a result calling
    /// this method repeatedly will get all the substring in the input string.
    /// Actually it does not return path, but any sub string spilt by ":"
    ///
    static TString GetFirstPath(TString& path);

    ///////////////////////////////////////////////
    /// \brief Returns the list of rest librarys found in a path.
    ///
    /// It just finds the files with name containing "REST" or "Rest" in that
    /// path.
    ///
    static std::vector<TString> GetRESTLibrariesInDirectory(TString path);

    ///////////////////////////////////////////////
    /// \brief Returns all the options in an option string
    ///
    /// This method gives string to the method GetFirstOption().
    /// And then adds the result to the list.
    ///
    static std::vector<TString> GetOptions(TString optionsStr);

    ///////////////////////////////////////////////
    /// \brief Returns the first option of the string
    ///
    static TString GetFirstOption(TString& path);

    ///////////////////////////////////////////////
    /// \brief Calls gSystem to load REST library.
    ///
    /// After this we can use reflection methods TClass::GetClass() and
    /// TRestMetadata::GetDataMemberRef()
    ///
    static void LoadRESTLibrary(bool silent = false);

    ///////////////////////////////////////////////
    /// \brief Reads an ASCII file containning a table with values
    ///
    /// This method will open the file fName. This file should contain a tabulated
    /// ASCII table containning numeric values. The values on the table will be
    /// loaded in the matrix provided through the argument `data`. The content of
    /// `data` will be cleared in this method.
    ///
    static int ReadASCIITable(TString fName, std::vector<std::vector<Double_t>>& data);

    ///////////////////////////////////////////////
    /// \brief Executes a shell command and returns its output in a string
    ///
    static std::string Execute(const char* cmd);

    static char* Assembly(TString typeName);

    template <class T>
    static std::string GetTypeName(T obj) {
        TClass* cl = TClass::GetClass(typeid(obj));
        if (cl != NULL) {
            return cl->GetName();
        } else {
            if (typeid(obj) == typeid(double)) {
                return "double";
            } else if (typeid(obj) == typeid(float)) {
                return "float";
            } else if (typeid(obj) == typeid(long double)) {
                return "long double";
            } else if (typeid(obj) == typeid(bool)) {
                return "bool";
            } else if (typeid(obj) == typeid(char)) {
                return "char";
            } else if (typeid(obj) == typeid(int)) {
                return "int";
            } else if (typeid(obj) == typeid(short)) {
                return "short";
            } else if (typeid(obj) == typeid(long)) {
                return "long";
            } else if (typeid(obj) == typeid(long long)) {
                return "long long";
            }
        }

        return "unknown";
    }

    /// Rest tools class
    ClassDef(TRestTools, 1);
};
#endif
