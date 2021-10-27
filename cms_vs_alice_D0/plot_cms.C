
#include "TH1.h"

void plot_cms() {
    gROOT->Reset(); 
    gROOT->Clear();

    gStyle->SetOptLogy(); 
    gStyle->SetOptLogx(); 

    TCanvas *c1 = new TCanvas{"c1", "Plots", 1000, 800}; 
    // c1->Divide(1, 2); 
    c1->cd();

    const int numeroPunti{9}; // Numero di punti del grafico

    // Definisco i vettori di dati 
    // Asse x
    // Prendo il punto al centro della colonna A 
    const double pt[numeroPunti] { 4.5, 5.5, 6.5, 7.5, 10, 14, 20, 32, 70 }; 
    // Prendo la distanza tra gli estremi 
    const double erroriPt[numeroPunti] { 0.5, 0.5, 0.5, 0.5, 2, 2, 4, 8, 30 }; 

    // Dati
    const double puntiData[numeroPunti] = { 430.24, 230.12, 135.84, 65.71, 20.97, 3.93, 0.81, 0.097, 0.0033 }; 
    // Errori sui dati
    const double erroriData[numeroPunti] = { 69.3987642606496, 26.4029645149575, 16.1875139308272, 10.1785702019874, 2.04942943263727, 0.476705883895996, 0.09981810763913, 0.015399057425854, 0.001090649702509 };


    // Punti simulati
    const double f0nll[numeroPunti] { 220, 117.4, 66, 38.96, 14.125, 3.175, 0.6925, 0.08375, 0.003266666666667 }; 
    const double erroriSimulatiPlus[numeroPunti] { 107, 47.4, 22.6, 11.42, 3.545, 0.655, 0.1225, 0.0125, 0.000376666666667 }; 
    const double erroriSimulatiMinus[numeroPunti] { 157.2, 71.2, 35, 18.38, 5.52, 0.915, 0.155, 0.01375, 0.0004 }; 

    auto grData = new TGraphErrors{numeroPunti, pt, puntiData, erroriPt, erroriData};
    auto grMc = new TGraphAsymmErrors{numeroPunti, pt, f0nll,  erroriPt, erroriPt, erroriSimulatiMinus, erroriSimulatiPlus};


    grData->SetMarkerColor(kRed);
    grData->SetMarkerStyle(20); // La pallina

    grMc->SetFillColor(kGreen);
    grMc->SetFillStyle(3001); // Il fill normale 

    auto multiGraph = new TMultiGraph{}; 
    multiGraph->Add(grData, "P"); // Usa il marker corrente (se non lo metto mi scrive la linea) 
    multiGraph->Add(grMc, "2p"); // L'opzione 2 riempie il rettangolo  
    // Ricorda, quando sovrapponi i grafici, il secondo va disegnato SENZA l'opzione "a"
    // L'opzione "a" ridisegna gli assi, se la metti al secondo si sminchia tutto 

    // Setto i tioli
    multiGraph->SetTitle("Plot CMS"); 
    multiGraph->GetXaxis()->SetTitle("p_{t} (GeV)");
    multiGraph->GetYaxis()->SetTitle("d#sigma / dp_{t} (#mub / GeV)");

    // Setto i minimi e massimi 
    multiGraph->SetMinimum(5e-4);
    multiGraph->SetMaximum(5e3);
    multiGraph->GetXaxis()->SetLimits(4, 1e2);

    // Disegno il Tgraph 
    multiGraph->Draw("a"); 

    // Disegno la legenda
    auto leg = new TLegend{0.7, 0.8, 0.9, 0.9};
    leg->AddEntry(grData, "Data"); 
    leg->AddEntry(grMc, "FONLL");
    leg->Draw();

    c1->SaveAs("./Plots/plot_cms.pdf");
}