




REST_VIEWER_Track(TString fName=" "){

    TRestTrackEvent *track = new TRestTrackEvent( );
    TRestTrack *tck = new TRestTrack();
    TRestVolumeHits hits;



    double maxX, minX, maxY, minY, maxZ, minZ;

    TFile *fFile = new TFile(fName,"r");
    TTree *fTree  = (TTree*)fFile->Get("TRestTrackEvent Tree");
    TBranch *fBranch  = fTree->GetBranch("eventBranch");
    fBranch->SetAddress(&track);

    Int_t fNFileEvents = fBranch->GetEntries();
    cout << "Number of events " << fNFileEvents << endl;
    Int_t count = 0;
    TGraph *fXY;
    TGraph *fXZ;
    TGraph *fYZ;

    for (int i = 0; i < fNFileEvents; i++)
    {
	cout<<"Processing event "<< i <<endl;
	
	maxX=-1e10; minX = 1e10; maxZ=-1e10; minZ=1e10; maxY=-1e10; minY=1e10 ;
	// Event is loaded.
    	fBranch->GetEntry( i );
	Int_t nTracks = 	track->GetNumberOfTracks();
	cout<<"Get entry "<<i<< " with "<<nTracks<<" tracks"<< endl;

         fXY = new TGraph[nTracks];
         fXZ = new TGraph[nTracks];
         fYZ = new TGraph[nTracks];

	for (int j = 0; j< nTracks; j++)
	{
	     tck = track->GetTrack( j );
	     hits = tck->GetVolumeHits();
	     cout<<"Number of hits "<< hits.GetNumberOfHits( ) <<endl;
	     	    
	     count = 0;
	
	    for(int nhit=0; nhit <  hits.GetNumberOfHits( ); nhit++ )
	    {
		  Double_t x= hits.GetX( nhit );
		  Double_t y= hits.GetY( nhit );
		  Double_t z= hits.GetZ( nhit );
		  Double_t en= hits.GetEnergy( nhit );

		  cout<<x<<" "<< y <<" "<< z <<"; energy "<< en << endl;
		  fXY[j].SetPoint(count, x, y);
		  fXZ[j].SetPoint(count, x, z);
		  fYZ[j].SetPoint(count, y, z);
	    
		  if(x>maxX)maxX=x;
		  if(x<minX)minX=x;
		  if(y>maxY)maxY=y;
		  if(y<minY)minY=y;
	 	  if(z>maxZ)maxZ=z;
		  if(z<minZ)minZ=z;

		  count++;
	    }
	}


TCanvas *fCanvas = new TCanvas("view_track","view_track",1,1,1000,600);
fCanvas->Divide(3,1);

	//TVirtualPad *padxy = 
fCanvas->cd(1)->DrawFrame(minX-10,minY-10,maxX+10,maxY+10);
	//padxy->DrawFrame(minX-10,minY-10,maxX+10,maxY+10);
	TVirtualPad *padxz = fCanvas->cd(2);
	padxz->DrawFrame(minX-10,minZ-10,maxX+10,maxZ+10);
	TVirtualPad *padyz = fCanvas->cd(3);
	padyz->DrawFrame(minY-10,minZ-10,maxY+10,maxZ+10);

         for(int ntr=0;ntr<nTracks;ntr++)
	{
	    fXY[ntr].SetMarkerColor(1+ntr);
	    fXY[ntr].SetMarkerSize(1.);
	    fXY[ntr].SetMarkerStyle(21);
	    fXZ[ntr].SetMarkerColor(1+ntr);
	    fXZ[ntr].SetMarkerSize(1.);
	    fXZ[ntr].SetMarkerStyle(21);
	    fYZ[ntr].SetMarkerColor(1+ntr);
	    fYZ[ntr].SetMarkerSize(1.);
	    fYZ[ntr].SetMarkerStyle(21);

	   fCanvas->cd(1)
	    //if(ntr==0)   
	        fXY[ntr].Draw("P");
             //else if (ntr>0)
	        //fXY[ntr].Draw("PSAME");


	    padxz->cd();
 	    if(ntr==0)    
	        fXZ[ntr].Draw("P");
             else if (ntr>0)
	        fXZ[ntr].Draw("PSAME");


	    padyz->cd();
	    if(ntr==0)    
	        fYZ[ntr].Draw("P");
             else if (ntr>0)
	        fYZ[ntr].Draw("PSAME");
	}

	fCanvas->Update();
	
	if(fYZ!=NULL)delete[] fYZ;
	if(fXZ!=NULL)delete[] fXZ;
	if(fXY!=NULL)delete[] fXY;

	getchar();
    }




}





