#include <TH1.h> 
#include <sys/time.h>

using namespace RooFit; 

void esercitazione(int events = 100000, int numeroBin = 200) {

    RooRealVar xvar{"xvar", "", 0.01, 10}; 
    xvar.setBins(numeroBin); 

    // Segnale 1
    // Prima BW 
    RooRealVar mean("m_{1}", "mean", 3, 0, 10);                    //Breit Wigner mean//
    RooRealVar gamma("#Gamma_{1}", "gamma", 1.5, 0.1, 5);              //Breit Wigner width//
    RooBreitWigner signal1("BW1", "BW signal1", xvar, mean, gamma); //Breit Wigner pdf//

    // Gaussian Resolution Function //
    RooRealVar zero("zero","Gaussian resolution mean", 0.);               // offset from mean
    RooRealVar sigma("#sigma_{1}", "sigma", 1, 0.1, 5);                     //Gaussian sigma//
    RooGaussian resol1("resol", "Gaussian resolution", xvar, zero, sigma); //Gaussian pdf//

    // Gaussian + BW convolution //
    RooNumConvPdf segnale1("convolution", "BW (X) gauss", xvar, signal1, resol1);

    // Segnale 2
    RooRealVar mean2("m_{2}", "mean", 7, 0, 10);                    //Breit Wigner mean//
    RooRealVar gamma2("#Gamma_{2}", "gamma", 1, 0.1, 5);              //Breit Wigner width//
    RooBreitWigner signal2("BW2", "BW signal2", xvar, mean2, gamma2); //Breit Wigner pdf//

    // Background //
    RooRealVar alpha("#alpha","Exponential Parameter",-0.05,-3.0,10);
    RooExponential bkg("Bkg","Bkg",xvar,alpha);

    // TotalPdf = Gaussian + Bkg //
    RooRealVar sigfrac("sig1frac","fraction of component 1 in signal",0.5,0.,2.) ;
    RooRealVar sig2frac("sig2frac","fraction of component 2 in signal",0.5,0.,2.) ;
    RooRealVar bkgfrac("sig2frac","fraction of bkg", 100, 50, 1000.) ;
    RooAddPdf total("totalPDF", "totalPDF", RooArgList(signal1, signal2, bkg), RooArgList(sigfrac, sig2frac, bkgfrac));


    // -------------------------- Genero dati 
    std::cout <<"\nGenerating " << events << " events\n" << std::endl ;

    // Uso come seed per la generazione casuale l'istante presente
    timeval trand;
    gettimeofday(&trand,NULL);

    // Uso secondi e microsecondi come seme (alla fine è uguale potevo usare un valore qualsiasi)
    long int msRand = trand.tv_sec * 1000 + trand.tv_usec / 1000;
    cout << "\n------" << endl;
    cout << "msRand = " << msRand ;
    cout << "\n------" << endl;
    // Setto il seme random 
    RooRandom::randomGenerator()->SetSeed(msRand);

    // Creo un dataset generando eventi random dalla pdf totale. 
    RooDataSet* data = total.generate(xvar,events);

    // -------------------------- Fitto dati 

    total.fitTo(*data, Extended(kTRUE)); 

    // ------------------------ Plotto 

    TCanvas *myC = new TCanvas("RooCanvas","Roofit Canvas", 1200, 1000);
    RooPlot *frame = xvar.frame("") ;

    // Titolo
    std::string titolo{"RooFit: " + std::to_string(events) + " events"}; 
    TString titoloTstring = titolo.c_str(); 
    frame->SetTitle(titoloTstring) ;
    frame->SetYTitle("# of events") ;

    // Plot data
    data->plotOn(frame);
    // Plot pdf 
    total.plotOn(frame,LineColor(kGreen)); // Totale (verde)
    total.plotOn(frame,Components(RooArgSet(signal1)),LineColor(kRed)); // Segnale1 (rosso)
    total.plotOn(frame,Components(RooArgSet(signal2)),LineColor(kCyan)); // Segnale1 (ciano)
    total.plotOn(frame,Components(RooArgSet(bkg)),LineColor(kBlue),LineStyle(kDashed)); // Bkg (verde)
    total.plotOn(frame,LineColor(kGreen)); // Correzione pull
    // Parameters 
    total.paramOn(frame, Layout(0.75,0.99,0.99));
    frame->getAttText()->SetTextSize(0.028);

    // Plot pull
    RooPlot * pullFrame{xvar.frame()}; 
    pullFrame->addObject(frame->pullHist(), "p");
    pullFrame->SetTitle("");
    pullFrame->SetYTitle("Pulls bin-by-bin");
    pullFrame->SetXTitle(" ");
    pullFrame->SetMinimum(-6);
    pullFrame->SetMaximum(6);

    myC->Divide(1, 2);

    // Disegno l'isto dei pull 
    myC->cd(2); // In basso  
    gPad->SetPad(0, 0, 1, 0.3);
    pullFrame->Draw(); // Disegno le pull 

    // Linee sul grafico dei pull
    TLine *lineplus = new TLine{0, 3, 10, 3};
    TLine *lineminus = new TLine{0, -3, 10, -3};
    TLine *linezero = new TLine{0, 0, 10, 0};

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

    myC->SaveAs("./Plots/plot.png"); 
}