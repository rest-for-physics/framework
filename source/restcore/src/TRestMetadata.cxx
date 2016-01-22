///______________________________________________________________________________;
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestMetadata.cxx
///
///             Base class from which to inherit all other REST metadata classes 
///             (config, etc...) 
///             TRestMetadata controls the I/O to config files.
///             It also offers inspection of the command line arguments
///
///             jun 2014:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Igor G. Irastorza
///             jul 2015:    Javier Galan
///_______________________________________________________________________________
//
// TODO IMPORTANT : Skip comments!!


#include "TRestMetadata.h"

int debug = 0;

const int NAME_NOT_FOUND = -2;
const int NOT_FOUND = -1;
const int ERROR = -1;
const int OK = 0;

ClassImp(TRestMetadata)
//______________________________________________________________________________
    TRestMetadata::TRestMetadata()
{
   // TRestMetadata default constructor
   SetDefaultConfigFilePath();
}


//______________________________________________________________________________
TRestMetadata::TRestMetadata( const char *cfgFileName)
{
    // TRestMetadata constructor loading data from config file
    SetDefaultConfigFilePath();
    fConfigFileName = cfgFileName;

    SetTitle("Config");
    SetName("TRestMetadata");
    fSectionName = "Metadata";

    CheckConfigFile( ); // just checking if file exists it will give a warning if not

    // TODO. CheckConfigGrammar
}


//______________________________________________________________________________
TRestMetadata::~TRestMetadata()
{
   // TRestMetadata destructor
 //  fclose( configFile );
}

//______________________________________________________________________________
void TRestMetadata::SetConfigFilePath(const char *configFilePath)
{
   // Lets the user define the default path where to search for the config 
   // file for this metadata object. Normally one should not use this function, 
   // as the constructor metadata provides a default value to
   // fConfigFilePath through the SetDefaultConfigFilePath() function

   fConfigFilePath = string(configFilePath);
}

//______________________________________________________________________________
string TRestMetadata::trim(string str)
{
    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last-first+1));
}

Int_t TRestMetadata::isAExpression( string in )
{
    size_t pos = 0;
    string st1 = "sqrt";
    string st2 = "0";
    in = Replace( in, st1, st2, pos );

    pos = 0;
    st1 = "log";
    st2 = "0";
    in = Replace( in, st1, st2, pos );

    // If number returns 1. If not returns 0
    return (in.find_first_not_of("-0123456789+*/.,)(") == std::string::npos && in.length() != 0);
}

Int_t TRestMetadata::isANumber( string in )
{
    // If number returns 1. If not returns 0
    return (in.find_first_not_of("-+0123456789.e") == std::string::npos && in.length() != 0);
}

string TRestMetadata::RemoveWhiteSpaces( string in )
{
    string out = in;
    size_t pos = 0;
    while( (pos = out.find(" ", pos)) != string::npos )
    {
        out.erase( pos, 1 ); 
        pos = pos + 1;
    }

    return out;
}

Int_t TRestMetadata::Count( string s, string sbstring)
{
    int count = 0;
    size_t nPos = s.find(sbstring, 0); // fist occurrence
    while(nPos != string::npos)
    {
        count++;
        nPos = s.find( sbstring, nPos+1);
    }

    return count;
}

string TRestMetadata::Replace( string in, string thisString, string byThisString, size_t fromPosition = 0 )
{
    string out = in;
    size_t pos = fromPosition;
    while( (pos = out.find( thisString , pos)) != string::npos )
    {
        if( debug ) cout << "replacing (" << thisString << ") by (" << byThisString << ")" << endl;
        out.replace( pos, thisString.length(), byThisString ); 
        pos = pos + 1;
    }

    return out;
}

Double_t TRestMetadata::StringToDouble( string in )
{
    if( isANumber ( in ) )
    {
        return stod ( in );
    }
    else
    {
        return -1;
    }
}

Int_t TRestMetadata::StringToInteger( string in )
{
    return (Int_t) StringToDouble( in );
}

