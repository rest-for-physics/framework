#include "TRestReflection.h"
#include "TEmulatedCollectionProxy.h"
#include "TRestStringHelper.h"
#include "TRestTools.h"
#include "TStreamerInfo.h"
#include "TSystem.h"

map<string, TDataType*> REST_Reflection::lDataType = map<string, TDataType*>();

REST_Reflection::AnyPtr_t REST_Reflection::Assembly(string typeName) {
    AnyPtr_t ptr = WrapType(typeName);
    ptr.Assembly();
    return ptr;
}

REST_Reflection::AnyPtr_t REST_Reflection::WrapType(string type) {
    return REST_Reflection::AnyPtr_t(0, type);
}

int REST_Reflection::AnyPtr_t::GetTypeID() {
	if (cl != 0) {
		if ((string)cl->GetName() == "string") {
			return TStreamerInfo::kSTL;
		}
		else if ((string)cl->GetName() == "TString") {
			return TStreamerInfo::kTString;
		}

		return cl->GetStreamerInfo()->GetElement(0)->GetType();
	}

	if (dt != 0) return dt->GetType();
	return -1;
}


void REST_Reflection::AnyPtr_t::Assembly() {
    if (!IsZombie() && onheap) {
        Destroy();
    }

    if (cl != NULL) {
        address = (char*)cl->New();
        onheap = true;
    } else if (dt != NULL) {
        address = (char*)malloc(size);
        memset(address, size, 0);
        onheap = true;
    }
}

void REST_Reflection::AnyPtr_t::Destroy() {
    if (address == NULL) return;
    if (onheap == false) {
        // It can only delete/free objects on heap memory
        cout << "In AnyPtr_t::Destroy() : cannot free on stack memory!" << endl;
        return;
    }

    if (cl != NULL) {
        cl->Destructor(address);
    } else if (dt != NULL) {
        free(address);
    }
}

