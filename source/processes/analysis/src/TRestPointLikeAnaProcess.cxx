///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestPointLikeAnaProcess.cxx
///
///_______________________________________________________________________________

#include "TRestPointLikeAnaProcess.h"
using namespace std;

ClassImp(TRestPointLikeAnaProcess)
    //______________________________________________________________________________
    TRestPointLikeAnaProcess::TRestPointLikeAnaProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestPointLikeAnaProcess::TRestPointLikeAnaProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName)) LoadDefaultConfig();

    fReadout = new TRestReadout(cfgFileName);
}

//______________________________________________________________________________
TRestPointLikeAnaProcess::~TRestPointLikeAnaProcess() {
    if (fReadout != NULL) {
        delete fReadout;
        fReadout = NULL;
    }
}

void TRestPointLikeAnaProcess::LoadDefaultConfig() {
    SetTitle("Default config");

    fThreshold = 50;
    fPedLevel = 0;
    fNChannels = 120;
    fPitch = 0.5;
    fGainFilename = "gain.txt";
    fCalGain = 100.;
}

//______________________________________________________________________________
void TRestPointLikeAnaProcess::Initialize() {
    SetSectionName(this->ClassName());

    fThreshold = 50;
    fPedLevel = 0;
    fNChannels = 120;
    fPitch = 0.5;
    fGainFilename = "gain.txt";
    fCalGain = 100.;

    fSignalEvent = NULL;

    fReadout = NULL;
}

void TRestPointLikeAnaProcess::LoadConfig(string cfgFilename) {
    if (LoadConfigFromFile(cfgFilename)) LoadDefaultConfig();

    // fReadout = new TRestReadout( cfgFilename.c_str() );
}

//______________________________________________________________________________
void TRestPointLikeAnaProcess::InitProcess() {
    TRestEventProcess::ReadObservables();

    fReadout = GetMetadata<TRestReadout>();

    if (fReadout == NULL) cout << "REST ERRORRRR : Readout has not been initialized" << endl;

    /*
        // Get the gain factor from a file
        TString fullPathName = (TString) GetMainDataPath() + "/../../gain/" +
       fGainFilename;

        cout << fullPathName << endl;

        FILE *calfile;
        calfile = fopen( fullPathName.Data() , "r");

        char auxchar[1024];
        double peak, peakerror, height, heighterror, fwhm, fwhmerror;
        double base, baseerror, res, reserror, ratio, ratioerror, chisquare;

        fscanf(calfile, "%[^\n]\n", auxchar);
        sscanf(auxchar, "%lf \t %lf \t %lf \t %lf \t %lf \t %lf \t %lf \t %lf \t
       %lf \t %lf \t %lf \t %lf \t %lf", &peak, &peakerror, &height, &heighterror,
       &fwhm, &fwhmerror, &base, &baseerror, &res, &reserror, &ratio, &ratioerror,
       &chisquare);

        fclose(calfile);

        fCalGain = peak;
    */

    fCalGain = 1.0;

    cout << "Gain factor " << fCalGain << endl;
}

