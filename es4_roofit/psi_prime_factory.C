#include <TH1.h> 
#include <string.h>

using namespace RooFit; 


void psi_prime_factory() {
    
    gStyle->SetTitleOffset(1.4, "Y"); // Il secondo parametro è l'asse su cui fare l'offset 
    gStyle->SetOptFit(1);

    gROOT->ForceStyle(); // Questo forza la lettura e scrittura su file con gli attributi grafici presenti 
    TFile f1{"./root_files/hlt_5_newSoftMuon_alsoInPsiPrimeWind.root","read"};
    TH1F* hPsiPrime = dynamic_cast<TH1F*>(f1.Get("PsiPrimeMass_bin9"));


    TCanvas *myC = new TCanvas("myC","PsiPrimeMassPlot", 700, 700);

    RooWorkspace w("w"); 
    
    double xMin = hPsiPrime->GetXaxis()->GetXmin();
    double xMax = hPsiPrime->GetXaxis()->GetXmax();
    int nBins = hPsiPrime->GetNbinsX(); // Salvo il numero di bin 

    RooRealVar x{"x", "m(#mu^{+}#mu^{-}) [GeV/c^{2}]", xMin, xMax}; 
    RooDataHist *istogramma = new RooDataHist("#mu#mu_hist", hPsiPrime->GetTitle(), RooArgSet(x), Import(*hPsiPrime)); 

    // Segnale 
    std::string nomePdfSegnale{"Gaussian::gaus(x[" + std::to_string(xMin) + "," + std::to_string(xMax) + "],mean[3.62, 3.60, 3.75], sigma[0.02, 0.001, 0.1])"}; 
    const char *nomePdfSegnaleChar{nomePdfSegnale.c_str()};
    w.factory(nomePdfSegnaleChar); 

    // Background 
    std::string nomePdfBack{"Chebychev::c(x[" + std::to_string(xMin) + "," + std::to_string(xMax) + "],{c1[-0.1,-10,10],c2[-0.1,-10,10]})"}; 
    const char *nomePdfBackChar{nomePdfBack.c_str()};
    w.factory(nomePdfBackChar); 
    
    // Totale 
    w.factory("SUM::totale(nSig[2e5, 1, 1e6] * gaus, nBkg[120e3, 1, 1e6] * c)");

    // Fit 
    w.pdf("totale")->fitTo(*istogramma, Extended(kTRUE), Minos(kTRUE)); 

    // Plot
    auto frame = x.frame(); 
    istogramma->plotOn(frame); 
    w.pdf("totale")->plotOn(frame);
    w.pdf("totale")->plotOn(frame, Components(RooArgSet(*w.pdf("gaus"))), LineColor(kRed));  // Plotta la componente di segnale
    w.pdf("totale")->plotOn(frame, Components(RooArgSet(*w.pdf("c"))), LineColor(kGreen), LineStyle(kDashed) ); // Plotta la componenete di background 
    myC->SaveAs("prova.png"); 
}