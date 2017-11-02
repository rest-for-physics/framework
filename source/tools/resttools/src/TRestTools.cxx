
#include "TRestTools.h"

#include <iostream>
#include <limits> 
using namespace std;

#include <dirent.h>

ClassImp(TRestTools)


std::vector <TString> TRestTools::GetListOfRESTLibraries(  )
{
    vector <TString> libraryList;

    vector <TString> libraryPathList;

    libraryPathList = GetListOfPathsInEnvVariable( "LD_LIBRARY_PATH" );

    for( unsigned int n = 0; n < libraryPathList.size(); n++ )
    {
        vector <TString> list = GetRESTLibrariesInDirectory( libraryPathList[n] );
        for( unsigned int i = 0; i < list.size(); i++ )
            libraryList.push_back( list[i] );
    }

    return libraryList;
}


std::vector <TString> TRestTools::GetListOfPathsInEnvVariable( TString envVariable  )
{
    vector <TString> pathList;

    TString p ( getenv( envVariable.Data() ) );

    while( p.Length() > 0 )
    {
        TString path = GetFirstPath( p );

        if ( path.Length() > 0 ) pathList.push_back( path );
    }

    return pathList;
}


std::vector <TString> TRestTools::GetOptions(TString optionsStr)
{
    vector <TString> optionsList;

    TString p ( optionsStr );

    while( p.Length() > 0 )
    {
        TString option = GetFirstOption( p );

        if ( option.Length() > 0 ) optionsList.push_back( option );
    }

    return optionsList;
}


TString TRestTools::GetFirstOption( TString &path )
{
    return GetFirstPath( path );
}


TString TRestTools::GetFirstPath( TString &path )
{
    TString resultPath;

    if ( path.First( ":" ) >= 0 )
    {
        resultPath = path( 0, path.First(":") );

        path = path( path.First(":")+1, path.Length() );
    }
    else
    {
        resultPath = path;
        path = "";
    }

    return resultPath;
}


std::vector <TString> TRestTools::GetRESTLibrariesInDirectory( TString path )
{
    vector <TString> fileList;
    DIR *dir;
    struct dirent *ent;
    if ( (dir = opendir ( path.Data() )) != NULL ) 
    {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) 
        {
            TString fName ( ent->d_name );
            if( (fName.Contains( "REST" ) || fName.Contains( "Rest" ))&&fName.Contains(".so") )
                fileList.push_back( fName );
        }
        closedir (dir);
    } 
    else 
    {
        /* could not open directory */
        perror ("");
    }

    return fileList;
}




