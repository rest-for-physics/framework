/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/


//////////////////////////////////////////////////////////////////////////
///
/// This is the main metadata readout class of REST. The readout class 
/// defines the methods allowing to stablish a relation between the hits
/// inside the TPC and the signals read out by the electronics daq.
///
/// A full readout description is composed by at least one readout plane,
/// (TRestReadoutPlane), where we can place any number of readout modules
/// (TRestReadoutModule). A readout module is composed by readout channels
/// (TRestReadoutChannel) which describe the basic active detection area,
/// which can take any complex shape by combinning primitive geometry 
/// elements (TRestReadoutPixel).
///
/// REST processes such as TRestSignalToHitsProcess and 
/// TRestHitsToSignalProcess use the TRestReadout class to transform the
/// spatial coordinates into raw signal data, and viceversa.
///
/// ### Constructing the readout through an RML file
/// 
/// The readout definition must be initialized through a RML file. The 
/// basic metadata structure of a readout follows this scheme
///
/// \code
/// <section TRestReadout ... >
///
///     <parameter name="mappingNodes" value="100" />
///
///     // First, we need to construct the different readout modules we will
///     // use inside the readout plane definition.
///     <readoutModule name="modName" size="(100,100)" tolerance="1.e-4" >
///
///         // Later, when we construct the readout plane we will use "modName" 
///         // to add the module to the readout plane definition.
///
///         <readoutChannel id="0">
///             <addPixel id="0" origin="(0,0)" size="(10,10)" rotation="45" />
///             <addPixel id="1" origin="(10,10)" size="(10,10)" rotation="45" />
///             ...
///             // Add as many pixel definitions as needed to create the channel definition
///             ...
///         </readoutChannel>
///
///         // And add as many readout channels as need to construct your readout module
///         <readoutChannel id="1">
///             ...
///             ...
///         </readoutChannel>
///
///         // The id number given to the readout channel is the physical readout
///         // channel used inside the decoding.
///         <readoutChannel id="n">
///             ...
///             ...
///         </readoutChannel>
///       
///     </readoutModule>
///
///     // If your readout is composed by modules with different size or channel topologies,
///     // you will need add additional <readoutModule ...> definitions here.
/// 
///     // The readout plane allows to define the position, the orientation (planeVector),
///     // and the position of the cathode that will define the detector active volume.
///     <readoutPlane position="(0,0,-990)" units="mm" 
///                   planeVector="(0,0,1)"  chargeCollection="1" 
///                   cathodePosition="(0,0,0)" units="mm" >
///
///            // We can then insert the modules inside each readout plane
///            <addReadoutModule name="modName" // We use the previously defined readout module
///                              origin="(0,0)" // Position of the left-bottom corner (rotation point).
///                              rotation="0"   // Rotation in degrees around position.
///                              decodingFile="module.dec"  // A file describing the decoding
///                              firstDaqChannel="0" >  // Offset introduced to the daq signal channel inside the decoding file.
///
///            <addReadoutModule name="modName"
///                              origin="(0,0)" rotation="0"
///                              decodingFile="module.dec" firstDaqChannel="272"  >
///
///           ...
///
///           // Add as many modules as needed to construct your readout.
///     </readoutPlane>
///
///     // If needed you may add any additional readout layers to your definition adding new
///     // readout plane <readoutPlane ...> definitions here.
/// 
///
/// </section>
/// \endcode
///
///
/// The *mappingNodes* parameter allows to specify the size of the virtual grid
/// that will be used to map the readout. The mapping allows to speed up the
/// process finding a pixel inside a module for a given x,y coordinates. 
/// In general, the number of mapping nodes should be high enough so that every
/// pixel from any readout channel is associated to, or contains, a node in the grid.
/// However, as higher is the number of nodes in the mapping grid, higher will 
/// be the required computation time to find a pixel for a given x,y coordinates.
/// If this value is not defined REST will try to find an optimum value for this
/// parameter. Therefore, it is recommended to do not specify this parameter, 
/// except for solving readout problems or optimization purposes. 
///
/// ### The decoding
///
/// The relation between the channel number imposed by the electronic acquisition
/// and the readout channel id defined inside *readoutChannel* is given to 
/// TRestReadout through a decoding file.
///
/// The *decodingFile* parameter inside each module added to the readout plane 
/// allows to set the filename (in the code above, *module.dec*) of a file
/// used to define the relation between the physical readout *id* defined 
/// at each readout channel, as <code> <readoutChannel id="n"> </code>
/// and the daq channel number defined at the acquisition system. 
/// If *no decoding* file is defined the relation between daq and readout
/// channel is assigned *one to one*.
/// The decoding file must be a text file definning two columns with as
/// many columns as the number of channels defined in the readout module.
/// The first column is the daq channel number, and the second column is 
/// the readout channel defined in the RML file.
///
/// An example of contents of a decoding file. Here 0, 1, 2, 3, ... are 
/// the physical readout channels corresponding to the daq channels 67, 
/// 65, 63, 61, ...
///
/// \code
/// 67      0
/// 65      1
/// 63      2
/// 61      3
/// 59      4
/// 57      5
/// 54      6
/// ...
/// ...
/// \endcode
/// 
/// The channel number defined on the left column will be shifted by the 
/// value *firstDaqChannel* defined when adding the readout module to the 
/// readout plane. This may allow to re-use a decoding file for different
/// readout modules in case we have a repetitive connection pattern.
///
/// ### Using the readout class
///
/// Once TRestReadout has been initialized, through and RML file or a
/// previously stored TRestReadout class stored in a ROOT file,
/// we can find the corresponding xy-position for a given readout channel 
/// id, module id, and readout plane, or the corresponding coordinates for
/// a given channel inside a module containned in a readout plane.
///
/// The following code shows how we can identify the readout plane 
/// (*planeId*), the readout module (*moduleId*), and the readout channel
/// (*chanId*) for a given x,y,z coordinates.
///
/// \code
///        for( int p = 0; p < fReadout->GetNumberOfReadoutPlanes(); p++ )
///        {
///            // We check if the xyz coordinates are in the volume defined by the readout plane
///            modId = fReadout->GetReadoutPlane(p)->isInsideDriftVolume( x, y, z );
///
///            // If the hit is containned in the readout, the last function returns the id of the
///            // readout module that contains the x,y coordinates.
///            if( modId >= 0 )
///            {
///                // We found the hit at planeId
///                planeId = p;
///
///                // Then we obtain the readoutChannel *id* as defined in the readout.
///                Int_t chanId = fReadout->GetReadoutPlane( p )->FindChannel( modId, x, y );
///
///                break;
///            }
///        }
/// \endcode
///
/// Once we found the readout channel, we can obtain the associated daq
/// channel number, that was defined using the decoding file.
/// 
/// \code
///            Int_t daqId = fReadout->GetReadoutPlane( planeId )
///                                  ->GetModule( modId )
///                                  ->GetChannel( chanId )->GetDaqID( );
/// \endcode
///
/// The other way around we can obtain the corresponding x and y coordinates for a given daq
/// channel id. The following code shows the basic idea to find *signalId*.
/// 
/// \code
///        for( int p = 0; p < fReadout->GetNumberOfReadoutPlanes(); p++ )
///        {
///            TRestReadoutPlane *plane = fReadout->GetReadoutPlane( p );
///            for( int m = 0; m < plane->GetNumberOfModules(); m++ )
///            {
///                TRestReadoutModule *mod = plane->GetModule( m );
///
///                // We iterate over all readout modules searching for the one that contains our signal id
///                if( mod->isDaqIDInside( signalID ) )
///                {
///                    // If we find it we use the readoutModule id, and the signalId corresponding
///                    // to the physical readout channel and obtain the x,y coordinates.
///
///                    readoutChannelID = mod->DaqToReadoutChannel( signalID );
///                    readoutModuleID = mod->GetModuleID();
///                    
///                    x = plane->GetX( readoutModuleID, readoutChannelID );
///                    y = plane->GetY( readoutModuleID, readoutChannelID );
///                    break;
///               }
///            }
///         }
/// \endcode
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2015-aug:  First concept.
///            Javier Galan
///
/// \class      TRestReadout
/// \author     Javier Galan
///
/// <hr>
///
///

