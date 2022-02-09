#ifndef RestTools_REST_Reflection
#define RestTools_REST_Reflection

#include <TFormula.h>
#include <TVector2.h>
#include <TVector3.h>
#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "Strlen.h"
#include "TBuffer.h"
#include "TBufferFile.h"
#include "TClass.h"
#include "TClassEdit.h"
#include "TDataMember.h"
#include "TDataType.h"
#include "TStreamerElement.h"
#include "TVirtualStreamerInfo.h"

#include "TRestStringHelper.h"

using namespace std;

class TRestEventProcess;

/// This namespace serves for the reflection functionality
namespace REST_Reflection {

struct DataType_Info {
    char name[20]{'u', 'n', 'k', 'n', 'o', 'w', 'n', 0};
    Int_t size = 0;
    const type_info* typeinfo = 0;

    DataType_Info() {}

    DataType_Info(string name) {
        switch (ToHash(name)) {
            case ToHash("char"):
                typeinfo = &typeid(char);
                size = sizeof(char);
                break;
            case ToHash("short"):
                typeinfo = &typeid(short);
                size = sizeof(short);
                break;
            case ToHash("int"):
                typeinfo = &typeid(int);
                size = sizeof(int);
                break;
            case ToHash("long"):
                typeinfo = &typeid(long);
                size = sizeof(long);
                break;
            case ToHash("long int"):
                typeinfo = &typeid(long int);
                size = sizeof(long int);
                break;
            case ToHash("long long"):
                typeinfo = &typeid(long long);
                size = sizeof(long long);
                break;
            case ToHash("bool"):
                typeinfo = &typeid(bool);
                size = sizeof(bool);
                break;
            case ToHash("float"):
                typeinfo = &typeid(float);
                size = sizeof(float);
                break;
            case ToHash("double"):
                typeinfo = &typeid(double);
                size = sizeof(double);
                break;
            case ToHash("long double"):
                typeinfo = &typeid(long double);
                size = sizeof(long double);
                break;
            case ToHash("unsigned char"):
                typeinfo = &typeid(unsigned char);
                size = sizeof(unsigned char);
                break;
            case ToHash("unsigned short"):
                typeinfo = &typeid(unsigned short);
                size = sizeof(unsigned short);
                break;
            case ToHash("unsigned int"):
                typeinfo = &typeid(unsigned int);
                size = sizeof(unsigned int);
                break;
            case ToHash("unsigned long"):
                typeinfo = &typeid(unsigned long);
                size = sizeof(unsigned long);
                break;
            case ToHash("unsigned long long"):
                typeinfo = &typeid(unsigned long long);
                size = sizeof(unsigned long long);
                break;
            case ToHash("unsigned long int"):
                typeinfo = &typeid(unsigned long int);
                size = sizeof(unsigned long int);
                break;
            case ToHash("char*"):
                typeinfo = &typeid(char*);
                size = sizeof(char*);
                break;
            case ToHash("size_t"):
                typeinfo = &typeid(size_t);
                size = sizeof(char*);
                break;
            // we also add some name of ROOT data types
            case ToHash("Char_t"):
                typeinfo = &typeid(Char_t);
                size = sizeof(Char_t);
                break;
            case ToHash("UChar_t"):
                typeinfo = &typeid(UChar_t);
                size = sizeof(UChar_t);
                break;
            case ToHash("Short_t"):
                typeinfo = &typeid(Short_t);
                size = sizeof(Short_t);
                break;
            case ToHash("Int_t"):
                typeinfo = &typeid(Int_t);
                size = sizeof(Int_t);
                break;
            case ToHash("UInt_t"):
                typeinfo = &typeid(UInt_t);
                size = sizeof(UInt_t);
                break;
            case ToHash("Long_t"):
                typeinfo = &typeid(Long_t);
                size = sizeof(Long_t);
                break;
            case ToHash("ULong_t"):
                typeinfo = &typeid(ULong_t);
                size = sizeof(ULong_t);
                break;
            case ToHash("Long64_t"):
                typeinfo = &typeid(Long64_t);
                size = sizeof(Long64_t);
                break;
            case ToHash("ULong64_t"):
                typeinfo = &typeid(ULong64_t);
                size = sizeof(ULong64_t);
                break;
            case ToHash("Float_t"):
                typeinfo = &typeid(Float_t);
                size = sizeof(Float_t);
                break;
            case ToHash("Float16_t"):
                typeinfo = &typeid(Float16_t);
                size = sizeof(Float16_t);
                break;
            case ToHash("Double_t"):
                typeinfo = &typeid(Double_t);
                size = sizeof(Double_t);
                break;
            case ToHash("Double32_t"):
                typeinfo = &typeid(Double32_t);
                size = sizeof(Double32_t);
                break;
            case ToHash("LongDouble_t"):
                typeinfo = &typeid(LongDouble_t);
                size = sizeof(LongDouble_t);
                break;
            case ToHash("Bool_t"):
                typeinfo = &typeid(Bool_t);
                size = sizeof(Bool_t);
                break;
            default:
                break;
        }
        if (typeinfo != 0) {
            strcpy(this->name, name.c_str());
        }
    }

