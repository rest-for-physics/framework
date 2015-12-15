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

const int debug = 0;

ClassImp(TRestReadout)
//______________________________________________________________________________
    TRestReadout::TRestReadout()
{
    Initialize();

}

TRestReadout::TRestReadout( char *cfgFileName) : TRestMetadata (cfgFileName)
{
    Initialize();

    LoadConfig( "readout", fConfigFileName );
}

void TRestReadout::Initialize()
{
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