#include "TRestReadout.h"
using namespace std;

const int debug = 0;

ClassImp(TRestReadout)
///////////////////////////////////////////////
/// \brief TRestReadout default constructor
///
TRestReadout::TRestReadout()
{
    Initialize();

}

///////////////////////////////////////////////
/// \brief TRestReadout constructor loading data from a config file
/// 
/// If no configuration path was defined using TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or relative.
///
/// By default the config file must be specified with full path, absolute or relative.
///
/// First TRestReadout section occurrence will be loaded.
///
/// \param cfgFileName A const char* giving the path to an RML file.
///
TRestReadout::TRestReadout( const char *cfgFileName) : TRestMetadata (cfgFileName)
{
    cout << "Loading readout. This might take few seconds" << endl;
    Initialize();

    LoadConfigFromFile( fConfigFileName );
}


///////////////////////////////////////////////
/// \brief TRestReadout constructor loading data from a config file
/// 
/// If no configuration path was defined using TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or relative.
///
/// By default the config file must be specified with full path, absolute or relative.
///
/// \param cfgFileName A const char* giving the path to an RML file.
/// \param name The name of the TRestReadout section to be loaded
///
TRestReadout::TRestReadout( const char *cfgFileName, string name) : TRestMetadata (cfgFileName)
{
    cout << "Loading readout. This might take few seconds" << endl;
    Initialize();

    LoadConfigFromFile( fConfigFileName, name );
}

