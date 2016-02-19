///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestReadout.cxx
///
///             Base class for managing run data storage. It contains a TRestEvent and TRestMetadata array. 
///
///             apr 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///             aug 2015    Javier Galan
///_______________________________________________________________________________


#include "TRestReadout.h"
using namespace std;

const int debug = 0;

ClassImp(TRestReadout)
//______________________________________________________________________________
    TRestReadout::TRestReadout()
{
    Initialize();

}

TRestReadout::TRestReadout( const char *cfgFileName) : TRestMetadata (cfgFileName)
{
    Initialize();

    LoadConfigFromFile( fConfigFileName );

    for( int i = 0; i < this->GetNumberOfModules(); i++ )
        this->GetReadoutModule(i)->DoReadoutMapping();
}

void TRestReadout::Initialize()
{
    SetName("readout");
}


//______________________________________________________________________________
TRestReadout::~TRestReadout()
{
    cout << "Deleting TRestReadout" << endl;
}


//______________________________________________________________________________
void TRestReadout::InitFromConfigFile()
{

    size_t position = 0;
    string moduleString;

    while( ( moduleString = GetKEYStructure( "readoutModule", position ) ) != "NotFound" )
    {
        TRestReadoutModule module;

        string moduleDefinition = GetKEYDefinition( "readoutModule", moduleString );
        module.SetModuleID( StringToInteger( GetFieldValue( "id", moduleDefinition ) ) );
        module.SetPlaneIndex( StringToInteger( GetFieldValue( "plane", moduleDefinition ) ) );
        module.SetOrigin( StringTo2DVector( GetFieldValue( "origin", moduleDefinition ) ) );
        module.SetSize( StringTo2DVector( GetFieldValue( "size", moduleDefinition ) ) );
        module.SetRotation( StringToDouble( GetFieldValue( "rotation", moduleDefinition ) ) );

        if( debug )
        {
            cout << "------module-----------------" << endl;
            cout << moduleString << endl;
            cout << "---------------------------" << endl;
            cout << "position : " << position << endl;
            getchar();
        }

        string channelString;
        size_t position2 = 0;
        while( ( channelString = GetKEYStructure( "readoutChannel", position2, moduleString ) ) != "" )
        {

            size_t position3 = 0;
            string channelDefinition = GetKEYDefinition( "readoutChannel", position3, channelString );

            TRestReadoutChannel channel;
            channel.SetID( StringToInteger( GetFieldValue( "id", channelDefinition ) ) );

            if( debug )
            {
                cout << "-----channel-----------------" << endl;
                cout << channelString << endl;
                cout << "---------------------------" << endl;
                getchar();
            }

            string pixelString;
            while( ( pixelString = GetKEYDefinition( "addPixel", position3, channelString ) ) != "" )
            {
                TRestReadoutPixel pixel;

                pixel.SetID( StringToInteger( GetFieldValue( "id", pixelString ) ) );
                pixel.SetOrigin( StringTo2DVector( GetFieldValue( "origin", pixelString ) ) );
                pixel.SetSize( StringTo2DVector( GetFieldValue( "size", pixelString ) ) );
                pixel.SetRotation( StringToDouble( GetFieldValue( "rotation", pixelString ) ) );

 //               cout << "Rotation : " << StringToDouble( GetFieldValue( "rotation", pixelString ) ) << endl;
 //               cout << "Rotation : " << pixel.GetRotation() << endl;

                channel.AddPixel( pixel );

                if(  debug )
                {
                    cout << "pixel" << endl;
                    cout << pixelString << endl;
                }
            }

            module.AddChannel( channel );

            position2++;

        }

        this->AddModule( module );


        position++;
    }

        this->PrintReadout();
}

Int_t TRestReadout::GetNumberOfChannels( ) 
{
    Int_t nChannels = 0;
    for( int md = 0; md < GetNumberOfModules(); md++ )
        nChannels += GetReadoutModule(md)->GetNumberOfChannels();
    return nChannels;
}

TRestReadoutModule *TRestReadout::GetModuleByID( Int_t modID )
{

    for( int md = 0; md < GetNumberOfModules(); md++ )
        if( this->GetModule( md )->GetModuleID() == modID )
            return this->GetModule( md );

    cout << "REST ERROR (GetReadoutModuleByID) : Module ID : " << modID << " was not found" << endl;
    return NULL;
}

TRestReadoutChannel *TRestReadout::GetChannelByID( Int_t modID, Int_t chID )
{
    TRestReadoutModule *module = GetModuleByID( modID );

    for( int ch = 0; ch < module->GetNumberOfChannels(); ch++ )
        if( module->GetChannel( ch )->GetID() == chID )
            return module->GetChannel( ch );

    cout << "REST ERROR (GetReadoutChannel) : Channel ID " << chID << " Module ID : " << modID << " was not found" << endl;
    return NULL;
}

Double_t TRestReadout::GetX( Int_t modID, Int_t chID )
{
    TRestReadoutModule *rModule = GetModuleByID( modID );
    Double_t xOrigin = rModule->GetModuleOriginX();

    TRestReadoutChannel *rChannel = GetChannelByID( modID, chID );

    Double_t x = numeric_limits<Double_t>::quiet_NaN();

    if( rChannel->GetNumberOfPixels() == 1 )
    {
         Double_t sX = rChannel->GetPixel(0)->GetSize().X();
         Double_t sY = rChannel->GetPixel(0)->GetSize().Y();

         if ( sX > 2 * sY ) return x;

         x = xOrigin + rChannel->GetPixel(0)->GetCenter().X();

         return x;
    }

    if( rChannel->GetNumberOfPixels() > 1 )
    {

        Double_t x1 = rChannel->GetPixel(0)->GetCenter().X();
        Double_t x2 = rChannel->GetPixel(1)->GetCenter().X();

        Double_t y1 = rChannel->GetPixel(0)->GetCenter().Y();
        Double_t y2 = rChannel->GetPixel(1)->GetCenter().Y();

        Double_t deltaX, deltaY;

        if( x2 - x1 > 0 ) deltaX = x2 - x1;
        else deltaX = x1 - x2;

        if( y2 - y1 > 0 ) deltaY = y2 - y1;
        else deltaY = y1 - y2;
        
        if( deltaY > deltaX ) x = xOrigin + rChannel->GetPixel(0)->GetCenter().X();
    }

    return x;
}