TVector3 TRestMetadata::StringTo3DVector( string in )
{
    // TODO make a warning when format is not correct
    TVector3 a;

    size_t startVector = in.find_first_of("(");
    if( startVector == string::npos ) return a;

    size_t endVector = in.find_first_of(")");
    if( endVector == string::npos ) return a;

    size_t n = count(in.begin(), in.end(), ',');
    if( n != 2 ) return a;

    size_t firstComma = in.find_first_of(",");
    size_t secondComma = in.find(",", firstComma+1);

    if( firstComma >= endVector || firstComma <= startVector ) return a;
    if( secondComma >= endVector || secondComma <= startVector ) return a;

    string X = in.substr( startVector+1, firstComma-startVector-1 );
    string Y = in.substr( firstComma+1, secondComma-firstComma-1 );
    string Z = in.substr( secondComma+1, endVector-secondComma-1 );

    a.SetXYZ( StringToDouble(X), StringToDouble(Y), StringToDouble(Z) );

    return a;
}

TVector2 TRestMetadata::StringTo2DVector( string in )
{
    // TODO make a warning when format is not correct
    TVector2 a(-1,-1);

    size_t startVector = in.find_first_of("(");
    if( startVector == string::npos ) return a;

    size_t endVector = in.find_first_of(")");
    if( endVector == string::npos ) return a;

    size_t n = count(in.begin(), in.end(), ',');
    if( n != 1 ) return a;

    size_t firstComma = in.find_first_of(",");

    if( firstComma >= endVector || firstComma <= startVector ) return a;

    string X = in.substr( startVector+1, firstComma-startVector-1 );
    string Y = in.substr( firstComma+1, endVector-firstComma-1 );


    a.Set( StringToDouble(X), StringToDouble(Y)  );

    return a;
}

bool TRestMetadata::fileExists(const std::string& filename)
{
    struct stat buf;
    if (stat(filename.c_str(), &buf) != -1)
    {
        return true;
    }
    return false;
}


//______________________________________________________________________________
void TRestMetadata::SetDefaultConfigFilePath()
{
   // Assigns a default value to fConfigFilePath derived from the environment

   char *path;
   char cfgpath[256];
   path = getenv("REST_PATH");
   sprintf(cfgpath, "%s/config/", path);
   SetConfigFilePath((const char *) cfgpath);
}

