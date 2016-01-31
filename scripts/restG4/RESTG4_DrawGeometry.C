
Int_t RESTG4_DrawGeometry( TString gdmlName )
{

    TGeoManager *geo = new TGeoManager();
    geo->Import(gdmlName);

    TGeoNode* node = geo->GetTopNode();
    for(int i=0;i< geo->GetNNodes();i++)
        geo->GetVolume(i)->SetTransparency(50);

    geo->GetTopVolume()->Draw("ogl");
    /////////////////////////////

}
