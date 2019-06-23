#include "TRestReflection.h"
#include "TEmulatedCollectionProxy.h"
#include "TStreamerInfo.h"

map<string, TDataType*> REST_Reflection::lDataType = map<string, TDataType*>();

REST_Reflection::AnyPtr_t REST_Reflection::Assembly(string typeName) {
    AnyPtr_t ptr = WrapType(typeName);
    ptr.Assembly();
    return ptr;
}

REST_Reflection::AnyPtr_t REST_Reflection::WrapType(string type) {
    TClass* cl = GetClass(type);
    TDataType* dt = GetDataType(type);

    if (cl != NULL) {
        REST_Reflection::AnyPtr_t ptr;
        ptr.type = type;
        ptr.address = 0;
        ptr.onheap = false;
        ptr.size = cl->Size();
        ptr.cl = cl;
        ptr.dt = 0;
        return ptr;
    }
    if (dt != NULL) {
        REST_Reflection::AnyPtr_t ptr;
        ptr.type = type;
        ptr.address = 0;
        ptr.onheap = false;
        ptr.size = dt->Size();
        ptr.cl = 0;
        ptr.dt = dt;
        return ptr;
    }

    return REST_Reflection::AnyPtr_t();
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

void REST_Reflection::AnyPtr_t::CloneTo(AnyPtr_t to) {
    if (IsZombie() || to.IsZombie()) {
        cout << "In AnyPtr_t::CloneTo() : the ptr is zombie! " << endl;
        return;
    }

    if (type != to.type) {
        cout << "In AnyPtr_t::CloneTo() : type doesn't match! (This :" << type << ", Target : " << to.type
             << ")" << endl;
        return;
    }

    if (cl == NULL) {
        memcpy(to.address, address, size);
    } else {
        if (cl->GetCollectionProxy() && dynamic_cast<TEmulatedCollectionProxy*>(cl->GetCollectionProxy())) {
            cout << "In AnyPtr_t::CloneTo() : the target is an stl collection but does not have a "
                    "compiled CollectionProxy. Please generate the dictionary for this collection."
                 << endl;
			cout << "Data not copied!" << endl;
			return;
        }
        TBufferFile buffer(TBuffer::kWrite);

        buffer.MapObject(address, cl);  // register obj in map to handle self reference
        cl->Streamer(address, buffer);

        buffer.SetReadMode();
        buffer.ResetMap();
        buffer.SetBufferOffset(0);

        buffer.MapObject(to.address, cl);  // register obj in map to handle self reference
        cl->Streamer(to.address, buffer);
    }
}

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
    type = _type;
    cl = GetClass(type);
    dt = GetDataType(type);
    if (cl == NULL && dt == NULL) {
        cout << "In AnyPtr_t::AnyPtr_t() : unrecognized type: \"" << type << "\"" << endl;
        return;
    }
    size = cl == 0 ? dt->Size() : cl->Size();
}

REST_Reflection::AnyPtr_t REST_Reflection::GetDataMember(REST_Reflection::AnyPtr_t obj, string name) {
	if (obj.cl != NULL && obj.cl->InheritsFrom("TObject")) {
		return GetDataMember((TObject*)obj, name);
	}
	return AnyPtr_t();
}

REST_Reflection::AnyPtr_t REST_Reflection::GetDataMember(TObject* obj, string name) {
    TClass* c = obj->IsA();
    TVirtualStreamerInfo* vs = c->GetStreamerInfo();
    TObjArray* ses = vs->GetElements();
    int n = ses->GetLast() + 1;

    for (int i = 0; i < n; i++) {
        TStreamerElement* ele = (TStreamerElement*)ses->At(i);
        if ((string)ele->GetFullName() == name) {
            char* addr = (char*)obj + ele->GetOffset();
            string type = ele->GetTypeName();

            AnyPtr_t ptr(addr, type);
            ptr.name = name;
            ptr.parent = (char*)obj;
            ptr.offset = ele->GetOffset();

            return ptr;
        }
    }
    return AnyPtr_t();
}

REST_Reflection::AnyPtr_t REST_Reflection::GetDataMember(TObject* obj, int ID) {
    TClass* c = obj->IsA();
    TVirtualStreamerInfo* vs = c->GetStreamerInfo();
    TObjArray* ses = vs->GetElements();
    int n = ses->GetLast() + 1;

    if (ID < n) {
        TStreamerElement* ele = (TStreamerElement*)ses->At(ID);
        char* addr = (char*)obj + ele->GetOffset();
        string type = ele->GetTypeName();

        AnyPtr_t ptr(addr, type);
        ptr.name = ele->GetName();
        ptr.parent = (char*)obj;
        ptr.offset = ele->GetOffset();

        return ptr;
    }
    return AnyPtr_t();
}

int REST_Reflection::GetNumberOfDataMembers(TObject* obj) {
    TClass* c = obj->IsA();
    TVirtualStreamerInfo* vs = c->GetStreamerInfo();
    TObjArray* ses = vs->GetElements();

    return ses->GetLast() + 1;
}