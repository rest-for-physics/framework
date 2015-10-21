
#include <TRestGas.h>


int main()
{

    char cfgFile[256];
    sprintf( cfgFile, "mySimulationTemplate.rml");

    TRestGas *gas = new TRestGas( cfgFile );

    delete gas;


}