///////////////////////////////////////////////
/// \brief Initializes the readout members and defines the section name
/// 
void TRestReadout::Initialize()
{
    SetSectionName( this->ClassName() );

    fDecoding = false;

    fNReadoutPlanes = 0;
    fReadoutPlanes.clear();
}

///////////////////////////////////////////////
/// \brief TRestReadout default destructor
///
TRestReadout::~TRestReadout()
{
}

///////////////////////////////////////////////
/// \brief Returns the number of readout planes defined on the readout
///
Int_t TRestReadout::GetNumberOfReadoutPlanes( ) 
{ 
    return fReadoutPlanes.size(); 
}

///////////////////////////////////////////////
/// \brief Returns the **total** number of modules implemented in **all**
/// the readout planes.
///
Int_t TRestReadout::GetNumberOfModules( )
{
    Int_t modules = 0;
    for( int p = 0; p < GetNumberOfReadoutPlanes( ); p++ )
        modules += GetReadoutPlane( p )->GetNumberOfModules( );
    return modules;
}

///////////////////////////////////////////////
/// \brief Returns the **total** number of channels implemented in **all**
/// the readout planes and modules.
///
Int_t TRestReadout::GetNumberOfChannels( )
{
    Int_t channels = 0;
    for( int p = 0; p < GetNumberOfReadoutPlanes( ); p++ )
        for( int m = 0; m < GetReadoutPlane( p )->GetNumberOfModules( ); m++ )
            channels += GetReadoutPlane( p )->GetModule( m )->GetNumberOfChannels( );
    return channels;
}


///////////////////////////////////////////////
/// \brief Returns the *id* of the readout module with a given *name*.
///
Int_t TRestReadout::GetModuleDefinitionId( TString name )
{
    for( unsigned int i = 0; i < fModuleDefinitions.size(); i++ )
        if( fModuleDefinitions[i].GetName() == name )
            return i;
    return -1;
}

