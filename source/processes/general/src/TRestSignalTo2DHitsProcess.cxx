///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalTo2DHitsProcess.cxx
///
///_______________________________________________________________________________

#include "TRestSignalTo2DHitsProcess.h"
#include "TF2.h"
#include "TFitResultPtr.h"
#include "TRandom.h"
#include "TVirtualFitter.h"

#include "Fit/BinData.h"
#include "Fit/Chi2FCN.h"
#include "Fit/DataRange.h"
#include "Fit/Fitter.h"
#include "HFitInterface.h"
#include "Math/IFunction.h"
#include "Math/Minimizer.h"
#include "Math/MinimizerOptions.h"
#include "Math/WrappedMultiTF1.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TFitter.h"
#include "TMinuitMinimizer.h"
#include "TMutex.h"
#include "TROOT.h"
#include "TVirtualMutex.h"

using namespace std;

ClassImp(TRestSignalTo2DHitsProcess)
    //______________________________________________________________________________
    TRestSignalTo2DHitsProcess::TRestSignalTo2DHitsProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestSignalTo2DHitsProcess::~TRestSignalTo2DHitsProcess() {
    delete fOutput2DHitsEvent;
    delete fInputSignalEvent;
}

//______________________________________________________________________________
void TRestSignalTo2DHitsProcess::Initialize() {
    // We define the section name (by default we use the name of the class)
    SetSectionName(this->ClassName());

    // We create the input/output specific event data
    fInputSignalEvent = new TRestSignalEvent();
    fOutput2DHitsEvent = new TRest2DHitsEvent();

    // We connect the TRestEventProcess input/output event pointers
    fInputEvent = fInputSignalEvent;
    fOutputEvent = fOutput2DHitsEvent;

    // TVirtualFitter::SetDefaultFitter("Minuit2");
}

void TRestSignalTo2DHitsProcess::InitProcess() {
    fReadout = (TRestReadout*)GetReadoutMetadata();
    if (fReadout != NULL) fOutput2DHitsEvent->SetReadout(fReadout);

    fOutput2DHitsEvent->SetROIX(TVector2(X1, X2));
    fOutput2DHitsEvent->SetROIY(TVector2(Y1, Y2));

    // longmunumxz = 0;
    // longmunumyz = 0;
    // mudeposxz = new TH1D("mudeposxzup", "muonXZenergydepos", 512, 0., 512.);
    // mudeposyz = new TH1D("mudeposyzup", "muonYZenergydepos", 512, 0., 512.);

    hxzt = new TH1D((TString) "hxzt" + ToString(this), "hh", 200, 0, 3.14);
    hxzr = new TH1D((TString) "hxzr" + ToString(this), "hh", 200, -(X2 - X1), (X2 - X1));
    fxz = new TF1((TString) "fxz" + ToString(this), "gaus");
    hyzt = new TH1D((TString) "hyzt" + ToString(this), "hh", 200, 0, 3.14);
    hyzr = new TH1D((TString) "hyzr" + ToString(this), "hh", 200, -(Y2 - Y1), (Y2 - Y1));
    fyz = new TF1((TString) "fyz" + ToString(this), "gaus");
}

//______________________________________________________________________________
TRestEvent* TRestSignalTo2DHitsProcess::ProcessEvent(TRestEvent* evInput) {
    fInputSignalEvent = (TRestSignalEvent*)evInput;

    if (fInputSignalEvent->GetNumberOfSignals() <= 0) return NULL;

    fOutput2DHitsEvent->Initialize();
    fOutput2DHitsEvent->SetEventInfo(fInputSignalEvent);
    fOutput2DHitsEvent->SetSubEventTag("general");

    int firingN = 0;
    for (int n = 0; n < fInputSignalEvent->GetNumberOfSignals(); n++) {
        TRestSignal* s = fInputSignalEvent->GetSignal(n);
        fOutput2DHitsEvent->AddSignal(s);
    }

    fOutput2DHitsEvent->RemoveSeparateZ();

    // the analysis
    double zlen = fOutput2DHitsEvent->GetZRange().Y() - fOutput2DHitsEvent->GetZRange().X();
    double xlen = fOutput2DHitsEvent->GetXRange().Y() - fOutput2DHitsEvent->GetXRange().X();
    double ylen = fOutput2DHitsEvent->GetYRange().Y() - fOutput2DHitsEvent->GetYRange().X();
    double firstx = fOutput2DHitsEvent->GetFirstX();
    double firsty = fOutput2DHitsEvent->GetFirstY();
    double firstz = fOutput2DHitsEvent->GetZRange().X();
    double lastz = fOutput2DHitsEvent->GetZRange().Y();

    SetObservableValue("zlen", zlen);
    SetObservableValue("xlen", xlen);
    SetObservableValue("ylen", ylen);
    SetObservableValue("firstx", firstx);
    SetObservableValue("firsty", firsty);
    SetObservableValue("firstz", firstz);
    SetObservableValue("lastz", lastz);

    // mutanthe = numeric_limits<double>::quiet_NaN();

    //TRest2DHitsEvent* eve = SelectTag();

    //if (eve != NULL) MuDepos(eve);

    // if (eve!=NULL && eve->GetNumberOfSignals() > 12 && eve->GetZRange().Y() -
    // eve->GetZRange().X() > 200) { 	return eve;
    //}
    // else
    //{
    //	return NULL;
    //}

    return fOutput2DHitsEvent;
}

