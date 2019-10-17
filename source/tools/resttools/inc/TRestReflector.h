#ifndef RestTools_REST_Reflection
#define RestTools_REST_Reflection

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

#include <TFormula.h>
#include <TVector2.h>
#include <TVector3.h>
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
    /// Prepare the ROOT dictionary for this type.
    int InitDictionary();

   public:
    string name = "";
    string type = "";
    char* address = 0;
    bool onheap = false;
    int size = 0;
    TClass* cl = 0;
    TDataType* dt = 0;

	/// If this object type wrapper is invalid
    bool IsZombie();
    /// Streamer method of varioud types. Supports deep-cloning of custom TObject-inherited classes
    void operator>>(TRestReflector to);
    /// Convert the wrappered type to string
    string ToString();
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
        val = *(T*)(address);
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
        *((T*)(address)) = val;
    }
    /// Assembly a new object, and save its address. The old object will be destroied if not null
    void Assembly();
    /// Destroy the current object. It will make the class to be zombie.
    void Destroy();
    /// Print the Hex memory map of the wrappered object
    void PrintMemory(int bytepreline = 16);
    /// Type conversion operator
    template <class T>
    operator T*() {
        return (T*)address;
    }
    /// Default constructor
    TRestReflector() {}
    /// Constructor from a certain address and a certain type.
    TRestReflector(char* address, string type);
    /// Constructor to wrap an object
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
    /// Constructor to wrap an object pointer
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

///////////////////////////////////////////////
/// \brief Get the data member of a TObject inherited class with certain name.
///
/// The output is wrapped with TRestReflector.
/// Note that the data member with //! annotation in the class definition will not 
/// be recognized.
TRestReflector GetDataMember(REST_Reflection::TRestReflector obj, string name);

///////////////////////////////////////////////
/// \brief Get the data member of a TObject inherited class with given index
///
/// The 0th data member of a class will always be its base class.
/// 
/// Example :
/// \code
///
/// TRestRun r;
/// cout << REST_Reflection::GetDataMember(r,0).name << endl; //prints "TRestMetadata"
/// cout << REST_Reflection::GetDataMember(r,4).name << endl; //prints "fRunType"
/// REST_Reflection::GetDataMember(r,4).SetValue((TString)"aaa");
/// r->PrintMetadata();  //the run tag printed will be "aaa"
///
/// \endcode
///
///
TRestReflector GetDataMember(REST_Reflection::TRestReflector obj, int ID);

///////////////////////////////////////////////
/// \brief Get the number of data members of a class
int GetNumberOfDataMembers(REST_Reflection::TRestReflector obj);

};  // namespace REST_Reflection

typedef REST_Reflection::TRestReflector any;

#endif