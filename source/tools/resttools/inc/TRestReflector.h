#ifndef RestTools_REST_Reflection
#define RestTools_REST_Reflection

#include <TFormula.h>
#include <TVector2.h>
#include <TVector3.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

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

using namespace std;

class TRestEventProcess;

/// This namespace serves for the reflection functionality
namespace REST_Reflection {

extern map<string, TDataType*> __ListOfDataTypes;

/// Wrap the string type name into ROOT type identifier "TDataType"
inline TDataType* GetDataType(string type) {
    if (__ListOfDataTypes[type] == NULL) {
        TDataType* dt = new TDataType(type.c_str());
        if (dt->GetType() == -1) {
            delete dt;
        } else {
            __ListOfDataTypes[type] = dt;
        }
    }
    return __ListOfDataTypes[type];
}
/// Get the type of a "data" object, returning the wrapped type identifier "TDataType"
template <typename T>
TDataType* GetDataType() {
    string type = "";
    {
        if (typeid(T) == typeid(unsigned int)) {
            type = "unsigned int";
        } else if (typeid(T) == typeid(unsigned)) {
            type = "unsigned";
        } else if (typeid(T) == typeid(int)) {
            type = "int";
        } else if (typeid(T) == typeid(unsigned long)) {
            type = "unsigned long";
        } else if (typeid(T) == typeid(long)) {
            type = "long";
        } else if (typeid(T) == typeid(unsigned long long)) {
            type = "unsigned long long";
        } else if (typeid(T) == typeid(long long)) {
            type = "long long";
        } else if (typeid(T) == typeid(unsigned short)) {
            type = "unsigned short";
        } else if (typeid(T) == typeid(short)) {
            type = "short";
        } else if (typeid(T) == typeid(unsigned char)) {
            type = "unsigned char";
        } else if (typeid(T) == typeid(char)) {
            type = "char";
        } else if (typeid(T) == typeid(bool)) {
            type = "bool";
        } else if (typeid(T) == typeid(float)) {
            type = "float";
        } else if (typeid(T) == typeid(double)) {
            type = "double";
        } else if (typeid(T) == typeid(signed char)) {
            type = "signed char";
        } else if (typeid(T) == typeid(Float16_t)) {
            type = "Float16_t";
        } else if (typeid(T) == typeid(Double32_t)) {
            type = "Double32_t";
        } else if (typeid(T) == typeid(char*)) {
            type = "char*";
        }
    }
    return GetDataType(type);
}

/// Wrap the string type name into ROOT type identifier "TClass"
inline TClass* GetClass(string type) { return TClass::GetClass(type.c_str()); }
/// Get the type of a "class" object, returning the wrapped type identifier "TClass"
template <typename T>
TClass* GetClass() {
    return TClass::GetClass(typeid(T));
}

/// Get the type name of an object
template <typename T>
std::string GetTypeName() {
    TClass* cl = GetClass<T>();
    if (cl != NULL) {
        return cl->GetName();
    }
    TDataType* dt = GetDataType<T>();
    if (dt != NULL) {
        return dt->GetName();
    }
    return "unknown";
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
    /// Address of the wrapped object
    char* address = 0;
    /// Size of the object
    int size = 0;
    /// Pointer to the corresponding TClass helper, if the wrapped object is in class type
    TClass* cl = 0;
    /// Pointer to the corresponding TDataType helper, if the wrapped object is in data type
    TDataType* dt = 0;
    /// If this object type wrapper is invalid
    bool IsZombie();
    /// Streamer method of varioud types. Supports deep-cloning of custom TObject-inherited classes
    void operator>>(TRestReflector to);
    /// Output overload by calling ToString();
    friend ostream& operator<<(ostream& cin, TRestReflector ptr) { return cin << ptr.ToString(); }
    /// Get the id of the wrapperd type. The enum used for type id is TStreamerInfo::EReadWrite
    int GetTypeID();
    /// Get the value of the wrapped type, not recommended to use
    template <typename T>
    void GetValue(T& val, bool check = false) {
        if (check) {
            if (GetTypeName<T>() != type) {
                cout << "In TRestReflector::GetValue() : type unmatch! " << endl;
                return;
            }
        }
        if (address != NULL) val = *(T*)(address);
    }
    /// Set the value of the wrapped type
    template <class T>
    void SetValue(const T& val, bool check = false) {
        if (check) {
            if (GetTypeName<T>() != type) {
                cout << "In TRestReflector::GetValue() : type unmatch! " << endl;
                return;
            }
        }
        if (address != NULL) *((T*)(address)) = val;
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
    /// Get its data member if the wrapped object is a class
    TRestReflector GetDataMember(string name);
    /// Get its data member if the wrapped object is a class
    TRestReflector GetDataMember(int ID);
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
    TRestReflector(void* address, string type);
    /// Constructor to wrap an object. Any typed object can be revieved as argument.
    template <class T>
    TRestReflector(const T& obj) {
        address = (char*)&obj;
        onheap = false;
        cl = REST_Reflection::GetClass<T>();
        dt = REST_Reflection::GetDataType<T>();
        if (cl == NULL && dt == NULL) {
            cout << "In TRestReflector::TRestReflector() : unrecognized type! " << endl;
            return;
        }
        InitDictionary();
    }
    /// Constructor to wrap an object pointer.
    template <class T>
    TRestReflector(T* obj) {
        address = (char*)obj;
        onheap = false;
        cl = REST_Reflection::GetClass<T>();
        dt = REST_Reflection::GetDataType<T>();
        if (cl == NULL && dt == NULL) {
            cout << "In TRestReflector::TRestReflector() : unrecognized type! " << endl;
            return;
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
/// If the type is class type, it will use ROOT streamer. The class member
/// with //! annotation will not be copied. The content of pointer class member
/// with //-> annotation will also be copied.
///
/// If the type is base data type, it will use memcpy()
void CloneAny(TRestReflector from, TRestReflector to);
};  // namespace REST_Reflection

typedef REST_Reflection::TRestReflector any;


class RESTVirtualConverter {
   public:
    virtual string ToString(void* obj) = 0;
    virtual void ParseString(void* obj, string str) = 0;
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
  template <>                                    \
    Converter<type>* Converter<type>::thisptr = new Converter<type>(&ToStringFunc, &ParseStringFunc);

#endif