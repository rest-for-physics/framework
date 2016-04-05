///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestReadoutModule.cxx
///
///             Base class for managing run data storage. It contains a TRestEvent and TRestMetadata array. 
///
///             apr 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///             aug 2015    Javier Galan
///_______________________________________________________________________________


#include "TRestReadoutModule.h"
using namespace std;

ClassImp(TRestReadoutModule)
//______________________________________________________________________________
    TRestReadoutModule::TRestReadoutModule()
{
    Initialize();

}


//______________________________________________________________________________
TRestReadoutModule::~TRestReadoutModule()
{
 //   cout << "Deleting TRestReadoutModule" << endl;
}


void TRestReadoutModule::Initialize()
{
    fReadoutChannel.clear();
    fModuleID = -1;
    
    fModuleOriginX = 0;
    fModuleOriginY = 0;

    fModuleSizeX = 0;
    fModuleSizeY = 0;

    fModuleRotation = 0;
}

void TRestReadoutModule::DoReadoutMapping( )
{
    ///////////////////////////////////////////////////////////////////////////////
    // We initialize the mapping readout net to sqrt(numberOfPixels)
    // However this might not be good for readouts where the pixels are assymmetric
    // /////////////////////////////////////////////////////////////////////////////
    Int_t totalNumberOfPixels = 0;
    for( int ch = 0; ch < this->GetNumberOfChannels( ); ch++ )
        totalNumberOfPixels += GetChannel(ch)->GetNumberOfPixels();

    Int_t nodes = TMath::Sqrt( totalNumberOfPixels );
    nodes = 2*nodes;
    cout << "Performing readout mapping optimization (This might take few seconds)" << endl;
    cout << "---------------------------------------------------------------------" << endl;
    cout << "Total number of pixels : " << totalNumberOfPixels << endl;
    cout << "Nodes : " << nodes << endl;

    fMapping.Initialize( nodes, nodes, GetModuleSizeX(), GetModuleSizeY() );

    for ( int ch = 0; ch < this->GetNumberOfChannels(); ch++ )
    {
        for ( int px = 0; px < this->GetChannel(ch)->GetNumberOfPixels(); px++ )
        {
            Double_t xPix = this->GetChannel(ch)->GetPixel(px)->GetCenter().X();
            Double_t yPix = this->GetChannel(ch)->GetPixel(px)->GetCenter().Y();

            Int_t nodeX = fMapping.GetNodeX( xPix );
            Int_t nodeY = fMapping.GetNodeY( yPix );

            if( fMapping.isNodeSet( nodeX, nodeY ) ) { cout << "ERROR. Node is already SET!!" << endl; getchar(); }
            fMapping.SetNode( nodeX, nodeY, ch, px );
        }
    }

    for( int i = 0; i < nodes; i++ )
        for( int j = 0; j < nodes; j++ )
        {
            Double_t x = fMapping.GetX( i );
            Double_t y = fMapping.GetY( j );
            Double_t xAbs = TransformToPhysicalCoordinates( x, y ).X();
            Double_t yAbs = TransformToPhysicalCoordinates( x, y ).Y();

            if( !fMapping.isNodeSet( i, j ) )
            {
                for( int ch = 0; ch < GetNumberOfChannels( ) && !fMapping.isNodeSet( i, j ); ch++ )
                {
                    for( int px = 0; px < GetChannel(ch)->GetNumberOfPixels( ) && !fMapping.isNodeSet(i, j); px++ )
                    {
                        if( isInsidePixel( ch, px, xAbs , yAbs ) )
                        {
                            fMapping.SetNode( i, j, ch,  px );
                        }
                    }
                }
            }
        }

    if( !fMapping.AllNodesSet( ) ) cout << "Not all nodes set" << endl;
    else cout << "All Nodes set" << endl;

    for( int i = 0; i < nodes; i++ )
        for( int j = 0; j < nodes; j++ )
        {
            if( !fMapping.isNodeSet( i, j ) )
            {
                Double_t x = fMapping.GetX(i);
                Double_t y = fMapping.GetY(j);
                Double_t xAbs = TransformToPhysicalCoordinates( x, y ).X();
                Double_t yAbs = TransformToPhysicalCoordinates( x, y ).Y();
                cout << "Node NOT SET : " << i << " , " << j << " Mapping x : " << x << " y : " << y << endl;

                for( int ch = 0; ch < GetNumberOfChannels( ); ch++ )
                {
                    for( int px = 0; px < GetChannel(ch)->GetNumberOfPixels( ); px++ )
                    {
                        if( isInsidePixel( ch, px, xAbs, yAbs ) )
                        {
                            cout << "X : " << xAbs << " , " << yAbs << " Is inside channel : " << ch << " pixel : " << px << endl;
                        }
                    }
                }
            }
        }

    cout << "Nodes not set : " << fMapping.GetNumberOfNodesNotSet( ) << endl;

}

