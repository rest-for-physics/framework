const Int_t nGases = 4;
const Int_t nSteps = 50;

Double_t eMin = 1.e2;
Double_t eMax = 1.e4;

Double_t maxDriftVel = 0;
Double_t maxLDiff = 0;
Double_t maxTDiff = 0;

int REST_UTILS_DrawCombinedGasCurves( )
{

    gSystem->Load("librestsim.so");
    gSystem->Load("librestcore.so");

    TRestGas *gas[nGases];
    TGraph *driftVelGraph[nGases], *diffLGraph[nGases], *diffTGraph[nGases];

    TRestGas *gas[0] = new TRestGas( "GasDefinitionXeTMA1Pct.rml", true );
    TRestGas *gas[1] = new TRestGas( "GasDefinitionXeTMA3Pct.rml", true );
    TRestGas *gas[2] = new TRestGas( "GasDefinitionXeTMA7Pct.rml", true );
    TRestGas *gas[3] = new TRestGas( "GasDefinitionXeTMA9Pct.rml", true );

    for( int i = 0; i < nGases; i++ )
        gas[i]->SetGasPressure(10);


    Double_t eField[nSteps], driftVel[nGases][nSteps], diffLong[nGases][nSteps], diffTrans[nGases][nSteps];
    for( int i = 0; i < nSteps; i++ )
        eField[i] = eMin + (double) i * (eMax-eMin)/nSteps;

    for( int j = 0; j < nGases; j++ )
    {
        for( int i = 0; i < nSteps; i++ )
        {
            driftVel[j][i] = gas[j]->GetDriftVelocity( eField[i] );
            if( driftVel[j][i] > maxDriftVel ) maxDriftVel = driftVel[j][i]; 

            diffLong[j][i] = gas[j]->GetLongitudinalDiffusion( eField[i] );
            if( diffLong[j][i] > maxLDiff ) maxLDiff = diffLong[j][i]; 

            diffTrans[j][i] = gas[j]->GetTransversalDiffusion( eField[i] );
            if( diffTrans[j][i] > maxTDiff ) maxTDiff = diffTrans[j][i]; 
        }
    }


    TCanvas* c = new TCanvas("Drift velocity", "  ");
    c->SetLogx(true);
    for( int i = 0; i < nGases; i++ )
    {
        driftVelGraph[i] = new TGraph( nSteps, eField, driftVel[i] );
        driftVelGraph[i]->SetLineColor(i+1);
        driftVelGraph[i]->SetLineWidth(2);
        driftVelGraph[i]->SetMaximum( maxDriftVel );
        driftVelGraph[i]->SetMinimum( 0. );
    }

    TString str;
    str.Form("Drift Velocity");
    driftVelGraph[0]->SetTitle(str);
    driftVelGraph[0]->GetXaxis()->SetTitle("E [V/cm]");
    driftVelGraph[0]->GetYaxis()->SetTitle("Drift velocity [cm/us]");
    driftVelGraph[0]->GetYaxis()->SetTitleOffset(0);

    Double_t xl1=.55, yl1=0.55, xl2=xl1+.3, yl2=yl1+.3;
    TLegend *leg = new TLegend( xl1, yl1, xl2, yl2 );

    for( int i = 0; i < nGases; i++ )
        leg->AddEntry( driftVelGraph[i], gas[i]->GetGasMixture(), "L" );   // h1 and h2 are histogram pointers


    driftVelGraph[0]->Draw("");
    for( int i = 1; i < nGases; i++ )
        driftVelGraph[i]->Draw("same");

    leg->Draw("same");

    TCanvas* c2 = new TCanvas("Longitudinal diffusion", "  ");
    c2->SetLogx(true);
    for( int i = 0; i < nGases; i++ )
    {
        diffLGraph[i] = new TGraph( nSteps, eField, diffLong[i] );
        diffLGraph[i]->SetLineColor(i+1);
        diffLGraph[i]->SetLineWidth(2);
        diffLGraph[i]->SetMaximum( maxLDiff );
        diffLGraph[i]->SetMinimum( 0. );
    }

    TString str;
    str.Form("Longitudinal diffusion");
    diffLGraph[0]->SetTitle(str);
    diffLGraph[0]->GetXaxis()->SetTitle("E [V/cm]");
    diffLGraph[0]->GetYaxis()->SetTitle("Diffusion [sqrt(cm)/cm]");
    diffLGraph[0]->GetYaxis()->SetTitleOffset(2);

    Double_t xl1=.55, yl1=0.55, xl2=xl1+.3, yl2=yl1+.3;
    TLegend *leg = new TLegend( xl1, yl1, xl2, yl2 );

    for( int i = 0; i < nGases; i++ )
        leg->AddEntry( diffLGraph[i], gas[i]->GetGasMixture(), "L" );   // h1 and h2 are histogram pointers


    diffLGraph[0]->Draw("");
    for( int i = 1; i < nGases; i++ )
        diffLGraph[i]->Draw("same");

    leg->Draw("same");

    TCanvas* c3 = new TCanvas("Transversal diffusion", "  ");
    c2->SetLogx(true);
    for( int i = 0; i < nGases; i++ )
    {
        diffTGraph[i] = new TGraph( nSteps, eField, diffTrans[i] );
        diffTGraph[i]->SetLineColor(i+1);
        diffTGraph[i]->SetLineWidth(2);
        diffTGraph[i]->SetMaximum( maxTDiff );
        diffTGraph[i]->SetMinimum( 0. );
    }

    TString str;
    str.Form("Transversal diffusion");
    diffTGraph[0]->SetTitle(str);
    diffTGraph[0]->GetXaxis()->SetTitle("E [V/cm]");
    diffTGraph[0]->GetYaxis()->SetTitle("Diffusion [sqrt(cm)/cm]");
    diffTGraph[0]->GetYaxis()->SetTitleOffset(2);

    Double_t xl1=.55, yl1=0.55, xl2=xl1+.3, yl2=yl1+.3;
    TLegend *leg = new TLegend( xl1, yl1, xl2, yl2 );

    for( int i = 0; i < nGases; i++ )
        leg->AddEntry( diffTGraph[i], gas[i]->GetGasMixture(), "L" );   // h1 and h2 are histogram pointers


    diffTGraph[0]->Draw("");
    for( int i = 1; i < nGases; i++ )
        diffTGraph[i]->Draw("same");

    leg->Draw("same");
}
