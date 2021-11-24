///////////////////////////////////
// run with root: .x phiKK_fit.C
///////////////////////////////////
#include <vector>
#include "TH1.h"

using namespace RooFit;

void phiKK_fit() {
  //
  gROOT->Reset();
  gROOT->Clear(); 
  gROOT->ForceStyle();
  gStyle->SetTitleOffset(1.4, "Y");
  gStyle->SetOptFit(1);
  //
  TFile f1{"./root_files/phiKK.root","read"};
  //
  /////////////////////// PREPARE HISTO
  //
  TH1F * hPhiKK{(TH1F*) f1.Get("CW_PhiMass_NoM")};
  //
  TCanvas *myC = new TCanvas("myC","PhiMassPlot", 900, 680); 
  //  
  // Double_t xMin = hPhiKK->GetXaxis()->GetXmin(); 
  // Double_t xMax = hPhiKK->GetXaxis()->GetXmax();
  Int_t nBins = hPhiKK->GetNbinsX();
  //
  RooRealVar xVar("xVar", "m(K^{+}K^{-}) [GeV/c^{2}]", 0.9865, 1.0665); 
  xVar.setBins((nBins/10)*0.2);  
  // 
  RooDataHist* KKHist = new RooDataHist("KK_hist", hPhiKK->GetTitle(), RooArgSet(xVar), Import(*hPhiKK,kFALSE));
  //
  //////////////////////// CONFIGURE and MAKE FIT
  //
  // --- SIG
  RooRealVar mean("mean","mean of voigtian", 1.0197, 1.019, 1.0205);
  //RooRealVar mean("mean","mean of voigtian", 1.0197); // value works!
  RooRealVar gamma("gamma","width of BW", 0.0045, 0.001, 0.01);
  RooRealVar sigma("#sigma", "mass resolution", 0.0013); // to set at the MC value of 1.3MeV
  // 
  RooVoigtian sigPDF("sigPDF","sigPDF", xVar, mean, gamma, sigma);
  //
  // --- BKG
  RooRealVar c0("c_{0}", "c0", 0.001 ,-15, 15);
  RooRealVar c1("c_{1}", "c1", -0.1 ,-15, 15);
  RooRealVar c2("c_{2}", "c2", -0.1 ,-15, 15);
  RooRealVar c3("c_{3}", "c3", 0.1 ,-10, 10);
  RooRealVar c4("c_{4}", "c4", 0.1 ,-10, 10);
  RooRealVar c5("c_{5}", "c5", 0.05 ,-10, 10);   
  RooRealVar c6("c_{6}", "c6", 0.0 ,-1, 1);
  //
  //////RooChebychev bkgPDF("bkgPDF", "bkgPDF", xVar, RooArgSet(c0,c1,c2,c3,c4,c5,c6)); // works awful
  RooChebychev bkgPDF("bkgPDF", "bkgPDF", xVar, RooArgSet(c0,c1,c2,c3,c4,c5) ); // enough well (*)
  //// RooChebychev bkgPDF("bkgPDF", "bkgPDF", xVar, RooArgSet(c0,c1,c2,c3) ); // not bad but needs extra degrees of freedom
  //
  //////RooBernstein bkgPDF("bkgPDF", "bkgPDF", xVar, RooArgSet(c0,c1,c2,c3,c4,c5,c6));
  //////RooBernstein bkgPDF("bkgPDF", "bkgPDF", xVar, RooArgSet(c0,c1,c2,c3,c4,c5) );
  //
  //////////
  RooRealVar a0("a_{0}", "a0", 0.001 ,-1., 1.);
  RooRealVar a1("a_{1}", "a1", 0.001 ,-0.5, 0.5);
  RooRealVar a2("a_{2}", "a2", -0.0001 ,-2., 2.);
  RooRealVar a3("a_{3}", "a3", 0.0);
  RooRealVar a4("a_{4}", "a4", 0.0 ,-0.1, 0.1);
  RooRealVar a5("a_{5}", "a5", 0.0 ,-0.025, 0.05);
  RooRealVar a6("a_{6}", "a6", 0.0 ,-0.001, 0.001);
  //RooChebychev bkgPDF("bkgPDF", "bkgPDF", xVar, RooArgSet(a0,a1,a2,a3,a4,a5,a6)); // bit worse than (*)
  //
  RooRealVar nSig("nSig", "Number of signal candidates ", 5e+5, 1000., 5e+6);
  RooRealVar nBkg("nBkg", "Bkg component", 2e+6, 1000., 5e+7); 
  //
  RooAddPdf* totalPDF = new RooAddPdf("totalPDF", "totalPDF", RooArgList(sigPDF, bkgPDF), RooArgList(nSig, nBkg));     
  //
  totalPDF->fitTo(*KKHist, Extended(kFALSE), Minos(kTRUE));
  //
  //////////////////////// PLOT RESULTS
  //
  RooPlot* xframe = xVar.frame();
  //xframe->SetTitle( hPhiKK->GetTitle() );
  xframe->SetTitle(""); 
  xframe->SetYTitle("Candidates / ? MeV/c^{2}"); 
  //xframe->SetTitleOffset(1.45,"Y");
  //
  KKHist->plotOn(xframe);
  totalPDF->plotOn(xframe);
  //
  totalPDF->plotOn(xframe, Components(RooArgSet(sigPDF)), LineColor(kRed));
  totalPDF->plotOn(xframe, Components(RooArgSet(bkgPDF)), LineColor(kGreen), LineStyle(kDashed) );
  totalPDF->paramOn(xframe, Parameters(RooArgSet(mean,sigma,gamma,nSig)), Layout(0.52,0.99,0.9));
  //
  totalPDF->plotOn(xframe);
  //
  //myC->cd();
  //xframe->Draw(); // add the pulls bin-by-bin instead:
  //
  RooPlot* framePull = xVar.frame();
  framePull->SetTitle("Pulls bin-by-bin");
  framePull->addObject( (TObject*)xframe->pullHist(), "p" );
  framePull->SetMinimum(-6);
  framePull->SetMaximum(6);
  //
  myC->Divide(0,2);
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


  myC->cd(1);
  gPad->SetPad(0.,0.3,1.,1.);
  xframe->Draw();
  //
  ///////////////////////// SAVE IN OUTPUT PLOT
  //
  myC->SaveAs("./Plots/PhiKK-Voigtian.png");
  //
  delete myC; 
//////////////////////////////////////////////////////////////////
}