Int_t TRestMetadata::LoadSectionMetadata( string section, string cfgFileName )
{
    // TODO we should remove here commented code inside the config.rml 
    // Comments not yet allowed inside config file!

    fSectionName = section;

    fConfigFileName = cfgFileName;
    if( debug > 1 ) cout << fConfigFileName << endl;
    string fileName = fConfigFilePath + fConfigFileName;
    if( debug > 1 ) cout << fileName << endl;

    ifstream file(fileName);

    // We load all the config file in a temporal buffer
    string temporalBuffer;
    string line;
    while(getline(file, line)) temporalBuffer += line;

    // We temporally associate the globals to the configBuffer
    size_t pos = 0;
    configBuffer = GetKEYStructure( "globals", pos, temporalBuffer );

    // We extract the values from globals. 
    // Globals will not be stored but they will be used by the REST framework during execution

    pos = 0;
    fDataPath = GetParameter( "mainDataPath", pos, configBuffer );
    string vLevelString  = GetParameter( "verboseLevel", pos, configBuffer );

    if( vLevelString == "silent" )
    {
        fVerboseLevel = REST_Silent;
        cout << "Setting verbose level to silent : " << fVerboseLevel << endl;
    }
    else if ( vLevelString == "warning" )
    {
        fVerboseLevel = REST_Warning;
        cout << "Setting verbose level to warning : " << fVerboseLevel <<  endl;

    }
    else if ( vLevelString == "info" )
    {
        fVerboseLevel = REST_Info;
        cout << "Setting verbose level to info : " << fVerboseLevel << endl;

    }
    else if ( vLevelString == "debug" )
    {
        fVerboseLevel = REST_Debug;
        cout << "Setting verbose level to debug : " << fVerboseLevel << endl;
    }

    // Then we just extract the corresponding section defined in the derived class (fSectionName)
    size_t sectionPosition = FindSection( temporalBuffer );
    if ( sectionPosition == (size_t) NOT_FOUND ) { cout << "Section " << fSectionName << " not found" << endl; return -1; }

    configBuffer = GetKEYStructure( "section", sectionPosition, temporalBuffer );

    string sectionDefinition = GetKEYDefinition( "section", configBuffer );
    if( (TString) this->GetName() == "Not defined" ) 
    {
        string nameref = GetFieldValue( "nameref", sectionDefinition );
        string fileref = GetFieldValue( "file", sectionDefinition );

        if( nameref != "Not defined" && fileref != "Not defined" )
        {
            configBuffer = GetSectionByNameFromFile( nameref, fileref );
        }
        else
        {
            cout << "REST Error : Section : " << fSectionName << " --> name, nameref or file not found!!" << endl;

            configBuffer = "";
        }
    }

    if( configBuffer == "" ) cout << "REST error : Config buffer is EMPTY" << endl;

    size_t position = 0;
    string value, myParam;
    while( position != string::npos )
    {
        myParam = GetMyParameter( value, position );

        // We replace only the pure parameters that we find between quotes (not inside mathematical expressions)
        /*
        myParam  = "\"" + myParam + "\"";
        value = "\"" + value + "\"";
        */
        if( myParam != "\"\"" )
        {
            if( debug ) cout << myParam << " = " << value << endl;
            configBuffer = Replace( configBuffer, myParam, value, position );
        }
    }
    configBuffer = ReplaceMathematicalExpressions( configBuffer );

    while( Count ( configBuffer, "<for" ) > 0 )
        configBuffer = ExpandForLoops( configBuffer );

    position = 0;
    while( position != string::npos )
    {
        myParam = GetMyParameter( value, position );
        if( myParam != "\"\"" )
        {
            if( debug ) cout << myParam << " = " << value << endl;
            configBuffer = Replace( configBuffer, myParam, value, position );
        }
    }

    /*
    cout << "===================================" << endl;
    cout << configBuffer << endl;
    cout << "===================================" << endl;
    getchar();
    */

    if( debug > 0 )
    {
        cout << "=====config buffer===(before mathematical replacement)======" << endl;
        cout << configBuffer << endl;
        cout << endl << "======================" << endl;
        getchar();
    }

    configBuffer = ReplaceMathematicalExpressions( configBuffer );

    /*
    cout << "===================================" << endl;
    cout << configBuffer << endl;
    cout << "===================================" << endl;
    getchar();
    */

    if( debug > 0 )
    {
        cout << "=====config buffer=================" << endl;
        cout << configBuffer << endl;
        cout << endl << "======================" << endl;
        getchar();
    }

    if( file != NULL ) file.close();
    return 0;
}

// This might be an improved version of GetKEYStructure()
string TRestMetadata::ExtractLoopStructure( string in, size_t pos )
{
    if( debug > 2 )
    {
        cout << "------IN------" << endl;
        cout << in << endl;
    }
    string startKey = "<for";
    string endKey = "/for";

    string output = "";

    int forDepth = 0;
    size_t startPos = in.find( startKey );
    if( pos == string::npos ) return output;

    pos = startPos + 4;
    forDepth++;

    while( pos != string::npos && forDepth > 0 )
    {
        size_t nextForStart = in.find( startKey, pos );
        size_t nextForEnd = in.find( endKey, pos );
        if( nextForEnd == string::npos ) break;

        if( nextForStart < nextForEnd ) 
        {
            forDepth++;
            pos = nextForStart+4;
        }
        else
        {
            forDepth--;
            pos = nextForEnd+4;
        }
    }

    return in.substr( startPos, pos-startPos+1 );

}


