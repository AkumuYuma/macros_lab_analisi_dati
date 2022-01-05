#include <TH1.h>

namespace rf = RooFit;
using namespace rf;

void psi_fit() {
    gROOT->Reset();
    gROOT->Clear();

    TFile f{"./root_files/esame-dec2014.root", "READ"};
    TH1D * histo{(TH1D *)(f.Get("PsiPrime_Mass_cut6"))} ; // Attenzione che il dynamic cast non funziona sulla macchina virtuale (versione vecchia dell'interprete)

    RooRealVar x{"x", "x", 3.6, 4}; // è una variabile random, gli argomenti sono: "nome", "titolo", valoremin, valoremax
    RooDataHist rfHisto{histo->GetName(), histo->GetTitle(), RooArgSet(x), rf::Import(*histo, kFALSE)}; // Nota che gli devi passare la variabile aleatoria

    RooPlot * xFrame{x.frame(Title(""))}; 

    // Grafica frame    
    xFrame->SetTitle("Inclusive J/#psi#pi#pi invariant mass spectrum");
    xFrame->SetYTitle("Candidates/5MeV/c^{2}");
    xFrame->SetXTitle("m(J/#psi#pi#pi)");
    
    // Plotto l'istogramma
    rfHisto.plotOn(xFrame); // E poi plotti l'histogramma sul frame generato dalla variabile

    // Canvas 
    TCanvas * myC = new TCanvas("myC", "Plots", 700, 700);
    myC->SetBottomMargin(0.41); myC->SetTopMargin(0.05);

    // -------------------------------- FIT


    // Segnale crystal ball
    RooRealVar meanCB{"meanCB", "meanCB", 3.685, 3.675, 3.695}; // Media
    RooRealVar sigmaCB{"sigmaCB", "sigmaCB", 0.0042, 0.0004, 0.005}; // Sigma
    // In genere alfa e n sono numeri piccoli
    RooRealVar alpha{"alpha", "alpha", 1, 0.00001, 10000};
    RooRealVar nCB{"nCB", "nCB", 1, 0.0001, 10000};

    // Definisco la pdf
    RooCBShape cbPdf{"cbPdf", "cbPdf", x, meanCB, sigmaCB, alpha, nCB};

    // Background cheby
    RooRealVar c0{"c0", "1st coeff", 0.3, -100000, 100000};
    RooRealVar c1{"c1", "2nd coeff", -0.1, -100000, 100000};
    RooChebychev ch{"cheby", "Chebychev", x, RooArgList(c0, c1)};

    // Sta un segnaletto vicino alle 3.8 (Se togli questa parte e vedi le pull si vede che sta un punto fuori, allora metti pure questo )
    RooRealVar mean{"mean", "mean", 3.87, 3.85, 3.88};
    RooRealVar sigma{"sigma", "sigma", 0.005, 0.001, 0.015};

    RooGaussian g{"g", "gaussian", x, mean, sigma};

    // Pdf totale

    // Coefficenti 

    // Componente segnale Crystal Ball
    RooRealVar nSig{"nSig", "Number of signal cands", 11861, 1, 1e7};
    nSig.setConstant(kTRUE); // Fatto successivamente, prima fittato, poi settato costante
    // Componente bkg Cheby
    RooRealVar nBkg{"nBjg", "Number of bkg component", 120e3, 1, 1e8};
    // Componente segnale gaussaian
    RooRealVar nSigGaussiana{"nSigGaussiana", "Componente gaussiana piccolina", 1e3, 1, 1e8};

    // Pdf 
    RooAddPdf totalPdf{"totalPdf", "totalPdf", RooArgList(cbPdf, ch, g), RooArgList(nSig, nBkg, nSigGaussiana)}; // Nota che gli ho aggiunto anche la gaussiana a destra

    // Fit 
    totalPdf.fitTo(rfHisto, Extended(kTRUE));

    // Plot 
    myC->Divide(1, 1); 
    myC->cd(1); 
    totalPdf.plotOn(xFrame, rf::LineColor(kRed));
    totalPdf.plotOn(xFrame, rf::Components(RooArgSet(cbPdf)), rf::LineColor(kGreen));
    totalPdf.plotOn(xFrame, rf::Components(g), rf::LineColor(kCyan)); // Gaussiana a destra
    totalPdf.plotOn(xFrame, rf::Components(ch), rf::LineStyle(kDashed));

    totalPdf.plotOn(xFrame, rf::LineColor(kRed)); // Correzione per i pull
    totalPdf.paramOn(xFrame, Parameters(RooArgSet(meanCB, sigmaCB, nSig, mean, sigma, nSigGaussiana)),
     Format("NEU", AutoPrecision(2)), 
     Layout(0.45, 0.9, 0.9)); // Format decide il formato con cui mostra i parametri, tipo AutoPrecision decide il numero di cifre significative
    xFrame->getAttText()->SetTextSize(0.025); // Per ridurre la dimensione della scritta dei parametri  

    xFrame->Draw();
    myC->SaveAs("./Plots/Histo_plot.png");


    // ------------------ Pull
    RooPlot * pullFrame{x.frame("")};
    pullFrame->addObject( xFrame->pullHist(), "p");
    pullFrame->SetTitle("");
    pullFrame->SetYTitle("Pulls bin-by-bin");
    pullFrame->SetXTitle(" ");
    pullFrame->SetMinimum(-6);
    pullFrame->SetMaximum(6);

    // Draw di tutto 
    myC->Clear();
    myC->Divide(1, 2);

    myC->cd(1); // Vado al primo pad 
    gPad->SetPad(0, 0.3, 1, 1); // Setto dimensioni e posizione 
    xFrame->Draw(); // Disegno il frame con istogramma e fit sul pad 

    myC->cd(2); // Vado al secondo pad (Qui metterò le pull)
    gPad->SetPad(0, 0, 1, 0.3); // Setto posizione e dimensioni  
    pullFrame->Draw(); // Disegno il frame delle pull sul pad che ho preparato 

    // Linee (Sul pad delle pull)
    TLine lineplus{3.6, 3, 4, 3};
    TLine lineminus{3.6, -3, 4, -3};
    TLine linezero{3.6, 0, 4, 0};
 
    lineplus.SetLineStyle(2); // Tratteggiata 
    lineplus.SetLineColor(2); // Rossa
    lineplus.Draw("same");

    lineminus.SetLineStyle(2); // Tratteggiata 
    lineminus.SetLineColor(2); // Rossa 
    lineminus.Draw("same");

    linezero.SetLineStyle(1); // Continua
    linezero.SetLineColor(4); // Blu 
    linezero.Draw("same");

    myC->SaveAs("./Plots/histo_pull.png");

    delete myC;


}