Double_t TRestReadout::GetY( Int_t modID, Int_t chID )
{
    TRestReadoutModule *rModule = GetModuleByID( modID );
    Double_t yOrigin = rModule->GetModuleOriginY();

    TRestReadoutChannel *rChannel = GetChannelByID( modID, chID );

    Double_t y = numeric_limits<Double_t>::quiet_NaN();

    if( rChannel->GetNumberOfPixels() == 1 )
    {
         Double_t sX = rChannel->GetPixel(0)->GetSize().X();
         Double_t sY = rChannel->GetPixel(0)->GetSize().Y();

         if ( sY > 2 * sX ) return y;

         y = yOrigin + rChannel->GetPixel(0)->GetCenter().Y();

         return y;
    }

    if( rChannel->GetNumberOfPixels() > 1 )
    {

        Double_t x1 = rChannel->GetPixel(0)->GetCenter().X();
        Double_t x2 = rChannel->GetPixel(1)->GetCenter().X();

        Double_t y1 = rChannel->GetPixel(0)->GetCenter().Y();
        Double_t y2 = rChannel->GetPixel(1)->GetCenter().Y();

        Double_t deltaX, deltaY;

        if( x2 - x1 > 0 ) deltaX = x2 - x1;
        else deltaX = x1 - x2;

        if( y2 - y1 > 0 ) deltaY = y2 - y1;
        else deltaY = y1 - y2;
        
        if( deltaY < deltaX ) y = yOrigin + rChannel->GetPixel(0)->GetCenter().Y();
    }


    return y;
}

void TRestReadout::PrintReadout( )
{
    cout << endl;
    cout << "====================================" << endl;
    cout << "Readout : " << GetTitle() << endl;
    cout << "====================================" << endl;
    cout << endl;
    for( int i = 0; i < GetNumberOfModules(); i++ )
    {
        this->GetReadoutModule(i)->PrintReadoutModule();
        cout << "==============================" << endl;
        cout << endl;
    }


}

void TRestReadout::Draw()
{
    /*
    for( int modID = 0; modID < GetNumberOfModules(); modID++ )
    {
        TRestReadoutModule md = GetReadoutModule( modID );

        Double_t x[5], y[5];
        x[0] = md.GetModuleOriginX();
        y[0] = md.GetModuleOriginY();

        x[1] = x[0] + md.GetModuleSizeX();
        y[1] = y[0];

        x[2] = x[1];
        y[2] = y[1] + md.GetModuleSizeY();

        x[3] = x[2] - md.GetModuleSizeX();
        y[3] = y[2];

        x[4] = x[3];
        y[4] = y[3] - md.GetModuleSizeY();


        TGraph modGraph( 5, x, y );

        drawingElement.push_back( modGraph );
    }

    for( unsigned int i = 0; i < drawingElement.size(); i++ )
    {
        drawingElement[i].Draw("same");
    }
    */
}


TH2Poly *TRestReadout::GetReadoutHistogram( ){

Double_t x[4];
Double_t y[4];

double xmin,xmax,ymin,ymax;

GetBoundaries(xmin,xmax,ymin,ymax);

TH2Poly *readoutHistogram = new TH2Poly("ReadoutHistogram","ReadoutHistogram",xmin,xmax,ymin,ymax);

for( int mdID = 0; mdID < this->GetNumberOfModules( ); mdID++ )
   {
   TRestReadoutModule *module = this->GetReadoutModule( mdID );
   
   int nChannels = module->GetNumberOfChannels();
   
        for( int ch = 0; ch < nChannels; ch++ )
        {
        TRestReadoutChannel *channel = module->GetChannel( ch );
        Int_t nPixels = channel->GetNumberOfPixels();
           
           for( int px = 0; px < nPixels; px++ )
           {
             for( int v = 0; v < 4; v++ )
                {
                    x[v] = module->GetPixelVertex( ch, px, v ).X();
                    y[v] = module->GetPixelVertex( ch, px, v ).Y();
                    //cout<<x[v]<<" "<<y[v]<<" ";
                }
           
           //cout<<endl;
           
           readoutHistogram->AddBin(4,x,y);
           }
        
        }
   }


return readoutHistogram;

}

void TRestReadout::GetBoundaries(double &xmin,double &xmax,double &ymin,double &ymax){

Double_t x[4];
Double_t y[4];

xmin=1E9,xmax=-1E9,ymin=1E9,ymax=-1E9;

for( int mdID = 0; mdID < this->GetNumberOfModules( ); mdID++ )
    {
        TRestReadoutModule *module = this->GetReadoutModule( mdID );
        
        for( int v = 0; v < 4; v++ )
        {
            x[v] = module->GetVertex( v ).X();
            y[v] = module->GetVertex( v ).Y();
            
            if(x[v]<xmin)xmin=x[v];
            if(y[v]<ymin)ymin=y[v];
            if(x[v]>xmax)xmax=x[v];
            if(y[v]>ymax)ymax=y[v];
            
        }
     }


}
