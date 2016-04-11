///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestReadoutPlane.cxx
///
///             Base class for managing run data storage. It contains a TRestEvent and TRestMetadata array. 
///
///             apr 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///             aug 2015    Javier Galan
///_______________________________________________________________________________


#include "TRestReadoutPlane.h"
using namespace std;

const int debug = 0;

ClassImp(TRestReadoutPlane)
//______________________________________________________________________________
TRestReadoutPlane::TRestReadoutPlane()
{
    Initialize();
}


//______________________________________________________________________________
TRestReadoutPlane::~TRestReadoutPlane()
{
    cout << "Deleting TRestReadoutPlane" << endl;
}

void TRestReadoutPlane::Initialize()
{
    cout << "Initializing TRestReadoutPlane" << endl;

    fCathodePosition = TVector3( 0, 0, 0 );
    fPosition = TVector3( 0, 0, 0 );
    fPlaneVector = TVector3( 0, 0, 0 );

    fNModules = 0; 
    fReadoutModules.clear();
}

//______________________________________________________________________________
Int_t TRestReadoutPlane::GetNumberOfChannels( ) 
{
    Int_t nChannels = 0;
    for( int md = 0; md < GetNumberOfModules(); md++ )
        nChannels += GetReadoutModule(md)->GetNumberOfChannels();
    return nChannels;
}

TRestReadoutModule *TRestReadoutPlane::GetModuleByID( Int_t modID )
{

    for( int md = 0; md < GetNumberOfModules(); md++ )
        if( this->GetModule( md )->GetModuleID() == modID )
            return this->GetModule( md );

    cout << "REST ERROR (GetReadoutModuleByID) : Module ID : " << modID << " was not found" << endl;
    return NULL;
}

TRestReadoutChannel *TRestReadoutPlane::GetChannelByID( Int_t modID, Int_t chID )
{
    TRestReadoutModule *module = GetModuleByID( modID );

    for( int ch = 0; ch < module->GetNumberOfChannels(); ch++ )
        if( module->GetChannel( ch )->GetID() == chID )
            return module->GetChannel( ch );

    cout << "REST ERROR (GetReadoutChannelByID) : Channel ID " << chID << " Module ID : " << modID << " was not found" << endl;
    return NULL;
}

Double_t TRestReadoutPlane::GetX( Int_t modID, Int_t chID )
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

Double_t TRestReadoutPlane::GetY( Int_t modID, Int_t chID )
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

Double_t TRestReadoutPlane::GetDistanceTo( TVector3 pos )
{
        return ( pos - GetPosition() ).Dot( GetPlaneVector() );
}

Int_t TRestReadoutPlane::isInsideDriftVolume( Double_t x, Double_t y, Double_t z )
{
    TVector3 pos = TVector3( x, y, z );

    return isInsideDriftVolume( pos );
}

Int_t TRestReadoutPlane::isInsideDriftVolume( TVector3 pos )
{
    Double_t distance = GetDistanceTo( pos );

    if( distance > 0 && distance < fTotalDriftDistance )
    {
        for( int m = 0; m < GetNumberOfModules( ); m++ )
            if( GetModule( m )->isInside( pos.X(), pos.Y() ) ) return m;
    }

    return -1;
}


void TRestReadoutPlane::Print( Int_t fullDetail )
{
        cout << "-- Readout plane : " << GetID( ) << endl;
        cout << "----------------------------------------------------------------" << endl;
        cout << "-- Position : X = " << fPosition.X() << " mm, " << " Y : " << fPosition.Y() << " mm, Z : " << fPosition.Z() << " mm" << endl;
        cout << "-- Vector : X = " << fPlaneVector.X() << " mm, " << " Y : " << fPlaneVector.Y() << " mm, Z : " << fPlaneVector.Z() << " mm" << endl;
        cout << "-- Cathode Position : X = " << fCathodePosition.X() << " mm, " << " Y : " << fCathodePosition.Y() << " mm, Z : " << fCathodePosition.Z() << " mm" << endl;
        cout << "-- Total drift distance : " << fTotalDriftDistance << " mm" << endl;
        cout << "-- Charge collection : " << fChargeCollection << endl;
        cout << "-- Total modules : " << GetNumberOfModules() << endl;
        cout << "-- Total channels : " << GetNumberOfChannels() << endl;
        cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;

    for( int i = 0; i < GetNumberOfModules(); i++ )
        this->GetReadoutModule(i)->Print( fullDetail );

}

void TRestReadoutPlane::Draw()
{
    /*
    for( int modID = 0; modID < GetNumberOfModules(); modID++ )
    {
        TRestReadoutPlaneModule md = GetReadoutModule( modID );

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


TH2Poly *TRestReadoutPlane::GetReadoutHistogram( )
{

    Double_t x[4];
    Double_t y[4];

    double xmin, xmax, ymin, ymax;

    GetBoundaries( xmin, xmax, ymin, ymax );

    TH2Poly *readoutHistogram = new TH2Poly( "ReadoutHistogram", "ReadoutHistogram", xmin, xmax, ymin, ymax );

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
                }

                readoutHistogram->AddBin(4,x,y);
            }

        }
    }

    return readoutHistogram;
}

void TRestReadoutPlane::GetBoundaries( double &xmin, double &xmax, double &ymin, double &ymax )
{

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

            if( x[v] < xmin ) xmin = x[v];
            if( y[v] < ymin ) ymin = y[v];
            if( x[v] > xmax ) xmax = x[v];
            if( y[v] > ymax ) ymax = y[v];

        }
    }
}

