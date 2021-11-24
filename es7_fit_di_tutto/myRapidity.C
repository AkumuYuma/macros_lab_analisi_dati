#include <TROOT.h>
#include <TFile.h>
#include <TF1.h>
#include <TH1.h>
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
#include <TAxis.h>

using namespace RooFit;

//PER ESEGUIRE: .L myFinalTestPsiprime.C    main()
void myRapidity(){

  // Operazioni solite di clear e reset
  gROOT->Reset();
  gROOT->Clear();
  
  // Opzioni di stile
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(1111);

  
  // Dichiarazione file
  TFile f1("./root_files/hlt_5_newSoftMuon_alsoInPsiPrimeWind.root","READ");
  
  //char *numeri[22] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23"};
  TString numeri[22] =  {"2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23"};
  
  //Canvas per i plot
  //TCanvas *myC[22];
  TCanvas * myCanvas = new TCanvas("myCanvas", "myCanvas", 300, 500);
  
  //Vettore per grafico
  double vec[22];

  double vec_err_h[22];
  double vec_err_l[22];
  double vec_err[22];

  
  
  
  for (int i = 0; i<22; i++){
  
    // myC[i] = new TCanvas("myC", "Plots", 700, 700);
  
    //Apertura file e lettura istogramma  
    TH1D *histo = (TH1D*)f1.Get("PsiPrimeMass_bin"+numeri[i]);

    //histo->Draw();

    // Crezione oggetto RooDataHist
    RooRealVar x("x", "x", 3.4, 3.96);
    RooDataHist *psiprime = new RooDataHist(histo->GetName(), histo->GetTitle(), RooArgSet(x), RooFit::Import(*histo, kFALSE));

    //Frame for plot e opzioni di stile
    RooPlot *xframe = x.frame(Title(""));
    xframe->SetTitle(" #gamma#mu#mu invariant mass spectrum");
    xframe->SetTitleOffset(1.32,"y");
    xframe->SetLabelSize(0.035, "y");
    xframe->SetTitleSize(0.037, "y");
    xframe->SetYTitle("Candidates/Mev/c^{2}"); //ATTENZIONE A VERO BINNAGGIO
    xframe->SetTitleOffset(1.26,"x");
    xframe->SetXTitle("m(#mu#mu)");

    // Metto il RooDataHisto sul frame
    psiprime->plotOn(xframe);

    // Tolgo titolo istogramma
    char title[128]="";
    psiprime->SetTitle();

    //INIZIO FIT

    // Costruzione segnale  picco
    RooRealVar meanCB1("meanCB1", "meanCB1", 3.675, 3.64, 3.72);
    RooRealVar sigmaCB1("sigmaCB1", "sigmaCB1", 0.040, 0.001, 0.700);
    RooRealVar alpha1("alpha1", "alpha1", 5.0, 0.01, 50.);
    RooRealVar nCB1("nCB1", "nCB1", 3.0, 0.001, 20.);
    RooCBShape myCB1("myCB1", "myCB1", x, meanCB1, sigmaCB1, alpha1, nCB1);


    // Background Chebychev
    // Costruzioen background Chebychev 2: polinomio di secondo grado
    RooRealVar c1("c1", "1st coeff", -0.30, -1000, 100); //originale -0.3
    RooRealVar c2("c2", "2nd coeff", 0.1, -1000, 100); // originale 0.01
    RooChebychev cheby("cheby", "Chebichev 2", x, RooArgList(c1,c2));

    //Background Exponential
    //RooRealVar aExp("aExp", "aExp", 0., -10000., 10000.);
    //RooExponential myExp("myExp", "myExp", x, aExp);

    //Frazioni segnale e background per umbinned fit
    RooRealVar nSig1("nSig1", "Number of firsr signal candidates", 4e+5, 1e+2, 1e+8);
    //RooRealVar nSig2("nSig2", "Number of second signal candidates", 4e+4, 1e+3, 1e+7);
    RooRealVar nBkg("nBkg", "Number of background candidates", 60e+4, 1e+2, 1e+8);
    RooAddPdf *totalPdf = new RooAddPdf("totalPdf", "totalPdf", RooArgList(myCB1, cheby), RooArgList(nSig1, nBkg));
    // Fondo con esponenziale: commenta giu o su a seconda di cosa vuoi
    //RooAddPdf *totalPdf = new RooAddPdf("totalPdf", "totalPdf", RooArgList(myCB1, myCB2, myExp), RooArgList(nSig1, nSig2, nBkg));

    // Operazione di fit e plot su frame di it e componenti
    totalPdf->fitTo(*psiprime, Extended(kTRUE));

    //Riempimento vettore
    vec[i] = sigmaCB1.getValV();
    //vec_err_h[i]= sigmaCB1.getAsymErrorHi();
    //vec_err_l[i]= sigmaCB1.getAsymErrorLo();
    vec_err[i] = sigmaCB1.getError();
    
    totalPdf->plotOn(xframe, RooFit::LineColor(kRed));
    totalPdf->plotOn(xframe, RooFit::Components(RooArgSet(myCB1)),LineColor(kGreen));
    totalPdf->plotOn(xframe, RooFit::Components(RooArgSet(cheby)), RooFit::LineStyle(kDashed));
    // Plot con fondo esponenziale: commenta giu o su a seconda di cosa vuoi
    //totalPdf->plotOn(xframe, RooFit::Components(RooArgSet(myExp)), RooFit::LineStyle(kDashed));

    // Ridisegno il full fit per fare correttamente le pull dopo
    totalPdf->plotOn(xframe, RooFit::LineColor(kRed));
    // Box parametri
    totalPdf->paramOn(xframe, Parameters(RooArgList(meanCB1, sigmaCB1)),Layout(0.50, 0.9, 0.9));

    //myC[i]->cd(1);
    //myC[i]->Update();
    //myC[i]->
    myCanvas->cd();
    xframe->Draw();

    myCanvas->SaveAs("./Plots_buoni/bin"+numeri[i]);
    
    //if (i<21)
    //  myCanvas->Clear();


}

  //for (int i=0;i<22; i++)
    //printf("Valore sigma bin %d : %f\n", i+2, vec[i]);

  double rapidity[22] = {-2.1, -1.9, -1.7, -1.5, -1.3, -1.1, -0.9, -0.7, -0.5, -0.3, -0.1, 0.1, 0.3, 0.5, 0.7, 0.9, 1.1, 1.3, 1.5, 1.7, 1.9, 2.1};
  double rapidity_err[22];
  for (int i=0; i<22; i++) rapidity_err[i]=0.1;

  TGraph *grafico = new TGraph(22, rapidity, vec);
  TGraphAsymmErrors *grafico_errori = new TGraphAsymmErrors(22, rapidity, vec, rapidity_err, rapidity_err, vec_err, vec_err);
  grafico_errori->SetMarkerStyle(20);
  grafico_errori->SetMarkerColor(kRed);

  TCanvas *finale = new TCanvas("final", "final", 700, 700);

  grafico_errori->Draw("AP");

  TF1 *myFunc = new TF1("myFunc", "[0]+ [1]*x + [2]*x*x", -2.2, 2.2);
  myFunc->SetParameters(0.02, 1, 1);
  
  finale->SaveAs("./Plots_buoni/grafico.png");
  delete finale; delete myCanvas; 
}
