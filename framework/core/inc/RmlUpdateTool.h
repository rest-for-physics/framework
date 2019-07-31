#include "TRestMetadata.h"
#include "TRestStringHelper.h"
#include "tinyxml.h"

class RmlUpdateTool : public TRestMetadata {
   private:
    string fFileName = "";        // the input file name
    string fOutputFileName = "";  // the output file name
    string fFileContent = "";     // the whole content of input file
    bool fStatus = false;

   protected:
    void InitFromConfigFile() {}
    bool UpdateSyntax();
    bool UpdateStructure();
    string WriteFile();

   public:
    void Import(string Filename);
    bool Is2_1();
    bool UpdateSucceed() { return fStatus; }
    bool Process();
    string GetOutputFile() { return fOutputFileName; }

    RmlUpdateTool();
    RmlUpdateTool(string Filename, bool process = false);
};
