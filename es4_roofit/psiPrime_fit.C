///////////////////////////////////
// run with root: .x psiPrime_fit.C
///////////////////////////////////
#include <vector>
#include <TH1.h>

//gROOT->Reset();
//gROOT->Clear();


using namespace RooFit;

void psiPrime_fit() {

  gStyle->SetTitleOffset(1.4, "Y"); // Il secondo parametro è l'asse su cui fare l'offset 
  gStyle->SetOptFit(1);

  gROOT->ForceStyle(); // Questo forza la lettura e scrittura su file con gli attributi grafici presenti 
  TFile* f1 = TFile::Open("./root_files/hlt_5_newSoftMuon_alsoInPsiPrimeWind.root","read");


  TH1F* hPsiPrime = dynamic_cast<TH1F*>(f1->Get("PsiPrimeMass_bin9"));

  Double_t xMin = hPsiPrime->GetXaxis()->GetXmin();
  Double_t xMax = hPsiPrime->GetXaxis()->GetXmax();
  Int_t nBins = hPsiPrime->GetNbinsX(); // Salvo il numero di bin 

  TCanvas *myC = new TCanvas("myC","PsiPrimeMassPlot", 700, 700);

  // Creo una variabile che rappresenti la massa 
  RooRealVar xVar("xVar", "m(#mu^{+}#mu^{-}) [GeV/c^{2}]", xMin, xMax);
  // Rendo la variabile reale binnata. 
  xVar.setBins(nBins);

  RooDataHist* MuMuHist = new RooDataHist("#mu#mu_hist", hPsiPrime->GetTitle(), RooArgSet(xVar), Import(*hPsiPrime,kFALSE));

  // PDF SEGNALE (Gaussiana)
  // Parametri: 
  // Il primo parametro numerico è il valore iniziale, il secondo è e il terzo sono i limiti
  RooRealVar mG("mean", "mean", 3.62, 3.60, 3.75); // Valore della media. Fisso il valore iniziale del parametro
  RooRealVar sigma1("#sigma_{1}", "sigma1", 0.02, 0.001, 0.1); // Valore della sigma

  //  pdf 
  RooGaussian sigPDF("sigPDF", "Signal", xVar, mG, sigma1);

  // PDF background
  // Parametri: 
  RooRealVar c1("c_{1}", "c1", -0.1 ,-10, 10);
  RooRealVar c2("c_{2}", "c2", -0.1 ,-10, 10);
  // pdf 
  RooChebychev bkgPDF("bkgPDF", "bkgPDF", xVar, RooArgSet(c1,c2));

  // Sovrappongo con i pesi 
  RooRealVar nSig("nSig", "Number of signal candidates ", 2e+5, 1., 1e+6);
  RooRealVar nBkg("nBkg", "Bkg component", 120e+3, 1., 1e+6);

  // Creo la pdf somma 
  RooArgList listaPdf{sigPDF, bkgPDF}; 
  RooArgList listaPesi{nSig, nBkg}; 
  RooAddPdf* totalPDF = new RooAddPdf("totalPDF", "totalPDF", listaPdf, listaPesi);

  // Fitting 
  totalPDF->fitTo(*MuMuHist, Extended(kTRUE), Minos(kTRUE)); // Con questo fitta con Minos

  // Plotting
  RooPlot* xframe = xVar.frame();
  xframe->SetTitle( hPsiPrime->GetTitle() );
  xframe->SetYTitle("Candidates / 10 MeV/c^{2}");

  MuMuHist->plotOn(xframe);

  totalPDF->plotOn(xframe);
  totalPDF->plotOn(xframe, Components(RooArgSet(sigPDF)), LineColor(kRed));  // Plotta la componente di segnale
  totalPDF->plotOn(xframe, Components(RooArgSet(bkgPDF)), LineColor(kGreen), LineStyle(kDashed) ); // Plotta la componenete di background 

  // Riscrive la pdf.
  // Se devi fare il pull questa riga deve essere l'ultima a fare plotOn così quando usi il metodo pullHist() fa gli scarti con questo
  totalPDF->plotOn(xframe); // non necessario: il fondo e' tratteggiato

  // Scrivo i parametri 
  totalPDF->paramOn(xframe, Parameters(RooArgSet(mG,sigma1,nSig)), Layout(0.52,0.99,0.9)); //box con stime parametri

  myC->cd();

  //  ------------------------------ Parte sulle pull (esercitazione 4B)
  //  Aggiungere codice da pdf
  RooPlot *framePull = xVar.frame();
  framePull->SetTitle("Pulls bin-by-bin");

  // quando faccio xframe->pullHist() lui calcola gli scarti tra l'istogramma e l'ultima pdf in xframe.
  // Se vedi, l'ultima cosa è il fondo alla riga 74, quindi calcolerebbe gli scarti tra il fondo e l'istogramma.
  // Ecco perchè rifaccio totalPDF->plotOn(xframe);
  framePull->addObject( dynamic_cast<TObject *>(xframe->pullHist()), "p");
  framePull->SetMinimum(-6);
  framePull->SetMaximum(6);

  myC->Divide(0, 2);
  myC->cd(2);
  gPad->SetPad(0., 0., 1., 0.3);
  framePull->Draw();
  TLine *line = new TLine{3.4, 0., 4., 0.};
  line->SetLineColor(2);
  line->Draw("same");
  myC->cd(1);
  gPad->SetPad(0., 0.3, 1, 1);
  xframe->Draw();
  //
  // myC->Update();
  myC->SaveAs("./Plots/PsiPrimeMassFit_gauss_poly1ord.png");
  //
  myC->Clear(); // Pulisco il canvas che poi riscrivo qua


  ////////////////////////////////////////////////////////////////// CB+exp //

  RooAbsPdf *sigCBPdf ;
  RooAbsPdf *bkgExpPdf ;
  //
  RooRealVar mGCB("mean", "meanCB", 3.7, 3.67, 3.73);
  RooRealVar sigma1CB("#sigma_{CB}", "sigma1CB", 0.02, 0.001, 0.1);
  RooRealVar alpha("#alpha","alpha",2., 0.5, 10.);
  RooRealVar nSigma("n#sigma","nSigma", 2., 0.1, 25.);
  sigCBPdf = new RooCBShape("sigCBPdf","sigCBPdf",xVar,mGCB,sigma1CB,alpha,nSigma);
  //
  RooRealVar gamma("#Gamma","Gamma",-1e-1, -2., -1e-2) ;
  bkgExpPdf = new RooExponential("bkgExpPdf","bkgExpPdf",xVar, gamma);
  RooRealVar nBkgExp("nbkg","",120e+3,1.,1e+6);
  RooRealVar nSigCB("N_{SIG}","",2e+5,1.,1e+6);
  //
  RooAddPdf* totalCBExpPDF = new RooAddPdf("totalCBExpPDF","totalCBExpPDF", RooArgList(*sigCBPdf, *bkgExpPdf), RooArgList(nSigCB, nBkgExp));
  //
  totalCBExpPDF->fitTo(*MuMuHist, Extended(kTRUE));
  //
  RooPlot* xframe1 = xVar.frame();
  xframe1->SetTitle( hPsiPrime->GetTitle() );
  xframe1->SetYTitle("Candidates / 10 MeV/c^{2}");
  MuMuHist->plotOn(xframe1);
  totalCBExpPDF->plotOn(xframe1);
  //
  totalCBExpPDF->plotOn(xframe1, Components(RooArgSet(*sigCBPdf)), LineColor(kRed));
  totalCBExpPDF->plotOn(xframe1, Components(RooArgSet(*bkgExpPdf)), LineColor(kGreen), LineStyle(kDashed) );
  totalCBExpPDF->plotOn(xframe1); // Riga magica per far venire bene i pull
  //
  totalCBExpPDF->paramOn(xframe1, Parameters(RooArgSet(mGCB,sigma1CB,alpha,nSigma,nSigCB,gamma)), Layout(0.529,0.99,0.9)); //box con stime parametri
  //

  RooPlot *framePull1 = xVar.frame();
  framePull1->SetTitle("Pulls bin-by-bin");

  // quando faccio xframe->pullHist() lui calcola gli scarti tra l'istogramma e l'ultima pdf in xframe.
  // Se vedi, l'ultima cosa è il fondo alla riga 74, quindi calcolerebbe gli scarti tra il fondo e l'istogramma.
  // Ecco perchè rifaccio totalPDF->plotOn(xframe);
  framePull1->addObject( dynamic_cast<TObject *>(xframe1->pullHist()), "p");
  framePull1->SetMinimum(-6);
  framePull1->SetMaximum(6);

  myC->Divide(0, 2);
  myC->cd(2);
  gPad->SetPad(0., 0., 1., 0.3);
  framePull1->Draw();
  TLine *line1 = new TLine{3.4, 0., 4., 0.};
  line1->SetLineColor(2);
  line1->Draw("same");
  myC->cd(1);
  gPad->SetPad(0., 0.3, 1, 1);
  xframe1->Draw();
  //////////////////////////////////////////////////////////////////
  myC->SaveAs("./Plots/PsiPrimeMassFit_CB_Exp.png");

}