    template <typename T>
    DataType_Info(T* obj) {
        string name = "";
        if (typeid(T) == typeid(char)) {
            name = "char";
        } else if (typeid(T) == typeid(short)) {
            name = "short";
        } else if (typeid(T) == typeid(int)) {
            name = "int";
        } else if (typeid(T) == typeid(long)) {
            name = "long";
        } else if (typeid(T) == typeid(long int)) {
            name = "long int";
        } else if (typeid(T) == typeid(long long)) {
            name = "long long";
        } else if (typeid(T) == typeid(bool)) {
            name = "bool";
        } else if (typeid(T) == typeid(float)) {
            name = "float";
        } else if (typeid(T) == typeid(double)) {
            name = "double";
        } else if (typeid(T) == typeid(long double)) {
            name = "long double";
        } else if (typeid(T) == typeid(unsigned char)) {
            name = "unsigned char";
        } else if (typeid(T) == typeid(unsigned short)) {
            name = "unsigned short";
        } else if (typeid(T) == typeid(unsigned int)) {
            name = "unsigned int";
        } else if (typeid(T) == typeid(unsigned long)) {
            name = "unsigned long";
        } else if (typeid(T) == typeid(unsigned long long)) {
            name = "unsigned long long";
        } else if (typeid(T) == typeid(unsigned long int)) {
            name = "unsigned long int";
        } else if (typeid(T) == typeid(char*)) {
            name = "char*";
        }

        if (name != "") {
            strcpy(this->name, name.c_str());
            size = sizeof(T);
            typeinfo = &typeid(T);
        }
    }
};

extern map<void*, TClass*> RESTListOfClasses_typeid;
extern map<string, TClass*> RESTListOfClasses_typename;

/// Wrap the string type name into ROOT type identifier "TClass"
///
/// Quicker than TClass::GetClass() since it stores limited objects in the map, no need to
/// iterate all the valid types. Do not call this method before main function.
///
inline TClass* GetClassQuick(string type) {
    auto iter = RESTListOfClasses_typename.find(type);
    if (iter != RESTListOfClasses_typename.end()) {
        return iter->second;
    } else {
        TClass* cl = TClass::GetClass(type.c_str());
        RESTListOfClasses_typename[type] = cl;
        return cl;
    }
    return NULL;
}

/////////////////////////////
/// \brief Get the type of a "class" object, returning the wrapped type identifier "TClass".
///
/// Quicker than TClass::GetClass() since it stores limited objects in the map, no need to
/// iterate all the valid types. Do not call this method before main function.
///
template <typename T>
TClass* GetClassQuick() {
    void* typeidaddr = (void*)&typeid(T);
    auto iter = RESTListOfClasses_typeid.find(typeidaddr);
    if (iter != RESTListOfClasses_typeid.end()) {
        return iter->second;
    } else {
        TClass* cl = TClass::GetClass(typeid(T));
        RESTListOfClasses_typeid[typeidaddr] = cl;
        return cl;
    }
    return NULL;
}

/// Get the type name of an object
template <typename T>
std::string GetTypeName() {
    TClass* cl = TClass::GetClass(typeid(T));
    if (cl != nullptr) {
        return cl->GetName();
    }
    return string(DataType_Info((T*)0).name);
}
/// Get the type name of an object
template <class T>
std::string GetTypeName(T obj) {
    return GetTypeName<T>();
}

class TRestReflector {
   private:
    /// Prepare the ROOT dictionary for this type
    int InitDictionary();
    /// If on heap, we can call Destroy() to TRestReflector. True only when initailized from Assembly()
    bool onheap = false;

