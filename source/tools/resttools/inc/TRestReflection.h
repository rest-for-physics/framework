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

/// String helper classes. Declared static to be able to have direct access to the methods
namespace REST_Reflection {

extern map<string, TDataType*> lDataType;
inline TDataType* GetDataType(string type) {
    if (lDataType[type] == NULL) {
        TDataType* dt = new TDataType(type.c_str());
        if (dt->GetType() == -1) {
            delete dt;
        } else {
            lDataType[type] = dt;
        }
    }
    return lDataType[type];
}
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

inline TClass* GetClass(string type) { return TClass::GetClass(type.c_str()); }
template <typename T>
TClass* GetClass() {
    return TClass::GetClass(typeid(T));
}

///////////////////////////////////////////////
/// \brief Get the type name of an object
///
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
template <class T>
std::string GetTypeName(T obj) {
    return GetTypeName<T>();
}

class AnyPtr_t {
private:
	int InitDictionary();
public:
	//basic info
	string name = "";
    string type = "";
    char* address = 0;
    bool onheap = false;
    int size = 0;
    TClass* cl = 0;
    TDataType* dt = 0;

    bool IsZombie() { return (type == "" || address == 0 || size == 0 || (cl == 0 && dt == 0)); }

	void operator>>(AnyPtr_t to);

    string ToString();
    friend ostream& operator<<(ostream& cin, AnyPtr_t ptr) { return cin << ptr.ToString(); }

	int GetTypeID() {
		if (cl != 0) return cl->GetStreamerInfo()->GetElement(0)->GetType();
		if (dt != 0) return dt->GetType();
		return -1;
	}
    template <typename T>
    void GetValue(T& val, bool check = false) {
        if (check) {
            if (GetTypeName<T>() != name) {
                cout << "In AnyPtr_t::GetValue() : type unmatch! " << endl;
                return;
            }
        }
        val = *(T*)(address);
    }
    template <class T>
    void SetValue(const T& val, bool check = false) {
        if (check) {
            if (GetTypeName<T>() != name) {
                cout << "In AnyPtr_t::GetValue() : type unmatch! " << endl;
                return;
            }
        }
        *((T*)(address)) = val;
    }

    // Assembly a new object, and save its address. The old object will be destroied if not null
    void Assembly();
    // Destroy the current object. It will make the class to be zombie.
    void Destroy();
	// Print the Hex memory map of the wrappered object
	void PrintMemory(int bytepreline=16);

	template <class T>
	operator T*() {
		return (T*)address;
	}

    AnyPtr_t() {}
    AnyPtr_t(char* address, string type);
    template <class T>
    AnyPtr_t(const T& obj) {
        address = (char*)&obj;
        onheap = false;
        cl = REST_Reflection::GetClass<T>();
        dt = REST_Reflection::GetDataType<T>();
        if (cl == NULL && dt == NULL) {
            cout << "In AnyPtr_t::AnyPtr_t() : unrecognized type! " << endl;
            return;
        }
		InitDictionary();
    }
    template <class T>
    AnyPtr_t(T* obj) {
        address = (char*)obj;
        onheap = false;
        cl = REST_Reflection::GetClass<T>();
        dt = REST_Reflection::GetDataType<T>();
        if (cl == NULL && dt == NULL) {
            cout << "In AnyPtr_t::AnyPtr_t() : unrecognized type! " << endl;
            return;
        }
		InitDictionary();
    }
};

///////////////////////////////////////////////
/// \brief Assembly an object of type: typeName, returning the allocated memory address and size
///
AnyPtr_t Assembly(string typeName);

///////////////////////////////////////////////
/// \brief Wrap information an object of type: typeName, memory is not allocated
///
AnyPtr_t WrapType(string typeName);

void CloneAny(AnyPtr_t from, AnyPtr_t to);

// data member reflection tools
AnyPtr_t GetDataMember(REST_Reflection::AnyPtr_t obj, string name);
AnyPtr_t GetDataMember(REST_Reflection::AnyPtr_t obj, int ID);
int GetNumberOfDataMembers(REST_Reflection::AnyPtr_t obj);

};  // namespace REST_Reflection

typedef REST_Reflection::AnyPtr_t any;

#endif