///////////////////////////////////////////////
/// \brief Returns a pointer to the readout plane by index
///
TRestReadoutPlane *TRestReadout::GetReadoutPlane( int p ) 
{ 
    if( p < fNReadoutPlanes) 
        return &fReadoutPlanes[p]; 
    else
    {
       cout << "REST WARNING. TRestReadout::GetReadoutPlane." << endl;
       cout << "Readout plane index exceeded." << endl;
       cout << "Index requested : " << p << endl; 
       cout << "Number of readout planes defined : " << fNReadoutPlanes << endl;
    }

    return NULL;
}

///////////////////////////////////////////////
/// \brief Adds a readout plane to the readout
///
void TRestReadout::AddReadoutPlane( TRestReadoutPlane plane )
{ 
    fReadoutPlanes.push_back( plane );
    fNReadoutPlanes++; 
}

///////////////////////////////////////////////
/// \brief Initializes the readout members using the information given in
/// the TRestReadout RML section.
///
void TRestReadout::InitFromConfigFile()
{

    size_t position = 0;
    string planeString;

    fMappingNodes = StringToInteger( GetParameter( "mappingNodes", "0" ) );

    string moduleString;
    size_t posSection = 0;
    while( ( moduleString = GetKEYStructure( "readoutModule", posSection ) ) != "NotFound" )
    {
        TRestReadoutModule module;
        if( GetVerboseLevel() >= REST_Essential ) module.EnableWarnings();

        string moduleDefinition = GetKEYDefinition( "readoutModule", moduleString );

        module.SetName( GetFieldValue( "name", moduleDefinition ) );
        module.SetSize( StringTo2DVector( GetFieldValue( "size", moduleDefinition ) ) );
        module.SetTolerance( StringToDouble( GetFieldValue( "tolerance", moduleDefinition ) ) );

        if(GetVerboseLevel() >= REST_Debug)
        {
            cout << "------module-----------------" << endl;
            cout << moduleString << endl;
            cout << "---------------------------" << endl;
			cout << "name : " << module.GetName() << endl;
			cout << "position : " << posSection << endl;
			cout << "size : " << module.GetModuleSizeX()<<","<<module.GetModuleSizeY() << endl;
			cout << "tolotance : " << module.GetTolerance() << endl;
            GetChar();
        }

        string channelString;
        size_t position2 = 0;
        while( ( channelString = GetKEYStructure( "readoutChannel", position2, moduleString ) ) != "" )
        {
            size_t position3 = 0;
            string channelDefinition = GetKEYDefinition( "readoutChannel", position3, channelString );

            TRestReadoutChannel channel;

            Int_t id = StringToInteger( GetFieldValue( "id", channelDefinition ) );
            channel.SetID( id );
            channel.SetDaqID( -1 );

            string pixelString;
            while( ( pixelString = GetKEYDefinition( "addPixel", position3, channelString ) ) != "" )
            {
                TRestReadoutPixel pixel;

                pixel.SetID( StringToInteger( GetFieldValue( "id", pixelString ) ) );
                pixel.SetOrigin( StringTo2DVector( GetFieldValue( "origin", pixelString ) ) );
                pixel.SetSize( StringTo2DVector( GetFieldValue( "size", pixelString ) ) );
                pixel.SetRotation( StringToDouble( GetFieldValue( "rotation", pixelString ) ) );

                channel.AddPixel( pixel );
            }

            module.AddChannel( channel );

            position2++;
        }

        module.DoReadoutMapping( fMappingNodes );

        fModuleDefinitions.push_back( module );

        posSection++;
    }

    Int_t addedChannels = 0;
    while( ( planeString = GetKEYStructure( "readoutPlane", position ) ) != "NotFound" )
    {
        TRestReadoutPlane plane;

        string planeDefinition = GetKEYDefinition( "readoutPlane", planeString );
        plane.SetID( GetNumberOfReadoutPlanes() );
        plane.SetPosition( Get3DVectorFieldValueWithUnits( "position", planeDefinition ) );
        plane.SetCathodePosition( Get3DVectorFieldValueWithUnits( "cathodePosition", planeDefinition ) );
        plane.SetPlaneVector( StringTo3DVector( GetFieldValue( "planeVector", planeDefinition ) ) );
        plane.SetChargeCollection( StringToDouble( GetFieldValue( "chargeCollection", planeDefinition ) ) );

        Double_t tDriftDistance = plane.GetDistanceTo( plane.GetCathodePosition() );
        plane.SetTotalDriftDistance( tDriftDistance );

        string moduleDefinition;
        size_t posPlane = 0;
        while( ( moduleDefinition = GetKEYDefinition( "addReadoutModule", posPlane, planeString ) ) != "" )
        {
            TString modName = GetFieldValue( "name", moduleDefinition );
            Int_t mid = GetModuleDefinitionId( modName );

			if (mid == -1) 
			{
				warning << "Readout module with name \"" << modName << "\" not found! Skipping this definition..." << endl;
				warning << "In addReadoutModule definition: " << moduleDefinition << endl;
				warning << endl;
				continue;
			}

            fModuleDefinitions[mid].SetModuleID( StringToInteger( GetFieldValue( "id", moduleDefinition ) ) );
            fModuleDefinitions[mid].SetOrigin( StringTo2DVector( GetFieldValue( "origin", moduleDefinition ) ) );
            fModuleDefinitions[mid].SetRotation( StringToDouble( GetFieldValue( "rotation", moduleDefinition ) ) );

            Int_t firstDaqChannel = StringToInteger( GetFieldValue( "firstDaqChannel", moduleDefinition ) );
            if( firstDaqChannel == -1 ) firstDaqChannel = addedChannels;

            string decodingFile = GetFieldValue( "decodingFile", moduleDefinition );
            if( decodingFile == "Not defined" || decodingFile == "" ) fDecoding = false;
            else fDecoding = true;

            if( fDecoding && !fileExists( decodingFile.c_str() ) )
            { 
                cout << "REST ERROR : The decoding file does not exist" << endl;
                cout << "--------------------------------" << endl;
                cout << "File : " << decodingFile << endl;
                cout << "Default decoding will be used. readoutChannel=daqChannel" << endl;
                cout << "To avoid this message and use the default decoding define : decodingFile=\"\"" << endl;
                cout << "--------------------------------" << endl;
                cout << "Press a KEY to continue..." << endl;
                getchar();
                fDecoding = false;
            }

            vector <Int_t> rChannel;
            vector <Int_t> dChannel;
            if( fDecoding && fileExists( decodingFile.c_str() ) )
            {
                FILE *f = fopen( decodingFile.c_str(), "r" );

                Int_t daq, readout;
                while( !feof( f ) )
                {
                    if( fscanf(f,"%d\t%d\n", &daq, &readout ) <= 0 )
                    {
                        cout << "REST Error!!. TRestReadout::InitFromConfigFile. Contact rest-dev@cern.ch" << endl;
                        exit(-1);
                    }
					//we skip blank daq channels if readout id is <0
					//e.g. daq id: 22, readout id: -1
					if (readout >=0) {
						rChannel.push_back(readout);
						dChannel.push_back(daq + firstDaqChannel);
					}

                }
                fclose(f);
            }

            if(GetVerboseLevel() >= REST_Debug)
            {
                cout << "------module-----------------" << endl;
                cout << moduleDefinition << endl;
                cout << "---------------------------" << endl;
                cout << "position : " << posPlane << endl;
                getchar();
            }

            if( fDecoding && (unsigned int) fModuleDefinitions[mid].GetNumberOfChannels() != rChannel.size() )
            {
                cout << "REST ERROR : TRestReadout."
                    << " The number of channels defined in the readout is not the same" 
                    << " as the number of channels found in the decoding." << endl;
                exit(1);
            }

            for( int ch = 0; ch < fModuleDefinitions[mid].GetNumberOfChannels(); ch++ )
            {
                if( !fDecoding )
                {
                    Int_t id = fModuleDefinitions[mid].GetChannel(ch)->GetID( );
                    rChannel.push_back( id );
                    dChannel.push_back( id + firstDaqChannel );
                }

                // WRONG version before --> fModuleDefinitions[mid].GetChannel(ch)->SetID( rChannel[ch] );
                fModuleDefinitions[mid].GetChannelByID( rChannel[ch] )->SetDaqID( dChannel[ch] );

                addedChannels++;

            }
            fModuleDefinitions[mid].SetMinMaxDaqIDs();
            plane.AddModule( fModuleDefinitions[mid] );

            posPlane++;
        }

        this->AddReadoutPlane( plane );
		if (fVerboseLevel >= REST_Info) {
			cout << "Drawing readoutplane " << plane.GetID() << endl;
			plane.Draw();
		}


        position++;
    }

    ValidateReadout( );
}