string TRestMetadata::ExpandForLoops( const string buffer )
{
    string outputBuffer = buffer;

    /*
    string forLoop = GetKEYStructure( "for", outputBuffer );

    cout << "-------Loop FOR------" << endl;
    cout << forLoop << endl;
    getchar();
    */

    // Searching the most internal for
    if( debug > 2 ) 
    {
        cout << "------input for ExtractLoopStructure-------" << endl;
        cout << outputBuffer << endl;
    }
    size_t pos = 0;
    string forLoop = ExtractLoopStructure( outputBuffer, pos );

    if( debug > 0 )
    {
        cout << " For loop to expand " << endl;
        cout << " ----------- " << endl;
        cout << forLoop << endl;
        cout << " ----------- " << endl;
        getchar();
    }

    // We replace the place loop content in output buffer by a TAG for later replacement
    outputBuffer = Replace( outputBuffer, forLoop, "PLACE FOR LOOP EXPANSION" );

    if( debug > 0 )
    {
        cout << "Input buffer modified" << endl;
        cout << " ----------- " << endl;
        cout << outputBuffer << endl;
        cout << " ----------- " << endl;
        getchar();
    }


    // We obtain the for loop parameters
    string forDefinition = GetKEYDefinition( "for", forLoop );

    forDefinition = ReplaceMathematicalExpressions( forDefinition );

    string variable = GetFieldValue( "variable", forDefinition );
    string varStr = "[" + variable + "]";
    Double_t from = StringToDouble( GetFieldValue( "from", forDefinition ) );
    Double_t to = StringToDouble ( GetFieldValue( "to", forDefinition ) );
    Double_t step = StringToDouble ( GetFieldValue( "step", forDefinition ) );

    // We obtain the for loop content to be repeated
    string forContent = forLoop.substr( forDefinition.length(), forLoop.find_last_of( "</for" ) - forDefinition.length()-4 );
    forContent = forContent.substr( forContent.find("<"), forContent.length()-forContent.find("<") );


    if( debug > 1 )
    {
        cout << " For content " << endl;
        cout << " ----------- " << endl;
        cout << forContent << endl;
    }

    // We replace the variable by its loop value and to forReplacement
    string forReplacement = "";
    for( double n = from; n < to; n = n + step )
    {
        ostringstream ss;
        ss << n;
        string nStr = ss.str();

        forReplacement += Replace( forContent, varStr, nStr );
    }

    if( debug > 1 )
    {
        cout << " For replacement " << endl;
        cout << "+++++++++++++++++" << endl;
        cout << forReplacement << endl;
        cout << "+++++++++++++++++" << endl;
    }

    // We replace the resulting repeated sentences in the previously defined TAG 
    outputBuffer = Replace( outputBuffer, "PLACE FOR LOOP EXPANSION", forReplacement );

    if( debug > 1 )
    {
        cout << " Final result " << endl;
        cout << "+++++++++++++++++" << endl;
        cout << outputBuffer << endl;
        cout << "+++++++++++++++++" << endl;
    }

    return outputBuffer;
}

string TRestMetadata::EvaluateExpression( string exp )
{
    if( !isAExpression( exp ) ) { return exp; }

    TFormula formula("tmp", exp.c_str());

    ostringstream sss;
    Double_t number = formula.EvalPar(0);
    if( number > 0 && number < 1.e-300 ) 
        { cout << "REST Warning! Expression not recognized --> " << exp << endl;  return exp; }

    sss << number;
    string out = sss.str();

    return out;
}

string TRestMetadata::ReplaceMathematicalExpressions( const string buffer )
{
    string outputBuffer = buffer;


    int startPosition = 0;
    int endPosition = 0;

    // searching any fields within quotes ""
    while ( ( startPosition = outputBuffer.find( "\"", endPosition ) ) != (int) string::npos )
    {
        endPosition = outputBuffer.find( "\"", startPosition+1 );
        if( endPosition == (int) string::npos ) break;

        string expression = outputBuffer.substr( startPosition+1, endPosition-startPosition-1 );
        endPosition++;

        string replacement = "";

        if( expression[0] == '(' && expression[expression.length()-1] == ')' )
        {
            replacement += "(";
            string firstComponent = expression.substr( 1, expression.find(",")-1 );

            replacement += EvaluateExpression( firstComponent );
            replacement += ",";

            if ( Count( expression, "," ) == 2 )
            {
                string secondComponent = expression.substr( expression.find(",")+1, expression.find_last_of(",")-expression.find(",")-1 );
                replacement += EvaluateExpression( secondComponent );
                replacement += ",";

            }

            string lastComponent = expression.substr( expression.find_last_of(",")+1, expression.find_last_of(")")-expression.find_last_of(",")-1 );
            replacement += EvaluateExpression( lastComponent );
            replacement += ")";
        }
        else
        {
            replacement += EvaluateExpression( expression );
        }

        outputBuffer.replace( startPosition+1, endPosition-startPosition-2, replacement );


        if( debug )
        {
        cout << "Expression : " << expression << " replacement : " << replacement << endl;
        cout << "-----------------" << endl;
        cout << "Remainning buffer" << endl;
        cout << "-----------------" << endl;
        }
        endPosition = endPosition + replacement.length() - expression.length();
        if( debug ) 
            cout << outputBuffer.substr( endPosition ) << endl;

    }

    return outputBuffer;
}