Int_t TRestReadoutModule::FindChannel( Double_t absX, Double_t absY )
{

    Double_t x = TransformToModuleCoordinates( absX, absY ).X();
    Double_t y = TransformToModuleCoordinates( absX, absY ).Y();

    Int_t nodeX = fMapping.GetNodeX( x );
    Int_t nodeY = fMapping.GetNodeY( y );

    Int_t channel = fMapping.GetChannelByNode( nodeX, nodeY );
    Int_t pixel = fMapping.GetPixelByNode( nodeX, nodeY );

    Int_t repeat = 1;
    Int_t count = 0;
    Int_t forward = 1;
    Int_t xAxis = 1;

    Int_t totalNodes = fMapping.GetNumberOfNodesX() * fMapping.GetNumberOfNodesY();

    // We test if x,y is inside the channel/pixel obtained from the readout mapping
    // If not we start to look in the readout mapping neighbours
    while ( !this->isInsidePixel( channel, pixel, absX, absY ) )
    {
        count++;
        if( xAxis == 1 && forward == 1 ) nodeX++;
        else if ( xAxis == 0 && forward == 1 ) nodeY++;
        else if ( xAxis == 1 && forward == 0 ) nodeX--;
        else if ( xAxis == 0 && forward == 0 ) nodeY--;

        Int_t nNodes = fMapping.GetNumberOfNodesX();

        if( nodeX < 0 ) nodeX = nNodes-1;
        if( nodeY < 0 ) nodeY = nNodes-1;
        if( nodeX >= nNodes ) nodeX = 0;
        if( nodeY >= nNodes ) nodeY = 0;

        if( count >= repeat )
        {
            if( xAxis == 1 && forward == 1 ) { xAxis = 0; forward = 0; }
            else if( xAxis == 0 && forward == 0 ) { xAxis = 1; forward = 0; repeat++; }
            else if( xAxis == 1 && forward == 0 ) { xAxis = 0; forward = 1; }
            else if( xAxis == 0 && forward == 1 ) { xAxis = 1; forward = 1; repeat++; }

            count = 0;
        }

        channel = fMapping.GetChannelByNode( nodeX, nodeY );
        pixel = fMapping.GetPixelByNode( nodeX, nodeY );

        if( count > totalNodes/10 ) 
        {
            cout << "REST Error? I did not found any channel for hit position (" << x << "," << y << ")" << endl; 

            for( int ch = 0; ch < GetNumberOfChannels( ); ch++ )
                for( int px = 0; px < GetChannel(ch)->GetNumberOfPixels( ); px++ )
                    if( isInsidePixel( ch, px, absX, absX ) ) { cout << "( " << x << " , " << y << ") Should be in channel " << ch << " pixel : " << px << endl; 

                        cout << "Corresponding node :  nX: " << fMapping.GetNodeX_ForChannelAndPixel( ch, px ) << " nY : " << fMapping.GetNodeY_ForChannelAndPixel( ch, px ) << endl; 
                        cout << "Channel : " << ch << " Pixel : " << px << endl;
                        cout << "Pix X : " << GetChannel(ch)->GetPixel(px)->GetCenter().X() << " Pix Y : " <<  GetChannel(ch)->GetPixel(px)->GetCenter().Y() << endl; }
            sleep(5);
         //   getchar();
            return -1; 
        }
    }

    return channel;
}

TRestReadoutChannel *TRestReadoutModule::GetChannelByID( int id )
{
    Int_t chNumber = -1;
    for( unsigned int i = 0; i < fReadoutChannel.size(); i++ )
        if( fReadoutChannel[i].GetID() == id )
        {
            if( chNumber != -1 ) cout << "REST Warning : Found several channels with the same ID" << endl;
            chNumber = i;
        }
    if( chNumber != -1 )
        return &fReadoutChannel[chNumber];

    cout << "REST Warning : Readout channel with ID : " << id << " not found in module : " << fModuleID  << endl;

    return NULL; 
}
 
Bool_t TRestReadoutModule::isInside( Double_t x, Double_t y )
{
    TVector2 v(x,y);
    return isInside( v );
}

Bool_t TRestReadoutModule::isInside( TVector2 pos )
{
    TVector2 rotPos = TransformToModuleCoordinates( pos );

    if( rotPos.X() >= 0 && rotPos.X() <= fModuleSizeX )
        if( rotPos.Y() >= 0 && rotPos.Y() <= fModuleSizeY )
            return true;

    return false;
}

Bool_t TRestReadoutModule::isInsideChannel( Int_t channel, Double_t x, Double_t y )
{
    TVector2 pos(x,y);

    return isInsideChannel( channel, pos );
}

Bool_t TRestReadoutModule::isInsideChannel( Int_t channel, TVector2 pos )
{
    pos = TransformToModuleCoordinates( pos );
    for( int idx = 0; idx < GetChannel(channel)->GetNumberOfPixels(); idx++ )
        if( GetChannel( channel )->GetPixel( idx )->isInside( pos ) ) return true;
    return false;
}

