

int REST_UTILS_GenerateGasFile( char *cfgFile)
{
    gSystem->Load("librestcore.so");

    TRestGas *gas = new TRestGas( cfgFile, true );

 //   gas->SetGasPressure( 1. );

    gas->PlotDriftVelocity(1, 1.e7, 50);
    gas->PlotLongitudinalDiffusion(1, 5000, 50);
    gas->PlotTransversalDiffusion(1, 5000, 50);
    gas->PlotTownsendCoefficient(1, 5000, 50);

}