// int CheckFitFunction(const TF1 * f1, int dim) {
//	// Check validity of fitted function
//	if (!f1) {
//		Error("Fit", "function may not be null pointer");
//		return -1;
//	}
//	if (f1->IsZombie()) {
//		Error("Fit", "function is zombie");
//		return -2;
//	}
//
//	int npar = f1->GetNpar();
//	if (npar <= 0) {
//		Error("Fit", "function %s has illegal number of parameters =
//%d", f1->GetName(), npar); 		return -3;
//	}
//
//	// Check that function has same dimension as histogram
//	if (f1->GetNdim() > dim) {
//		Error("Fit", "function %s dimension, %d, is greater than fit
// object dimension, %d", 			f1->GetName(), f1->GetNdim(), dim);
// return -4;
//	}
//	if (f1->GetNdim() < dim - 1) {
//		Error("Fit", "function %s dimension, %d, is smaller than fit
// object dimension -1, %d", 			f1->GetName(), f1->GetNdim(), dim);
// return -5;
//	}
//
//	return 0;
//
//}
//
// void GetFunctionRange(const TF1 & f1, ROOT::Fit::DataRange & range) {
//	// get the range form the function and fill and return the DataRange
// object 	Double_t fxmin, fymin, fzmin, fxmax, fymax, fzmax;
// f1.GetRange(fxmin, fymin, fzmin, fxmax, fymax, fzmax);
//	// support only one range - so add only if was not set before
//	if (range.Size(0) == 0) range.AddRange(0, fxmin, fxmax);
//	if (range.Size(1) == 0) range.AddRange(1, fymin, fymax);
//	if (range.Size(2) == 0) range.AddRange(2, fzmin, fzmax);
//	return;
//}
//
// int Fit2(TH1 * h1, TF1 *f1, string option)
//{
//	Foption_t fitOption;
//	ROOT::Fit::FitOptionsMake(ROOT::Fit::kHistogram, option.c_str(),
// fitOption); 	ROOT::Fit::DataRange range(h1->GetMinimum(), h1->GetMaximum());
//	ROOT::Math::MinimizerOptions minOption;
//
//#ifdef DEBUG
//	printf("fit function %s\n", f1->GetName());
//#endif
//
//
//	Int_t special = f1->GetNumber();
//	Bool_t linear = f1->IsLinear();
//	Int_t npar = f1->GetNpar();
//	if (special == 299 + npar)  linear = kTRUE; // for polynomial functions
//												//
// do not use linear fitter in these case 	if (fitOption.Bound ||
// fitOption.Like || fitOption.Errors || fitOption.Gradient || fitOption.More ||
// fitOption.User || fitOption.Integral || fitOption.Minuit) 		linear =
// kFALSE;
//
//	// create an empty TFitResult
//	TFitResult tfr;
//	// create the fitter from an empty fit result
//	ROOT::Fit::Fitter fitter;
//	ROOT::Fit::FitConfig & fitConfig = fitter.Config();
//
//	// create options
//
//	ROOT::Fit::DataOptions opt;
//	opt.fIntegral = fitOption.Integral;
//	opt.fUseRange = fitOption.Range;
//	opt.fExpErrors = fitOption.PChi2;  // pearson chi2 with expected errors
//	if (fitOption.Like || fitOption.PChi2) opt.fUseEmpty = true;  // use
// empty bins in log-likelihood fits 	if (special == 300) opt.fCoordErrors =
// false; // no need to use coordinate errors in a pol0 fit 	if
// (fitOption.NoErrX) opt.fCoordErrors = false;  // do not use coordinate errors
// when requested 	if (fitOption.W1) opt.fErrors1 = true; 	if (fitOption.W1
//> 1) opt.fUseEmpty = true; // use empty bins with weight=1
//
//	if (fitOption.BinVolume) {
//		opt.fBinVolume = true; // scale by bin volume
//		if (fitOption.BinVolume == 2) opt.fNormBinVolume = true; //
// scale by normalized bin volume
//	}
//
//	if (opt.fUseRange) {
//		GetFunctionRange(*f1, range);
//	}
//
//
//	// fill data
//	std::shared_ptr<ROOT::Fit::BinData> fitdata(new ROOT::Fit::BinData(opt,
// range)); 	ROOT::Fit::FillData(*fitdata, h1, f1); 	if (fitdata->Size() ==
// 0) { 		Warning("Fit", "Fit data is empty "); 		return -1;
//	}
//
//	// switch off linear fitting in case data has coordinate errors and the
// option is set 	if (fitdata->GetErrorType() ==
// ROOT::Fit::BinData::kCoordError
//&& fitdata->Opt().fCoordErrors) linear = false;
//	// linear fit cannot be done also in case of asymmetric errors
//	if (fitdata->GetErrorType() == ROOT::Fit::BinData::kAsymError &&
// fitdata->Opt().fAsymErrors) linear = false;
//
//	// this functions use the TVirtualFitter
//	if (special != 0 && !fitOption.Bound && !linear) {
//		if (special == 100)      ROOT::Fit::InitGaus(*fitdata, f1); //
// gaussian 		else if (special == 110 || special == 112)
// ROOT::Fit::Init2DGaus(*fitdata, f1); // 2D gaussians ( xygaus or bigaus)
// else if (special == 400)      ROOT::Fit::InitGaus(*fitdata, f1); // landau
// (use the same) 		else if (special == 410)
// ROOT::Fit::Init2DGaus(*fitdata, f1); // 2D landau (use the same)
//
//		else if (special == 200)      ROOT::Fit::InitExpo(*fitdata, f1);
//// exponential
//
//	}
//
//
//	// set the fit function
//	// if option grad is specified use gradient
//	if ((linear || fitOption.Gradient))
//		fitter.SetFunction(ROOT::Math::WrappedMultiTF1(*f1));
//	else
//		fitter.SetFunction(static_cast<const
// ROOT::Math::IParamMultiFunction &>(ROOT::Math::WrappedMultiTF1(*f1)));
//
//	// error normalization in case of zero error in the data
//	if (fitdata->GetErrorType() == ROOT::Fit::BinData::kNoError)
// fitConfig.SetNormErrors(true);
//	// normalize errors also in case you are fitting a Ndim histo with a N-1
// function 	if (int(fitdata->NDim()) == 0) fitConfig.SetNormErrors(true);
//
//
//	// here need to get some static extra information (like max iterations,
// error def, etc...)
//
//
//	// parameter settings and transfer the parameters values, names and
// limits from the functions
//	// is done automatically in the Fitter.cxx
//	for (int i = 0; i < npar; ++i) {
//		ROOT::Fit::ParameterSettings & parSettings =
// fitConfig.ParSettings(i);
//
//		// check limits
//		double plow, pup;
//		f1->GetParLimits(i, plow, pup);
//		if (plow*pup != 0 && plow >= pup) { // this is a limitation -
// cannot fix a parameter to zero value parSettings.Fix();
//		}
//		else if (plow < pup) {
//			if (!TMath::Finite(pup) && TMath::Finite(plow))
//				parSettings.SetLowerLimit(plow);
//			else if (!TMath::Finite(plow) && TMath::Finite(pup))
//				parSettings.SetUpperLimit(pup);
//			else
//				parSettings.SetLimits(plow, pup);
//		}
//
//		// set the parameter step size (by default are set to 0.3 of
// value)
//		// if function provides meaningful error values
//		double err = f1->GetParError(i);
//		if (err > 0)
//			parSettings.SetStepSize(err);
//		else if (plow < pup && TMath::Finite(plow) &&
// TMath::Finite(pup))
//{ // in case of limits improve step sizes 			double step = 0.1
//* (pup - plow);
//			// check if value is not too close to limit otherwise
// trim
// value 			if (parSettings.Value() < pup && pup -
// parSettings.Value() < 2 * step) 				step = (pup - parSettings.Value()) / 2;
// else
// if (parSettings.Value() > plow && parSettings.Value() - plow < 2 * step)
// step = (parSettings.Value() - plow) / 2;
//
//			parSettings.SetStepSize(step);
//		}
//
//
//	}
//
//	// needed for setting precision ?
//	//   - Compute sum of squares of errors in the bin range
//	// should maybe use stat[1] ??
//	//   Double_t ey, sumw2=0;
//	//    for (i=hxfirst;i<=hxlast;i++) {
//	//       ey = GetBinError(i);
//	//       sumw2 += ey*ey;
//	//    }
//
//
//	// set all default minimizer options (tolerance, max iterations, etc..)
//	fitConfig.SetMinimizerOptions(minOption);
//
//	// specific  print level options
//	if (fitOption.Verbose) fitConfig.MinimizerOptions().SetPrintLevel(3);
//	if (fitOption.Quiet)    fitConfig.MinimizerOptions().SetPrintLevel(0);
//
//	// specific minimizer options depending on minimizer
//	if (linear) {
//		if (fitOption.Robust) {
//			// robust fitting
//			std::string type = "Robust";
//			// if an h is specified print out the value adding to
// the
// type 			if (fitOption.hRobust > 0 && fitOption.hRobust < 1.)
// type += " (h=" + ROOT::Math::Util::ToString(fitOption.hRobust) + ")";
//			fitConfig.SetMinimizer("Linear", type.c_str());
//			fitConfig.MinimizerOptions().SetTolerance(fitOption.hRobust);
//// use tolerance for passing robust parameter
//		}
//		else
//			fitConfig.SetMinimizer("Linear", "");
//	}
//	else {
//		if (fitOption.More) fitConfig.SetMinimizer("Minuit",
//"MigradImproved");
//	}
//
//
//	// check if Error option (run Hesse and Minos) then
//	if (fitOption.Errors) {
//		// run Hesse and Minos
//		fitConfig.SetParabErrors(true);
//		fitConfig.SetMinosErrors(true);
//	}
//
//
//	// do fitting
//	bool fitok = false;
//
//	fitok = fitter.Fit(*fitdata);
//
//	const ROOT::Fit::FitResult & fitResult = fitter.Result();
//
//	std::cout << fitok << " " << fitResult.Chi2() << endl;
//
//	// one could set directly the fit result in TF1
//	int iret = fitResult.Status();
//	if (!fitResult.IsEmpty()) {
//		// set in f1 the result of the fit
//		f1->SetChisquare(fitResult.Chi2());
//		f1->SetNDF(fitResult.Ndf());
//		f1->SetNumberFitPoints(fitdata->Size());
//
//		assert((Int_t)fitResult.Parameters().size() >= f1->GetNpar());
//		f1->SetParameters(const_cast<double*>(&(fitResult.Parameters().front())));
//		if (int(fitResult.Errors().size()) >= f1->GetNpar())
//			f1->SetParErrors(&(fitResult.Errors().front()));
//
//
//	}
//
//	return iret;
//}
//
// int TRestSignalTo2DHitsProcess::Fit(TH1 * h1, TF1 *f1, string option)
//{
//	Foption_t fitOption;
//	ROOT::Fit::FitOptionsMake(ROOT::Fit::kHistogram, option.c_str(),
// fitOption); 	ROOT::Fit::DataOptions opt; 	ROOT::Fit::DataRange
// range(h1->GetBinCenter(0), h1->GetBinCenter(h1->GetNbinsX() + 1));
//
//
//	Int_t special = f1->GetNumber();
//	Bool_t linear = f1->IsLinear();
//	Int_t npar = f1->GetNpar();
//	if (special == 299 + npar)  linear = kTRUE;
//	if (fitOption.Bound || fitOption.Like || fitOption.Errors ||
// fitOption.Gradient || fitOption.More || fitOption.User || fitOption.Integral
//|| fitOption.Minuit) 		linear = kFALSE;
//	{
//		opt.fIntegral = fitOption.Integral;
//		opt.fUseRange = fitOption.Range;
//		opt.fExpErrors = fitOption.PChi2;  // pearson chi2 with expected
// errors 		if (fitOption.Like || fitOption.PChi2) opt.fUseEmpty = true;
// // use empty bins in log-likelihood fits 		if (special == 300)
// opt.fCoordErrors = false; // no need to use coordinate errors in a pol0 fit
// if (fitOption.NoErrX) opt.fCoordErrors = false;  // do not use coordinate
// errors when requested 		if (fitOption.W1) opt.fErrors1 = true;
// if (fitOption.W1 > 1) opt.fUseEmpty = true; // use empty bins with weight=1
//
//		if (fitOption.BinVolume) {
//			opt.fBinVolume = true; // scale by bin volume
//			if (fitOption.BinVolume == 2) opt.fNormBinVolume = true;
//// scale by normalized bin volume
//		}
//
//		if (opt.fUseRange) {
//			GetFunctionRange(*f1, range);
//		}
//	}
//
//
//	ROOT::Fit::BinData data(opt, range);
//	ROOT::Fit::FillData(data, h1, f1);
//	if (data.Size() == 0) {
//		Warning("Fit", "Fit data is empty ");
//		return -1;
//	}
//
//	if (special != 0 && !fitOption.Bound && !linear) {
//		if (special == 100)      ROOT::Fit::InitGaus(data, f1); //
// gaussian 		else if (special == 110 || special == 112)
// ROOT::Fit::Init2DGaus(data, f1); // 2D gaussians ( xygaus or bigaus)
// else if (special == 400)      ROOT::Fit::InitGaus(data, f1); // landau (use
// the same) 		else if (special == 410)      ROOT::Fit::Init2DGaus(data, f1); // 2D
// landau (use the same) 		else if (special == 200)
// ROOT::Fit::InitExpo(data, f1); // exponential
//	}
//
//	auto fitFunction = std::shared_ptr<ROOT::Math::IParamMultiFunction>(new
// ROOT::Math::WrappedMultiTF1(*f1, f1->GetNdim()));
//	ROOT::Fit::Chi2FCN<ROOT::Math::IMultiGenFunction> func(data,
//*fitFunction); 	ROOT::Fit::FitConfig config(f1->GetNpar());
//	ROOT::Math::MinimizerOptions minOption;
//	config.CreateParamsSettings(*fitFunction);
//
//	for (int i = 0; i < f1->GetNpar(); ++i) {
//		ROOT::Fit::ParameterSettings & parSettings =
// config.ParSettings(i);
//
//		// check limits
//		double plow, pup;
//		f1->GetParLimits(i, plow, pup);
//		if (plow*pup != 0 && plow >= pup) { // this is a limitation -
// cannot fix a parameter to zero value parSettings.Fix();
//		}
//		else if (plow < pup) {
//			if (!TMath::Finite(pup) && TMath::Finite(plow))
//				parSettings.SetLowerLimit(plow);
//			else if (!TMath::Finite(plow) && TMath::Finite(pup))
//				parSettings.SetUpperLimit(pup);
//			else
//				parSettings.SetLimits(plow, pup);
//		}
//
//		// set the parameter step size (by default are set to 0.3 of
// value)
//		// if function provides meaningful error values
//		double err = f1->GetParError(i);
//		if (err > 0)
//			parSettings.SetStepSize(err);
//		else if (plow < pup && TMath::Finite(plow) &&
// TMath::Finite(pup))
//{ // in case of limits improve step sizes 			double step = 0.1
//* (pup - plow);
//			// check if value is not too close to limit otherwise
// trim
// value 			if (parSettings.Value() < pup && pup -
// parSettings.Value() < 2 * step) 				step = (pup - parSettings.Value()) / 2;
// else
// if (parSettings.Value() > plow && parSettings.Value() - plow < 2 * step)
// step = (parSettings.Value() - plow) / 2;
//
//			parSettings.SetStepSize(step);
//		}
//
//
//	}
//
//	config.SetMinimizerOptions(minOption);
//
//
//	auto minimizer = std::shared_ptr<TMinuitMinimizer>(new
// TMinuitMinimizer());
//
//	minimizer->UseStaticMinuit(false);
//
//	auto fMinimizerOpts = config.MinimizerOptions();
//
//	minimizer->SetPrintLevel(fMinimizerOpts.PrintLevel());
//	minimizer->SetMaxFunctionCalls(fMinimizerOpts.MaxFunctionCalls());
//	minimizer->SetMaxIterations(fMinimizerOpts.MaxIterations());
//	minimizer->SetTolerance(fMinimizerOpts.Tolerance());
//	minimizer->SetPrecision(fMinimizerOpts.Precision());
//	minimizer->SetValidError(config.ParabErrors());
//	minimizer->SetStrategy(fMinimizerOpts.Strategy());
//	minimizer->SetErrorDef(fMinimizerOpts.ErrorDef());
//
//	minimizer->SetFunction(func);
//
//	minimizer->SetVariables(config.ParamsSettings().begin(),
// config.ParamsSettings().end());
//
//	bool ret = minimizer->Minimize();
//
//	ROOT::Fit::FitResult fitResult;
//
//	fitResult.FillResult(minimizer, config, fitFunction, ret, data.Size(),
// true, NULL);
//
//
//
//	f1->SetChisquare(fitResult.Chi2());
//	f1->SetNDF(fitResult.Ndf());
//	f1->SetNumberFitPoints(data.Size());
//
//	assert((Int_t)fitResult.Parameters().size() >= f1->GetNpar());
//	f1->SetParameters(const_cast<double*>(&(fitResult.Parameters().front())));
//	if (int(fitResult.Errors().size()) >= f1->GetNpar())
//		f1->SetParErrors(&(fitResult.Errors().front()));
//
//	return fitResult.Status();
//
//}

