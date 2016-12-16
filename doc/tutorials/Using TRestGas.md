\brief This tutorial shows how to use TRestGas to produce pre-generated gas files and obtain basic gas properties.

The TRestGas metadata members description can be found detailed in the TRestGas class documentation. Here we provide few commands to show how to generate a new gas mixture and access its basic properties, as drift velocity, electron diffusion, and typical gas coefficients.

We start by definning a standalone TRestGas section inside our RML file. Our configuration file could be something like this.

\code
<!-- File : argonMixture.rml -->

<globals>
    <parameter name="gasDataPath" value="." />
</globals>

<section TRestGas name="Argon-Isobutane 4Pct 10-10E3V/cm" title="Argon-Isobutane Mixture (4Pct Isobutane)">
    <parameter name="pressure" value="1" />
    <parameter name="temperature" value="293.15" />
    <parameter name="maxElectronEnergy" value="400" />
    <parameter name="W_value" value="26.145" />
    <parameter name="nCollisions" value="10" />
    <eField Emin="10" Emax="1000." nodes="20" />
    <gasComponent name="ar" fraction="0.96" />
    <gasComponent name="iC4H10" fraction="0.04" />
</section>
\endcode

The *gasDataPath* defines the path where our gas file will be generated. In this case the file will be generated at the same location where we launch our program.

We can generate the gas file by specifying the full path to our configuration file and providing the name of the gas we want to use. In this case we will assume that we are launching the commands from the same directory where *argonMixture.rml* is found. We first start *ROOT* interpreter using *restRoot* to load all the REST libraries in the ROOT environment.

\code

:~ restRoot

[0] TRestGas *gas = new TRestGas( "argonMixture.rml", "Argon-Isobutane 4Pct 10-10E3V/cm", true );

\endcode

This should start the generation of the gas file, the calculation will be running for a few hours. When it is finished a new .gas file will have been created at *gasDataPath*. The filename format defines in a unique way the parameters used in the TRestGas section. In this way, it is used by TRestGas to determine if the gas mixture we want to use is already existing as a .gas file. 

If the gas file exists TRestGas will directly load the gas file. After the previous command is finished you can verify it by closing the session and starting again.

\code

[1] .q

:~ restRoot

[0] TRestGas *gas = new TRestGas( "argonMixture.rml", "Argon-Isobutane 4Pct 10-10E3V/cm", true );

\endcode

This time the recently generated gas file will be loaded and no long calculation will be required.

We can now access the members of TRestGas to obtain the values for the drift velocity or the tranversal electron diffusion. The following command will print on screen the drift velocity.

\code

[1] cout << "The drift velocity at 100 V/cm is : " << gas->GetDriftVelocity( 100 ) << endl;

\endcode

You can also find the information of the gas you are using by calling PrintMetadata().

\code

[2] gas->PrintMetadata();

\endcode

Although one must be careful with the definition range of the electric field and the pressure, we can retrieve the gas properties at different gas conditions. This code changes the value of the pressure to 10 atm and prints the drift velocity at that pressure value.

\code

[3] gas->SetPressure( 10 );

[4] cout << "The drift velocity at 100V/cm at 10 bar is : " << gas->GetDriftVelocity( 100 ) << endl;

\endcode

Finally, we can quickly visualize the dependency of different gas properties as a function of the field. The following code will plot the drift velocity at 10 bar, for a field range between 1V/cm and 1000V/cm drawing 100 extrapolated points.

\code

[5] gas->PlotDriftVelocity( 1, 1000, 100 )

[4] cout << "The drift velocity at 100V/cm at 10 bar is : " << gas->GetDriftVelocity( 100 ) << endl;

\endcode

Thats it. Other gas parameters and relevant information related to TRestGas can be found in the class documentation.