//______________________________________________________________________________
TRestEvent* TRestPointLikeAnaProcess::ProcessEvent(TRestEvent* evInput) {
    fSignalEvent = (TRestSignalEvent*)evInput;

    // Variables for the cluster signal analysis
    double xmean, z1mean, z2mean, ymean, x2, z12, y2, z22, cxz, cyz, energy;
    double xsigma, z1sigma, ysigma, z2sigma, timeDelay;
    int i, k, l, n2dc, nchannel, ptime, lenp, used;
    int auxcharge, xcharge, ycharge, totcharge, xbins, ybins, z1bins, z2bins;
    int cond1, cond2, cond3, xl, xr, yl, yr, xlok, ylok, xrok, yrok, first;
    double xskew, yskew, zmean, z2, zsize, amplitude;

    TArrayI z1bin, z2bin, xbin, ybin, auxarray;
    z1bin.Set(512);
    z2bin.Set(512);
    xbin.Set(fNChannels);
    ybin.Set(fNChannels);

    int readoutChannel, daqChannel, planeId = 0;

    // Int_t modId;
    TRestReadoutModule* module;

    // The event limits in x and y direction are found.
    xl = yl = 0;
    xr = yr = 0;

    for (k = 0; k < fNChannels; k++) {
        xbin.fArray[k] = 0;
        ybin.fArray[k] = 0;
    }

    for (k = 0; k < fSignalEvent->GetNumberOfSignals(); k++) {
        daqChannel = fSignalEvent->GetSignal(k)->GetSignalID();
        TRestReadoutPlane* plane = fReadout->GetReadoutPlane(planeId);

        for (int m = 0; m < plane->GetNumberOfModules(); m++) {
            module = plane->GetModule(m);
            if (module->isDaqIDInside(daqChannel)) break;
        }

        // modId = module->GetModuleID();

        readoutChannel = module->DaqToReadoutChannel(daqChannel);
        // cout<<"daqChannel "<<daqChannel<<" readoutChannel
        // "<<readoutChannel<<endl;

        Int_t xStrip = 0;
        Int_t yStrip = 0;
        if (readoutChannel < fNChannels) xStrip = 1;
        if (readoutChannel >= fNChannels) yStrip = 1;

        if (yStrip == 0) {
            n2dc = 0;
            nchannel = readoutChannel;
        }  // It is x active channel
        else if (xStrip == 0) {
            n2dc = 1;
            nchannel = readoutChannel - fNChannels;
        }  // It is y active channel

        ptime = fSignalEvent->GetSignal(k)->GetMinTime();
        lenp = fSignalEvent->GetSignal(k)->GetMaxTime() - fSignalEvent->GetSignal(k)->GetMinTime();

        // The activated bins are found.
        for (l = 0; l < lenp - 1; l++) {
            auxcharge = fSignalEvent->GetSignal(k)->GetData(l) - fPedLevel - fThreshold;

            if ((auxcharge > 0) && (ptime >= 0) && (lenp < 200)) {
                if (n2dc == 0) xbin.fArray[nchannel] = 1;
                if (n2dc == 1) ybin.fArray[nchannel] = 1;
            }
        }
    }

    // Left limit of x.
    first = 1;
    xlok = 0;
    while (first > 0) {
        for (i = 0; i < fNChannels; i++) {
            if ((xbin.fArray[i] == 1) && (xlok == 0)) {
                cond1 = (xbin.fArray[i + 1] == 1);
                cond2 = (xbin.fArray[i + 2] == 1);
                cond3 = (xbin.fArray[i + 3] == 1);

                if ((cond1 + cond2 + cond3 >= 1) || (first == 2)) {
                    xl = i - 1;
                    xlok = 1;
                }
            }
        }

        first++;
        if (xlok > 0) first = -100;
        if (first == 3) first = -100;
    }

    // Left limit of y.
    first = 1;
    ylok = 0;
    while (first > 0) {
        for (i = 0; i < fNChannels; i++) {
            if ((ybin.fArray[i] == 1) && (ylok == 0)) {
                cond1 = (ybin.fArray[i + 1] == 1);
                cond2 = (ybin.fArray[i + 2] == 1);
                cond3 = (ybin.fArray[i + 3] == 1);

                if ((cond1 + cond2 + cond3 >= 1) || (first == 2)) {
                    yl = i - 1;
                    ylok = 1;
                }
            }
        }

        first++;
        if (ylok > 0) first = -100;
        if (first == 3) first = -100;
    }

    // Right limit of x.
    first = 1;
    xrok = 0;
    while (first > 0) {
        for (i = fNChannels; i >= 0; i--) {
            if ((xbin.fArray[i] == 1) && (xrok == 0)) {
                cond1 = (xbin.fArray[i - 1] == 1);
                cond2 = (xbin.fArray[i - 2] == 1);
                cond3 = (xbin.fArray[i - 3] == 1);

                if ((cond1 + cond2 + cond3 >= 1) || (first == 2)) {
                    xr = i + 1;
                    xrok = 1;
                }
            }
        }

        first++;
        if (xrok > 0) first = -100;
        if (first == 3) first = -100;
    }

    // Right limit of y.
    first = 1;
    yrok = 0;
    while (first > 0) {
        for (i = fNChannels; i >= 0; i--) {
            if ((ybin.fArray[i] == 1) && (yrok == 0)) {
                cond1 = (ybin.fArray[i - 1] == 1);
                cond2 = (ybin.fArray[i - 2] == 1);
                cond3 = (ybin.fArray[i - 2] == 1);
                if ((cond1 + cond2 + cond3 >= 1) || (first == 2)) {
                    yr = i + 1;
                    yrok = 1;
                }
            }
        }

        first++;
        if (yrok > 0) first = -100;
        if (first == 3) first = -100;
    }

    if (xl < 0) xl = 0;
    if (yl < 0) yl = 0;

    // cout << "Cluster limits " << endl;
    // cout << "X dir " << xl << " " << xr << endl;
    // cout << "Y dir " << yl << " " << yr << endl;
    // getchar();

    xmean = z1mean = z2mean = ymean = 0.0;
    x2 = z12 = y2 = z22 = cxz = cyz = 0.0;
    xcharge = ycharge = xbins = ybins = z1bins = z2bins = 0;
    xskew = yskew = zmean = z2 = zsize = amplitude = 0;

    for (k = 0; k < 512; k++) {
        z1bin.fArray[k] = 0;
        z2bin.fArray[k] = 0;
    }

    // The event observables are calculated using the limits in x and y.
    for (k = 0; k < fSignalEvent->GetNumberOfSignals(); k++) {
        daqChannel = fSignalEvent->GetSignal(k)->GetSignalID();
        TRestReadoutPlane* plane = fReadout->GetReadoutPlane(planeId);

        for (int m = 0; m < plane->GetNumberOfModules(); m++) {
            module = plane->GetModule(m);
            if (module->isDaqIDInside(daqChannel)) break;
        }

        // modId = module->GetModuleID();

        readoutChannel = module->DaqToReadoutChannel(daqChannel);

        Int_t xStrip = 0;
        Int_t yStrip = 0;
        if (readoutChannel < fNChannels) xStrip = 1;
        if (readoutChannel >= fNChannels) yStrip = 1;

        if (yStrip == 0) {
            n2dc = 0;
            nchannel = readoutChannel;
        } else if (xStrip == 0) {
            n2dc = 1;
            nchannel = readoutChannel - fNChannels;
        }

        ptime = fSignalEvent->GetSignal(k)->GetMinTime();
        lenp = fSignalEvent->GetSignal(k)->GetMaxTime() - fSignalEvent->GetSignal(k)->GetMinTime();

        amplitude += fSignalEvent->GetSignal(k)->GetMaxValue();
        zmean += fSignalEvent->GetSignal(k)->GetMaxValue() *
                 fSignalEvent->GetSignal(k)->GetTime(fSignalEvent->GetSignal(k)->GetMaxIndex());
        z2 += fSignalEvent->GetSignal(k)->GetMaxValue() *
              fSignalEvent->GetSignal(k)->GetTime(fSignalEvent->GetSignal(k)->GetMaxIndex()) *
              fSignalEvent->GetSignal(k)->GetTime(fSignalEvent->GetSignal(k)->GetMaxIndex());

        used = 0;
        for (l = 0; l < lenp - 1; l++) {
            auxcharge = fSignalEvent->GetSignal(k)->GetData(l) - fPedLevel - fThreshold;

            if ((auxcharge > 0) && (ptime >= 0) && (lenp < 200)) {
                if ((n2dc == 0) && (nchannel >= xl) && (nchannel <= xr)) {
                    xcharge += auxcharge;
                    xmean += auxcharge * nchannel * fPitch;
                    x2 += auxcharge * nchannel * nchannel * fPitch * fPitch;
                    z1mean += auxcharge * (ptime + l);
                    z12 += auxcharge * (ptime + l) * (ptime + l);
                    cxz += auxcharge * nchannel * fPitch * (ptime + l);
                    xskew += auxcharge * nchannel * nchannel * nchannel * fPitch * fPitch * fPitch;

                    if (used == 0) {
                        xbins++;
                        used = 1;
                    }
                    if (z1bin.fArray[ptime + l] == 0) {
                        z1bin.fArray[ptime + l] = 1;
                        z1bins++;
                    }
                }

                if ((n2dc == 1) && (nchannel >= yl) && (nchannel <= yr)) {
                    ycharge += auxcharge;
                    ymean += auxcharge * nchannel * fPitch;
                    y2 += auxcharge * nchannel * nchannel * fPitch * fPitch;
                    z2mean += auxcharge * (ptime + l);
                    z22 += auxcharge * (ptime + l) * (ptime + l);
                    cyz += auxcharge * nchannel * fPitch * (ptime + l);
                    yskew += auxcharge * nchannel * nchannel * nchannel * fPitch * fPitch * fPitch;

                    if (used == 0) {
                        ybins++;
                        used = 1;
                    }
                    if (z2bin.fArray[ptime + l] == 0) {
                        z2bin.fArray[ptime + l] = 1;
                        z2bins++;
                    }
                }
            }
        }
    }

    // The total charge is calculated.
    totcharge = xcharge + ycharge;

    // Values are normalized.
    xmean /= xcharge;
    x2 /= xcharge;
    z1mean /= xcharge;
    z12 /= xcharge;
    ymean /= ycharge;
    y2 /= ycharge;
    z2mean /= ycharge;
    z22 /= ycharge;
    cxz /= xcharge;
    cyz /= ycharge;
    xskew /= xcharge;
    yskew /= ycharge;
    zmean /= amplitude;
    z2 /= amplitude;

    // The widths in each direction are calculated.
    xsigma = TMath::Sqrt((x2 - xmean * xmean));
    z1sigma = TMath::Sqrt((z12 - z1mean * z1mean));
    ysigma = TMath::Sqrt((y2 - ymean * ymean));
    z2sigma = TMath::Sqrt((z22 - z2mean * z2mean));

    zsize = TMath::Sqrt((z2 - zmean * zmean));

    // The skew in each direction are calculated.
    xskew += -3. * xsigma * xsigma * xmean - xmean * xmean * xmean;
    yskew += -3. * ysigma * ysigma * ymean - ymean * ymean * ymean;
    if (xskew != 0) xskew /= TMath::Power(xsigma, 3.);
    if (yskew != 0) yskew /= TMath::Power(ysigma, 3.);

    // The total energy is calculated with an iron calibration.
    energy = totcharge * 5.9 / fCalGain;

    // The total time length of the event
    timeDelay = fSignalEvent->GetMaxTime() - fSignalEvent->GetMinTime();

    // cout << "x mean = "<< xmean << " y mean = " << ymean << endl;
    // cout << "x charge = "<< xcharge << " y charge = " << ycharge << endl;
    // cout << "x sigma = "<< xsigma << " y sigma = " << ysigma << endl;
    // cout << "x bins = "<< xbins << " y bins = " << ybins << endl;
    // getchar();

    // A new value for each observable is added
    SetObservableValue("eventCharge", totcharge);      // Event charge in ADC
    SetObservableValue("eventAmplitude", amplitude);   // Event amplitude in ADC
                                                       // Observables in X
    SetObservableValue("xCharge", xcharge);            // Charge in X direction in ADC
    SetObservableValue("xzBins", xbins);               // Multiplicity in x direction
    SetObservableValue("xMean", xmean);                // Mean position in x direction
    SetObservableValue("z1Mean", z1mean);              // Mean position in z1 direction
    SetObservableValue("xSigma", xsigma);              // Sigma in x direction
    SetObservableValue("z1Sigma", z1sigma);            // Sigma in z1 direction
    SetObservableValue("xskew", xskew);                // Skew in x direction
                                                       // Observables in Y
    SetObservableValue("yCharge", ycharge);            // Charge in Y direction in ADC
    SetObservableValue("yzBins", ybins);               // Multiplicity in y direction
    SetObservableValue("yMean", ymean);                // Mean position in y direction
    SetObservableValue("z2Mean", z1mean);              // Mean position in z2 direction
    SetObservableValue("ySigma", ysigma);              // Sigma in y direction
    SetObservableValue("z2Sigma", z2sigma);            // Sigma in z2 direction
    SetObservableValue("yskew", yskew);                // Skew in y direction
                                                       // General observables
    SetObservableValue("energy", energy);              // Energy in keV
    SetObservableValue("z1Bins", z1bins);              // Number of bins in Z1 direction
    SetObservableValue("z2Bins", z2bins);              // Number of bins in Z2 direction
    SetObservableValue("zsize", zsize);                // Size in z direction
    SetObservableValue("totalTimeLength", timeDelay);  // Total time length of the event

    return fSignalEvent;
}

//______________________________________________________________________________
void TRestPointLikeAnaProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestPointLikeAnaProcess::InitFromConfigFile() {
    fThreshold = StringToInteger(GetParameter("threshold"));
    fPedLevel = StringToInteger(GetParameter("pedestal"));
    fNChannels = StringToInteger(GetParameter("channels"));
    fPitch = GetDblParameterWithUnits("pitch");
    fGainFilename = GetParameter("gainFile");
}
