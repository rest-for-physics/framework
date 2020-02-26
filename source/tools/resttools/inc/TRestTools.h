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

#include <string>
#include <vector>

#include "TObject.h"

using namespace std;

#define UNUSED(x) (void)x

class TRestTools {
   public:
    ///////////////////////////////////////////////
    /// \brief Returns the list of rest libraries.
    ///
    /// This method finds the rest library in system env "LD_LIBRARY_PATH",
    /// calling the method GetRESTLibrariesInDirectory(). This requests rest being
    /// installed correctly.
    ///
    // static std::vector<string> GetListOfRESTLibraries();

    ///////////////////////////////////////////////
    /// \brief Returns all paths in an env variable.
    ///
    /// This method gives the env variable string to the method GetFirstPath().
    /// And then adds the result to the list.
    ///
    // static std::vector<string> GetListOfPathsInEnvVariable(string envVariable);

    ///////////////////////////////////////////////
    /// \brief Returns the first sub string spilt by ":" in a string
    ///
    /// The sub string will be removed from the input string. As a result calling
    /// this method repeatedly will get all the substring in the input string.
    /// Actually it does not return path, but any sub string spilt by ":"
    ///
    // static TString GetFirstPath(TString& path);

    ///////////////////////////////////////////////
    /// \brief Returns the list of rest librarys found in a path.
    ///
    /// It just finds the files with name containing "REST" or "Rest" in that
    /// path.
    ///
    // static std::vector<string> GetRESTLibrariesInDirectory(string path);

    ///////////////////////////////////////////////
    /// \brief Returns all the options in an option string
    ///
    /// This method gives string to the method GetFirstOption().
    /// And then adds the result to the list.
    ///
    static std::vector<string> GetOptions(string optionsStr);

    ///////////////////////////////////////////////
    /// \brief Returns the first option of the string
    ///
    // static TString GetFirstOption(TString& path);

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
    static int ReadASCIITable(string fName, std::vector<std::vector<Double_t>>& data);
    static int PrintTable(std::vector<std::vector<Double_t>> data, Int_t start = 0, Int_t end = 0);

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

    ///////////////////////////////////////////////
    /// \brief Executes a shell command and returns its output in a string
    ///
    static std::string Execute(string cmd);
    static std::string DownloadHttpFile(string remoteFile);

    /// Rest tools class
    ClassDef(TRestTools, 1);
};
#endif
