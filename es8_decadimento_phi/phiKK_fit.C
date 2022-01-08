///////////////////////////////////
// run with root: .x phiKK_fit.C
///////////////////////////////////
#include <vector>
#include "TH1.h"

using namespace RooFit;

void phiKK_fit() {

  gROOT->Reset();
  gROOT->Clear(); 
  gROOT->ForceStyle();
  gStyle->SetTitleOffset(1.4, "Y");
  gStyle->SetOptFit(1);

  TFile f1{"./root_files/phiKK.root","read"};
  TCanvas *myC = new TCanvas("myC","PhiMassPlot", 900, 680); 

  TH1F * hPhiKK{(TH1F*) f1.Get("CW_PhiMass_NoM")};
  Int_t nBins = hPhiKK->GetNbinsX();

  // Variabile 
  RooRealVar xVar("xVar", "m(K^{+}K^{-}) [GeV/c^{2}]", 0.9865, 1.0665); 
  xVar.setBins((nBins/10)*0.2); // A cazzo di cane?  

  RooDataHist* KKHist = new RooDataHist("KK_hist", hPhiKK->GetTitle(), RooArgSet(xVar), Import(*hPhiKK,kFALSE));


  // --- SIG (Voigtiana)
  // Parametri 
  RooRealVar mean("mean","mean of voigtian", 1.0197, 1.019, 1.0205);
  RooRealVar gamma("gamma","width of BW", 0.0045, 0.001, 0.01);
  RooRealVar sigma("#sigma", "mass resolution", 0.0013); // to set at the MC value of 1.3MeV
  // Pdf 
  RooVoigtian sigPDF("sigPDF","sigPDF", xVar, mean, gamma, sigma);

  // --- BKG (cheby a 5)
  // Param 
  RooRealVar c0("c_{0}", "c0", 0.001 ,-15, 15);
  RooRealVar c1("c_{1}", "c1", -0.1 ,-15, 15);
  RooRealVar c2("c_{2}", "c2", -0.1 ,-15, 15);
  RooRealVar c3("c_{3}", "c3", 0.1 ,-10, 10);
  RooRealVar c4("c_{4}", "c4", 0.1 ,-10, 10);
  RooRealVar c5("c_{5}", "c5", 0.05 ,-10, 10);   
  RooRealVar c6("c_{6}", "c6", 0.0 ,-1, 1); // Non usato (con 6 viene un pochino peggio il fit)
  // Pdf 
  RooChebychev bkgPDF("bkgPDF", "bkgPDF", xVar, RooArgSet(c0,c1,c2,c3,c4,c5)); // enough well (*)

  // Pdf totale 
  // Param 
  RooRealVar nSig("nSig", "Number of signal candidates ", 5e+5, 1000., 5e+6);
  RooRealVar nBkg("nBkg", "Bkg component", 2e+6, 1000., 5e+7); 
  // Pdf 
  RooAddPdf totalPDF{"totalPDF", "totalPDF", RooArgList(sigPDF, bkgPDF), RooArgList(nSig, nBkg)};     

  // Fit  
  totalPDF.fitTo(*KKHist, Extended(kFALSE), Minos(kTRUE));

  // Plot 
  RooPlot* xframe = xVar.frame();
  xframe->SetTitle(""); 
  xframe->SetYTitle("Candidates / 0.667 MeV/c^{2}"); 
  // Plotto istogramma 
  KKHist->plotOn(xframe);
  // Plotto la pdf (e le sue componenti)
  totalPDF.plotOn(xframe);
  totalPDF.plotOn(xframe, Components(RooArgSet(sigPDF)), LineColor(kRed));
  totalPDF.plotOn(xframe, Components(RooArgSet(bkgPDF)), LineColor(kGreen), LineStyle(kDashed) );
  totalPDF.paramOn(xframe, Parameters(RooArgSet(mean,sigma,gamma,nSig)), Layout(0.52,0.99,0.9));
  totalPDF.plotOn(xframe); // Per le pull


  // Pull e disegno 
  RooPlot* framePull = xVar.frame();
  framePull->SetTitle("Pulls bin-by-bin");
  framePull->addObject( (TObject*)xframe->pullHist(), "p" );
  framePull->SetMinimum(-6);
  framePull->SetMaximum(6);

  myC->Divide(0,2);

  // Istogramma e pdf 
  myC->cd(1);
  gPad->SetPad(0.,0.3,1.,1.);
  xframe->Draw();

  // Pull
  myC->cd(2);
  gPad->SetPad(0.,0.,1.,0.3);
  framePull->Draw();

  // Linee sul grafico dei pull
  TLine lineplus{0.9865, 3, 1.0665, 3};
  TLine lineminus{0.9865, - 3, 1.0665, - 3};
  TLine linezero{0.9865, 0, 1.0665, 0};

  lineplus.SetLineStyle(2);
  lineplus.SetLineColor(2);
  lineplus.Draw("same");

  lineminus.SetLineStyle(2);
  lineminus.SetLineColor(2);
  lineminus.Draw("same");

  linezero.SetLineStyle(2);
  linezero.SetLineColor(4);
  linezero.Draw("same");


  myC->SaveAs("./Plots/PhiKK-Voigtian.png");
  delete myC; 
}

