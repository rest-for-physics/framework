///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackAnalysisProcess.cxx
///
///  TRestRawSignalAnalysisProcess.cxx
///
///  List of vailable cuts:
///
///  NTracksXCut
///  NTracksYCut
///  NTracksXYZCut
///
///  To add cut, write "cut" sections in your rml file:
///
/// \code
/// <TRestTrackAnalysisProcess name=""  ... >
///     <parameter name="cutsEnabled" value="true" />
///     <cut name="NTracksXCut" value="(1,3)" />
/// </TRestTrackAnalysisProcess>
///
///             First implementation of Geant4 analysis process into REST_v2
///             Date : mar/2016
///             Author : J. Galan
///
///_______________________________________________________________________________


#include "TRestTrackAnalysisProcess.h"
using namespace std;


ClassImp(TRestTrackAnalysisProcess)
    //______________________________________________________________________________
TRestTrackAnalysisProcess::TRestTrackAnalysisProcess()
{
    Initialize();
}

//______________________________________________________________________________
TRestTrackAnalysisProcess::TRestTrackAnalysisProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) ) LoadDefaultConfig( );
}

//______________________________________________________________________________
TRestTrackAnalysisProcess::~TRestTrackAnalysisProcess()
{
    delete fTrackEvent;
}

void TRestTrackAnalysisProcess::LoadDefaultConfig()
{
    SetTitle( "Default config" );
}

//______________________________________________________________________________
void TRestTrackAnalysisProcess::Initialize()
{
    SetSectionName( this->ClassName() );

    fTrackEvent = new TRestTrackEvent();

    fOutputEvent = fTrackEvent;
    fInputEvent = fTrackEvent;

    //fCutsEnabled = false;
}

void TRestTrackAnalysisProcess::LoadConfig( std::string cfgFilename, std::string name )
{
    if( LoadConfigFromFile( cfgFilename, name ) ) LoadDefaultConfig( );
}

//______________________________________________________________________________
void TRestTrackAnalysisProcess::InitProcess()
{
    std::vector <string> fObservables;
    fObservables = TRestEventProcess::ReadObservables();

	for (unsigned int i = 0; i < fObservables.size(); i++)
	{
		if (fObservables[i].find("nTracks_LE_Y_") != string::npos)
		{
			Double_t energy = StringToDouble(fObservables[i].substr(13, fObservables[i].length()).c_str());

			fTrack_LE_Y_EnergyObservables.push_back(fObservables[i]);
			fTrack_LE_Y_Threshold.push_back(energy);
			nTracks_LE_Y.push_back(0);
		}
		else if (fObservables[i].find("nTracks_HE_Y_") != string::npos)
		{
			Double_t energy = StringToDouble(fObservables[i].substr(13, fObservables[i].length()).c_str());

			fTrack_HE_Y_EnergyObservables.push_back(fObservables[i]);
			fTrack_HE_Y_Threshold.push_back(energy);
			nTracks_HE_Y.push_back(0);
		}
		else if (fObservables[i].find("nTracks_LE_X_") != string::npos)
		{
			Double_t energy = StringToDouble(fObservables[i].substr(13, fObservables[i].length()).c_str());

			fTrack_LE_X_EnergyObservables.push_back(fObservables[i]);
			fTrack_LE_X_Threshold.push_back(energy);
			nTracks_LE_X.push_back(0);
		}
		else if (fObservables[i].find("nTracks_HE_X_") != string::npos)
		{
			Double_t energy = StringToDouble(fObservables[i].substr(13, fObservables[i].length()).c_str());

			fTrack_HE_X_EnergyObservables.push_back(fObservables[i]);
			fTrack_HE_X_Threshold.push_back(energy);
			nTracks_HE_X.push_back(0);
		}
		else if (fObservables[i].find("nTracks_LE_") != string::npos)
		{
			Double_t energy = StringToDouble(fObservables[i].substr(11, fObservables[i].length()).c_str());

			fTrack_LE_EnergyObservables.push_back(fObservables[i]);
			fTrack_LE_Threshold.push_back(energy);
			nTracks_LE.push_back(0);
		}
		else if (fObservables[i].find("nTracks_HE_") != string::npos)
		{
			Double_t energy = StringToDouble(fObservables[i].substr(11, fObservables[i].length()).c_str());

			fTrack_HE_EnergyObservables.push_back(fObservables[i]);
			fTrack_HE_Threshold.push_back(energy);
			nTracks_HE.push_back(0);
		}
	}
}

//______________________________________________________________________________
void TRestTrackAnalysisProcess::BeginOfEventProcess() 
{
}