//______________________________________________________________________________
Int_t TRestMetadata::CheckConfigFile( )
{
    string fileName = fConfigFilePath + fConfigFileName;
    cout << "Config filename : " << fileName << endl;

    ifstream ifs;
    ifs.open ( fileName, std::ifstream::in);

    if( !ifs ) { cout << "WARNING. Config file could not be opened. Right path/filename?" << endl; exit(1); return ERROR; }
    else ifs.close();

    return OK;
}



void TRestMetadata::CheckSection( )
{

    /*
     *  TODO : Check if the section defined in TRestSpecificMetadata is in the config file
     *
     *
     * */


}

Int_t TRestMetadata::FindEndSection( Int_t initPos )
{
    Int_t endSectionPos = configBuffer.find("</section>", initPos );

    if( (size_t) endSectionPos == string::npos ) return NOT_FOUND;
    else return endSectionPos;
}

string TRestMetadata::GetMyParameter( string &value, size_t &pos )
{

    string parameterString = GetKEYDefinition( "myParameter", pos );
 //   cout << "Parameter string : " << parameterString << endl;

    if( parameterString.find( "name" ) != string::npos && parameterString.find( "value" ) != string::npos )
    {
        value = GetFieldValue( "value", parameterString );
 //       pos = parameterString.find("value");
        return GetFieldValue( "name", parameterString );
    }

    return "";
}

string TRestMetadata::GetParameter( string parName, size_t &pos, string inputString )
{
    // TODO : this can be probably removed since now we store only the section on configBuffer
    // TODO : It can be useful a GetParameter( string parName, string sectionBuffer )

    /* TODO
     *
     *  Implement method FindAnySection 
     *  if AnySection position is less than EndSection >> Then </section> has been forgotten.
     *  Make WARNING
     *
     * */

    /*
     *  TODO To impose in this code that parameter must be preceded by parameter KEY word
     *
     *  This will not be a problem if the parameter name is not found anywhere else in the section.
     *  But if the parameter name is written somewhere else it may cause problems.
     *  We must find first the parameter KEY and then seach the name in the parameter substring.
     *
     * */

    while( 1 )
    {
        string parameterString = GetKEYDefinition( "parameter", pos, inputString );
 //       cout << "Parameter string : " << parameterString << endl;

        if( parameterString.find( parName ) != string::npos )
        {
            return GetFieldValue( "value", parameterString );
        }
        else
        {
            if( debug > 1 ) cout << " I did not found" << endl;
            cout << "Something went wrong. Parameter (" << parName << ") NOT found" << endl;
            return "";
        }
    }

    cout << "Something went wrong. Parameter (" << parName << ") NOT found" << endl;
    return "";
}

string TRestMetadata::GetParameter( string parName, TString defaultValue )
{
    // TODO : this can be probably removed since now we store only the section on configBuffer
    // TODO : It can be useful a GetParameter( string parName, string sectionBuffer )
    size_t position = 0;

    /* TODO
     *
     *  Implement method FindAnySection 
     *  if AnySection position is less than EndSection >> Then </section> has been forgotten.
     *  Make WARNING
     *
     * */

    /*
     *  TODO To impose in this code that parameter must be preceded by parameter KEY word
     *
     *  This will not be a problem if the parameter name is not found anywhere else in the section.
     *  But if the parameter name is written somewhere else it may cause problems.
     *  We must find first the parameter KEY and then seach the name in the parameter substring.
     *
     * */

    string parameterString;
    while( position != string::npos )
    {
        parameterString = GetKEYDefinition( "parameter", position );
        if( debug > 1 ) cout << "Parameter string : " << parameterString << endl;

        if( parameterString.find( parName ) != string::npos )
        {
            return GetFieldValue( "value", parameterString );
        }
        else
        {
            if( debug > 1 ) cout << " I did not found" << endl;
        }
    }
    debug = 0;

    cout << "Parameter (" << parName << ") NOT found" << endl;
    cout << "Returning default value (" << defaultValue << ")" << endl;
    return defaultValue.Data();
}

