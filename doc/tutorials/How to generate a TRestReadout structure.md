\brief This tutorial shows how to generate a TRestReadout structure and store it to disk for later use in TRestManager

We will address two different examples following basic readout topologies. These examples can be also found at the repository at REST_v2/config/template/readouts.rml.

You will find a variety of more complex examples at REST_v2/data/definitions/readouts/readouts.rml. More details about readout construction are available at the documentation of TRestReadout class. The class TRestMetadata describes detailed information on how to write RML files.

## Example 1. A basic pixelated readout 

In this example we generate a readout with a single readout plane, and one pixelated readout module placed inside. To achieve that each channel has a unique pixel definition.

\code
// We define some environment variables that we can later use as ${VARIABLE}
<environment>
    <variable name="PIX_SIZE" value="3" overwrite="true" />
    <variable name="CHANNELS" value="8" overwrite="true" />
</environment>

<section TRestReadout name="pixelReadout" title="A basic pixel readout. ${CHANNELS}x${CHANNELS} channels. Pixel size : ${PIX_SIZE} mm" >

    // These parameters are later keywords inside the section
    // and will be sustituted by their value.
    <myParameter name="nChannels" value="${CHANNELS}" />
    <myParameter name="pixelSize" value="${PIX_SIZE}" />

    // Mapping nodes is the number of nodes N, in a NxN grid. 
    // This grid allows for faster channel/pixel finding algorithm.

    // If the mappingNodes value is 0. The value will be automatically assigned by REST.
    <parameter name="mappingNodes" value="0" />

    // This is just the module definition. 
    <readoutModule name="pixelModule" size="(nChannels*pixelSize, nChannels*pixelSize)" tolerance="1.e-4" >

        // We use for loops to generate any number of channels given by the CHANNELS variable.
        // The loop variable must be placed between [] in order to be evaluated.
        <for variable="nChX" from="0" to="nChannels-1" step="1" />
            <for variable="nChY" from="0" to="nChannels-1" step="1" />

                // The readout channel id will be used to identify the channel and associate it to a daq id
                <readoutChannel id="[nChX]*nChannels+[nChY]" >
                    // In this example we define one pixel per channel. 
                    // But we can define any number of pixels inside a channel
                    <addPixel id="0" origin="([nChX]*pixelSize, [nChY]*pixelSize)" size="(${PIX_SIZE},${PIX_SIZE})" rotation="0" />
                </readoutChannel>

            </for>
        </for>

    </readoutModule>

    // The real readout implementation is done inside the readout plane. 

    // The readout plane parameters define the active volume.
    <readoutPlane position="(0,0,-990)" units="mm" planeVector="(0,0,1)"
        chargeCollection="1" 
        cathodePosition="(0,0,0)" units="mm" >

        // We can add any number of modules

        // name="pixelModule" is the name defined at the <readoutModule structure.
        <addReadoutModule id="0" name="pixelModule"
        // We define the module position inside the readout plane
            origin="(-nChannels*pixelSize/2,-nChannels*pixelSize/2)" 
            rotation="0" />

    </readoutPlane>

</section>
\endcode

## Example 2. A multilayer stripped readout 

In this example we define a stripped readout using single pixels with y-dimension much longer than x-dimension. 
We create two readout module definitions, one for each axis, and place each readout module at a different readout planes.

\code

<environment>
    <variable name="PIX_SIZE" value="3" overwrite="true" />
    <variable name="CHANNELS" value="8" overwrite="true" />
</environment>

<section TRestReadout name="strippedReadout" title="A basic pixel readout. ${CHANNELS}+${CHANNELS} channels. Pitch size : ${PIX_SIZE} mm" >
    <myParameter name="nChannels" value="${CHANNELS}" />
    <myParameter name="pixelSize" value="${PIX_SIZE}" />

    // In case of errors during the readout generation you might need to
    // define this value manually in the mapping nodes parameter.
    <parameter name="mappingNodes" value="nChannels" />

    // X-strips readout module definition
    <readoutModule name="stripsX" size="(nChannels*pixelSize, nChannels*pixelSize)" tolerance="1.e-4" >

        <for variable="nChX" from="0" to="nChannels-1" step="1" />
            <readoutChannel id="[nChX]" >
                <addPixel id="0" origin="([nChX]*pixelSize, 0)" size="(${PIX_SIZE},${PIX_SIZE}*nChannels)" rotation="0" />
            </readoutChannel>
        </for>

    </readoutModule>

    // Y-strips readout module definition
    <readoutModule name="stripsY" size="(nChannels*pixelSize, nChannels*pixelSize)" tolerance="1.e-4" >

        <for variable="nChY" from="0" to="nChannels-1" step="1" />
            <readoutChannel id="[nChY]" >
                <addPixel id="0" origin="(0, [nChY]*pixelSize)" size="(${PIX_SIZE}*nChannels,${PIX_SIZE})" rotation="0" />
            </readoutChannel>
        </for>

    </readoutModule>

    // We define a first readout plane
    <readoutPlane position="(0,0,-990)" units="mm" planeVector="(0,0,1)"
        chargeCollection="1" 
        cathodePosition="(0,0,0)" units="mm" >

        // This readout plane includes the readout with the strips along Y-axis (X-position)
        <addReadoutModule id="0" name="stripsX"
            origin="(-nChannels*pixelSize/2,-nChannels*pixelSize/2)" 
            rotation="0" />

    </readoutPlane>

    // We define a second readout plane covering the same active volume.
    <readoutPlane position="(0,0,-990)" units="mm" planeVector="(0,0,1)"
        chargeCollection="1" 
        cathodePosition="(0,0,0)" units="mm" >

        // This readout plane includes the readout with the strips along X-axis (Y-position)
        <addReadoutModule id="0" name="stripsY"
            origin="(-nChannels*pixelSize/2,-nChannels*pixelSize/2)" 
            rotation="0" />

    </readoutPlane>

