#include <TH1.h> 

using namespace RooFit; 

void template_fit() {

    gROOT->Clear(); 
    gROOT->Reset(); 
    gStyle->SetTitleOffset(1.4, "Y"); // Il secondo parametro è l'asse su cui fare l'offset 
    gStyle->SetOptFit(1);

    // ----------------------- 0. Lettura file e preparazione istogramma. 
    const char * nomeFile = ""; 
    TFile file{nomeFile, "READ"}; 

    const char * nomeIsto = ""; 
    TH1F * istogramma{(TH1F*) file.Get(nomeIsto)};
    auto numeroBin = istogramma->GetNbinsX(); // Magari serve per settare il binnaggio alla variabile 

    
    // ---------------------- 1. Definizione variabile e istogramma 
    double start = 0; // Limite inferiore variabile 
    double end = 0; // Limite superiore variabile 
    RooRealVar variabile{"variabile", "titolo", start, end}; 

    RooDataHist* data = new RooDataHist("data", istogramma->GetTitle(), RooArgSet(variabile), Import(*istogramma,kFALSE));


    // ---------------------- 2. Definizione pdf di segnale 
    // Commentare le pdf non usate. 
    // Per ogni parametro ci sono 3 variabili: 
    // nomeparametroNomePDFvalue -> Valore di inizializzazione 
    // nomeparametroNomePDFstart -> Limite inferiore 
    // nomeparametroNomePDFend -> Limite superiore
    // Settare i valore in base al grafico 

    // ----- Crystal ball (Da usare se picco asimmetrico)

    // La alfa è legata a DOVE inizia la coda: 
    // Per n fissato, alfa grande -> la coda è lontana dal picco 
    //                alfa piccolo -> La coda è vicina al picco 
    // La n è legata a quanto RAPIDAMENTE la coda va a 0: 
    // Per entrambi puoi partire da 1 e dare range grandi e poi modificare piano piano.

    // Params 
    double meanCBvalue = 0; 
    double meanCBstart = 0; 
    double meanCBend = 0; 
    RooRealVar meanCB{"meanCB", "meanCB", meanCBvalue, meanCBstart, meanCBend}; // Media
    double sigmaCBvalue = 0; 
    double sigmaCBstart = 0; 
    double sigmaCBend = 0; 
    RooRealVar sigmaCB{"sigmaCB", "sigmaCB", sigmaCBvalue, sigmaCBstart, sigmaCBend}; // Sigma
    double alphaCBvalue = 0; 
    double alphaCBstart = 0; 
    double alphaCBend = 0; 
    RooRealVar alphaCB{"alpha", "alpha", alphaCBvalue, alphaCBstart, alphaCBend};
    double nCBvalue = 0; 
    double nCBstart = 0; 
    double nCBend = 0; 
    RooRealVar nCB{"nCB", "nCB", nCBvalue, nCBstart, nCBend};
    // Pdf 
    RooCBShape cbPdf{"cb1Pdf", "cb1Pdf", variabile, meanCB, sigmaCB, alphaCB, nCB};

    // ----- Voigtiana (Da usare se picco simmetrico e stretto < 5-10 MeV) 
    // Gamma e sigma sono entrambe legate alla larghezza del picco. 
    // Più o meno vedi la semilarghezza e assegna metà a uno e metà all'altro 

    // Parametri 
    double meanVvalue = 0; 
    double meanVstart = 0; 
    double meanVend = 0; 
    RooRealVar meanV{ "meanV","mean of voigtian", meanVvalue, meanVstart, meanVend };
    double gammaVvalue = 0; 
    double gammaVstart = 0; 
    double gammaVend = 0; 
    RooRealVar gammaV{ "gammaV","width of BW", gammaVvalue, gammaVstart, gammaVend };
    double sigmaVvalue = 0; 
    double sigmaVstart = 0; 
    double sigmaVend = 0; 
    RooRealVar sigmaV{ "#sigma V", "mass resolution", sigmaVvalue, sigmaVstart, sigmaVend }; 
    // Pdf 
    RooVoigtian voigPdf{ "voidPdf","voidPdf", variabile, meanV, gammaV, sigmaV };
    
    // ----- Gaussiana (Da usare se picco simmetrico e largo 20-30 MeV)
    // Parametri 
    double meanGvalue = 0; 
    double meanGstart = 0; 
    double meanGend = 0; 
    RooRealVar meanG{"meanG", "meanG", meanGvalue, meanGstart, meanGend}; // Media
    double sigmaGvalue = 0; 
    double sigmaGstart = 0; 
    double sigmaGend = 0; 
    RooRealVar sigmaG{"sigmaG", "sigmaG", sigmaGvalue, sigmaGstart, sigmaGend}; // Sigma
    // Pdf 
    RooGaussian gausPdf{"gausPdf", "gausPdf", variabile, meanG, sigmaG}; 


    // ---------------------- 3. Definizione pdf di background 
    // Stesse regole pdf segnale 

    // ----- Cheby  
    // Per quanto riguarda i parametri della cheby, spesso vengono inizializzati a valori 
    // abbastanza piccoli (intorno a 0.1, -0.1, 0.5, 0.05, 0) 
    // e vengono fatti variare in un range abbastanza grande. 
    // Io ho messo un range grandissimo, ma spesso il range è [-15, 15] o [-10, 10]

    // Parametri 
    double c0value = 0; 
    RooRealVar c0{"c0", "1st coeff", c0value, -100000, 100000};
    double c1value = 0; 
    RooRealVar c1{"c1", "2nd coeff", c1value, -100000, 100000};
    // Se necessario inserire altri coefficienti 
    // Pdf 
    RooChebychev chebyPdf{"chebyPdf", "chebyPdf", variabile, RooArgSet(c0,c1)}; 

    // ----- Esponenziale 
    // Poche info sulla alpha, quando l'abbiamo trovata partiva da un valore di -0.05 e 
    // variava tra [-3, 10]. 

    // Parametri 
    double alphaExpValue = 0; 
    RooRealVar alphaExp{ "#alpha","Exponential Parameter",alphaExpValue, -10, 10};
    // Pdf 
    RooExponential expPdf{ "expPdf","expPdf",variabile ,alphaExp };
    
    
    // ---------------------- 4. Definizione pdf totale 
    // Per i valori di nSig e nBkg più o meno devi guardare quando è alto il picco rispetto 
    // al fondo. In questo caso lascio i valori da cui ho preso le righe, tanto hanno un range 
    // di variazione abbastanza grande.  

    RooAddPdf *totalPdf;

    // ----- Caso 1: Un solo picco 
    // In questo caso abbiamo una sola pdf di segnale e una sola di bkg 

    // Numero candidati
    RooRealVar nSigOnePeak{ "nSigOnePeak", "Number of signal candidates ", 5e+5, 1000., 5e+6 };
    RooRealVar nBkgOnePeak{ "nBkgOnePeak", "Bkg component", 2e+6, 1000., 5e+7 }; 
    // Pdf  
    // Ricorda di definire chi sono sigPdf e bkgPdf
    // Esempio con gaus e cheby: 
    auto sigPdf = gausPdf;  
    auto bkgPdf = chebyPdf; 
    auto listaPdfOnePeak = RooArgList(sigPdf, bkgPdf); 
    RooAddPdf totalPdfOnePeak{"totalPdfOnePeak", "totalPdfOnePeak", listaPdfOnePeak, RooArgList(nSigOnePeak, nBkgOnePeak)};     

    // ----- Caso 2: Due picchi 
    // Creo una pdf totale fatta dalla somma delle 3 pdf con 3 parametri. 

    // Numero candidati
    RooRealVar nSig1TwoPeaks{"nSig1TwoPeaks", "number of sig entries for peak 1", 5e+5, 1000., 5e+6}; 
    RooRealVar nSig2TwoPeaks{"nSig2TwoPeaks", "number of sig entries for peak 2", 5e+5, 1000., 5e+6};
    RooRealVar nBkgTwoPeaks{ "nBkgTwoPeaks", "Bkg component", 2e+6, 1000., 5e+7 }; 

    // Pdf 
    auto sig1Pdf = gausPdf; 
    auto sig2Pdf = voigPdf; 
    auto bkgPdfTwoPeaks = chebyPdf; 
    auto listaPdfTwoPeaks = RooArgList(sig1Pdf, sig2Pdf, bkgPdfTwoPeaks); 
    RooAddPdf totalPdfTwoPeaks{"totalPdfTwoPeaks", "total pdf", listaPdfTwoPeaks, RooArgList(nSig1TwoPeaks, nSig2TwoPeaks, nBkgTwoPeaks)}; 

    // Specializzo quella totale 
    // Esempio: un solo picco 
    totalPdf = &totalPdfOnePeak; 
    // totalPdf = &totalPdfTwoPeaks; 


    // ---------------------- 5. Fit 
    totalPdf->fitTo(*data, Extended(kTRUE)); 

    // --------------------- 6. Plot 

    // ----- 6.1 Creazione canvas e frame 
    // Creo il canvas 
    TCanvas *myC = new TCanvas("RooCanvas","Roofit Canvas", 1200, 1000);
    // Genero il frame 
    RooPlot *frame = variabile.frame("");
    
    // ----- 6.2 Setting dei titoli 
    // Setto i titoli 
    // Titolo frame 
    const char * titoloFrame = ""; 
    frame->SetTitle(titoloFrame); 
    // Titolo asse y
    std::string titoloAsseY{"# events/" + std::to_string(istogramma->GetXaxis()->GetBinWidth(1)) + "GeV"}; 
    TString titoloAsseY_c = titoloAsseY.c_str();
    frame->SetYTitle(titoloAsseY_c); 
    // Titolo asse x 
    const char * titoloAsseX = "m(Inserire qualcosa) [GeV]";
    frame->SetXTitle(titoloAsseX); 

    // ----- 6.3 Plot dei dati e parametri

    // Dati 
    data->plotOn(frame);

    // Pdf  
    // Caso 1: un solo picco 
    totalPdf->plotOn(frame,LineColor(kGreen)); // Totale (verde)
    totalPdf->plotOn(frame,Components(RooArgSet(sigPdf)),LineColor(kRed)); // Segnale1 (rosso)
    totalPdf->plotOn(frame,Components(RooArgSet(bkgPdf)),LineColor(kBlue),LineStyle(kDashed)); // Bkg (verde)
    totalPdf->plotOn(frame,LineColor(kGreen)); // Correzione pull

    // Caso 2: due picchi 
    // totalPdf->plotOn(frame,LineColor(kGreen)); // Totale (verde)
    // totalPdf->plotOn(frame,Components(RooArgSet(sig1Pdf)),LineColor(kRed)); // Segnale1 (rosso)
    // totalPdf->plotOn(frame,Components(RooArgSet(sig2Pdf)),LineColor(kCyan)); // Segnale2 (ciano)
    // totalPdf->plotOn(frame,Components(RooArgSet(bkgPdf)),LineColor(kBlue),LineStyle(kDashed)); // Bkg (verde)
    // totalPdf->plotOn(frame,LineColor(kGreen)); // Correzione pull

    // Parameters (come se avessi scelto la gaussiana)
    totalPdf->paramOn(
                frame,
                Parameters(RooArgSet(meanG, sigmaG, nSigOnePeak, nBkgOnePeak)), 
                Layout(0.75,0.99,0.99)); // Sistemare il layout
    frame->getAttText()->SetTextSize(0.028); // Sistemare la grandezza del testo 

    
    // -------------------- 7. Pull 
    RooPlot * pullFrame{variabile.frame()}; 
    pullFrame->addObject(frame->pullHist(), "p");
    pullFrame->SetTitle("");
    pullFrame->SetYTitle("Pulls bin-by-bin");
    pullFrame->SetXTitle(" ");
    pullFrame->SetMinimum(-6);
    pullFrame->SetMaximum(6);


    // -------------------- 8. Operazioni di draw 
    myC->Divide(1, 2);

    // Disegno l'isto dei pull 
    myC->cd(2); // In basso  
    gPad->SetPad(0, 0, 1, 0.3);
    pullFrame->Draw(); // Disegno le pull 

    // Linee sul grafico dei pull
    TLine *lineplus = new TLine{start, 3, end, 3}; // start e end sono gli estremi della variabile
    TLine *lineminus = new TLine{start, -3, end, -3};
    TLine *linezero = new TLine{start, 0, end, 0};

    lineplus->SetLineStyle(2);
    lineplus->SetLineColor(2); // Rosso 
    lineplus->Draw("same");

    lineminus->SetLineStyle(2);
    lineminus->SetLineColor(2); // Rosso 
    lineminus->Draw("same");

    linezero->SetLineStyle(2);
    linezero->SetLineColor(4); // Blu 
    linezero->Draw("same");

    // Disegno l'istogramma con i dati e le pdf  
    myC->cd(1); 
    gPad->SetPad(0, 0.3, 1, 1);
    frame->Draw();

    myC->SaveAs("./plot.png"); 

}