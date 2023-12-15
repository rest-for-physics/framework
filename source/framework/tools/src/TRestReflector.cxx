#include "TRestReflector.h"

#include "TEmulatedCollectionProxy.h"
#include "TRestStringHelper.h"
#include "TRestTools.h"
#include "TStreamerInfo.h"
#include "TSystem.h"

using namespace std;

namespace REST_Reflection {
////////////////////////////////////////////////////////////////
///
/// Wrapper class for different type objects
///
/// Supports both class objects(string, vector, TRestMetadata, etc.) and basic type objects(int, double,
/// etc.).
/// Implements object wrapping, type assembly, data member reflection, memory streaming, and string
/// conversion.
/// We set a type alias `any` for this class.
///
/// Example 1: Type assembly & Value assignment
/// \code
///
/// TRestEventProcess* proc = REST_Reflection::Assembly("TRestRawSignalAnalysisProcess");
/// proc->PrintMetadata();
/// any member = REST_Reflection::GetDataMember(obj, "fIntegralRange");
/// TVector2 v(10, 50);
/// member.SetValue(v);
/// proc->PrintMetadata();
///
/// \endcode
///
/// Example 2: Object wrapping & Memory streaming
/// \code
///
/// map<int, double> a{{6, 1.4}, {7, 1.23}};
/// map<int, double> b;
/// ToString(any(a));    // returns string: "map<int,double> @0x7f2b03ff0038"
/// any(a) >> any(b);     // copy a's data to b
/// b[6]; // we can get value 1.4
///
/// \endcode
///
/// Example 3: Convert to string
/// \code
///
/// TVector2 a(1,3);
/// ToString(a);          // shows the string "(1,3)"
/// vector<int> b{3,5,7,9,2};
/// cout << any(b) << endl;   // prints the content of b in string: "{3,5,7,9,2}"
///
/// \endcode
///
/// \class TRestReflector
///
TRestReflector::TRestReflector(void* _address, const string& _type) {
    address = (char*)_address;
    onheap = false;
    cl = GetClassQuick(_type);
    DataType_Info dt = DataType_Info(_type);
    if (cl == nullptr && dt.size == 0) {
        cout << "In TRestReflector::TRestReflector() : unrecognized type: \"" << _type << "\"" << endl;
        return;
    }

    typeinfo = cl == nullptr ? dt.typeinfo : cl->GetTypeInfo();
    is_data_type = dt.size > 0;
    size = cl == nullptr ? dt.size : cl->Size();
    type = cl == nullptr ? dt.name : cl->GetName();

    InitDictionary();
}

void TRestReflector::Assembly() {
    if (!IsZombie() && onheap) {
        Destroy();
    }

    if (cl != nullptr) {
        address = (char*)cl->New();
        onheap = true;
    } else if (is_data_type) {
        address = (char*)malloc(size);
        memset(address, 0, size);
        onheap = true;
    }
}

void TRestReflector::Destroy() const {
    if (address == nullptr) {
        return;
    }
    if (!onheap) {
        // It can only delete/free objects on heap memory
        cout << "In TRestReflector::Destroy() : cannot free on stack memory!" << endl;
        return;
    }

    if (cl != nullptr) {
        cl->Destructor(address);
    } else if (is_data_type) {
        free(address);
    }
}

void TRestReflector::PrintMemory(int bytepreline) {
    if (!IsZombie()) {
        int i = 0;
        while (i < size) {
            stringstream save;
            for (unsigned char j = 0; j < bytepreline && i < size; j++) {
                // 44 03 00 d1 56 00 00 05 00 16 60 4a 38 08 81 8e
                // 44 03 00 D1 56 00 00 05 00 16 60 4A 38 08 81 8E
                save << std::uppercase << std::setfill('0') << std::setw(2) << std::hex
                     << ((*(address + i)) & 0xff) << " ";
                i++;
            }
            cout << save.str() << endl;
        }
    }
}

void TRestReflector::operator>>(const TRestReflector& to) { CloneAny(*this, to); }

string TRestReflector::ToString() const {
    if (type == "string") {
        return *(string*)(address);
    }
    if (address == nullptr) {
        return "null";
    }
    RESTVirtualConverter* converter = RESTConverterMethodBase[typeinfo->hash_code()];
    if (converter != nullptr) {
        return converter->ToString(address);
    } else {
        return Form("Type: %s, Address: %p", type.c_str(), address);
    }
}

void TRestReflector::ParseString(const string& str) const {
    if (type == "string") {
        *(string*)(address) = str;
    } else {
        RESTVirtualConverter* converter = RESTConverterMethodBase[typeinfo->hash_code()];
        if (converter != nullptr) {
            converter->ParseString(address, str);
        } else {
            cout << "Method for parsing string to " << type << " has not been registered!" << endl;
        }
    }
}

int TRestReflector::InitDictionary() {
    if (is_data_type) {
        return 0;
    }

    if (cl != nullptr) {
        if (cl->GetCollectionProxy() && dynamic_cast<TEmulatedCollectionProxy*>(cl->GetCollectionProxy())) {
            // cout << "In TRestReflector::CloneTo() : the target is an stl collection but does not have a "
            //	"compiled CollectionProxy. Please generate the dictionary for this collection."
            //	<< endl;
            // cout << "Data not copied!" << endl;
        } else {
            return 0;
        }
    }

    if (type.empty() || size == 0 || cl == nullptr) {
        cout << "Error in CreateDictionary: object is zombie!" << endl;
        return -1;
    }

    int pos = ((string)type).find('<');

    string basetype = ((string)type).substr(0, pos);
    vector<string> stltypes{"vector", "list", "map", "set", "array", "deque"};
    bool flag = false;
    for (auto stltype : stltypes) {
        if (basetype == stltype) {
            flag = true;
        }
    }
    if (!flag) {
        cout << "Error in CreateDictionary: unknown type \"" << type << "\"" << endl;
        return -1;
    }

    string typeformatted = Replace(type, ">", "_");
    typeformatted = Replace(typeformatted, "<", "_");
    typeformatted = Replace(typeformatted, ",", "_");
    typeformatted = RemoveWhiteSpaces(typeformatted);

    string sofilename = REST_USER_PATH + (string) "/AddonDict/Dict_" + typeformatted + ".so";

    // we directly load the dictionary if it exists
    if (TRestTools::fileExists(sofilename)) {
        cout << "Loading external dictionary for: \"" << type << "\":" << endl;
        cout << sofilename << endl;
    } else {
        // we create a new library of dictionary for that type
        if (!TRestTools::isPathWritable(REST_USER_PATH)) {
            cout << "Error in CreateDictionary: cannot create dictionary, path not writeable!" << endl;
            cout << "path: \"" << REST_USER_PATH << "\"" << endl;
            cout << "This is possible in case you are using public installation of REST, install one by your "
                    "own?"
                 << endl;
            return -1;
        }
        if (system(Form("mkdir -p %s/AddonDict", REST_USER_PATH.c_str())) != 0) {
            cout << "mkdir failed to create directory" << endl;
            return -1;
        }

        string linkdeffilename = REST_USER_PATH + (string) "/AddonDict/LinkDef.h";
        ofstream ofs(linkdeffilename);
        ofs << "#include <map>" << endl;
        ofs << "#include <vector>" << endl;
        ofs << "#include <map>" << endl;
        ofs << "#include <set>" << endl;
        ofs << "#include <list>" << endl;
        ofs << "#include <array>" << endl;
        ofs << "#ifdef __ROOTCLING__" << endl;
        ofs << "#pragma link C++ class " << type << ";" << endl;
        ofs << "#endif" << endl;
        ofs.close();

        string cxxfilename = REST_USER_PATH + (string) "/AddonDict/" + typeformatted + ".cxx";

        cout << "Creating external dictionary for: \"" << type << "\":" << endl;
        cout << sofilename << endl;

        if (system(Form("rootcling -f %s -c %s", cxxfilename.c_str(), linkdeffilename.c_str())) != 0) {
            cout << "rootcling failed to generate dictionary" << endl;
            return -1;
        }

        if (system(Form("gcc %s `root-config --cflags` "
                        "`root-config --libs` -lGui -lGeom -lGdml -lMinuit -L/usr/lib64 "
                        "-lstdc++ -shared -fPIC -o %s",
                        cxxfilename.c_str(), sofilename.c_str())) != 0) {
            cout << "gcc failed to generate library for the dictionary" << endl;
            return -1;
        }
    }

    gSystem->Load(sofilename.c_str());
    RESTListOfClasses_typeid.clear();
    RESTListOfClasses_typename.clear();
    cl = GetClassQuick(type);      // reset the TClass after loading external library.
    typeinfo = cl->GetTypeInfo();  // update the typeinfo
    return 0;
}

bool TRestReflector::IsZombie() const {
    return (type.empty() || address == nullptr || size == 0 || (cl == nullptr && !is_data_type));
}

TRestReflector Assembly(const string& typeName) {
    TRestReflector ptr = WrapType(typeName);
    ptr.Assembly();
    return ptr;
}

TRestReflector WrapType(const string& type) { return TRestReflector(nullptr, type); }

void CloneAny(const TRestReflector& from, const TRestReflector& to) {
    if (from.IsZombie() || to.IsZombie()) {
        cout << "In TRestReflector::CloneTo() : the ptr is zombie! " << endl;
        return;
    }

    if (from.type != to.type) {
        cout << "In TRestReflector::CloneTo() : type doesn't match! (This :" << from.type
             << ", Target : " << to.type << ")" << endl;
        return;
    }

    RESTVirtualConverter* converter = RESTConverterMethodBase[from.typeinfo->hash_code()];
    if (converter != nullptr) {
        converter->CloneObj(from.address, to.address);
    } else {
        cout << "Method for cloning type: \"" << from.type << "\" has not been registered!" << endl;
    }
}

TRestReflector TRestReflector::GetDataMember(const string& name) {
    if (cl != nullptr) {
        TDataMember* mem = cl->GetDataMember(name.c_str());
        if (mem == nullptr) {
            // find data member also in base class.
            TVirtualStreamerInfo* vs = cl->GetStreamerInfo();
            TObjArray* ses = vs->GetElements();
            int n = ses->GetLast() + 1;
            for (int i = 0; i < n; i++) {
                TStreamerElement* ele = (TStreamerElement*)ses->At(i);
                string type = ele->GetTypeName();
                if (type == "BASE") {
                    char* addr = address + ele->GetOffset();
                    type = ele->GetClass()->GetName();
                    return TRestReflector(addr, type).GetDataMember(name);
                }
            }
        } else {
            char* addr = address + mem->GetOffset();
            string type = mem->GetTypeName();
            TRestReflector ptr(addr, type);
            ptr.name = name;
            return ptr;
        }
    }
    return TRestReflector();
}

TRestReflector TRestReflector::GetDataMember(int ID) {
    if (cl != nullptr) {
        TList* list = cl->GetListOfDataMembers();
        if (ID < GetNumberOfDataMembers()) {
            TDataMember* mem = (TDataMember*)list->At(ID);
            char* addr = address + mem->GetOffset();
            string type = mem->GetTypeName();
            string name = mem->GetName();
            TRestReflector ptr(addr, type);
            ptr.name = name;
            return ptr;
        }
    }
    return TRestReflector();
}

vector<string> TRestReflector::GetListOfDataMembers() const {
    vector<string> dataMembers;

    // add datamembers from base class first
    TVirtualStreamerInfo* vs = cl->GetStreamerInfo();
    TObjArray* ses = vs->GetElements();
    int n = ses->GetLast() + 1;
    for (int i = 0; i < n; i++) {
        TStreamerElement* ele = (TStreamerElement*)ses->At(i);
        string type = ele->GetTypeName();
        if (type == "BASE") {
            char* addr = address + ele->GetOffset();
            type = ele->GetClass()->GetName();

            auto baseDataMembers = TRestReflector(addr, type).GetListOfDataMembers();
            dataMembers.insert(dataMembers.end(), baseDataMembers.begin(), baseDataMembers.end());
        }
    }

    // then add datamembers of this class
    TList* list = cl->GetListOfDataMembers();
    for (int i = 0; i < list->GetSize(); i++) {
        TDataMember* mem = (TDataMember*)list->At(i);
        string name = mem->GetName();

        dataMembers.push_back(name);
    }

    return dataMembers;
}

string TRestReflector::GetDataMemberValueString(const string& name) {
    TRestReflector member = GetDataMember(name);
    if (!member.IsZombie()) {
        return member.ToString();
    }
    return "";
}

int TRestReflector::GetNumberOfDataMembers() const {
    if (cl != nullptr) {
        return cl->GetNdata();
    }
    return 0;
}
}  // namespace REST_Reflection