</section>

\endcode

\warning For the moment, the process TRestHitsToSignalProcess is not able to process a multilayer readout plane, and/or charge collection sharing between different readout planes, covering the same active volume. Although few changes would be needed to adapt this process.

## Readout generation and storage in a ROOT file

Here we assume the previous examples are defined in a file named *readouts.rml* and this file is found at the working directory.

The following code will instantiate the TRestReadout class using the pixelated and stripped definitions, and save them to a ROOT file.

\code

// We start a ROOT session with REST libraries and scripts loaded by using restRoot
~ restRoot

// We give the filename and the readout names as arguments for the TRestReadout constructors
[0] TRestReadout *pixRead = new TRestReadout( "readouts.rml", "pixelReadout");

[1] TRestReadout *stripRead = new TRestReadout( "readouts.rml", "strippedReadout");

// We create a new ROOT file with "RECREATE" option or open an existing file with "UPDATE" option
[2] TFile *f = new TFile( "readouts.root", "RECREATE" );

[3] pixRead->Write("pixel");

[4] stripRead->Write("strip");

[5] f->Close();

// We exit from ROOT session
[6] .q
\endcode

After executing this code we will have a *readouts.root* file with two different readouts, named *pixel* and *strip*. 

\note The original readout name given at the RML file has been lost. And in order to reference it in ROOT or REST we will use the names given at write time, *pixel* and *strip*.

## Recovering the TRestReadout saved on a ROOT file

We can easily recover the TRestReadout as any other ROOT structure. In order to quickly look inside a REST/ROOT file we can use the executable **restPrintFileContents** to check the existing objects (readouts) inside the file.

\code

~$ restPrintFileContents readouts.root

\endcode

The following code recovers the TRestReadout structure

\code

~$ restRoot

[0] TFile *f = new TFile( "readouts.root" );

// We get a pointer to the pixelated readout
[1] TRestReadout *r = f->Get("pixel");

// We print the metadata information of this readout
[2] r->PrintMetadata();

// And we print it again with full detail, with info about channels and pixels positions.
[3] r->PrintMetadata(1);

[4] .q
\endcode

## Readout visualization

The readout visualization is still far from optimal, but a couple of ways are available in order to verify the task of readout design.

In a ROOT session we can call the method TRestReadoutPlane::GetReadoutHistogram to draw the pixel boundaries. 

\code

~$ restRoot

[0] TFile *f = new TFile( "readouts.root" );

// We get a pointer to the pixelated readout
[1] TRestReadout *r = f->Get("strip");

// We draw first the readout plane 0. A canvas inside ROOT is automatically generated
[2] r->GetReadoutPlane(0)->GetReadoutHistogram()->Draw();

// We can draw the other strips readout plane on top of the existing drawing
[3] r->GetReadoutPlane(1)->GetReadoutHistogram()->Draw("same");

[4] .q

\endcode

Or, we can directly use the script *REST_Readout_Viewer* to draw one of the readout planes.

\code

~$ restRoot

// By default the plane with index 0 will be drawn, if not specified
[0] REST_Readout_Viewer( "readouts.root", "strip" );

// We can also draw the other readut plane
[1] REST_Readout_Viewer( "readouts.root", "strip", 1 );

\endcode

## Readout validation

The construction of complex readouts requires to evaluate the proper channel spatial definition. Complex readouts will be composed of channels in which several pixels are combined and overlapped. The overlap between different pixels on the same readout channel will never suppose a problem. However, different channels overlap may affect the final response of the readout channels.

In order to test the readout we can produce a random virtual hit generation, with (x,y) coordinates inside the range of the readout modules in a given readout plane. We may then activate few test channels and draw only those hits which dropped in the activated channels. The script *REST_UTILS_CheckReadout* allows to perform this task. To produce a faster result we can focus in a small area of the readout, defined by the *region* parameter. We can activate the 128 first channels with a channel *mask* definition.

The following code shows the use of this script that works for any TRestReadout class stored previously in a ROOT file.

\code

$~ restRoot

// We define the 128 bits mask to enable different channels
[0] Int_t mask[4];

[1] mask[0] = 0x80000100;   // Channels 8 and 31 enabled    

[2] mask[1] = 0x000000FF;   // Channels from 32 to 47 enabled

[3] mask[2] = 0x0;  // All channels disabled [From 64 to 95]

[4] mask[3] = 0x0;  // All channels disabled [From 96 to 127]


// We define also a reduced region of the readout where we will launch random (x,y)
[5] Double_t region[4];

[6] Double_t region[0] = 0.2;   // Xmin starts at 20% of full area

[7] Double_t region[1] = 0.8;   // Xmax ends at 80% of full area

[8] Double_t region[2] = 0.4;   // Ymin starts at 40% of full area

[9] Double_t region[2] = 0.9;   // Ymax ends at 90% of full area

// The last two arguments, N and pId are optional.

// The number of (x,y) coordinates to be generated.
Int_t N = 1E4;

// The plane readout index to be checked
Int_t pId = 0;

// This script will launch the generation of random (x,y) positions at the specified region,
// and it will draw only the hits on the activated channels (8, 31, 32-47).

REST_UTILS_CheckReadout( "readouts.root", "pixel", region, mask, N, pId )

\endcode

These figures show the result of running the *REST_UTILS_CheckReadout* script for different channels.

![Validating different channels and regions in different readout topologies.](readoutValidation.png) 

