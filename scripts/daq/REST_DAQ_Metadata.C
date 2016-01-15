
void REST_DAQ_Metadata(char *cfg){

TRestRun *run = new TRestRun(cfg);

TRestDAQMetadata *daqMetadata = new TRestDAQMetadata(cfg);
daqMetadata->SetScriptsBuffer( );

daqMetadata->PrintRunScript( );
daqMetadata->PrintPedScript( );

daqMetadata->SetParFromPedBuffer( );

daqMetadata->PrintMetadata( );

run->OpenOutputFile();
run->AddMetadata(daqMetadata);
run->CloseOutputFile();

}

void LoadDAQMetadataFromFile(TString fName){

TRestRun *run = new TRestRun( );

run->OpenInputFile( fName );

TRestDAQMetadata *daqMetadata;
if((daqMetadata = (TRestDAQMetadata *)run->GetHistoricMetadata(0))!=NULL ){

daqMetadata->PrintRunScript( );
daqMetadata->PrintPedScript( );
daqMetadata->PrintMetadata( );
}


}