// gets the field from parName <key --- parName="XX" --- >
string TRestMetadata::GetFieldFromKEY( string parName, string key )
{
    size_t position = 0;

    string parameterString;
    while( position != string::npos )
    {
        parameterString = GetKEYDefinition( key, position );
        if( debug ) cout << key << " string : " << parameterString << endl;
        if( parameterString.find( parName ) != string::npos )
            return GetFieldValue( parName, parameterString );
    }

    cout << "Something went wrong. Parameter (" << parName << ") NOT found" << endl;
    return "";
}

// Searches in substr
string TRestMetadata::GetFieldValue( string fieldName, string definition )
{
    string fldName = fieldName + "=\"";

    size_t pos, pos2;
    pos = definition.find( fldName ); 

    if( (pos = definition.find( fldName )) == string::npos ) {  return "Not defined"; }
    else
    {
        pos = definition.find( "\"", pos );
        pos++;
        pos2 = definition.find( "\"", pos );
        return definition.substr( pos, pos2-pos );
    }
}

// Searches in the configBuffer from position
string TRestMetadata::GetFieldValue( string fieldName, size_t fromPosition )
{
    string fldName = fieldName + "=\"";

    size_t pos = 0,pos2;
    size_t endDefinition = configBuffer.find(">", fromPosition);
    string definition = configBuffer.substr( fromPosition, endDefinition-fromPosition);
    pos = definition.find( fldName ); 

    if( (pos = definition.find( fldName )) == string::npos ) {  return "Not defined"; }
    else
    {
        pos = definition.find( "\"", pos );
        pos++;
        pos2 = definition.find( "\"", pos );
        return definition.substr( pos, pos2-pos );
    }
}


// returns the string containing <KEY --- >
string TRestMetadata::GetKEYDefinition( string keyName )
{
    Int_t fromPosition = 0;
    size_t startPos = configBuffer.find( keyName, fromPosition );
    size_t endPos = configBuffer.find( ">", startPos );

    fromPosition = endPos;

    if( startPos == string::npos ) return "";
    else return configBuffer.substr( startPos, endPos-startPos );

}

string TRestMetadata::GetKEYDefinition( string keyName, size_t &fromPosition )
{
    size_t startPos = configBuffer.find( keyName, fromPosition );
    size_t endPos = configBuffer.find( ">", startPos );

    fromPosition = endPos;

    if( startPos == string::npos ) return "";
    else return configBuffer.substr( startPos, endPos-startPos );

}

string TRestMetadata::GetKEYDefinition( string keyName, string buffer )
{

    size_t startPos = buffer.find( keyName, 0 );
    size_t endPos = buffer.find( ">", startPos );

    return buffer.substr( startPos, endPos-startPos );

}

string TRestMetadata::GetKEYDefinition( string keyName, size_t &fromPosition, string buffer )
{

    size_t startPos = buffer.find( keyName, fromPosition );
    if ( startPos == string::npos ) return "";
    size_t endPos = buffer.find( ">", startPos );
    if ( endPos == string::npos ) return "";

    fromPosition = endPos;

    return buffer.substr( startPos, endPos-startPos );

}