   public:
    /// Name field
    string name = "";
    /// Type of the wrapped object
    string type = "";
    /// value of typeid(T).name() of the wrapped object
    const type_info* typeinfo = 0;
    /// Address of the wrapped object
    char* address = 0;
    /// Size of the object
    int size = 0;
    /// Pointer to the corresponding TClass helper, if the wrapped object is in class type
    TClass* cl = 0;
    /// Pointer to the corresponding TDataType helper, if the wrapped object is in data type
    bool is_data_type = false;
    /// If this object type wrapper is invalid
    bool IsZombie();
    /// Deep copy the content of the wrapped object to `to`.
    void operator>>(TRestReflector to);
    /// Output overload by calling ToString();
    friend ostream& operator<<(ostream& cin, TRestReflector ptr) { return cin << ptr.ToString(); }
    /// Get the value of the wrapped type, not recommended to use
    template <typename T>
    T GetValue() {
        if (typeid(T) != *this->typeinfo) {
            cout << "In TRestReflector::GetValue() : type unmatch! " << endl;
            cout << "Input: " << GetTypeName<T>() << ", this: " << this->type << endl;
            return T();
        }
        if (address != nullptr) return *(T*)(address);
        return T();
    }
    /// Set the value of the wrapped type
    template <class T>
    void SetValue(const T& val) {
        if (typeid(T) != *this->typeinfo) {
            cout << "In TRestReflector::SetValue() : type unmatch! " << endl;
            cout << "Input: " << GetTypeName<T>() << ", this: " << string(this->type) << endl;
            return;
        }
        if (address != nullptr) *((T*)(address)) = val;
    }
    /// Convert the wrapped object to string
    string ToString();
    /// Set the value of the wrapped object from string
    void ParseString(string str);
    /// Assembly a new object, and save its address. The old object will be destroied if not null
    void Assembly();
    /// Destroy the current object. It will make the class to be zombie.
    void Destroy();
    /// Print the Hex memory map of the wrappered object
    void PrintMemory(int bytepreline = 16);
    /// Find the class's datamember as TRestReflector object, including those from base class
    TRestReflector GetDataMember(string name);
    /// Get the i-th datamember of the class, ignoring those from base class
    TRestReflector GetDataMember(int ID);
    /// Get a list of the class's datamembers as a vector of string, including those from base class
    vector<string> GetListOfDataMembers();
    /// Get the value of datamember as string.
    string GetDataMemberValueString(string name);
    /// Get the number of data members of a class
    int GetNumberOfDataMembers();

    /// Type conversion operator. With this, one can implicitly convert TRestReflector object to
    /// pointer of certain type. For example, `TRestEvent* eve =
    /// REST_Reflection::Assembly("TRestRawSignalEvent");`
    template <class T>
    operator T*() {
        return (T*)address;
    }
    /// Default constructor
    TRestReflector() {}
    /// Constructor from a certain address and a certain type.
    TRestReflector(void* address, const string& type);
    /// Constructor to wrap an object. Any typed object can be revieved as argument.
    template <class T>
    TRestReflector(const T& obj) {
        address = (char*)&obj;
        InitFromTemplate<T>();
    }
    /// Constructor to wrap an object pointer.
    template <class T>
    TRestReflector(T* obj) {
        address = (char*)obj;
        InitFromTemplate<T>();
    }

    template <class T>
    void InitFromTemplate() {
        onheap = false;
        cl = REST_Reflection::GetClassQuick<T>();
        DataType_Info dt = DataType_Info((T*)0);
        if (cl == nullptr && dt.size == 0) {
            cout << "In TRestReflector::TRestReflector() : unrecognized type! " << endl;
            return;
        }

        typeinfo = &typeid(T);
        is_data_type = dt.size > 0;
        size = sizeof(T);
        if (cl == nullptr) {
            type = dt.name;
        } else {
            type = cl->GetName();
        }

        InitDictionary();
    }
};

///////////////////////////////////////////////
/// \brief Assembly an object of type: typeName, returning the allocated memory address and size
///
TRestReflector Assembly(string typeName);

///////////////////////////////////////////////
/// \brief Wrap information an object of type: typeName, memory is not allocated
///
TRestReflector WrapType(string typeName);

///////////////////////////////////////////////
/// \brief Deep copy the content of object `from` to `to`
///
/// Calls RESTVirtualConverter::CloneObj(). The actual methods are registered in converter.cpp
/// If not registered, you can add it manually with AddConverter() macro
void CloneAny(TRestReflector from, TRestReflector to);
};  // namespace REST_Reflection

typedef REST_Reflection::TRestReflector any;
typedef REST_Reflection::TRestReflector RESTValue;

class RESTVirtualConverter {
   public:
    virtual string ToString(void* obj) = 0;
    virtual void ParseString(void* obj, string str) = 0;
    virtual void CloneObj(void* from, void* to) = 0;
};

// type name, {toString method, parseString method}
extern map<string, RESTVirtualConverter*> RESTConverterMethodBase;

template <class T>
class Converter : RESTVirtualConverter {
   public:
    string (*ToStringFunc)(T);
    T (*ParseStringFunc)(string);
    static Converter<T>* thisptr;

    string ToString(void* obj) override { return ToStringFunc(*(T*)obj); }
    void ParseString(void* obj, string str) override {
        T newobj = ParseStringFunc(str);
        *((T*)(obj)) = newobj;
    }
    void CloneObj(void* from, void* to) override { *((T*)(to)) = *((T*)(from)); }

    Converter(string (*_ToStringFunc)(T), T (*_ParseStringFunc)(string)) {
        ToStringFunc = _ToStringFunc;
        ParseStringFunc = _ParseStringFunc;
        string typestr = REST_Reflection::GetTypeName<T>();
        if (RESTConverterMethodBase.count(typestr) > 0) {
            cout << "Warning! converter for type: " << typestr << " already added!" << endl;
        } else {
            RESTConverterMethodBase[typestr] = this;
        }
    }
};

#define AddConverter(ToStringFunc, ParseStringFunc, type) \
    template <>                                           \
    Converter<type>* Converter<type>::thisptr = new Converter<type>(&ToStringFunc, &ParseStringFunc);

#endif
