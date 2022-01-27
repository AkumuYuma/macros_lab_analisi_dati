#include <TH1.h> 

using namespace RooFit; 

void prova_2() {

    gROOT->Clear(); 
    gROOT->Reset(); 
    gStyle->SetTitleOffset(1.4, "Y"); // Il secondo parametro è l'asse su cui fare l'offset 
    gStyle->SetOptFit(1);

    // ----------------------- 0. Lettura file e preparazione istogramma. 
    const char * nomeFile = "./root_files/file_root.root"; 
    TFile file{nomeFile, "READ"}; 

    const char * nomeIsto = "myInclusiveMuMuMass_hlt_5_v3Bv4"; 
    TH1F * istogramma{(TH1F*) file.Get(nomeIsto)};
    auto numeroBin = istogramma->GetNbinsX(); // Magari serve per settare il binnaggio alla variabile 

    
    // ---------------------- 1. Definizione variabile e istogramma 
    double start = 3.4; // Limite inferiore variabile 
    double end = 4; // Limite superiore variabile 
    RooRealVar variabile{"variabile", "m(#mu#mu)", start, end}; 
    variabile.setBins(numeroBin); 

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
    double meanCBvalue = 3.7; 
    double meanCBstart = 3.67; 
    double meanCBend = 3.73; 
    RooRealVar meanCB{"meanCB", "meanCB", meanCBvalue, meanCBstart, meanCBend}; // Media
    double sigmaCBvalue = 0.02; 
    double sigmaCBstart = 0.001; 
    double sigmaCBend = 0.1; 
    RooRealVar sigmaCB{"sigmaCB", "sigmaCB", sigmaCBvalue, sigmaCBstart, sigmaCBend}; // Sigma
    double alphaCBvalue = 2; 
    double alphaCBstart = 0.5; 
    double alphaCBend = 10; 
    RooRealVar alphaCB{"alphaCB", "alphaCB", alphaCBvalue, alphaCBstart, alphaCBend};
    double nCBvalue = 1; 
    double nCBstart = 0.1; 
    double nCBend = 25; 
    RooRealVar nCB{"nCB", "nCB", nCBvalue, nCBstart, nCBend};
    // Pdf 
    RooCBShape cbPdf{"cbPdf", "cbPdf", variabile, meanCB, sigmaCB, alphaCB, nCB};


    // ---------------------- 3. Definizione pdf di background 
    // Stesse regole pdf segnale 


    // ----- Esponenziale 
    // Poche info sulla alpha, quando l'abbiamo trovata partiva da un valore di -0.05 e 
    // variava tra [-3, 10]. 

    // Parametri 
    double alphaExpValue = -1e-1; 
    RooRealVar alphaExp{ "alphaExp","Exponential Parameter",alphaExpValue, -2., -1e-2};
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
    RooRealVar nSigOnePeak{ "nSigOnePeak", "Number of signal candidates ", 2.5e+5, 1000., 5e+7 };
    RooRealVar nBkgOnePeak{ "nBkgOnePeak", "Bkg component", 5e6, 1000., 5e+7 }; 
    // Pdf  
    // Ricorda di definire chi sono sigPdf e bkgPdf
    // Esempio con gaus e cheby: 
    auto sigPdf = cbPdf; // Ricorda di cambiare 
    auto bkgPdf = expPdf; // Ricorda di cambiare 
    auto listaPdfOnePeak = RooArgList(sigPdf, bkgPdf); 
    RooAddPdf totalPdfOnePeak{"totalPdfOnePeak", "totalPdfOnePeak", listaPdfOnePeak, RooArgList(nSigOnePeak, nBkgOnePeak)};     

    // Specializzo quella totale 
    totalPdf = &totalPdfOnePeak; 


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
    const char * titoloFrame = "massa(#mu#mu)"; 
    frame->SetTitle(titoloFrame); 
    // Titolo asse y (suppongo che i bin siano tutti uguali)
    std::string titoloAsseY{"# events/" + std::to_string(istogramma->GetXaxis()->GetBinWidth(1)) + "GeV"}; 
    TString titoloAsseY_c = titoloAsseY.c_str();
    frame->SetYTitle(titoloAsseY_c); 
    // Titolo asse x 
    const char * titoloAsseX = "m(#mu#mu) [GeV]"; 
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
                    Parameters(RooArgSet(meanCB, sigmaCB, alphaCB, nCB, alphaExp, nSigOnePeak, nBkgOnePeak)), 
                    Layout(0.6,0.90,0.90)
                    ); // Sistemare il layout
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

    delete myC;

}