// returns the string containing <KEY --- /KEY>
string TRestMetadata::GetKEYStructure( string keyName )
{
    string strNotFound = "NotFound";

    size_t position = 0;

    string startKEY = "<" + keyName;
    string endKEY = "/" + keyName;

    size_t initPos = configBuffer.find( startKEY, position );

    if( initPos == string::npos ) { cout << "KEY (" << keyName << ") >> not found!!" << endl; return strNotFound; }

    size_t endPos = configBuffer.find( endKEY, position );

    if( endPos == string::npos ) { cout << "KEY (" << keyName << " << not found!!" << endl; return strNotFound; }

    return configBuffer.substr( initPos, endPos-initPos + endKEY.length()+1 );

}
//
// returns the string containing <KEY --- /KEY>
string TRestMetadata::GetKEYStructure( string keyName, string buffer )
{

    size_t position = 0;

    string startKEY = "<" + keyName;
    string endKEY = "/" + keyName;

    size_t initPos = buffer.find( startKEY, position );

    if( initPos == string::npos ) { if( debug ) cout << "KEY not found!!" << endl; return "NotFound"; }

    size_t endPos = buffer.find( endKEY, position );

    if( endPos == string::npos ) { if( debug ) cout << "KEY not found!!" << endl; return "NotFound"; }


    return buffer.substr( initPos, endPos-initPos + endKEY.length()+1 );

}

string TRestMetadata::GetKEYStructure( string keyName, size_t &fromPosition )
{
    size_t position = fromPosition;

    string startKEY = "<" + keyName;
    string endKEY = "/" + keyName;

    size_t initPos = configBuffer.find( startKEY, position );

    if( initPos == string::npos ) { if( debug ) cout << "KEY not found!!" << endl; return "NotFound"; }

    size_t endPos = configBuffer.find( endKEY, position );

    if( endPos == string::npos  ) { if( debug ) cout << "END KEY not found!!" << endl; return "NotFound"; }

    fromPosition = endPos;

    return configBuffer.substr( initPos, endPos-initPos + endKEY.length()+1 );
}

string TRestMetadata::GetKEYStructure( string keyName, size_t &fromPosition, string buffer )
{
    size_t position = fromPosition;

    if( debug > 1 ) cout << "Buffer : " << buffer << endl;
    if( debug > 1 ) cout << "Start position : " << position << endl;

    string startKEY = "<" + keyName;
    string endKEY = "/" + keyName;

    if( debug > 1 ) cout << "Reduced buffer : " << buffer.substr( position ) << endl;

    size_t initPos = buffer.find( startKEY, position );
    if( debug > 1 ) cout << "initPos : " << initPos << endl;

    if( initPos == string::npos ) { if( debug > 1 ) cout << "KEY not found!!" << endl; return ""; }

    size_t endPos = buffer.find( endKEY, initPos );

    if( debug > 1 ) cout << "End position : " << endPos << endl;

    //TODO Check if a new section starts. If not it might get two complex strings if the KEY_Structure was not closed using /KEY

    if( endPos == string::npos  ) { if( debug > 1 )  cout << "END KEY not found!!" << endl; return ""; }

    fromPosition = endPos;

    return buffer.substr( initPos, endPos-initPos + endKEY.length()+1 );
}

string TRestMetadata::GetSectionByNameFromFile( string nref, string fref )
{
    string fileName = fConfigFilePath + fref;
    if( debug > 1 ) cout << __PRETTY_FUNCTION__ << " Filename : " << fileName << endl;

    ifstream file(fileName);

    if( file == NULL ) { cout << "REST Error : I could not open file : " << fileName << endl; return ""; }

    string temporalBuffer;
    string line;
    while(getline(file, line)) temporalBuffer += line;

    size_t position = 0;
    string sectionString;
    while( ( sectionString = GetKEYStructure( "section", position, temporalBuffer ) ) != "" )
    {

        size_t pos = 0;
        if( FindSection( sectionString, pos ) != -1 ) 
            if ( GetFieldValue( "name", sectionString ) == nref ) {  return sectionString; }
    }

    return "";
}

