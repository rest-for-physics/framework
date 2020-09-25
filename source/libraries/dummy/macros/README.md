REST macros directory that will be installed and accessible using restRoot command interface.

We need to create a dummy macro here.

Also a naming convention to be defined.

Macros which relate to any library should be placed in the macros at the main `RestFramework` repository. 
For example:
- REST_PrintEvent( TString fName, TRestXXEvent *evt, Int_t entry)
- REST_PrintEventById( TString fName, TRestXXEvent *evt, Int_t evId )
- REST_DrawEvent( TString fName, TRestXXEvent *evt, Int_t entry)
- REST_DrawEventById( TString fName, TRestXXEvent *evt, Int_t evId )
- ...