void REST_Reflection::AnyPtr_t::PrintMemory(int bytepreline) {
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

void REST_Reflection::CloneAny(AnyPtr_t from, AnyPtr_t to) {
    if (from.IsZombie() || to.IsZombie()) {
        cout << "In AnyPtr_t::CloneTo() : the ptr is zombie! " << endl;
        return;
    }

    if (from.type != to.type) {
        cout << "In AnyPtr_t::CloneTo() : type doesn't match! (This :" << from.type
             << ", Target : " << to.type << ")" << endl;
        return;
    }

    if (from.cl == NULL) {
        memcpy(to.address, from.address, from.size);
    } else {
        TBufferFile buffer(TBuffer::kWrite);

        buffer.MapObject(from.address, from.cl);  // register obj in map to handle self reference
        from.cl->Streamer(from.address, buffer);

        buffer.SetReadMode();
        buffer.ResetMap();
        buffer.SetBufferOffset(0);

        buffer.MapObject(to.address, to.cl);  // register obj in map to handle self reference
        to.cl->Streamer(to.address, buffer);
    }
}

void REST_Reflection::AnyPtr_t::operator>>(AnyPtr_t to) { REST_Reflection::CloneAny(*this, to); }

string REST_Reflection::AnyPtr_t::ToString() {
    char* ladd = address;
    char* buffer = new char[500]();

    int atype = GetTypeID();
    int j;
    // assert(!((kOffsetP + kChar) <= atype && atype <= (kOffsetP + kBool) &&
    // count == 0));
    switch (atype) {
            // basic types
        case TStreamerInfo::kBool: {
            Bool_t* val = (Bool_t*)ladd;
            sprintf(buffer, "%d", *val);
            break;
        }
        case TStreamerInfo::kChar: {
            Char_t* val = (Char_t*)ladd;
            sprintf(buffer, "%d", *val);
            break;
        }
        case TStreamerInfo::kShort: {
            Short_t* val = (Short_t*)ladd;
            sprintf(buffer, "%d", *val);
            break;
        }
        case TStreamerInfo::kInt: {
            Int_t* val = (Int_t*)ladd;
            sprintf(buffer, "%d", *val);
            break;
        }
        case TStreamerInfo::kLong: {
            Long_t* val = (Long_t*)ladd;
            sprintf(buffer, "%ld", *val);
            break;
        }
        case TStreamerInfo::kLong64: {
            Long64_t* val = (Long64_t*)ladd;
            sprintf(buffer, "%lld", *val);
            break;
        }
        case TStreamerInfo::kFloat: {
            Float_t* val = (Float_t*)ladd;
            sprintf(buffer, "%f", *val);
            break;
        }
        case TStreamerInfo::kFloat16: {
            Float_t* val = (Float_t*)ladd;
            sprintf(buffer, "%f", *val);
            break;
        }
        case TStreamerInfo::kDouble: {
            Double_t* val = (Double_t*)ladd;
            sprintf(buffer, "%g", *val);
            break;
        }
        case TStreamerInfo::kDouble32: {
            Double_t* val = (Double_t*)ladd;
            sprintf(buffer, "%g", *val);
            break;
        }
        case TStreamerInfo::kUChar: {
            UChar_t* val = (UChar_t*)ladd;
            sprintf(buffer, "%u", *val);
            break;
        }
        case TStreamerInfo::kUShort: {
            UShort_t* val = (UShort_t*)ladd;
            sprintf(buffer, "%u", *val);
            break;
        }
        case TStreamerInfo::kUInt: {
            UInt_t* val = (UInt_t*)ladd;
            sprintf(buffer, "%u", *val);
            break;
        }
        case TStreamerInfo::kULong: {
            ULong_t* val = (ULong_t*)ladd;
            sprintf(buffer, "%lu", *val);
            break;
        }
        case TStreamerInfo::kULong64: {
            ULong64_t* val = (ULong64_t*)ladd;
            sprintf(buffer, "%llu", *val);
            break;
        }
        case TStreamerInfo::kBits: {
            UInt_t* val = (UInt_t*)ladd;
            sprintf(buffer, "%d", *val);
            break;
        }

        case TStreamerInfo::kCounter: {
            Int_t* val = (Int_t*)ladd;
            sprintf(buffer, "%d", *val);
            break;
        }
            // char *
        case TStreamerInfo::kCharStar: {
            char** val = (char**)ladd;
            if (*val) sprintf(buffer, "%s", *val);
            break;
        }
            // Class *  derived from TObject with comment field  //->
        case TStreamerInfo::kObjectp: {
            TObject** obj = (TObject**)(ladd);
            sprintf(buffer, "(%s)%lx", cl ? type.c_str() : "unknown_type", (Long_t)(*obj));
            break;
        }

            // Class*   derived from TObject
        case TStreamerInfo::kObjectP: {
            TObject** obj = (TObject**)(ladd);
            sprintf(buffer, "(%s)%lx", cl ? type.c_str() : "unknown_type", (Long_t)(*obj));
            break;
        }

            // Class    derived from TObject
        case TStreamerInfo::kObject: {
            TObject* obj = (TObject*)(ladd);
            sprintf(buffer, "%s", obj->GetName());
            break;
        }

            // Special case TStreamerInfo::for TString, TObject, TNamed
        case TStreamerInfo::kTString: {
            TString* st = (TString*)(ladd);
            sprintf(buffer, "%s", st->Data());
            break;
        }
        case TStreamerInfo::kTObject: {
            TObject* obj = (TObject*)(ladd);
            sprintf(buffer, "%s", obj->GetName());
            break;
        }
        case TStreamerInfo::kTNamed: {
            TNamed* named = (TNamed*)(ladd);
            sprintf(buffer, "%s/%s", named->GetName(), named->GetTitle());
            break;
        }

            // Class *  not derived from TObject with comment field  //->
        case TStreamerInfo::kAnyp: {
            TObject** obj = (TObject**)(ladd);
            sprintf(buffer, "%s @0x%lx", cl ? type.c_str() : "unknown_type", (Long_t)(*obj));
            break;
        }

            // Class*   not derived from TObject
        case TStreamerInfo::kAnyP: {
            TObject** obj = (TObject**)(ladd);
            sprintf(buffer, "%s @0x%lx", cl ? type.c_str() : "unknown_type", (Long_t)(*obj));
            break;
        }
            // Any Class not derived from TObject
        case TStreamerInfo::kOffsetL + TStreamerInfo::kObjectp:
        case TStreamerInfo::kOffsetL + TStreamerInfo::kObjectP:
        case TStreamerInfo::kAny: {
            sprintf(buffer, "printing kAny case TStreamerInfo::(%d)", atype);
            //         if (aElement) {
            //            TMemberStreamer *pstreamer = aElement->GetStreamer();
            //            if (pstreamer == 0) {
            //               //sprintf(buffer,"ERROR, Streamer is null\n");
            //               //aElement->ls();
            //               break;
            //            }
            //            //(*pstreamer)(b,ladd,0);
            //         }
            break;
        }
            // Base Class
        case TStreamerInfo::kBase: {
            sprintf(buffer, "printing kBase case TStreamerInfo::(%d)", atype);
            // aElement->ReadBuffer(b,pointer);
            break;
        }

        case TStreamerInfo::kOffsetL + TStreamerInfo::kObject:
        case TStreamerInfo::kOffsetL + TStreamerInfo::kTString:
        case TStreamerInfo::kOffsetL + TStreamerInfo::kTObject:
        case TStreamerInfo::kOffsetL + TStreamerInfo::kTNamed:
        case TStreamerInfo::kStreamer: {
            sprintf(buffer, "printing kStreamer case TStreamerInfo::(%d)", atype);
            //         TMemberStreamer *pstreamer = aElement->GetStreamer();
            //         if (pstreamer == 0) {
            //            //sprintf(buffer,"ERROR, Streamer is null\n");
            //            //aElement->ls();
            //            break;
            //         }
            //         //UInt_t start,count;
            //         //b.ReadVersion(&start, &count);
            //         //(*pstreamer)(b,ladd,0);
            //         //b.CheckByteCount(start,count,IsA());
            break;
        }

        case TStreamerInfo::kStreamLoop: {
            sprintf(buffer, "printing kStreamLoop case TStreamerInfo::(%d)", atype);
            //         TMemberStreamer *pstreamer = aElement->GetStreamer();
            //         if (pstreamer == 0) {
            //            //sprintf(buffer,"ERROR, Streamer is null\n");
            //            //aElement->ls();
            //            break;
            //         }
            // Int_t *counter = (Int_t*)(count);
            // UInt_t start,count;
            /// b.ReadVersion(&start, &count);
            //(*pstreamer)(b,ladd,*counter);
            // b.CheckByteCount(start,count,IsA());
            break;
        }
        case TStreamerInfo::kSTL: {
            if (cl) {
                static TClassRef stringClass("string");
                if (ladd && cl == stringClass) {
                    std::string* st = (std::string*)(ladd);
                    sprintf(buffer, "%s", st->c_str());
                } else {
                    sprintf(buffer, "%s @0x%lx", type.c_str(), (Long_t)(ladd));
                }
            } else {
                sprintf(buffer, "unknown_type @0x%lx", (Long_t)(ladd));
            }
            break;
        }
        default: { buffer[0] = 0; }
    }

    string result(buffer);
    delete[] buffer;
    return result;
}

REST_Reflection::AnyPtr_t::AnyPtr_t(char* _address, string _type) {
    address = _address;
    onheap = false;
    cl = GetClass(_type);
    dt = GetDataType(_type);
    if (cl == NULL && dt == NULL) {
        cout << "In AnyPtr_t::AnyPtr_t() : unrecognized type: \"" << _type << "\"" << endl;
        return;
    }
    InitDictionary();
}

int REST_Reflection::AnyPtr_t::InitDictionary() {
    size = cl == 0 ? dt->Size() : cl->Size();
    type = cl == 0 ? dt->GetName() : cl->GetName();

    if (type == "" || size == 0 || (cl == 0 && dt == 0)) {
        cout << "Error in REST_Reflection::CreateDictionary: object is zombie!" << endl;
        return -1;
    }

    if (dt != NULL) return 0;

    if (cl != NULL) {
        if (cl->GetCollectionProxy() && dynamic_cast<TEmulatedCollectionProxy*>(cl->GetCollectionProxy())) {
            // cout << "In AnyPtr_t::CloneTo() : the target is an stl collection but does not have a "
            //	"compiled CollectionProxy. Please generate the dictionary for this collection."
            //	<< endl;
            // cout << "Data not copied!" << endl;
        } else {
            return 0;
        }
    }

    if (1) {
        int pos = type.find("<");

        string basetype = type.substr(0, pos);
        vector<string> stltypes{"vector", "list", "map", "set", "array", "deque"};
        bool flag = false;
        for (auto stltype : stltypes) {
            if (basetype == stltype) {
                flag = true;
            }
        }
        if (!flag) {
            cout << "Error in REST_Reflection::CreateDictionary: unknown type \"" << type << "\"" << endl;
            return -1;
        }

        char* restpath = getenv("REST_PATH");
        if (restpath == NULL) {
            cout << "This cannot happen!" << endl;
            return -1;
        }

        string typeformatted = Replace(type, ">", "_");
        typeformatted = Replace(typeformatted, "<", "_");
        typeformatted = Replace(typeformatted, ",", "_");
        typeformatted = RemoveWhiteSpaces(typeformatted);

        string sofilename = restpath + (string) "/lib/AddonDict/Dict_" + typeformatted + ".so";

        // we directly load the dictionary if it exists
        if (TRestTools::fileExists(sofilename)) {
            cout << "Loading external dictionary for: \"" << type << "\":" << endl;
            cout << sofilename << endl;
            gSystem->Load(sofilename.c_str());
            cl = GetClass(type);  // reset the TClass after loading external library.
            return 0;
        }

        // we create a new library of dictionary for that type
        if (!TRestTools::isPathWritable(restpath)) {
            cout
                << "Error in REST_Reflection::CreateDictionary: cannot create dictionary, path not writeable!"
                << endl;
            cout << "path: \"" << restpath << "\"" << endl;
            cout << "This is possible in case you are using public installation of REST, install one by your "
                    "own?"
                 << endl;
            return -1;
        }
        system(Form("mkdir -p %s/lib/AddonDict", restpath));

        string linkdeffilename = restpath + (string) "/lib/AddonDict/LinkDef.h";
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

        string cxxfilename = restpath + (string) "/lib/AddonDict/" + typeformatted + ".cxx";

        cout << "Creating external dictionary for: \"" << type << "\":" << endl;
        cout << sofilename << endl;

        // cout << Form("rootcling -f %s -c %s", outfilename.c_str(), infilename.c_str()) << endl;
        int a = system(Form("rootcling -f %s -c %s", cxxfilename.c_str(), linkdeffilename.c_str()));
        if (a != 0) {
            cout << "rootcling failed to generate dictionary" << endl;
            return -1;
        }

        int b =
            system(Form("gcc %s -std=c++11 -I`root-config --incdir` "
                        "`root-config --libs` -lGui -lEve -lGeom -lMathMore -lGdml -lMinuit -L/usr/lib64 "
                        "-lstdc++ -shared -fPIC -o %s",
                        cxxfilename.c_str(), sofilename.c_str()));

        // int c =
        //    system(Form("gcc %s/lib/AddonDict/*.cxx -std=c++11 -I`root-config --incdir` "
        //                "`root-config --libs` -lGui -lEve -lGeom -lMathMore -lGdml -lMinuit -L/usr/lib64 "
        //                "-lstdc++ -shared -fPIC -o %s/lib/AddonDict/libRestAddonDict.so",
        //                restpath, restpath));

        if (b != 0 /*|| c != 0*/) {
            cout << "gcc failed to generate library for the dictionary" << endl;
            return -1;
        }

        gSystem->Load(Form("%s", sofilename.c_str()));
        cl = GetClass(type);  // reset the TClass after loading external library.
    }

    return 0;
}

REST_Reflection::AnyPtr_t REST_Reflection::GetDataMember(REST_Reflection::AnyPtr_t obj, string name) {
    TClass* c = obj.cl;
    if (c != NULL) {
        TVirtualStreamerInfo* vs = c->GetStreamerInfo();
        TObjArray* ses = vs->GetElements();
        int n = ses->GetLast() + 1;

        for (int i = 0; i < n; i++) {
            TStreamerElement* ele = (TStreamerElement*)ses->At(i);
            if ((string)ele->GetFullName() == name) {
                char* addr = (char*)obj + ele->GetOffset();
                string type = ele->GetTypeName();
                if (type == "BASE") {
                    type = ele->GetClass()->GetName();
                }
                if (type == obj.type) {
                    return AnyPtr_t();
                }

                AnyPtr_t ptr(addr, type);
                ptr.name = name;

                return ptr;
            }
        }
    }
    return AnyPtr_t();
}

REST_Reflection::AnyPtr_t REST_Reflection::GetDataMember(REST_Reflection::AnyPtr_t obj, int ID) {
    TClass* c = obj.cl;
    if (c != NULL) {
        TVirtualStreamerInfo* vs = c->GetStreamerInfo();
        TObjArray* ses = vs->GetElements();
        int n = ses->GetLast() + 1;

        if (ID < n) {
            TStreamerElement* ele = (TStreamerElement*)ses->At(ID);
            char* addr = (char*)obj + ele->GetOffset();
            string type = ele->GetTypeName();
            if (type == "BASE") {
                type = ele->GetClass()->GetName();
            }
            if (type == obj.type) {
                return AnyPtr_t();
            }

            AnyPtr_t ptr(addr, type);
            ptr.name = ele->GetName();
            return ptr;
        }
    }
    return AnyPtr_t();
}

int REST_Reflection::GetNumberOfDataMembers(REST_Reflection::AnyPtr_t obj) {
    TClass* c = obj.cl;
    TVirtualStreamerInfo* vs = c->GetStreamerInfo();
    TObjArray* ses = vs->GetElements();

    return ses->GetLast() + 1;
}