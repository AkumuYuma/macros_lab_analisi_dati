////////////////////////
// To run it:
// root> .L yield.C
// root> main()
//
///////////////////////

#include <TROOT.h>
#include <TFile.h>
#include <TH1.h>
#include <TF1.h>
#include <TF2.h>
#include <TFormula.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TProfile.h>
#include <TString.h>
#include <TLine.h>
#include <TPad.h>
#include <TMath.h>
#include <TLatex.h>
#include <TLegend.h>
#include <iostream>
#include <TColor.h>
#include "TAxis.h"

using namespace RooFit;

/////////////////////////---inizio main

int main() {

  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(10); 
  gStyle->SetTitleOffset(1.25,"y");
  //
  TCanvas* myC = new TCanvas("myC","Plots",1000, 800);
  //
  ///////////////////////////////////////////////////
  //
  TFile f1("DatasetAandB_KaonTrackRefit_Bwin_new_21aug13.root","READ");
  TH1D *hist = (TH1D*)f1.Get("myJpsiKKKmass_all"); 
  // in alternativa c'e' anche l'istogramma myJpsiKKKmass_tight
  //
  ///////////////////////////////////////////////////
  //
  RooRealVar x("x","x",5.15,5.45);
  RooDataHist Bmass(hist->GetName(),hist->GetTitle(),RooArgSet(x),RooFit::Import(*hist, kFALSE));
  //
  RooPlot* xframe = x.frame("");
  Bmass.plotOn(xframe);
  //// 
  myC->cd();
  //xframe->Draw();
  //
  // -- SIGNAL
  RooRealVar mg("mg","Gaussian's mean",5.28,5.275,5.285);
  RooRealVar wg("wg","Gaussian's width",0.010,0.005,0.015);
  RooGaussian gauss1("gauss1","Gauss(x,mg,wg)",x,mg,wg);
  // -- BKG
  RooRealVar c0("c0","1st coeff",0.5,-1000.,1000.);
  RooRealVar c1("c1","2nd coeff",-0.5,-1000.,1000.);
  //--RooRealVar c2("c2","3rd coeff",0.1,-1000.,1000.);
  RooChebychev cheby("cheby","Chebyshev",x,RooArgList(c0,c1)); // 2 coeff. means 2nd order polynominal
  //
  // -- TOTAL pdf : f*gauss1 + (1-f)*cheby
  RooRealVar fsig("fsig","narrow fraction",0.05,0.0,1.0);
  RooAddPdf model("model","gauss1+cheby",RooArgList(gauss1,cheby),fsig); // configured in this way this is not extended
  //
  // -- Execute FIT
  model.fitTo(Bmass,RooFit::Minos(kTRUE));
  model.plotOn(xframe,RooFit::LineColor(kRed));
  model.plotOn(xframe,RooFit::Components(cheby),RooFit::LineStyle(kDashed));
  model.paramOn(xframe, Parameters(RooArgSet(mg,wg,fsig)), Layout(0.53,0.9,0.9)); // 3rd is up
  //
  xframe->SetTitle("Not extended fit : just fsig and (1-fsig)");
  xframe->Draw();
  //
  myC->SaveAs("./myBmass.png");
  myC->Update();
  myC->cd();
  //
  /////////////////////////////////////////// extended fit :
  //
  myC->Divide(1,1);
  RooRealVar y("y","y",5.15,5.45);
  RooDataHist BmassExt(hist->GetName(),hist->GetTitle(),RooArgSet(y),RooFit::Import(*hist, kFALSE));
  RooPlot* yframe = y.frame("");
  BmassExt.plotOn(yframe);
  myC->cd();
  yframe->Draw();
  //
  RooRealVar mge("mge","Gaussian's mean",5.28,5.275,5.285);
  RooRealVar wge("wge","Gaussian's width",0.010,0.005,0.015);
  RooGaussian gausse("gausse","Gauss(y,mge,wge)",y,mge,wge);
  //
  RooRealVar c0e("c0e","1st coeff",0.5,-1000,1000);
  RooRealVar c1e("c1e","2nd coeff",-0.5,-1000,1000);
  //--RooRealVar c1e("c2e","3rd coeff",-0.5,-1000,1000)
  //
  RooChebychev chebye("chebye","Chebyshev",y,RooArgList(c0e,c1e));
  RooRealVar nsig("nsig","sig fraction",500.,0.,5000.);
  RooRealVar nbkg("nbkg","bkg fraction",2000.,0.,200000.);
  //
  RooAddPdf model_extended("model_extended","gauss+cheby EXT",RooArgList(gausse,chebye),RooArgList(nsig,nbkg));
  //
  RooAbsReal* nll = model_extended.createNLL(BmassExt);
  RooMinuit m(*nll);
  m.migrad();
  m.hesse();
  m.minos();
  //
  model_extended.plotOn(yframe,RooFit::LineColor(kRed));
  model_extended.plotOn(yframe,RooFit::Components(chebye),RooFit::LineStyle(kDashed));
  model_extended.paramOn(yframe, Parameters(RooArgSet(mge,wge,nsig,nbkg)), Layout(0.53,0.9,0.9)); // 2nd is right
  yframe->SetTitle("Extended fit : nsig and nbkg");
  yframe->Draw();
  //
  myC->SaveAs("./myBmassExtended.png");
  myC->Update();
  myC->cd();
  //
  //////////////// now plot Likelihood and Profile Likelihood Ratio functions : 
  //
  myC->Divide(1,1);
  //
  // plot the likelihood as a function of the parameter of interest (here nsig):
  RooPlot* nsig_frame = nsig.frame(RooFit::Bins(60),RooFit::Range(2000,4000));
  nll->plotOn(nsig_frame,RooFit::ShiftToZero(),RooFit::LineStyle(kDashed),LineColor(kBlue));
  //
  // make the Profile Likelihood ratio (that can be represented as a regular RooFit function)
  RooAbsReal* pll_nsig = nll->createProfile(nsig);
  pll_nsig->plotOn(nsig_frame,RooFit::ShiftToZero(),LineColor(kRed));
  nsig_frame->SetMinimum(-1);
  nsig_frame->SetMaximum(5);
  nsig_frame->Draw();
  //
  TLine *line0 = new TLine(2000,0,4000,0);
  line0->SetLineColor(1);
  line0->SetLineWidth(0.9);
  line0->SetLineStyle(2);
  line0->Draw("same");
  //
  TLine *line05 = new TLine(2000,0.5,4000,0.5);
  line05->SetLineColor(1);
  line05->SetLineWidth(0.9);
  line05->SetLineStyle(2);
  line05->Draw("same");
  //
  TLine *lineN1 = new TLine(3100,-1.,3100,0.5);
  lineN1->SetLineColor(2);
  lineN1->SetLineWidth(1.0);
  lineN1->SetLineStyle(2);
  lineN1->Draw("same");
  //
  TLine *lineN2 = new TLine(2750,-1.,2750,0.5);
  lineN2->SetLineColor(2);
  lineN2->SetLineWidth(1.0);
  lineN2->SetLineStyle(2);
  lineN2->Draw("same");
  //
  myC->SaveAs("./myLikelihood.png");
  myC->Update();
  myC->cd();
  //
  delete myC;
  //
  gROOT->Reset();
  gROOT->Clear();
  //
return 0;
}