Bool_t TRestReadoutModule::isInsidePixel( Int_t channel, Int_t pixel, Double_t x, Double_t y )
{
    TVector2 pos(x,y);

    if( channel < 0 || pixel < 0 ) return false;

    return isInsidePixel( channel, pixel, pos );
}

Bool_t TRestReadoutModule::isInsidePixel( Int_t channel, Int_t pixel, TVector2 pos )
{
    if( channel < 0 || pixel < 0 ) return false;

    pos = TransformToModuleCoordinates( pos );
    if( GetChannel( channel )->GetPixel( pixel )->isInside( pos ) ) return true;
    return false;
}

TVector2 TRestReadoutModule::GetPixelOrigin( Int_t channel, Int_t pixel ) 
{
    TVector2 pixPosition( GetChannel( channel )->GetPixel(pixel)->GetOrigin() );
    pixPosition = pixPosition.Rotate( fModuleRotation * TMath::Pi()/ 180. );
    return pixPosition;
}

TVector2 TRestReadoutModule::GetPixelVertex( Int_t channel, Int_t pixel, Int_t vertex ) 
{
    TVector2 pixPosition = GetChannel( channel )->GetPixel(pixel)->GetVertex( vertex );

    pixPosition = pixPosition.Rotate( fModuleRotation * TMath::Pi()/ 180. );
    pixPosition = pixPosition + TVector2( fModuleOriginX, fModuleOriginY );
    return pixPosition;
}

TVector2 TRestReadoutModule::GetPixelCenter( Int_t channel, Int_t pixel )
{
    TVector2 corner1( GetPixelVertex( channel, pixel, 0 ) );
    TVector2 corner2( GetPixelVertex( channel, pixel, 2 ) );

    TVector2 center = (corner1+corner2)/2.;
    return center;
}

TVector2 TRestReadoutModule::GetVertex( int n ) const 
{
    TVector2 vertex( 0, 0 );
    TVector2 origin( fModuleOriginX, fModuleOriginY );

    if( n%4 == 0 ) return origin;
    else if( n%4 == 1 )
    {
        vertex.Set( fModuleSizeX, 0 );
        vertex = vertex.Rotate( fModuleRotation * TMath::Pi()/180. ); 

        vertex = vertex + origin;
    }
    else if( n%4 == 2 )
    {
        vertex.Set( fModuleSizeX, fModuleSizeY );
        vertex = vertex.Rotate( fModuleRotation * TMath::Pi()/180. ); 

        vertex = vertex + origin;
    }
    else if( n%4 == 3 )
    {
        vertex.Set( 0, fModuleSizeY );
        vertex = vertex.Rotate( fModuleRotation * TMath::Pi()/180. ); 

        vertex = vertex + origin;
    }
    return vertex;
}


Int_t TRestReadoutModule::GetNumberOfChannels( ) { return fReadoutChannel.size(); }

void TRestReadoutModule::AddChannel( TRestReadoutChannel &rChannel ) 
{
    Int_t channelError = 0;

    for( int i = 0; i < rChannel.GetNumberOfPixels(); i++ )
    {
        // TODO we expect here that the user will only do pixel rotations between 0 and 90 degrees,
        // we must force that on pixel definition or fix it here
        Double_t oX = rChannel.GetPixel( i )->GetVertex( 3 ).X();
        Double_t oY = rChannel.GetPixel( i )->GetVertex( 3 ).Y();
        Double_t sX = rChannel.GetPixel( i )->GetVertex( 1 ).X();
        Double_t sY = rChannel.GetPixel( i )->GetVertex( 1 ).Y();

        if( oX < 0 || oY < 0 || sX > fModuleSizeX || sY > fModuleSizeY )
        {
            cout << "REST Warning (AddChannel) pixel outside the module boundaries" << endl;
            cout << "Pixel origin = (" << oX << " , " << oY << ")" << endl;
            cout << "Pixel size = (" << sX << " , " << sY << ")" << endl;
            channelError++;
        }
    }

    if( channelError == 0 ) fReadoutChannel.push_back( rChannel ); 
    else {
        cout << "REST Warning (AddChannel) : Channel ID " << rChannel.GetID() << " not Added. Found " << channelError << " pixels outside the module boundaries" << endl;
    }
}

void TRestReadoutModule::Draw()
{

}

void TRestReadoutModule::Print( Int_t fullDetail )
{
        cout << "-- Readout module : " << GetModuleID( ) << endl;
        cout << "----------------------------------------------------------------" << endl;
        cout << "-- Origin position : X = " << fModuleOriginX << " mm " << " Y : " << fModuleOriginY << " mm" << endl;
        cout << "-- Size : X = " << fModuleSizeX << " Y : " << fModuleSizeY << endl;
        cout << "-- Rotation : " << fModuleRotation << " degrees" << endl;
        cout << "-- Total channels : " << GetNumberOfChannels() << endl;
        cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;

        if( fullDetail )
            for( int n = 0; n < GetNumberOfChannels(); n++ )
                fReadoutChannel[n].Print();

}
