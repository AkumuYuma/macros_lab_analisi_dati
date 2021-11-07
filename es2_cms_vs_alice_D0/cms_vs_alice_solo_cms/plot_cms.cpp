
#include "TH1.h"

double calcolaErroreRapporto(double x1, double e1, double x2, double e2) {
    /*
        Propaga l'errore con la formula del rapporto.
        simgaY = sqrt(1/x2^2 * e1^2 + (x1/x2^2) ^ 2 * e2^2)
        param:
        x1, x2: valori
        e1, e2: errori
        return: errore su x1/x2
    */
   double x1Quadro{TMath::Power(x1, 2)};
   double x2Quadro{TMath::Power(x2, 2)};
   double e1Quadro{TMath::Power(e1, 2)};
   double e2Quadro{TMath::Power(e2, 2)};
   return TMath::Sqrt( (1/x2Quadro) * e1Quadro  + ( TMath::Power(x1/x2Quadro, 2) * e2Quadro ) );
}


void plot_cms() {

    // -------------------------- Operazioni preliminarie
    gROOT->Reset();
    gROOT->Clear();
    gStyle->SetOptLogx(); // La scala logaritmica su X la setto globalmente usando il puntatore globale gStyle


    const int numeroPunti{9}; // Numero di punti del grafico

    // --------------------  Definisco i vettori di dati

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


    // Dati/dati
    // Punti 
    double puntiUno[numeroPunti];
    // Errori (ottenuti propagando l'errore di data/data)
    double erroriPuntiUno[numeroPunti];
    // Rapporto Mc/Data
    // Punti
    double puntiRapporto[numeroPunti];
    // Errori (ottenuti con la propagazione)
    double erroriRapportoPlus[numeroPunti];
    double erroriRapportoMinus[numeroPunti];
    // Riempio il vettore con i rapporti e  gli errori
    for (int i{0}; i < numeroPunti; ++i) {
        puntiRapporto[i] = f0nll[i]/puntiData[i];
        erroriRapportoPlus[i] = calcolaErroreRapporto(f0nll[i], erroriSimulatiPlus[i], puntiData[i], erroriData[i]);
        erroriRapportoMinus[i] = calcolaErroreRapporto(f0nll[i], erroriSimulatiMinus[i], puntiData[i], erroriData[i]);
        puntiUno[i] = puntiData[i]/puntiData[i];
        erroriPuntiUno[i] = calcolaErroreRapporto(puntiData[i], erroriData[i], puntiData[i], erroriData[i]);
    }

    // --------------------------- Definisco i grafici
    auto grData = new TGraphErrors{numeroPunti, pt, puntiData, erroriPt, erroriData};
    auto grMc = new TGraphAsymmErrors{numeroPunti, pt, f0nll,  erroriPt, erroriPt, erroriSimulatiMinus, erroriSimulatiPlus};
    auto grRapporto = new TGraphAsymmErrors{numeroPunti, pt, puntiRapporto, erroriPt, erroriPt, erroriRapportoMinus, erroriRapportoPlus};
    auto grUno = new TGraphErrors{numeroPunti, pt, puntiUno, erroriPt, erroriPuntiUno};

    // Sovrappongo i dati e mc in un unico grafico
    auto multiGraph = new TMultiGraph{};
    multiGraph->Add(grData, "P"); // Usa il marker corrente (se non lo metto mi scrive la linea)
    multiGraph->Add(grMc, "2p"); // L'opzione 2 riempie il rettangolo
    // Ricorda, quando sovrapponi i grafici, il secondo va disegnato SENZA l'opzione "a"
    // L'opzione "a" ridisegna gli assi, se la metti al secondo si sminchia tutto


    // ------------------------ Disegno

    // Opzioni grafiche per dati e mc
    grData->SetMarkerColor(kRed);
    grData->SetMarkerStyle(20); // La pallina

    grMc->SetFillColor(kGreen);
    grMc->SetMarkerColor(kGreen);
    grMc->SetFillStyle(3001); // Il fill normale

    // Opzioni grafiche per il rapporto
    grRapporto->SetMarkerColor(kBlack);
    grRapporto->SetMarkerColor(kRed);
    grRapporto->SetFillColor(kRed);
    grRapporto->SetFillStyle(3001);

    // Setto i titoli
    // Multi graph
    multiGraph->SetTitle("");
    multiGraph->GetXaxis()->SetLabelSize(0); // Tolgo i numeri all'asse delle x per il multi graph (i numeri li disengno sul grafico del rapporto)
    multiGraph->GetXaxis()->SetTitle("");
    multiGraph->GetYaxis()->SetTitle("d#sigma / dp_{t} (#mub / GeV)");

    // Dati/dati
    grUno->SetMarkerStyle(20);
    grUno->SetTitle("");
    grUno->SetMarkerColor(kBlack);


    // Rapporto
    grRapporto->SetTitle("");
    grRapporto->GetXaxis()->SetTitle("p_{t} (GeV)");
    grRapporto->GetYaxis()->SetTitle("Predictions/Data");
    // Necessarie perchè la grandezza del font di default viene decisa in base
    // alla grandezza del pad su cui viene disegnato l'asse, dato che sto facendo un pad
    // più piccolo, devo ingrandire il font dei titoli
    // Ingrandisco i titoli degli assi
    grRapporto->GetXaxis()->SetTitleSize(0.08); // in % rispetto alla larghezza del pad
    grRapporto->GetYaxis()->SetTitleSize(0.08);
    // Ingrandisco i numeri sugli assi
    grRapporto->GetXaxis()->SetLabelSize(0.08); // in % rispetto alla larghezza del pad
    grRapporto->GetYaxis()->SetLabelSize(0.08);
    grRapporto->GetXaxis()->SetTitleOffset(1.2);
    grRapporto->GetYaxis()->SetTitleOffset(0.55);
    grRapporto->GetXaxis()->SetLimits(4, 1e2); // Gli assi x dei due grafici devono essere allineati

    // Setto i minimi e massimi
    // Multi graph
    multiGraph->SetMinimum(5e-4);
    multiGraph->SetMaximum(5e3);
    multiGraph->GetXaxis()->SetLimits(4, 1e2);
    // Rapporto
    grRapporto->SetMinimum(0);
    grRapporto->SetMaximum(2.6);


    // ---------------- Disegno nei vari pad

    // Definisco un canvas
    auto c1 = new TCanvas{"c1", "Plots"};

    // Nota: Quando uso c1->cd() selezioni un pad quindi il puntatore gPad punterà a quel pad.
    // Quindi tutte le opzioni grafiche tipo gPad->SetLogy(), agiranno solo su quel pad.
    // Comunque per poter fare pad non della stessa altezza ho dovuto creare i pad e non usare divide

    // In basso a sinistra è (0, 0), in alto a destra è (1, 1)
    // NOTA sui PAD:
    // Definisco i due pad, è necessario definirli uno dopo l'altro, questo perchè
    // se definissi il secondo pad dopo aver fatto il Draw del primo grafico, cambierebbe il sistema di coordinate
    // e non avrei più in basso a sinistra (0, 0) -> in alto a destra (1, 1), ma le coordinate sarebbero
    // dipendenti dai valori scritti sul grafico (cioè dalle coordinate dei punti)
    auto p1 = new TPad{"p1", "Dati vs mc", 0.05, 0.3, 0.95, 0.95}; // xmin, ymin, xmax, ymax
    p1->SetBottomMargin(0); // Tolgo il margine bianco attorno al pad
    p1->SetTickx(2); // Con questa mi disegna le tacchette anche sopra per il primo pad
    p1->SetTicky(2); // Tacchette anche a destra per il primo pad
    auto p2 = new TPad{"p2", "Rapporto", 0.05, 0, 0.95, 0.3};
    // Setto i margini per il secondo
    p2->SetTopMargin(0);
    p2->SetBottomMargin(0.25); // Se non setto questi due i titoli degli assi non si vedono bene
    p2->SetLeftMargin(0.1);
    p2->SetTicky(2);

    // Disegno i due pad (disegno prima questi e poi tutto il resto viene sopra)
    p1->Draw();
    p2->Draw();

    // ----------------- pad 1 (in alto) --------------------
    // Seleziono il primo pad, imposto la scala logaritmica e disegno il grafico
    p1->cd();
    p1->SetLogy();
    // Disegno il Tgraph
    multiGraph->Draw("a");

    // ------------------ Aggiunte grafiche al pad 1 (scritte e legenda)

    // Disegno la legenda
    auto leg = new TLegend{0.15, 0.2, 0.35, 0.3}; // xmin, ymin, xmax, ymax rispetto al pad
    leg->AddEntry(grData, "Data");
    leg->AddEntry(grMc, "FONLL", "F"); // L'opzione F disegna il rettangolino, se non la metti ti disegna anche la croce nella legenda
    leg->SetBorderSize(0); // Per disegnarla senza il rettangolo intorno
    leg->Draw();

    // Scritta sezione d'urto
    TLatex latex{};
    latex.SetTextSize(0.06);
    latex.DrawLatex(40, 8e3,"29 nb^{-1} (13 TeV)"); // I primi due parametri sono x e y da cui parte la scritta, le unità sono quelle sul grafico
    latex.SetTextSize(0.08);
    latex.DrawLatex(5, 1010, "CMS");
    latex.SetTextSize(0.07);
    latex.DrawLatex(15, 1010, "Prompt D^{0} and #bar{D^{0}} production");


    // ------------------- pad 2 (in basso) -----------------
    // Seleziono il secondo pad e disegno il grafico del rapporto
    p2->cd();
    // Disegno il rapporto
    grRapporto->Draw("a2p");
    grUno->Draw("psame");

    c1->SaveAs("./Plots/plot_cms.pdf");

    // Cancello i puntatori che non mi servono più
    delete grMc;
    delete grData;
    delete multiGraph;
    delete grRapporto;
    delete leg;
    delete p1;
    delete p2;
    delete c1;
}
