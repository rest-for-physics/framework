#include "TRestReflector.h"
#include "TRestStringHelper.h"
#include "TRestTools.h"

map<string, RESTVirtualConverter*> RESTConverterMethodBase;
template <class T>
string ToStringSimple(T source) {
    return ToString(source);
}
AddConverter(ToStringSimple, StringToInteger, int);
AddConverter(ToStringSimple, StringToDouble, double);
AddConverter(ToStringSimple, StringToBool, bool);
AddConverter(ToStringSimple, StringToFloat, float);
AddConverter(ToStringSimple, StringToLong, Long64_t);

char StringToChar(string in) { return in.size() > 0 ? (char)in[0] : 0; }
AddConverter(ToStringSimple, StringToChar, char);
short StringToShort(string in) { return StringToInteger(in); }
AddConverter(ToStringSimple, StringToShort, short);

long StringToLong32(string in) { return StringToDouble(in); }
AddConverter(ToStringSimple, StringToLong32, long);
unsigned char StringToUChar(string in) { return in.size() > 0 ? (unsigned char)in[0] : 0; }
AddConverter(ToStringSimple, StringToUChar, unsigned char);
unsigned short StringToUShort(string in) { return StringToInteger(in); }
AddConverter(ToStringSimple, StringToUShort, unsigned short);
unsigned int StringToUInt(string in) { return StringToInteger(in); }
AddConverter(ToStringSimple, StringToUInt, unsigned int);
unsigned long StringToULong(string in) { return StringToInteger(in); }
AddConverter(ToStringSimple, StringToULong, unsigned long);
unsigned long long StringToULL(string in) { return StringToInteger(in); }
AddConverter(ToStringSimple, StringToULL, unsigned long long);
TString StringToTString(string in) { return (TString)in; }
AddConverter(ToStringSimple, StringToTString, TString);

string TVector2ToString(TVector2 vec) { return Form("(%g,%g)", vec.X(), vec.Y()); }
AddConverter(TVector2ToString, StringTo2DVector, TVector2);
string TVector3ToString(TVector3 vec) { return Form("(%g,%g,%g)", vec.X(), vec.Y(), vec.Z()); }
AddConverter(TVector3ToString, StringTo3DVector, TVector3);

string StringToString(string in) { return in; }
AddConverter(StringToString, StringToString, string);

template <class T>
string VectorToString(vector<T> vec) {
    stringstream ss;
    ss << "{";
    for (int i = 0; i < vec.size(); i++) {
        ss << Converter<T>::thisptr->ToStringFunc(vec.at(i));
        if (i < vec.size() - 1) {
            ss << ",";
        }
    }
    ss << "}";
    return ss.str();
}

template <class T>
vector<T> StringToVector(string vec) {
    vector<T> result;
    if (vec[0] == '{' && vec[vec.size() - 1] == '}') {
        vec.erase(vec.begin());
        vec.erase(vec.end() - 1);
        vector<string> parts = Split(vec, ",");

        for (string part : parts) {
            while (part[0] == ' ') {
                part.erase(part.begin());
            }
            while (part[part.size() - 1] == ' ') {
                part.erase(part.end() - 1);
            }
            result.push_back(Converter<T>::thisptr->ParseStringFunc(part));
        }

    } else {
        cout << "illegal format!" << endl;
        return vector<T>{};
    }

    return result;
}
AddConverter(VectorToString, StringToVector, vector<string>);
AddConverter(VectorToString, StringToVector, vector<int>);
AddConverter(VectorToString, StringToVector, vector<double>);
AddConverter(VectorToString, StringToVector, vector<TString>);

template <class T1, class T2>
string MapToString(map<T1, T2> vec) {
    stringstream ss;
    ss << "{";
    int cont = 0;
    for (auto const& x : vec) {
        if (cont > 0) ss << ",";
        cont++;

        ss << "[";
        ss << Converter<T1>::thisptr->ToStringFunc(x.first);
        ss << ":";
        ss << Converter<T2>::thisptr->ToStringFunc(x.second);
        ss << "]";
    }
    ss << "}";
    return ss.str();
}

template <class T1, class T2>
map<T1, T2> StringToMap(string vec) {
    map<T1, T2> result;
    // input string format: {[dd:7],[aa:8],[ss:9]}
    if (vec[0] == '{' && vec[vec.size() - 1] == '}') {
        vec.erase(vec.begin());
        vec.erase(vec.end() - 1);
        vector<string> parts = Split(vec, ",");

        for (string part : parts) {
            while (part[0] == ' ') {
                part.erase(part.begin());
            }
            while (part[part.size() - 1] == ' ') {
                part.erase(part.end() - 1);
            }

            if (part[0] == '[' && part[part.size() - 1] == ']') {
                part.erase(part.begin());
                part.erase(part.end() - 1);
                vector<string> key_value = Split(part, ":");
                if (key_value.size() == 2) {
                    T1 key = Converter<T1>::thisptr->ParseStringFunc(key_value[0]);
                    T2 value = Converter<T2>::thisptr->ParseStringFunc(key_value[1]);
                    result[key] = value;
                } else {
                    cout << "illegal format!" << endl;
                    return map<T1, T2>{};
                }
            } else {
                cout << "illegal format!" << endl;
                return map<T1, T2>{};
            }
        }

    } else {
        cout << "illegal format!" << endl;
        return map<T1, T2>{};
    }

    return result;
}

#define comma ,
AddConverter(MapToString, StringToMap, map<TString comma double>);
AddConverter(MapToString, StringToMap, map<TString comma TVector2>);