///////////////////////////////////////////////
/// \brief This method is not implemented yet. But it could 
/// do some checks to help verifying the readout.
///
void TRestReadout::ValidateReadout( )
{
    cout << "--------------------------------------------------" << endl;
    cout << "TRestReadout::ValidateReadout:: NOT IMPLEMENTED" << endl;
    cout << "This function should crosscheck that there are no repeated DaqChannels IDs" << endl;
    cout << "If any checks are implemented in the future. Those checks should be an option." << endl;
    cout << "No dead area in the readout module" << endl;
    cout << "And other checks" << endl;
    cout << "--------------------------------------------------" << endl;
}


///////////////////////////////////////////////
/// \brief It returns the x-coordinate for the given readout 
/// plane, *plane*, a given module, *modID*, and a given
/// channel, *chID*.
///
Double_t TRestReadout::GetX( Int_t plane, Int_t modID, Int_t chID )
{
    return GetReadoutPlane( plane )->GetX( modID, chID );
}

///////////////////////////////////////////////
/// \brief It returns the y-coordinate for the given readout 
/// plane, *plane*, a given module, *modID*, and a given
/// channel, *chID*.
///
Double_t TRestReadout::GetY( Int_t plane, Int_t modID, Int_t chID )
{
    return GetReadoutPlane( plane )->GetY( modID, chID );
}

