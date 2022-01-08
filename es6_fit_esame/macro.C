#include <TH1.h>

namespace rf = RooFit; 
using namespace rf; 

// Plotta il grafico, disegna le pull e ne plotta la distribuzione con una gaussiana.
// Il parametro fixedTails indica se le due Crystal Ball dei due segnali devono avere stesso parametro per le due code
// Il parametro bkg è la distribuzione da usare per il bkg 
// Può essere "exp" (valore di default) o "cheby" 
void macro(bool fixedTails = true, const TString bkg = "exp", const TString extension = "png") {
    gROOT->Clear(); 
    gROOT->Reset(); 

    TCanvas * myC = new TCanvas("myC", "Plots", 700, 700);
    myC->SetBottomMargin(0.41); myC->SetTopMargin(0.05);

    TFile f{"./root_files/histo-esame28Marzo2017.root", "READ"}; 
    TH1D * histo{(TH1D *)f.Get("chic")}; // Sempre il problema della conversione sulla macchina remota  

    RooRealVar x{"x", "x", 3.4, 3.6}; 

    RooDataHist rfHisto{histo->GetName(), histo->GetTitle(), RooArgSet(x), rf::Import(*histo, kFALSE)}; // Nota che gli devi passare la variabile aleatoria

    RooPlot * xFrame{x.frame(Title(""))}; 
    xFrame->SetTitle("Spettro chic");
    xFrame->SetYTitle("Counts");
    xFrame->SetXTitle("m()");

    rfHisto.plotOn(xFrame); // E poi plotti l'histogramma sul frame generato per la variabile


    // --------- Fit 

    // CB 1 
    // Parametri 
    RooRealVar meanCB1{"meanCB1", "meanCB1", 3.5, 3.49, 3.53}; // Media
    RooRealVar sigmaCB1{"sigmaCB1", "sigmaCB1", 0.01, 0.0001, 0.1}; // Sigma
    RooRealVar alpha1{"alpha1", "alpha1", 1, 0.00001, 10000};
    RooRealVar nCB1{"nCB1", "nCB1", 1, 0.0001, 10000};
    // Pdf
    // Tentativo: Per fittare meglio le code fisso i parametri alfa e n e li impongo uguali per entrambe le CB 
    RooCBShape cb1Pdf{"cb1Pdf", "cb1Pdf", x, meanCB1, sigmaCB1, alpha1, nCB1};

    // CB 2
    // Parametri 
    RooRealVar meanCB2{"meanCB2", "meanCB2", 3.55, 3.54, 3.58}; // Media
    RooRealVar sigmaCB2{"sigmaCB2", "sigmaCB2", 0.01, 0.0001, 0.1}; // Sigma
    RooRealVar alpha2{"alpha2", "alpha2", 1, 0.00001, 10000};
    RooRealVar nCB2{"nCB2", "nCB2", 1, 0.0001, 10000};

    // Code fissate o no 
    if (fixedTails) 
        // Fisso le code 
        alpha2 = alpha1; 
        nCB2 = nCB1; 
    
    // Pdf 
    RooCBShape cb2Pdf{"cb2Pdf", "cb2Pdf", x, meanCB2, sigmaCB2, alpha2, nCB2};
    
    // Fondo
    // Devo per forza definire entrambe le pdf e poi usare solo quella scelta dall'utente
    // Questo perchè in C++ non è possibile determinare il tipo di una variabile a run-time (cioè dentro un if) 

    // Fondo exp 
    RooRealVar c{"c", "c", -1, -100, 1};
    RooExponential expPdf{"expPdf", "expPdf", x, c}; 

    // Fondo cheby 
    RooRealVar c0{"c0", "1st coeff", 0.3, -100000, 100000};
    RooRealVar c1{"c1", "2nd coeff", -0.1, -100000, 100000};
    RooChebychev chebyPdf{"cheby", "Chebychev", x, RooArgList(c0, c1)};

    // Pdf Totale 
    // Componenti
    RooRealVar nSig1{"nSig", "Segnale 1", 1e3, 1, 1e7};
    RooRealVar nSig2{"nSig2", "Segnale 2", 1e3, 1, 1e8};
    RooRealVar nBkg{"nBkg", "Background", 120e3, 1, 1e8};

    // Definisco la pdf totale 
    // La definisco come un generico puntatore a una RooAddPdf e poi la specializzo a seconda della scelta dell'utente 
    // Nota che questo giochetto funziona solo se usi il puntatore, non funziona con gli oggetti statici (mi sa ma non sono sicuro)
    RooAddPdf * totalPdf; 
    if (bkg == "exp") {
        // La inizializzo in base al background scelto
        RooAddPdf * totalPdfExp = new RooAddPdf("totalPdf", "totalPdf", RooArgList(cb1Pdf, cb2Pdf, expPdf), RooArgList(nSig1, nSig2, nBkg)); 
        totalPdf = totalPdfExp; 
    } else if (bkg == "cheby") {
        RooAddPdf * totalPdfCheby = new RooAddPdf("totalPdf", "totalPdf", RooArgList(cb1Pdf, cb2Pdf, chebyPdf), RooArgList(nSig1, nSig2, nBkg)); 
        totalPdf = totalPdfCheby; 
    } else {
        // Se il bkg è sconosciuto lancio un errore
        throw "Background sconosciuto!"; 
        exit(1); 
    }

    // Fit  
    totalPdf->fitTo(rfHisto, Extended(kTRUE));
    // Plot 
    totalPdf->plotOn(xFrame, rf::LineColor(kRed));
    totalPdf->plotOn(xFrame, rf::Components(RooArgSet(cb1Pdf)), rf::LineColor(kGreen)); // CB a sinistra
    totalPdf->plotOn(xFrame, rf::Components(cb2Pdf), rf::LineColor(kCyan)); // CB a destra

    // Scelgo quale fondo fittare 
    if (bkg == "exp") totalPdf->plotOn(xFrame, rf::Components(expPdf), rf::LineStyle(kDashed));
    else if (bkg == "cheby") totalPdf->plotOn(xFrame, rf::Components(chebyPdf), rf::LineStyle(kDashed));

    totalPdf->plotOn(xFrame, rf::LineColor(kRed)); // Correzione per i pull
    totalPdf->paramOn(xFrame, Parameters(RooArgSet(meanCB1, sigmaCB1, nSig1, meanCB2, sigmaCB2, nSig2)), Layout(0.6, 0.9, 0.9)) ; 

    xFrame->getAttText()->SetTextSize(0.02); 

    // -------------------- Pull 
    
    // Salvo l'istogramma con le pull in una variabile (lo fitterò con una gaussiana)
    RooHist * rooHisto_pull{xFrame->pullHist()}; 

    // Creo un nuovo frame per le pull sempre a partire dalla variabile x
    // Su xFrame c'è già tutta la roba fittata, invece così ne faccio uno nuovo 
    RooPlot * pullFrame{x.frame()}; 
    pullFrame->addObject(rooHisto_pull, "p");
    pullFrame->SetTitle("");
    pullFrame->SetYTitle("Pulls bin-by-bin");
    pullFrame->SetXTitle(" ");
    pullFrame->SetMinimum(-6);
    pullFrame->SetMaximum(6);

    myC->Divide(1, 2);

    // Disegno l'istogramma con i dati 
    myC->cd(1); 
    gPad->SetPad(0, 0.3, 1, 1);
    xFrame->Draw();

    // Disegno l'isto dei pull 
    myC->cd(2); // In basso  
    gPad->SetPad(0, 0, 1, 0.3);
    pullFrame->Draw(); // Disegno le pull 

    // Linee sul grafico dei pull
    TLine lineplus{3.4, 3, 3.6, 3};
    TLine lineminus{3.4, -3, 3.6, -3};
    TLine linezero{3.4, 0, 3.6, 0};

    lineplus.SetLineStyle(2);
    lineplus.SetLineColor(2); // Rosso 
    lineplus.Draw("same");

    lineminus.SetLineStyle(2);
    lineminus.SetLineColor(2); // Rosso 
    lineminus.Draw("same");

    linezero.SetLineStyle(2);
    linezero.SetLineColor(4); // Blu 
    linezero.Draw("same");

    // Salvo il file 
    if (fixedTails) {
        myC->SaveAs("./Plots/histo_fixed_tails_bkg_" + bkg + "." + extension);
    } else {
        myC->SaveAs("./Plots/histo_bkg_" + bkg + "." + extension);
    }

    // ------------------------- Distrubuzione delle pull

    myC->Clear(); 
    myC->Divide(1, 1); 
    myC->cd(); 

    // Uso un istogramma normale di Root così posso fittare con una normale TF1  
    auto histo_pull = new TH1D("hp", "hp", 50, -6, 6); 
    for (int i{0}; i < rooHisto_pull->GetN(); ++i) {
        histo_pull->Fill(rooHisto_pull->GetPointY(i)); 
    }
    
    // Fitto l'istogramma con una gaussiana 
    TF1 * myGaus = new TF1("myGaus", "gaus", -6, 6); 
    myGaus->SetParameters(20, 0, 3); 
    histo_pull->Fit(myGaus); 

    // ------------- Cose grafiche distribuzione delle pull 
    // Titolo 
    TString title{"pull distribution "}; 
    if (fixedTails) {
        title += "fixed tails "; 
    } else {
        title += "no fixed tails "; 
    }
    title += bkg + " bkg ";

    histo_pull->SetTitle(title);
    histo_pull->GetXaxis()->SetTitle("x - #mu (#sigma units)");
    histo_pull->GetYaxis()->SetTitle("Count"); 

    // Mostro i risultati del fit
    gStyle->SetOptFit(1111); 
    histo_pull->Draw(); 
    // --------------------------------------------------------------

    if (fixedTails) {
        myC->SaveAs("./Plots/pull_distrib_fixed_tails_bkg_" + bkg + "." + extension); 
    } else {
        myC->SaveAs("./Plots/pull_distrib_bkg_" + bkg + "." + extension); 
    }


    delete myC; 
}