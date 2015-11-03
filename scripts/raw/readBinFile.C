

int readBinFile(char *fName){

FILE *fInputBinFile = fopen(fName,"rb");

char runUid[26];

fread(runUid, 1, 26, fInputBinFile);
cout<<runUid<<endl;

int eventSize=0;


unsigned short nullword;
fread(&nullword, sizeof(nullword),1,fInputBinFile);
cout<<"NULL word "<<nullword<<endl;
printBits(nullword);

int frameBits=0;

unsigned short startDF;
fread(&startDF, sizeof(startDF),1,fInputBinFile);
cout<<"PFX_START "<<startDF<<endl;
printBits(startDF);
frameBits+=sizeof(startDF);

cout<<" Version "<< ((startDF & 0x1E0) >> 5) <<endl;
cout<<" FEC "<<(startDF & 0x1F)<<endl;

unsigned short payload;
fread(&payload, sizeof(payload),1,fInputBinFile);
cout<<"Frame payload "<<payload<<endl;
printBits(payload);
frameBits+=sizeof(payload);

int counter=0;

while(1){

unsigned short startEv;
fread(&startEv, sizeof(startEv),1,fInputBinFile);
cout<<"Start of Event "<<startEv<<endl;
printBits(startEv);
frameBits+=sizeof(startEv);
eventSize+=sizeof(startEv);
cout<<" Event type "<<(startEv & 0xF)<<endl;

unsigned short tsh,tsm,tsl;
fread(&tsh, sizeof(tsh),1,fInputBinFile);
fread(&tsm, sizeof(tsm),1,fInputBinFile);
fread(&tsm, sizeof(tsl),1,fInputBinFile);

frameBits+=sizeof(tsh)*3;
eventSize+=sizeof(tsh)*3;
printBits(tsh);
printBits(tsm);
printBits(tsl);

cout<<" TimeStamp "<<2147483648*tsl+32768*tsm+tsh<<endl;

unsigned int evID;
fread(&evID, sizeof(evID),1,fInputBinFile);
printBits(evID);
frameBits+=sizeof(evID);
eventSize+=sizeof(evID);

cout<<"Event ID "<<evID<<endl;

if(counter=!evID){cout<<"Counter and ID didn't match!!!!"<<endl;exit(0);}

cout<<"Bits readed "<<frameBits<<" /"<<payload<<endl;

//getchar( );

unsigned short dat=0;

	while( ((dat & 0xFFF0)>>4) !=14 ){
	fread(&dat, sizeof(dat),1,fInputBinFile);
	frameBits+=sizeof(dat);
	eventSize+=sizeof(dat);
	printBits(dat);
	if((dat & 0xC000) >> 14 == 3 ){
	cout<<"fecN "<<((dat & 0x3E00) >> 9)<<" asicN "<<((dat & 0x180)>>7)<<" channel "<<((dat & 0x7F))<<endl;
		
	}
	
	if(dat==15){
	cout<<" End of frame "<<"Bits readed "<<frameBits<<" /"<<payload<<endl;
	frameBits=0;
	fread(&startDF, sizeof(startDF),1,fInputBinFile);
	printBits(startDF);
	cout<<"PFX_START "<<startDF<<endl;
	
	frameBits+=sizeof(startDF);
	eventSize+=sizeof(startDF);

	cout<<" Version "<< ((startDF & 0x1E0) >> 5) <<endl;
	cout<<" FEC "<<(startDF & 0x1F)<<endl;

	fread(&payload, sizeof(payload),1,fInputBinFile);
	cout<<"Frame payload "<<payload<<endl;
	printBits(payload);
	frameBits+=sizeof(payload);
	eventSize+=sizeof(payload);
	}
	
	
	}
cout<<" End of event "<< dat<<endl;
int evSize = (dat & 0xF)*32768;

counter++;

fread(&dat, sizeof(dat),1,fInputBinFile);
frameBits+=sizeof(dat);
eventSize+=sizeof(dat);
printBits(dat);

evSize+=dat;

cout<<"Event size "<< evSize<<"/"<<eventSize<<endl;

evSize=0;

if(payload<=frameBits+2){
	while( dat !=15 ){
	fread(&dat, sizeof(dat),1,fInputBinFile);
	frameBits+=sizeof(dat);
	eventSize+=sizeof(dat);
	printBits(dat);
	}
	cout<<" End of frame "<<"Bits readed "<<frameBits<<" /"<<payload<<endl;
	frameBits=0;
	fread(&startDF, sizeof(startDF),1,fInputBinFile);
	printBits(startDF);
	cout<<"PFX_START "<<startDF<<endl;
	
	frameBits+=sizeof(startDF);
	eventSize+=sizeof(startDF);

	cout<<" Version "<< ((startDF & 0x1E0) >> 5) <<endl;
	cout<<" FEC "<<(startDF & 0x1F)<<endl;

	fread(&payload, sizeof(payload),1,fInputBinFile);
	cout<<"Frame payload "<<payload<<endl;
	printBits(payload);
	frameBits+=sizeof(payload);
	eventSize+=sizeof(payload);
	}



cout<<" Bits readed "<<frameBits<<"/"<<payload<<endl;

//getchar( );


}

}


void  printBits(unsigned short num)
{
   for(unsigned short bit=0;bit<(sizeof(bit) * 8); bit++)
   {
      printf("%i ", num & 0x01);
      num = num >> 1;
   }
   
   printf("\n");
}

void  printBits(unsigned  int num)
{
   for(unsigned short bit=0;bit<(sizeof(bit) * 8); bit++)
   {
      printf("%i ", num & 0x01);
      num = num >> 1;
   }
   
   printf("\n");
}