//void TRestSignalTo2DHitsProcess::MakeCluster() {
//    vector<double> xzz;
//    vector<double> xzx;
//    vector<double> xze;
//
//    if (fOutput2DHitsEvent->GetNumberOfXZSignals() > 3) {
//        double baselinemean = 0;
//        double baselinermsmean = 0;
//        // we first make cluster along z(same x)
//        for (int i = 0; i < fOutput2DHitsEvent->GetNumberOfXZSignals(); i++) {
//            auto s = fOutput2DHitsEvent->GetXZSignal(i);
//            auto x = fOutput2DHitsEvent->GetX(i);
//            double baseline = TMath::Mean(s.begin() + fBaseLineRange.X(), s.begin() + fBaseLineRange.Y());
//            double baselinerms =
//                TMath::StdDev(s.begin() + fBaseLineRange.X(), s.begin() + fBaseLineRange.Y());
//            baselinemean += baseline / fOutput2DHitsEvent->GetNumberOfXZSignals();
//            baselinermsmean += baselinerms / fOutput2DHitsEvent->GetNumberOfXZSignals();
//
//            for (int j = fBaseLineRange.Y(); j < s.size(); j++) {
//                if (s[j] > baseline + fSignalThreshold * baselinerms) {
//                    int pos = j;
//                    vector<double> pulse;
//                    pulse.push_back(s[j]);
//                    j++;
//                    while (j < s.size() && s[j] > baseline + fSignalThreshold * baselinerms) {
//                        pulse.push_back(s[j]);
//                        j++;
//                    }
//                    if (pulse.size() > fNPointsOverThreshold) {
//                        auto _e = max_element(begin(pulse), end(pulse));
//                        if (*_e > fSignalThreshold * baselinerms) {
//                            xzz.push_back((double)(distance(std::begin(pulse), _e) + pos));
//                            xzx.push_back(x);
//                            xze.push_back(*_e);
//                        }
//                    }
//                }
//            }
//        }
//
//        // then make cluster along x(same z)
//        int a = fOutput2DHitsEvent->GetZRange().X();
//        int b = fOutput2DHitsEvent->GetZRange().Y();
//        for (int i = a; i <= b;
//             i += (int)(5 * gRandom->Rndm((Long64_t)this) + 10))  // we sample every 15 points
//        {
//            auto hits = fOutput2DHitsEvent->GetXZHitsWithZ(i);
//            map<double, double>::iterator iter = hits.begin();
//            while (iter != hits.end()) {
//                if (iter->second > baselinemean + fSignalThreshold * baselinermsmean) {
//                    double pos = iter->first;
//                    double ene = iter->second;
//                    vector<double> pulse;
//                    pulse.push_back(iter->second);
//                    iter++;
//                    while (iter != hits.end() &&
//                           iter->second > baselinemean + fSignalThreshold * baselinermsmean) {
//                        pulse.push_back(iter->second);
//                        if (iter->second > ene) {
//                            ene = iter->second;
//                            pos = iter->first;
//                        }
//                        iter++;
//                    }
//                    iter--;
//                    if (!TMath::IsNaN(pos)) {
//                        xzz.push_back((double)i);
//                        xzx.push_back(pos);
//                        xze.push_back(ene);
//                    }
//                }
//                iter++;
//            }
//        }
//
//        for (int i = 0; i < xzz.size(); i++) {
//            fOutput2DHitsEvent->AddXZCluster(xzx[i], xzz[i], xze[i]);
//        }
//
//        // tag firing event
//        if (xzz.size() > 10 &&
//            (*max_element(begin(xzz), end(xzz)) - *min_element(begin(xzz), end(xzz))) /
//                    (*max_element(begin(xzx), end(xzx)) - *min_element(begin(xzx), end(xzx))) <
//                0.1) {
//            fOutput2DHitsEvent->SetSubEventTag("firing");
//        }
//    }
//
//    vector<double> yzz;
//    vector<double> yzy;
//    vector<double> yze;
//
//    if (fOutput2DHitsEvent->GetNumberOfYZSignals() > 3) {
//        double baselinemean = 0;
//        double baselinermsmean = 0;
//        // we first make cluster along z(same y)
//        for (int i = 0; i < fOutput2DHitsEvent->GetNumberOfYZSignals(); i++) {
//            auto s = fOutput2DHitsEvent->GetYZSignal(i);
//            auto y = fOutput2DHitsEvent->GetY(i);
//            double baseline = TMath::Mean(s.begin() + fBaseLineRange.X(), s.begin() + fBaseLineRange.Y());
//            double baselinerms =
//                TMath::StdDev(s.begin() + fBaseLineRange.X(), s.begin() + fBaseLineRange.Y());
//            baselinemean += baseline / fOutput2DHitsEvent->GetNumberOfYZSignals();
//            baselinermsmean += baselinerms / fOutput2DHitsEvent->GetNumberOfYZSignals();
//
//            for (int j = fBaseLineRange.Y(); j < s.size(); j++) {
//                if (s[j] > baseline + fSignalThreshold * baselinerms) {
//                    int pos = j;
//                    vector<double> pulse;
//                    pulse.push_back(s[j]);
//                    j++;
//                    while (j < s.size() && s[j] > baseline + fSignalThreshold * baselinerms) {
//                        pulse.push_back(s[j]);
//                        j++;
//                    }
//                    if (pulse.size() > fNPointsOverThreshold) {
//                        auto _e = max_element(begin(pulse), end(pulse));
//                        if (*_e > fSignalThreshold * baselinerms) {
//                            yzz.push_back((double)(distance(std::begin(pulse), _e) + pos));
//                            yzy.push_back(y);
//                            yze.push_back(*_e);
//                        }
//                    }
//                }
//            }
//        }
//
//        // then make cluster along y(same z)
//        int a = fOutput2DHitsEvent->GetZRange().X();
//        int b = fOutput2DHitsEvent->GetZRange().Y();
//        for (int i = a; i <= b;
//             i += (int)(5 * gRandom->Rndm((Long64_t)this) + 10))  // we sample every 15 points
//        {
//            auto hits = fOutput2DHitsEvent->GetYZHitsWithZ(i);
//            map<double, double>::iterator iter = hits.begin();
//            while (iter != hits.end()) {
//                if (iter->second > baselinemean + fSignalThreshold * baselinermsmean) {
//                    double pos = iter->first;
//                    double ene = iter->second;
//                    vector<double> pulse;
//                    pulse.push_back(iter->second);
//                    iter++;
//                    while (iter != hits.end() &&
//                           iter->second > baselinemean + fSignalThreshold * baselinermsmean) {
//                        pulse.push_back(iter->second);
//                        if (iter->second > ene) {
//                            ene = iter->second;
//                            pos = iter->first;
//                        }
//                        iter++;
//                    }
//                    iter--;
//                    if (!TMath::IsNaN(pos)) {
//                        yzz.push_back((double)i);
//                        yzy.push_back(pos);
//                        yze.push_back(ene);
//                    }
//                }
//                iter++;
//            }
//        }
//
//        for (int i = 0; i < yzz.size(); i++) {
//            fOutput2DHitsEvent->AddYZCluster(yzy[i], yzz[i], yze[i]);
//        }
//
//        // tag firing event
//        if (yzz.size() > 10 &&
//            (*max_element(begin(yzz), end(yzz)) - *min_element(begin(yzz), end(yzz))) /
//                    (*max_element(begin(yzy), end(yzy)) - *min_element(begin(yzy), end(yzy))) <
//                0.1) {
//            fOutput2DHitsEvent->SetSubEventTag("firing");
//        }
//    }
//}
//
//TRest2DHitsEvent* TRestSignalTo2DHitsProcess::SelectTag() {
//    if (fOutput2DHitsEvent->GetSubEventTag() == "general")  // if no tags
//    {
//        if (fOutput2DHitsEvent->GetNumberOfSignals() < 6 && zlen < 50)
//        // 1. tag low count event from source
//        {
//            fOutput2DHitsEvent->SetSubEventTag("weak");
//        }
//        if (TMath::IsNaN(zlen) || fOutput2DHitsEvent->GetZRange().X() < 50 ||
//            fOutput2DHitsEvent->GetZRange().X() > 300)
//        // 2. tag abnormal event from source
//        {
//            fOutput2DHitsEvent->SetSubEventTag("abnormal");
//        }
//    }
//
//    fxz->SetParameter(1, -1);
//    fyz->SetParameter(1, -1);
//    fHough_XZ.clear();
//    fHough_YZ.clear();
//    if (fOutput2DHitsEvent->GetSubEventTag() == "general") {
//        MakeCluster();
//        fOutput2DHitsEvent->DoHough();
//        fHough_XZ = fOutput2DHitsEvent->GetHoughXZ();
//        fHough_YZ = fOutput2DHitsEvent->GetHoughYZ();
//    }
//    // 3. fit hough and tag muon/electron event
//    if (fOutput2DHitsEvent->GetSubEventTag() == "general") {
//        if (fHough_XZ.size() > 100) {
//            hxzt->Reset();
//            hxzr->Reset();
//            for (int i = 0; i < fHough_XZ.size(); i++) {
//                hxzt->Fill(fHough_XZ[i].y(), fHough_XZ[i].z());
//                hxzr->Fill(fHough_XZ[i].x(), fHough_XZ[i].z());
//            }
//
//            if (hxzt->GetMaximumBin() == 1) {
//                fOutput2DHitsEvent->SetSubEventTag("firing");
//            }
//
//            fxz->SetParameter(1, hxzt->GetBinCenter(hxzt->GetMaximumBin()));
//            int fitStatus;
//            if (fVerboseLevel >= REST_Debug) {
//                debug << "ID: " << fOutput2DHitsEvent->GetID() << ", hxzt points: " << fHough_XZ.size()
//                      << ", center: " << hxzt->GetBinCenter(hxzt->GetMaximumBin()) << endl;
//                fitStatus = hxzt->Fit(fxz, "N", "", hxzt->GetBinCenter(hxzt->GetMaximumBin()) - 0.2,
//                                      hxzt->GetBinCenter(hxzt->GetMaximumBin()) + 0.2);
//            } else {
//                fitStatus = hxzt->Fit(fxz, "QN", "", hxzt->GetBinCenter(hxzt->GetMaximumBin()) - 0.2,
//                                      hxzt->GetBinCenter(hxzt->GetMaximumBin()) + 0.2);
//            }
//
//            if (fitStatus == 0) {
//                if (fxz->GetParameter(2) < fHoughSigmaLimit) {
//                    double c = fxz->GetParameter(1);
//                    int from = hxzt->FindBin(c - fxz->GetParameter(2) * 2);  // 2 sigma
//                    int to = hxzt->FindBin(c + fxz->GetParameter(2));
//
//                    int sum = hxzt->Integral(from, to);
//
//                    if (sum > (double)hxzt->Integral() * fPeakPointRateLimit)
//                        fOutput2DHitsEvent->SetSubEventTag("muon");
//                } else if (fxz->GetParameter(2) > 0.8 && zlen > 250) {
//                    fOutput2DHitsEvent->SetSubEventTag("electron");
//                }
//            }
//        }
//        if (fHough_YZ.size() > 100) {
//            hyzt->Reset();
//            hyzr->Reset();
//            for (int i = 0; i < fHough_YZ.size(); i++) {
//                hyzt->Fill(fHough_YZ[i].y(), fHough_YZ[i].z());
//                hyzr->Fill(fHough_YZ[i].x(), fHough_YZ[i].z());
//            }
//            if (hyzt->GetMaximumBin() == 1) {
//                fOutput2DHitsEvent->SetSubEventTag("firing");
//            }
//
//            fyz->SetParameter(1, hyzt->GetBinCenter(hyzt->GetMaximumBin()));
//            int fitStatus;
//            if (fVerboseLevel >= REST_Debug) {
//                debug << "ID: " << fOutput2DHitsEvent->GetID() << ", hyzt points: " << fHough_YZ.size()
//                      << ", center: " << hyzt->GetBinCenter(hyzt->GetMaximumBin()) << endl;
//                fitStatus = hyzt->Fit(fyz, "N", "", hyzt->GetBinCenter(hyzt->GetMaximumBin()) - 0.2,
//                                      hyzt->GetBinCenter(hyzt->GetMaximumBin()) + 0.2);
//            } else {
//                fitStatus = hyzt->Fit(fyz, "QN", "", hyzt->GetBinCenter(hyzt->GetMaximumBin()) - 0.2,
//                                      hyzt->GetBinCenter(hyzt->GetMaximumBin()) + 0.2);
//            }
//
//            if (fitStatus == 0) {
//                if (fyz->GetParameter(2) < fHoughSigmaLimit) {
//                    double c = fyz->GetParameter(1);
//                    int from = hyzt->FindBin(c - fyz->GetParameter(2) * 2);  // 2 sigma
//                    int to = hyzt->FindBin(c + fyz->GetParameter(2));
//
//                    int sum = hyzt->Integral(from, to);
//
//                    if (sum > (double)hyzt->Integral() * fPeakPointRateLimit) {
//                        if (fOutput2DHitsEvent->GetSubEventTag() == "electron")
//                            fOutput2DHitsEvent->SetSubEventTag("general");
//                        else
//                            fOutput2DHitsEvent->SetSubEventTag("muon");
//                    }
//
//                } else if (fyz->GetParameter(2) > 0.8 && zlen > 250) {
//                    if (fOutput2DHitsEvent->GetSubEventTag() == "muon")
//                        fOutput2DHitsEvent->SetSubEventTag("general");
//                    else
//                        fOutput2DHitsEvent->SetSubEventTag("electron");
//                }
//            }
//        }
//    }
//
//    if (fOutput2DHitsEvent->GetSubEventTag() == "general")  // 4. tag electron event
//    {
//    }
//
//    // 0: uses all, 1: muon, 2: strong electron, 3: weak electron, 4: firing, 5:
//    // abnormal, 9: other
//    if (Count(fSelection, "0") == 0) {
//        if (Count(fSelection, "4") != 0) {
//            if (fOutput2DHitsEvent->GetSubEventTag() == "firing") {
//                info << "selecting firing event, id " << fOutput2DHitsEvent->GetID() << endl;
//                return fOutput2DHitsEvent;
//            }
//        }
//        if (Count(fSelection, "3") != 0) {
//            if (fOutput2DHitsEvent->GetSubEventTag() == "weak") {
//                info << "selecting event from calibration source, id " << fOutput2DHitsEvent->GetID() << endl;
//                return fOutput2DHitsEvent;
//            }
//        }
//        if (Count(fSelection, "1") != 0) {
//            if (fOutput2DHitsEvent->GetSubEventTag() == "muon") {
//                info << "selecting muon event, id " << fOutput2DHitsEvent->GetID() << endl;
//                return fOutput2DHitsEvent;
//            }
//        }
//        if (Count(fSelection, "2") != 0) {
//            if (fOutput2DHitsEvent->GetSubEventTag() == "electron") {
//                info << "selecting electron event, id " << fOutput2DHitsEvent->GetID() << endl;
//                return fOutput2DHitsEvent;
//            }
//        }
//        if (Count(fSelection, "5") != 0) {
//            if (fOutput2DHitsEvent->GetSubEventTag() == "abnormal") {
//                info << "selecting abnormal trigger, id " << fOutput2DHitsEvent->GetID() << endl;
//                return fOutput2DHitsEvent;
//            }
//        }
//        if (Count(fSelection, "6") != 0) {
//            if (fOutput2DHitsEvent->GetSubEventTag() == "pile up") {
//                info << "selecting pile up event, id " << fOutput2DHitsEvent->GetID() << endl;
//                return fOutput2DHitsEvent;
//            }
//        }
//        if (Count(fSelection, "9") != 0) {
//            if (fOutput2DHitsEvent->GetSubEventTag() == "general") {
//                info << "selecting unknown event, id " << fOutput2DHitsEvent->GetID() << endl;
//                return fOutput2DHitsEvent;
//            }
//        }
//
//        return NULL;
//    }
//    return fOutput2DHitsEvent;
//}
//
//void TRestSignalTo2DHitsProcess::MuDepos(TRest2DHitsEvent* eve) {
//    // if (eve->GetSubEventTag() == "muon")
//    //{
//    //	double firstx = 9999;
//    //	double firsty = 9999;
//
//    //	if (eve->GetZRangeInYZ().X() < eve->GetZRangeInXZ().Y() ||
//    // eve->GetZRangeInXZ().X() < eve->GetZRangeInYZ().Y()) {
//
//    //		if (fxz->GetParameter(1) == -1 && fyz->GetParameter(1) != -1)
//    //		{
//    //			if (xlen > 20)
//    //			{
//    //				double t = (xlen) / (eve->GetZRangeInXZ().Y() -
//    // eve->GetZRangeInXZ().X()); 				fxz->SetParameter(1, atan(t)
//    // + 1.5708);
//    //			}
//    //			else
//    // if(eve->GetZRangeInXZ().Y()-eve->GetZRangeInXZ().X()>100)
//    //			{
//    //				fxz->SetParameter(1, 1.5708);
//    //			}
//    //			else
//    //			{
//    //				return;
//    //			}
//    //		}
//    //		else if (fxz->GetParameter(1) != -1 && fyz->GetParameter(1) ==
//    //-1)
//    //		{
//    //			if (ylen > 20)
//    //			{
//    //				double t = (ylen) / (eve->GetZRangeInYZ().Y() -
//    // eve->GetZRangeInYZ().X()); 				fyz->SetParameter(1, atan(t)
//    // + 1.5708);
//    //			}
//    //			else if (eve->GetZRangeInYZ().Y() -
//    // eve->GetZRangeInYZ().X()>100)
//    //			{
//    //				fyz->SetParameter(1, 1.5708);
//    //			}
//    //			else
//    //			{
//    //				return;
//    //			}
//    //		}
//
//    //		firstx = eve->GetFirstX() - tan(fxz->GetParameter(1)
//    //- 1.5708)*(eve->GetZRangeInXZ().X() - eve->GetZRange().X()); 		firsty
//    //= eve->GetFirstY() - tan(fyz->GetParameter(1)
//    //- 1.5708)*(eve->GetZRangeInYZ().X() - eve->GetZRange().X());
//
//    //		if (eve->GetZRangeInXZ().Y() - eve->GetZRangeInXZ().X() > 345 &&
//    // eve->GetZRangeInXZ().X()>140 && fxz->GetParameter(1) != 1.5708) {
//    // for (int i
//    //= 0; i < 512; i++) { 				auto hitsz = eve->GetXZHitsWithZ(i);
//    // map<double, double>::iterator iter = hitsz.begin();
//    // double sum = 0; 				while (iter != hitsz.end()) {
//    // sum += iter->second; 					iter++;
//    //				}
//    //				//cout << i << " " << sum << endl;
//    //				if (sum < 1e5)
//    //					mudeposxz->SetBinContent(mudeposxz->FindBin(i),
//    // mudeposxz->GetBinContent(mudeposxz->FindBin(i)) + sum);
//    //			}
//    //			longmunumxz++;
//    //		}
//    //		if (eve->GetZRangeInYZ().Y() - eve->GetZRangeInYZ().X() > 345 &&
//    // eve->GetZRangeInYZ().X()>140 && fyz->GetParameter(1) != 1.5708) {
//    // for (int i
//    //= 0; i < 512; i++) { 				auto hitsz = eve->GetYZHitsWithZ(i);
//    // map<double, double>::iterator iter = hitsz.begin();
//    // double sum = 0; 				while (iter != hitsz.end()) {
//    // sum += iter->second; 					iter++;
//    //				}
//    //				if (sum < 1e5)
//    //					mudeposyz->SetBinContent(mudeposyz->FindBin(i),
//    // mudeposyz->GetBinContent(mudeposyz->FindBin(i)) + sum);
//    //			}
//    //			longmunumyz++;
//    //		}
//
//    //		if (firstx > X1 + 30 && firstx < X2 - 30 && firsty > Y1 + 30 &&
//    // firsty < Y2 - 30)
//    //			//the MM pentrating muon
//    //		{
//
//    //			info << "MM pentrating muon" << endl;
//
//    //
//
//    //			//if (eve->GetNumberOfXZSignals()>7 && xlen<15)
//    //			//	return;
//    //			//if (eve->GetNumberOfYZSignals()>7 && ylen<15)
//    //			//	return;
//    //			//if (eve->GetZRangeInXZ().X() > 450 ||
//    // eve->GetZRangeInYZ().X() > 450)
//    //			//	return;
//    //			double tan1 = tan(abs(fxz->GetParameter(1) - 1.5708));
//    //			double tan2 = tan(abs(fyz->GetParameter(1) - 1.5708));
//    //			mutanthe = sqrt(tan1*tan1 + tan2 * tan2);
//
//    //		}
//
//    //	}
//
//    //}
//}

void TRestSignalTo2DHitsProcess::EndProcess() {
    // mudeposxz->Scale(1 / (double)longmunumxz);
    // mudeposxz->SetEntries(longmunumxz);
    // mudeposyz->Scale(1 / (double)longmunumyz);
    // mudeposyz->SetEntries(longmunumyz);

    // mudeposxz->Write();
    // mudeposyz->Write();
}

void TRestSignalTo2DHitsProcess::InitFromConfigFile() {
    // fNoiseReductionLevel = StringToInteger(GetParameter("noiseReduction",
    // "0"));
    fSelection = GetParameter("selection", "0");

    fHoughSigmaLimit = StringToDouble(GetParameter("houghSigmaLimit", "0.05"));
    fPeakPointRateLimit = StringToDouble(GetParameter("peakPointRateLimit", "0.9"));

    TVector2 XROI = StringTo2DVector(GetParameter("XROI", "(-100,100)"));
    TVector2 YROI = StringTo2DVector(GetParameter("YROI", "(-100,100)"));

    X1 = XROI.X();
    X2 = XROI.Y();
    Y1 = YROI.X();
    Y2 = YROI.Y();
}