///////////////////////////////////////////////
/// \brief Prints on screen the details about the readout
/// definition.
///
/// \param fullDetail Prints all modules, channels and pixels info.
///
void TRestReadout::PrintMetadata( Int_t fullDetail )
{
    cout << endl;
    cout << "====================================" << endl;
    cout << "Readout : " << GetTitle() << endl;
    cout << "Number of readout planes : " << fNReadoutPlanes << endl;
    cout << "Decoding was defined : ";
    if( fDecoding ) cout << "YES" << endl; else cout << "NO" << endl;
    cout << "====================================" << endl;
    cout << endl;
    for( int p = 0; p < GetNumberOfReadoutPlanes(); p++ )
        this->GetReadoutPlane( p )->Print( fullDetail );
    cout << "====================================" << endl;

}

///////////////////////////////////////////////
/// \brief Draws the readout on screen. Not yet implemented. 
///
void TRestReadout::Draw()
{
    cout << " TRestReadout::Draw() is not implemented" << endl;
    cout << " To draw a TRestReadout class with name \"readoutName\"";
    cout << " stored in a ROOT file \"rootFile.root\"" << endl;
    cout << " You can use the script : REST_Readout_Viewer( \"rootFile.root\", \"readoutName\" )" << endl;
    cout << endl;
    cout << " Or you can access directly a readout plane and draw using : " << endl;
    cout << " readout->GetReadoutPlane( 0 )->Draw( ); " << endl;
}