Int_t TRestMetadata::FindSection( string buffer, size_t startPos )
{

    size_t sectionPos, pos = startPos, pos2 = 0;
    while ( (sectionPos = buffer.find( "<section", pos ) ) != string::npos )
    {
        /** TODO
         *          *
         *                   *  This code can be simplified now by using GetKeyString
         *                            *
         *                                     * */
        if( debug > 1 ) cout << "Start section pos : " << sectionPos << endl;

        pos = buffer.find( " ", sectionPos+1 );

        Int_t initPos = pos;

        pos = buffer.find( " ", pos+1 );

        string sectionName = trim( buffer.substr( initPos, pos-initPos ) );

        pos2 = 0;
        if( (pos2 = sectionName.find( "/>", pos2 )) != string::npos )
            sectionName = sectionName.substr( 0, pos2 );
        pos2 = 0;
        if( (pos2 = sectionName.find( ">", pos2 )) != string::npos )
            sectionName = sectionName.substr( 0, pos2 );

        if ( fSectionName == sectionName )
        {

            size_t tag_end = buffer.find( ">", sectionPos );
            string tmp = buffer.substr( sectionPos, tag_end-sectionPos  );
            TString name = GetFieldValue( "name", tmp );
            TString title = GetFieldValue( "title", tmp );

 //           if( name == "Not defined" ) cout << "Section name " << fSectionName << " not defined!" << endl;
            this->SetName ( name );
            this->SetTitle( title );

            return sectionPos;
        }
    }

    return NOT_FOUND;
}


void TRestMetadata::PrintMetadata()
{
        cout << "TRestMetadata content" << endl;
        cout << "-----------------------" << endl;
        cout << "Config file : " << fConfigFileName << endl;
        cout << "Section name : " << fSectionName << endl;        // section name given in the constructor of TRestSpecificMetadata
}

/* {{{ Trash
//______________________________________________________________________________
string TRestMetadata::SearchString(const char *name)
{
   // Search a datum of type string in a config file using "name" as key, 
   // and returns it as a string object.

   char str[256], str2[256];

   if (fConfigFileName == "") {
      cout << "Warning: " << GetName() <<
          " trying to load string but input file not assigned." << endl;
      return "";
   }

   if ((configFile = fopen(fConfigFileName, "rt")) == NULL) {
      cout << GetName() << ": COULD NOT OPEN " << fConfigFileName << endl;
      return "";
   } else {
      while (!feof(configFile)) {
         fscanf(configFile, "%s %[^\n]", str, str2);
         if (strstr(str, name) != NULL) {
            fclose(configFile);
            return string(str2);
         }
      }
   }

   cout << GetName() << ": " << name << " NOT FOUND in " << fConfigFileName << ", using NULL" << endl;
   fclose(configFile);
   return "";
}

//______________________________________________________________________________
Int_t TRestMetadata::SearchInt(const char *name)
{
   // Search a datum of type integer in a config file using "name" as key, 
   // and returns it as a Int_t object.

   char str[256];
   Int_t var;

   if (fConfigFileName == "") {
      cout << "Warning: " << GetName() <<
          " trying to load string but input file not assigned." << endl;
      return 0;
   }

   if ((configFile = fopen(fConfigFileName, "rt")) == NULL) {
      cout << GetName() << ": COULD NOT OPEN " << fConfigFileName << endl;
      return 0;
   } else {
      while (!feof(configFile)) {
         fscanf(configFile, "%s %i\n", str, &var);
         if (!strcmp(str, name)) {
            fclose(configFile);
            return var;
         }
      }
   }

   cout << GetName() << ": " << name << " NOT FOUND in " << fConfigFileName
       << ", using 0" << endl;
   fclose(configFile);
   return 0;
}

//______________________________________________________________________________
Double_t TRestMetadata::SearchDouble(const char *name)
{
   // Search a datum of type double in a config file using "name" as key, and returns it as a Double_t

   if (fConfigFileName == "") {
      cout << "Warning: " << GetName() <<
          " trying to load string but input file not assigned." << endl;
      return 0.0;
   }

   char str[256];
   double bo = 0.0;

   if ((configFile = fopen(fConfigFileName, "rt")) == NULL) {
      cout << GetName() << "COULD NOT OPEN " << fConfigFileName << endl;
      return 0.0;
   } else {
      while (!feof(configFile)) {
         fscanf(configFile, "%s %lf\n", str, &bo);
         if (!strcmp(str, name)) {
            fclose(configFile);
            return bo;
         }
      }
   }

   cout << GetName() << ": " << name << " NOT FOUND in " << fConfigFileName
       << ", using 0" << endl;
   fclose(configFile);
   return 0.0;
}
}}} */

/*
 *
*/;