//______________________________________________________________________________
TRestEvent* TRestTrackAnalysisProcess::ProcessEvent( TRestEvent *evInput )
{
    *fTrackEvent =  *(( TRestTrackEvent *) evInput);

    TString obsName;

    Double_t tckLenX = 0;
    Double_t tckLenY = 0;
    Double_t tckLenXYZ=0;
    Double_t tckMaxLenXYZ=0;
    Double_t tckMaxEnXYZ=0;
    Int_t nTracksX = 0;
    Int_t nTracksY = 0;
    Int_t nTracksXYZ = 0;
    Double_t maxX = 0, maxY = 0, maxZ = 0;;

    for( unsigned int n = 0; n < nTracks_HE.size(); n++ )
        nTracks_HE[n] = 0;
    for( unsigned int n = 0; n < nTracks_LE.size(); n++ )
        nTracks_LE[n] = 0;
    for( unsigned int n = 0; n < nTracks_HE_Y.size(); n++ )
        nTracks_HE_Y[n] = 0;
    for( unsigned int n = 0; n < nTracks_LE_Y.size(); n++ )
        nTracks_LE_Y[n] = 0;
	for (unsigned int n = 0; n < nTracks_HE_X.size(); n++)
		nTracks_HE_X[n] = 0;
	for (unsigned int n = 0; n < nTracks_LE_X.size(); n++)
		nTracks_LE_X[n] = 0;

    for( int tck = 0; tck < fTrackEvent->GetNumberOfTracks(); tck++ )
    {
        if( !fTrackEvent->isTopLevel( tck ) ) continue;

        TRestTrack *t = fTrackEvent->GetTrack( tck );
        Double_t en = t->GetEnergy( );

        if( t->isXZ() )
        {
            if( t->GetTrackLength() >= 0 )
            {
                nTracksX++;
                tckLenX += t->GetTrackLength();
            }
        }

        if( t->isYZ() )
        {
            if( t->GetTrackLength() >= 0 )
            {
                nTracksY++;
                tckLenY += t->GetTrackLength();
            }
        }

        if( t->isXYZ() )
        {
            nTracksXYZ++;
            tckLenXYZ += t->GetTrackLength();
            if(en> tckMaxEnXYZ)  
            {
                tckMaxEnXYZ=en;
                tckMaxLenXYZ= t->GetTrackLength();
                maxX = t->GetMeanPosition().X();
                maxY = t->GetMeanPosition().Y();
                maxZ = t->GetMeanPosition().Z();
            }
        }


        for( unsigned int n = 0; n < fTrack_HE_EnergyObservables.size(); n++ )
            if( en > fTrack_HE_Threshold[n] )
                nTracks_HE[n]++;
        for( unsigned int n = 0; n < fTrack_LE_EnergyObservables.size(); n++ )
            if( en < fTrack_LE_Threshold[n] )
                nTracks_LE[n]++;

        for( unsigned int n = 0; n < fTrack_HE_Y_EnergyObservables.size(); n++ )
            if( en > fTrack_HE_Y_Threshold[n] && t->isYZ() )
                nTracks_HE_Y[n]++;
        for( unsigned int n = 0; n < fTrack_LE_Y_EnergyObservables.size(); n++ )
            if( en < fTrack_LE_Y_Threshold[n] && t->isYZ() )
                nTracks_LE_Y[n]++;

		for (unsigned int n = 0; n < fTrack_HE_X_EnergyObservables.size(); n++)
			if (en > fTrack_HE_X_Threshold[n] && t->isXZ())
				nTracks_HE_X[n]++;
		for (unsigned int n = 0; n < fTrack_LE_X_EnergyObservables.size(); n++)
			if (en < fTrack_LE_X_Threshold[n] && t->isXZ())
				nTracks_LE_X[n]++;
    }

    Double_t evTimeDelay = 0;
    if( fPreviousEventTime.size() > 0 )
        evTimeDelay = fTrackEvent->GetTime() - fPreviousEventTime.back();
    obsName = this->GetName() + (TString) "_EventTimeDelay";
    fAnalysisTree->SetObservableValue( obsName, evTimeDelay );

    Double_t meanRate = 0;
    if( fPreviousEventTime.size() == 100 )
        meanRate = 100. / (fTrackEvent->GetTime()-fPreviousEventTime.front());

    obsName = this->GetName() + (TString) "_MeanRate_InHz";
    fAnalysisTree->SetObservableValue( obsName, meanRate );

    fTrackEvent->SetNumberOfXTracks( nTracksX );
    fTrackEvent->SetNumberOfYTracks( nTracksY );


	// Cuts
	if (fCuts.size() > 0)
	{
		auto iter = fCuts.begin();
		while (iter != fCuts.end()) {
				if (iter->first == "nTracksXCut")
					if (nTracksX > iter->second.Y() || nTracksX < iter->second.X())
						return NULL;
				if (iter->first == "nTracksYCut")
					if (nTracksY > iter->second.Y() || nTracksY < iter->second.X())
						return NULL;
				if (iter->first == "nTracksXYZCut")
					if (nTracksXYZ > iter->second.Y() || nTracksXYZ < iter->second.X())
						return NULL;
			
			iter++;
		}

		//if (nTracksX < fNTracksXCut.X() || nTracksX > fNTracksXCut.Y()) return NULL;
		//if (nTracksY < fNTracksYCut.X() || nTracksY > fNTracksYCut.Y()) return NULL;
	}

    Double_t x = 0, y = 0;

    TRestTrack *tX = fTrackEvent->GetMaxEnergyTrackInX();
    if( tX != NULL )
        x = tX->GetMeanPosition().X();

    TRestTrack *tY = fTrackEvent->GetMaxEnergyTrackInY();
    if( tY != NULL )
        y = tY->GetMeanPosition().Y();

    obsName = this->GetName() + (TString) "_xMean";
    fAnalysisTree->SetObservableValue( obsName, x );

    obsName = this->GetName() + (TString) "_yMean";
    fAnalysisTree->SetObservableValue( obsName, y );

    obsName = this->GetName() + (TString) "_LengthX";
    fAnalysisTree->SetObservableValue( obsName, tckLenX );

    obsName = this->GetName() + (TString) "_LengthY";
    fAnalysisTree->SetObservableValue( obsName, tckLenY );

    obsName = this->GetName() + (TString) "_nTracksX";
    fAnalysisTree->SetObservableValue( obsName, nTracksX );

    obsName = this->GetName() + (TString) "_nTracksY";
    fAnalysisTree->SetObservableValue( obsName, nTracksY );

    obsName = this->GetName() + (TString) "_nTracksXYZ";
    fAnalysisTree->SetObservableValue( obsName, nTracksXYZ );
    obsName = this->GetName() + (TString) "_LengthXYZ";
    fAnalysisTree->SetObservableValue( obsName, nTracksXYZ );
    obsName = this->GetName() + (TString) "_MaxLengthXYZ";
    fAnalysisTree->SetObservableValue( obsName, tckMaxLenXYZ );
    obsName = this->GetName() + (TString) "_MaxEnXYZ";
    fAnalysisTree->SetObservableValue( obsName, tckMaxEnXYZ );

    obsName = this->GetName() + (TString) "_maxXMean";
    fAnalysisTree->SetObservableValue( obsName, maxX );

    obsName = this->GetName() + (TString) "_maxYMean";
    fAnalysisTree->SetObservableValue( obsName, maxY );
    obsName = this->GetName() + (TString) "_maxZMean";
    fAnalysisTree->SetObservableValue( obsName, maxZ );

    for( unsigned int n = 0; n < fTrack_LE_EnergyObservables.size(); n++ )
    {
        TString obsName = fTrack_LE_EnergyObservables[n];
        obsName = this->GetName( ) + (TString) "_" + obsName;
        fAnalysisTree->SetObservableValue( obsName, nTracks_LE[n] );
    }
    for( unsigned int n = 0; n < fTrack_HE_EnergyObservables.size(); n++ )
    {
        TString obsName = fTrack_HE_EnergyObservables[n];
        obsName = this->GetName( ) + (TString) "_" + obsName;
        fAnalysisTree->SetObservableValue( obsName, nTracks_HE[n] );
    }

    for( unsigned int n = 0; n < fTrack_LE_Y_EnergyObservables.size(); n++ )
    {
        TString obsName = fTrack_LE_Y_EnergyObservables[n];
        obsName = this->GetName( ) + (TString) "_" + obsName;
        fAnalysisTree->SetObservableValue( obsName, nTracks_LE_Y[n] );
    }
    for( unsigned int n = 0; n < fTrack_HE_Y_EnergyObservables.size(); n++ )
    {
        TString obsName = fTrack_HE_Y_EnergyObservables[n];
        obsName = this->GetName( ) + (TString) "_" + obsName;
        fAnalysisTree->SetObservableValue( obsName, nTracks_HE_Y[n] );
    }

	for (unsigned int n = 0; n < fTrack_LE_X_EnergyObservables.size(); n++)
	{
		TString obsName = fTrack_LE_X_EnergyObservables[n];
		obsName = this->GetName() + (TString) "_" + obsName;
		fAnalysisTree->SetObservableValue(obsName, nTracks_LE_X[n]);
	}
	for (unsigned int n = 0; n < fTrack_HE_X_EnergyObservables.size(); n++)
	{
		TString obsName = fTrack_HE_X_EnergyObservables[n];
		obsName = this->GetName() + (TString) "_" + obsName;
		fAnalysisTree->SetObservableValue(obsName, nTracks_HE_X[n]);
	}

    return fTrackEvent;
}

//______________________________________________________________________________
void TRestTrackAnalysisProcess::EndOfEventProcess() 
{
    fPreviousEventTime.push_back( fTrackEvent->GetTimeStamp() );
    if( fPreviousEventTime.size() > 100 ) fPreviousEventTime.erase( fPreviousEventTime.begin() );
}

//______________________________________________________________________________
void TRestTrackAnalysisProcess::EndProcess()
{
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestTrackAnalysisProcess::InitFromConfigFile( )
{
	

}

