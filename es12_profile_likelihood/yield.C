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

void yield() {

  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(10); 
  gStyle->SetTitleOffset(1.25,"y");

  TCanvas* myC = new TCanvas("myC","Plots",1100, 800);

  TFile f1("./root_files/DatasetAandB_KaonTrackRefit_Bwin_new_21aug13.root","READ");
  // Prendo l'istogramma 
  TH1D *hist = (TH1D*)f1.Get("myJpsiKKKmass_all"); 

  RooRealVar x("x","x",5.15,5.45);
  // Creo un roodatahist
  RooDataHist Bmass(hist->GetName(),hist->GetTitle(),RooArgSet(x),RooFit::Import(*hist, kFALSE));
  // Frame per la variabile 
  RooPlot* xframe = x.frame("");
  // Plotto l'istogramma 
  Bmass.plotOn(xframe);

  // -- SIGNAL (gaussiana)
  RooRealVar mg("mg","Gaussian's mean",5.28,5.275,5.285);
  RooRealVar wg("wg","Gaussian's width",0.010,0.005,0.015);
  RooGaussian gauss1("gauss1","Gauss(x,mg,wg)",x,mg,wg);
  // -- BKG (cheby a 2)
  RooRealVar c0("c0","1st coeff",0.5,-1000.,1000.);
  RooRealVar c1("c1","2nd coeff",-0.5,-1000.,1000.);
  RooChebychev cheby("cheby","Chebyshev",x,RooArgList(c0,c1)); // 2 coeff. means 2nd order polynominal

  // -- TOTAL pdf : f*gauss1 + (1-f)*cheby (Questa volta con un solo parametro)
  RooRealVar fsig("fsig","narrow fraction",0.05,0.0,1.0);
  RooAddPdf model("model","gauss1+cheby",RooArgList(gauss1,cheby),fsig); // configured in this way this is not extended

  // -- Execute FIT
  model.fitTo(Bmass,RooFit::Minos(kTRUE));

  // Plot 
  model.plotOn(xframe,RooFit::LineColor(kRed));
  model.plotOn(xframe,RooFit::Components(cheby),RooFit::LineStyle(kDashed));
  model.paramOn(xframe, Parameters(RooArgSet(mg,wg,fsig)), Layout(0.53,0.9,0.9)); 

  xframe->SetTitle("Not extended fit : just fsig and (1-fsig)");
  myC->cd();
  xframe->Draw();

  myC->SaveAs("./Plots/myBmass.png");
  myC->Update();


  //------------------------------------------ extended fit :
  // Rifaccio lo stesso procedimento ma con il fit esteso
  myC->Divide(1,1);
  RooRealVar y("y","y",5.15,5.45); // Variabile 
  RooDataHist BmassExt(hist->GetName(),hist->GetTitle(),RooArgSet(y),RooFit::Import(*hist, kFALSE));
  RooPlot* yframe = y.frame("");
  BmassExt.plotOn(yframe);

  // Sig (gauss)
  RooRealVar mge("mge","Gaussian's mean",5.28,5.275,5.285);
  RooRealVar wge("wge","Gaussian's width",0.01,0.005,0.015);
  RooGaussian gausse("gausse","Gauss(y,mge,wge)",y,mge,wge);

  // bkg (Cheby a 2)
  RooRealVar c0e("c0e","1st coeff",0.5,-1000,1000);
  RooRealVar c1e("c1e","2nd coeff",-0.5,-1000,1000);

  RooChebychev chebye("chebye","Chebyshev",y,RooArgList(c0e,c1e));

  // Due parametri per il modello
  RooRealVar nsig("nsig","sig fraction",2500.,2000.,3800.);
  RooRealVar nbkg("nbkg","bkg fraction",2000.,0.,200000.);

  RooAddPdf model_extended("model_extended","gauss+cheby EXT",RooArgList(gausse,chebye),RooArgList(nsig,nbkg));

  // Fitto usando l'interfaccia di Minuit 
  RooAbsReal* nll = model_extended.createNLL(BmassExt);
  RooMinuit m(*nll);
  m.migrad();
  m.hesse();
  m.minos();  // get all asymmetric
  
  // Stampo il valore di nsig
  nsig.Print();

  RooFitResult* fitres = m.save(); // Salvo i risultati del fit 

  gStyle->SetPalette(1); //- for better color choice 
  fitres->correlationHist()->Draw("colz"); // Disegno l'istogramma di correlazione 
  myC->SaveAs("./Plots/myCorrelationMatrix.png");
  myC->Update();
  myC->cd();

  // Plotto il modello  
  model_extended.plotOn(yframe,RooFit::LineColor(kRed));
  model_extended.plotOn(yframe,RooFit::Components(chebye),RooFit::LineStyle(kDashed));
  model_extended.paramOn(yframe, Parameters(RooArgSet(mge,wge,nsig,nbkg)), Layout(0.53,0.9,0.9)); // 2nd is right
  yframe->SetTitle("Extended fit : nsig and nbkg");
  yframe->Draw();

  myC->SaveAs("./Plots/myBmassExtended.png");
  myC->Update();
  myC->cd();

  model_extended.plotOn(yframe, VisualizeError(*fitres));
  yframe->Draw();  
  BmassExt.plotOn(yframe);
  yframe->Draw("Esame");
  myC->SaveAs("./Plots/myErrorVisualization.png");
  myC->Update();
  myC->cd();

  RooPlot* paramFrame = new RooPlot(nsig,wge);
  fitres->plotOn(paramFrame,nsig,wge);
  paramFrame->Draw();
  myC->SaveAs("./Plots/pdfParamVisualization_nsig_wge.png"); 
  myC->Update();
  myC->cd();
  //     
  m.contour(nsig,wge);
  myC->SaveAs("./Plots/paramContours_nsig_wge.png");
  myC->Update();
  myC->cd();

  // Nota: plottando i risultati del fit nello spazio di nsig e wge e chiamando il metodo contour ottengo la stessa cosa. 

  //------- now plot Likelihood and Profile Likelihood Ratio functions : 

  myC->Divide(1,1);
  //
  // plot the likelihood as a function of the parameter of interest (here nsig):

  // Creo un frame per nsig (è questo il parametro di interesse)
  RooPlot* nsig_frame = nsig.frame(RooFit::Bins(60),RooFit::Range(2000,3800));

  // Disegno sullo stesso frame la nll e la profile 

  // Negative log likelihood (preso prima)
  nll->plotOn(nsig_frame,RooFit::ShiftToZero(),RooFit::LineStyle(kDashed),LineColor(kBlue));

  // make the Profile Likelihood ratio (that can be represented as a regular RooFit function)
  RooAbsReal* pll_nsig = nll->createProfile(nsig); 
  // Plotto 
  pll_nsig->plotOn(nsig_frame,RooFit::ShiftToZero(),LineColor(kRed));
  nsig_frame->SetMinimum(-1);
  nsig_frame->SetMaximum(5);
  nsig_frame->Draw();

  // Linee 
  TLine *line0 = new TLine(2000,0,3800,0);
  line0->SetLineColor(1);
  line0->SetLineWidth(2);
  line0->SetLineStyle(2);
  line0->Draw("same");

  TLine *line05 = new TLine(2000,0.5,3800,0.5);
  line05->SetLineColor(1);
  line05->SetLineWidth(2);
  line05->SetLineStyle(2);
  line05->Draw("same");
  //
  TLine *lineN1 = new TLine(3100,-1.,3100,0.5);
  lineN1->SetLineColor(2);
  lineN1->SetLineWidth(1);
  lineN1->SetLineStyle(2);
  lineN1->Draw("same");
  //
  TLine *lineN2 = new TLine(2749,-1.,2749,0.5);
  lineN2->SetLineColor(2);
  lineN2->SetLineWidth(1);
  lineN2->SetLineStyle(2);
  lineN2->Draw("same");
  //
  myC->SaveAs("./Plots/myLikelihood.png");
  myC->Update();
  myC->cd();

  delete myC;

  gROOT->Reset();
  gROOT->Clear();

}
