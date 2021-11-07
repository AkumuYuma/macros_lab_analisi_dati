#include "TH1.h"
#include <iostream>


//DUBBI RIMASTI: COME TOGLIERE LABELS SOPRA E A DESTRA E COME METTERE LINEE
//ORIZZONTALI CORRISPNDENTI ALLE LINEETTE DEL GRAFICO

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
   return TMath::Sqrt( (1/x2Quadro) * e1Quadro  + ( TMath::Power(x1/x2Quadro, 2) * e2Quadro ));
}

double calcolaErroreDifferenza(double e1, double e2) {
    /*
        Propaga l'errore con la formula della differenza.
        simgaY = sqrt(e1^2 + e2^2)
        param:
        x1, x2: valori
        e1, e2: errori
        return: errore su x1/x2
    */
   double e1Quadro{TMath::Power(e1, 2)};
   double e2Quadro{TMath::Power(e2, 2)};
   return TMath::Sqrt( e1Quadro +  e2Quadro );
}

//ARGOMENTO: CON 1 FACCIO PLOT SCALATI SOVRAPPOSTI E BASTA, CON ALTRO FACCIO NON SCALATI RAPPORTI E DIFFERENZE

void cms_vs_alice(bool scaling=false) {

    // -------------------------- Operazioni preliminarie
    gROOT->Reset();
    gROOT->Clear();
    gStyle->SetOptLogx(); // La scala logaritmica su X la setto globalmente usando il puntatore globale gStyle

    //PUNTI CMS
    const int numeroPunti{9}; // Numero di punti del grafico

    //PUNTI ATLAS
    const int numeroPuntiA{10};

    //PUNTI SCALATI
    const int numeroScalati{6};


    // --------------------  Definisco i vettori di dati

    // ------------------------- CMS
    // Asse X CMS
    // Prendo il punto al centro della colonna A
    double pt[numeroPunti] { 4.5, 5.5, 6.5, 7.5, 10, 14, 20, 32, 70 };
    // Prendo la distanza tra gli estremi
    double erroriPt[numeroPunti] { 0.5, 0.5, 0.5, 0.5, 2, 2, 4, 8, 30 };

    // Dati CMS
    double puntiData[numeroPunti] = { 430.24, 230.12, 135.84, 65.71, 20.97, 3.93, 0.81, 0.097, 0.0033 };
    // Errori sui dati CMS
    double erroriData[numeroPunti] = { 69.3987642606496, 26.4029645149575, 16.1875139308272, 10.1785702019874, 2.04942943263727, 0.476705883895996, 0.09981810763913, 0.015399057425854, 0.001090649702509 };

    //Fattori di scaling CMS: A QUELLI CHE UNISCONO I BIN ASSEGNO MEDIA PESATA COL PT PER VALORE ED ERRORE
    double var{(7.67*4+7.56*8)/12.0};
    double var_err{(0.33*4+0.39*8)/12.0};
    //PROVA PER VEDERE SE COINCIDONO CON SLIDES
    //double scaleFactors[numeroScalati] {8.4, 8.4, 8.4, 8.4, 8.4, 8.4};
    double scaleFactors[numeroScalati] = {7.81, 7.83, 7.82, 7.81, 7.76, var };
    double scaleFactorsErr[numeroScalati] = {0.38, 0.35, 0.325, 0.31, 0.295, var_err};

    // Punti simulati CMS
    double f0nll[numeroPunti] { 220, 117.4, 66, 38.96, 14.125, 3.175, 0.6925, 0.08375, 0.003266666666667 };
    double erroriSimulatiPlus[numeroPunti] { 107, 47.4, 22.6, 11.42, 3.545, 0.655, 0.1225, 0.0125, 0.000376666666667 };
    double erroriSimulatiMinus[numeroPunti] { 157.2, 71.2, 35, 18.38, 5.52, 0.915, 0.155, 0.01375, 0.0004 };

    // Rapporto Mc/Data CMS
    // Punti
    double puntiRapporto[numeroPunti];
    // Errori (ottenuti con la propagazione)
    double erroriRapportoPlus[numeroPunti];
    double erroriRapportoMinus[numeroPunti];

    std::cout << "Stampo info per rapporto MC/Data." << std::endl;
    // Riempio il vettore con i rapporti e  gli errori
    for (int i{0}; i < numeroPunti; ++i) {
        puntiRapporto[i] = f0nll[i]/puntiData[i];
        erroriRapportoPlus[i] = calcolaErroreRapporto(f0nll[i], erroriSimulatiPlus[i], puntiData[i], erroriData[i]);
        erroriRapportoMinus[i] = calcolaErroreRapporto(f0nll[i], erroriSimulatiMinus[i], puntiData[i], erroriData[i]);
	      printf("Rapporto= %.2f   Errore-= %.2f    Errore+= %.2f\n",puntiRapporto[i], erroriRapportoMinus[i], erroriRapportoPlus[i]);
    }

    //PER SCALING, CONTROLLO IL PARAMETRO DI INPUT E SE � RICHIESTO SCALO: PAR==1
     if(scaling){
      for (int i{0}; i < numeroPunti; ++i){
          puntiData[i]=puntiData[i]/8.4;
          erroriData[i]=erroriData[i]/8.4;
          f0nll[i]=f0nll[i]/8.4;
          erroriSimulatiPlus[i]=erroriSimulatiPlus[i]/8.4;
          erroriSimulatiMinus[i]=erroriSimulatiMinus[i]/8.4;
        }
     }

    // ---------------------------- ALICE
    // Asse x ALICE
    // Prendo il punto al centro della colonna A
    double ptA[numeroPuntiA] { 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 9, 11, 18 };
    // Prendo la distanza tra gli estremi
    double erroriPtA[numeroPuntiA] { 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 1, 1, 6};

    // Dati ALICE
    double puntiDataA[numeroPuntiA] = { 230.59, 157.28, 80.64, 38.64, 18.91, 10.67, 6.13, 3.03, 1.28, 0.25 };
    // Errori sui dati ALICE (asimmetrici)
    double erroriDataPlusA[numeroPuntiA] = {30.4830161, 14.3631541, 7.05327045, 3.38507371,1.71545045, 1.00160983, 0.57830982, 0.2788409, 0.12122706, 0.02561738 };
    double erroriDataMinusA[numeroPuntiA] = {30.7118734, 14.9980731, 7.50561283, 3.68594954, 1.91383653, 1.10091882, 0.63540715, 0.30307136, 0.12882546, 0.02561738 };


    // Punti simulati ALICE
    double f0nllA[numeroPuntiA] {153.2, 103.3, 53.1, 26.9, 14.29, 8.035, 4.752, 2.411, 1.0535, 0.18958333 };
    double erroriSimulatiPlusA[numeroPuntiA] {216.01, 110.7, 45.47, 19.2, 8.76, 4.315, 2.278, 1.009, 0.37475, 0.05360833 };
    double erroriSimulatiMinusA[numeroPuntiA] {141.71, 80.19, 32.65, 13.47, 5.96, 2.855, 1.472, 0.656, 0.2585, 0.04080833};

    // Rapporto Mc/Data ALICE
    // Punti
    double puntiRapportoA[numeroPuntiA];
    // Errori (ottenuti con la propagazione)
    double erroriRapportoPlusA[numeroPuntiA];
    double erroriRapportoMinusA[numeroPuntiA];
    // Riempio il vettore con i rapporti e  gli errori
    for (int i{0}; i < numeroPuntiA; ++i) {
        puntiRapportoA[i] = f0nllA[i]/puntiDataA[i];
        erroriRapportoPlusA[i] = calcolaErroreRapporto(f0nllA[i], erroriSimulatiPlusA[i], puntiDataA[i], erroriDataPlusA[i]);
        erroriRapportoMinusA[i] = calcolaErroreRapporto(f0nllA[i], erroriRapportoMinusA[i], puntiDataA[i], erroriDataMinusA[i]);
    }


    // --------------------------- Definisco i grafici

    //GRAFICI CMS
    auto grData = new TGraphErrors{numeroPunti, pt, puntiData, erroriPt, erroriData};
    auto grMc = new TGraphAsymmErrors{numeroPunti, pt, f0nll,  erroriPt, erroriPt, erroriSimulatiMinus, erroriSimulatiPlus};
    auto grRapporto = new TGraphAsymmErrors{numeroPunti, pt, puntiRapporto, erroriPt, erroriPt, erroriRapportoMinus, erroriRapportoPlus};

    // Sovrappongo i dati e mc in un unico grafico
    auto multiGraph = new TMultiGraph{};
    multiGraph->Add(grData, "P"); // Usa il marker corrente (se non lo metto mi scrive la linea)
    multiGraph->Add(grMc, "2p"); // L'opzione 2 riempie il rettangolo
    // Ricorda, quando sovrapponi i grafici, il secondo va disegnato SENZA l'opzione "a"
    // L'opzione "a" ridisegna gli assi, se la metti al secondo si sminchia tutto


    //GRAFICI ALICE
    auto grDataA = new TGraphAsymmErrors{numeroPuntiA, ptA, puntiDataA, erroriPtA,erroriPtA, erroriDataMinusA, erroriDataPlusA};
    auto grMcA = new TGraphAsymmErrors{numeroPuntiA, ptA, f0nllA,  erroriPtA, erroriPtA, erroriSimulatiMinusA, erroriSimulatiPlusA};
    auto grRapportoA = new TGraphAsymmErrors{numeroPuntiA, ptA, puntiRapportoA, erroriPtA, erroriPtA, erroriRapportoMinusA, erroriRapportoPlusA};

    // Sovrappongo i dati e mc in un unico grafico: VOGLIO CHE I DATI VADANO SOPRA, CIO� SECONDI
    multiGraph->Add(grMcA, "2p"); // L'opzione 2 riempie il rettangolo
    multiGraph->Add(grDataA, "P"); // Usa il marker corrente (se non lo metto mi scrive la linea)
    //HO MESSO I GRAFIFI NELLO STESSO MULTIGRAPH DI PRIMA

    /*
    //GRAFICI SCALING FLAT
    auto grDataScaled = new TGraphErrors{numeroPunti, pt, puntiDataScaled, erroriPt, erroriDataScaled};
    auto grMcScaled = new TGraphAsymmErrors{numeroPunti, pt, fonllScaled,  erroriPt, erroriPt, errorifonllScaledMinus, errorifonllScaledPlus};
    //Opzioni per il draw di SCALED
    grData->SetMarkerColor(1);
    grData->SetMarkerStyle(20); // La pallina

    grMc->SetFillColor(0); //PROVA: VOGLIO FARLO USCIRE BIANCO DENTRO
    grMc->SetMarkerColor(kGreen);
    g
    grMc->SetFillStyle(3001); // Il fill normale
    */


    // ------------------------ Disegno

    //CMS............................................
    // Opzioni grafiche per dati e mc
    grData->SetMarkerColor(1);
    grData->SetMarkerStyle(20); // La pallina

    grMc->SetFillColor(8);
    grMc->SetMarkerColor(kGreen);
    grMc->SetFillStyle(3001); // Il fill normale

    // Opzioni grafiche per il rapporto
    // grRapporto->SetMarkerColor(kBlack);
    grRapporto->SetMarkerColor(kGreen);
    grRapporto->SetFillColor(8);
    grRapporto->SetFillStyle(3001);

    //ALICE.........................................
    // Opzioni grafiche per dati e mc
    grDataA->SetMarkerColor(kViolet);
    grDataA->SetLineColor(kViolet);
    grDataA->SetMarkerStyle(21); // Il quadratino

    grMcA->SetFillColor(7); // 7==cyan
    grMcA->SetMarkerColor(7);
    grMcA->SetFillStyle(3001); // Il fill normale

    // Opzioni grafiche per il rapporto
    //grRapportoA->SetMarkerColor(kBlack);
    grRapportoA->SetMarkerColor(7);
    grRapportoA->SetFillColor(7);
    grRapportoA->SetFillStyle(3001);

    // Setto i titoli (SONO I PRIMI CHE SCRIVO CHE COMANDANO)
    // Multi graph
    multiGraph->SetTitle("");
    multiGraph->GetXaxis()->SetLabelSize(0); // Tolgo i numeri all'asse delle x per il multi graph (i numeri li disengno sul grafico del rapporto)
    multiGraph->GetXaxis()->SetTitle("");
    multiGraph->GetYaxis()->SetTitle("d#sigma / dp_{t} (#mub / GeV)");
    multiGraph->GetYaxis()->SetTitleSize(0.16*0.15/0.50);
    multiGraph->GetYaxis()->SetTitleOffset(0.24*0.50/0.15);
    multiGraph->GetYaxis()->SetLabelSize(0.16*0.15/0.50);
    //RICORDA: PER GRAFICI PIU GRANDI LA SIZE VA DIMINUITA, MA L'OFFSET VA AUMENTATO

    // Rapporto
    grRapporto->SetTitle("");
    // grRapporto->GetXaxis()->SetTitle("p_{t} (GeV)"); (LO METTO SOLO SE HO SOLO 2 GRAFICI)
    grRapporto->GetXaxis()->SetTitle("");
    grRapporto->GetYaxis()->SetTitle("FONLL/CMS");
    grRapporto->GetYaxis()->CenterTitle();

    // Necessarie perché la grandezza del font di default viene decisa in base
    // alla grandezza del pad su cui viene disegnato l'asse, dato che sto facendo un pad
    // più piccolo, devo ingrandire il font dei titoli
    // Ingrandisco i titoli degli assi
    grRapporto->GetXaxis()->SetTitleSize(0.15); // in % rispetto alla larghezza del pad ORIGINALI 0.08
    grRapporto->GetYaxis()->SetTitleSize(0.16);
    // Ingrandisco i numeri sugli assi
    grRapporto->GetXaxis()->SetLabelSize(0); // metto 0.08 se ho solo questo e 0 se ho altro sotto
    grRapporto->GetYaxis()->SetLabelSize(0.16);
    //grRapporto->GetXaxis()->SetTitleOffset(1.2);
    grRapporto->GetYaxis()->SetTitleOffset(0.24);

    // Metto gli stessi settaggi per rapporto ALICE + titolo asse x
    grRapportoA->SetTitle("");
    grRapportoA->GetYaxis()->SetTitle("FONLL/ALICE");
    grRapportoA->GetYaxis()->CenterTitle();
    grRapportoA->GetXaxis()->SetTitle("p_{t} (GeV)");
    grRapportoA->GetXaxis()->SetTitleSize(0.16*0.6); // in % rispetto alla larghezza del pad ORIGINALI 0.08
    grRapportoA->GetYaxis()->SetTitleSize(0.16*0.6);
    grRapportoA->GetXaxis()->SetLabelSize(0.16*0.6); // in % rispetto alla larghezza del pad
    grRapportoA->GetYaxis()->SetLabelSize(0.16*0.6);
    grRapportoA->GetXaxis()->SetTitleOffset(1.4);
    grRapportoA->GetYaxis()->SetTitleOffset(0.24/0.6);

    // Setto i minimi e massimi
    // Multi graph
    multiGraph->SetMinimum(2e-3);
    multiGraph->SetMaximum(20e3);
    multiGraph->GetXaxis()->SetLimits(0.80, 1.2e2);
    // Rapporto CMS
    grRapporto->SetMinimum(0.1);
    grRapporto->SetMaximum(1.8);
    grRapporto->GetXaxis()->SetLimits(0.80, 1.2e2);
    // Rapporto ALICE
    grRapportoA->SetMinimum(0);
    grRapportoA->SetMaximum(1.8);
    grRapportoA->GetXaxis()->SetLimits(0.80, 1.2e2);


    // ---------------- Disegno nei vari pad

    // Definisco un canvas
    auto c1 = new TCanvas{"c1", "Plots",800,600};

    // Nota: Quando uso c1->cd() selezioni un pad quindi il puntatore gPad punterà a quel pad.
    // Quindi tutte le opzioni grafiche tipo gPad->SetLogy(), agiranno solo su quel pad.
    // Comunque per poter fare pad non della stessa altezza ho dovuto creare i pad e non usare divide

    // In basso a sinistra è (0, 0), in alto a destra è (1, 1)
    // NOTA sui PAD:
    // Definisco i due pad, è necessario definirli uno dopo l'altro, questo perché
    // se definissi il secondo pad dopo aver fatto il Draw del primo grafico, cambierebbe il sistema di coordinate
    // e non avrei piè in basso a sinistra (0, 0) -> in alto a destra (1, 1), ma le coordinate sarebbero
    // dipendenti dai valori scritti sul grafico (cioè dalle coordinate dei punti)
    auto p1 = new TPad{"p1", "Dati vs mc", 0.05, 0.45, 0.95, 0.95}; // xmin, ymin, xmax, ymax  ORIGINALE:  0.05, 0.3, 0.95, 0.95
    p1->SetBottomMargin(0); // Tolgo il margine bianco attorno al pad (sotto)
    p1->SetTickx(2); // Con questa mi disegna le tacchette anche sopra per il primo pad
    p1->SetTicky(2); // Tacchette anche a destra per il primo pad

    auto p2 = new TPad{"p2", "Rapporto CMS", 0.05, 0.30, 0.95, 0.45}; //originale: 0.28-0-48
    // Setto i margini per il secondo
    p2->SetTopMargin(0);
    p2->SetBottomMargin(0); // Perché ho grafico sotto, altrimenti metto 0.25
    p2->SetLeftMargin(0.1);
    p2->SetTicky(2);

    auto p3 = new TPad{"p3", "Rapporto ALICE", 0.05, 0.05, 0.95, 0.30};  //originale superiore y 0.25
    p3->SetTopMargin(0);
    p3->SetBottomMargin(0.40); // Se non setto questi due i titoli degli assi non si vedono bene
    p3->SetLeftMargin(0.1);
    p3->SetTicky(2);

    // FORMULA ESTENSIONE TERZA CANVAS: A DIFFERENZA DELLA SECONDA, C'è MARGINE INFERIORE PER TITOLO
    // ALLORA SE Y3 è L'ALTEZZA DI P3, Y2 L'ALTEZZA DI P2 E M IL MARGINE:
    // Y3 - M*Y3 = Y2
    // ES: Y2=0.15 E M=0.4. ALLORA Y3=0.25

    // Disegno i due pad (disegno prima questi e poi tutto il resto viene sopra)
    p3->Draw();
    p2->Draw();
    p1->Draw();

    // ------------------- pad 3 (in basso) -----------------
    // Seleziono il secondo pad e disegno il grafico del rapporto
    p3->cd();
    // Disegno il rapporto
    grRapportoA->GetYaxis()->SetNdivisions(5);
    grRapportoA->GetYaxis()->SetTickLength(0.015);
    grRapportoA->Draw("a2p");
    // Linea
    TLine * line2 = new TLine(1,1,100,1);
    line2->SetLineColor(1);
    line2->SetLineStyle(1);
    line2->SetLineWidth(2);
    line2->Draw("Psame");
    //Punti su linea
    Double_t myVecA[numeroPuntiA]; //Creo vettore di uno
    for (int i=0;i<numeroPuntiA;i++) myVecA[i]=1;
    TGraph *tempA = new TGraph(numeroPuntiA,ptA, myVecA);
    tempA->SetMarkerStyle(21);
    tempA->SetMarkerColor(6);
    tempA->Draw("Psame");

    // ------------------- pad 2 (medio) -----------------
    // Seleziono il secondo pad e disegno il grafico del rapporto
    p2->cd();
    // Disegno il rapporto
    grRapporto->GetYaxis()->SetNdivisions(5);
    grRapporto->GetYaxis()->SetTickLength(0.015);
    grRapporto->Draw("a2p");
    //Linea su 1
    TLine *line1 = new TLine(1,1,100,1);
    line1->SetLineColor(1);
    line1->SetLineStyle(1);
    line1->SetLineWidth(2);
    line1->Draw("same");
    //Punti su linea
    Double_t myVec[numeroPunti]; //Creo vettore di uno
    for (int i=0;i<numeroPunti;i++) myVec[i]=1;
    TGraph *temp= new TGraph(numeroPunti,pt, myVec);
    temp->SetMarkerStyle(20);
    temp->SetMarkerColor(1);
    temp->Draw("Psame");



    // ----------------- pad 1 (in alto) --------------------
    // Seleziono il primo pad, imposto la scala logaritmica e disegno il grafico
    p1->cd();
    p1->SetLogy();
    // Disegno il Tgraph
    multiGraph->Draw("a");
    //Ridisegno i grafici dei punti per essere sicuro che stiamo sopra
    grData->Draw("Psame");
    grDataA->Draw("Psame");


    // ------------------ Aggiunte grafiche al pad 1 (scritte e legenda)

    // Disegno la legenda
    auto leg = new TLegend{0.15, 0.10, 0.35, 0.40}; // xmin, ymin, xmax, ymax rispetto al pad  ORIGINALI:0.15, 0.2, 0.35, 0.3
    leg->AddEntry(grData, "CMS Data");
    leg->AddEntry(grMc, "CMS FONLL", "F"); // L'opzione F disegna il rettangolino, se non la metti ti disegna anche la croce nella legenda
    leg->AddEntry(grDataA, "ALICE Data");
    leg->AddEntry(grMcA, "ALICE FONLL", "F");
    leg->SetBorderSize(0); // Per disegnarla senza il rettangolo intorno
    leg->Draw();

    // Scritta sezione d'urto
    TLatex latex{};
    latex.SetTextSize(0.06);
    latex.DrawLatex(38, 30e3,"29 nb^{-1} (13 TeV)"); // I primi due parametri sono x e y da cui parte la scritta, le unit� sono quelle sul grafico
    latex.SetTextSize(0.08);
    latex.DrawLatex(1.1, 3010, "CMS");
    latex.SetTextSize(0.07);
    latex.DrawLatex(4, 3010, "Prompt D^{0} and D^{0} production"); //non so fare anti


    c1->SaveAs("./Plots/plot_cms_vs_alice.pdf");


    // SISTEM0 I VETTORI PER IL CONFRONTO : DATI ALICE, DATI CMS E PT

    // -------------- CMS
    //PER CMS I PRIMI 5 VANNO BENE, POI DEVO UNIRE IL SESTO E IL SETTIMO: 12-16 e 16-24 -> 12-24
    puntiData[5]= 1.85216801; // Sesto e settimo uniti
    erroriData[5]= 0.22517802;

    //PER ALICE DEVO PARTIRE DAL QUARTO, UNIRE TERZULTIMO E PENULTIMO E PRENDERE ULTIMO
    for (int i=0;i<4;i++){
      puntiDataA[i]=puntiDataA[i+3];
      erroriDataPlusA[i]=erroriDataPlusA[i+3];
      erroriDataMinusA[i]=erroriDataPlusA[i+3];
    }
    puntiDataA[4]=2.155;
    erroriDataPlusA[4]=0.19964985;
    erroriDataMinusA[4]=0.21554596;
    puntiDataA[5]=puntiDataA[9];
    erroriDataPlusA[5]= erroriDataPlusA[9];
    erroriDataMinusA[5]= erroriDataMinusA[9];

    //PT: USO QUELLO DI CMS E UNISCO SOLO IL SESTO E IL SETTIMO
    pt[5]=18;
    erroriPt[5]=6;

    std::cout << "Stampe per debug" << std::endl;
    //Stampa per debug
    for(int i=0;i<numeroScalati;i++){
      printf("Punti data cms= %.4f  punti data alice= %.4f\n", puntiData[i], puntiDataA[i]);
    }

    //Punti cms scalati con scaling dipendente da pt
    double scaleData[numeroScalati];
    double scaleDataErr[numeroScalati];
    double scaleSim[numeroScalati];
    double scaleSimErrPlus[numeroScalati];
    double scaleSimErrMinus[numeroScalati];
    for (int i=0;i<numeroScalati;i++){
      scaleData[i]=puntiData[i]/scaleFactors[i];
      scaleDataErr[i]=calcolaErroreRapporto(puntiData[i], erroriData[i], scaleFactors[i], scaleFactorsErr[i]);
      scaleSim[i]=f0nll[i]/scaleFactors[i];
      scaleSimErrPlus[i]=calcolaErroreRapporto(f0nll[i], erroriSimulatiPlus[i], scaleFactors[i], scaleFactorsErr[i]);
      scaleSimErrMinus[i]=calcolaErroreRapporto(f0nll[i], erroriSimulatiMinus[i], scaleFactors[i], scaleFactorsErr[i]);
    }

    for(int i=0;i<numeroScalati;i++){
      printf("Punti cms scalati= %.4f  punti alice scalati= %.4f\n", scaleData[i], puntiDataA[i]);
    }



    //Rapporti CA CA E DIFFERENZA C-A/C
    double ca[numeroScalati];
    double caErrPlus[numeroScalati];
    double caErrMinus[numeroScalati];
    double ac[numeroScalati];
    double acErrPlus[numeroScalati];
    double acErrMinus[numeroScalati];
    double c_a[numeroScalati];
    double c_aErrPlus[numeroScalati];
    double c_aErrMinus[numeroScalati];

    for (int i=0;i<numeroScalati;i++){

      ca[i]=scaleData[i]/puntiDataA[i];
      caErrPlus[i]=calcolaErroreRapporto(scaleData[i], scaleDataErr[i]  ,puntiDataA[i], erroriDataPlusA[i]);
      caErrMinus[i]=calcolaErroreRapporto(scaleData[i], scaleDataErr[i], puntiDataA[i], erroriDataMinusA[i]);

      ac[i]=puntiDataA[i]/scaleData[i];
      acErrPlus[i]=calcolaErroreRapporto( puntiDataA[i], erroriDataPlusA[i], scaleData[i], scaleDataErr[i] );
      acErrMinus[i]=calcolaErroreRapporto( puntiDataA[i], erroriDataMinusA[i], scaleData[i], scaleDataErr[i] );

      c_a[i]=(scaleData[i]-puntiDataA[i])/scaleData[i];
      double diff=scaleData[i]-puntiDataA[i];
      double diffErrPlus=calcolaErroreDifferenza(scaleDataErr[i], erroriDataPlusA[i]);
      double diffErrMinus=calcolaErroreDifferenza(scaleDataErr[i], erroriDataMinusA[i]);
      c_aErrPlus[i]=calcolaErroreRapporto(diff, diffErrPlus,   scaleData[i], scaleDataErr[i]);
      c_aErrMinus[i]=calcolaErroreRapporto(diff, diffErrMinus,   scaleData[i], scaleDataErr[i]);
      //////////////////////////////////////////////

    }

    //Grafici scalati
    auto grca = new TGraphAsymmErrors(numeroScalati, pt, ca, erroriPt, erroriPt, caErrMinus, caErrPlus);
    auto grac = new TGraphAsymmErrors(numeroScalati, pt, ac, erroriPt, erroriPt, acErrMinus, acErrPlus);
    auto grc_a = new TGraphAsymmErrors(numeroScalati, pt, c_a, erroriPt, erroriPt, c_aErrMinus, c_aErrPlus);

    //Linea a 1 rapporto
    auto linea_rapporto= new TLine(4,1,24,1);
    auto linea_differenza= new TLine(4,0,24,0);
    linea_rapporto->SetLineWidth(2);
    linea_differenza->SetLineWidth(2);

    grca->SetTitle("");
    grca->GetXaxis()->SetTitle("p_{T} (GeV/c)");
    grca->GetYaxis()->SetTitle("CMS/ALICE D^{0} cross section");
    grca->SetMarkerStyle(20);
    grca->SetMarkerColor(kRed);
    grca->SetMinimum(0.7);
    grca->SetMaximum(2.05);
    grca->GetXaxis()->SetLimits(2,26);
    grca->GetXaxis()->SetLabelOffset(0);
    grca->GetYaxis()->SetLabelOffset(0);
    grca->SetLineColor(kRed);

    grac->SetTitle("");
    grac->GetXaxis()->SetTitle("p_{T} (GeV/c)");
    grac->GetYaxis()->SetTitle("ALICE/CMS D^{0} cross section");
    grac->SetMarkerStyle(20);
    grac->SetMarkerColor(kRed);
    grac->SetMinimum(0.42);
    grac->SetMaximum(1.28);
    grac->GetXaxis()->SetLimits(2,26);
    grac->GetXaxis()->SetLabelOffset(0);
    grac->GetYaxis()->SetLabelOffset(0);
    grac->SetLineColor(kRed);

    grc_a->SetTitle("");
    grc_a->GetXaxis()->SetTitle("p_{T} (GeV/c)");
    grc_a->GetYaxis()->SetTitle("(CMS-ALICE)/CMS D^{0} cross section");
    grc_a->SetMarkerStyle(20);
    grc_a->SetMarkerColor(kRed);
    grc_a->SetMinimum(-0.3);
    grc_a->SetMaximum(0.58);
    grc_a->GetXaxis()->SetLimits(2,26);
    grc_a->GetXaxis()->SetLabelOffset(0);
    grc_a->GetYaxis()->SetLabelOffset(0);
    grc_a->SetLineColor(kRed);

    gStyle->SetOptLogx(0); // Da ora in poi globalmente non voglio più log x

    // Disegno e salvo i tre grafici
    auto c2 = new TCanvas{"c2", "Plots",800,600};
    gPad->SetLogx(0);
    gPad->SetTickx(2);
    gPad->SetTicky(2);
    // gPad->SetTopMargin(0);
    //gPad->SetRightMargin(0);
    grca->Draw("AEP");
    linea_rapporto->Draw("same");
    c2->SaveAs("./Plots/cms_over_alice.pdf");

    auto c3 = new TCanvas{"c3", "Plots",800,600};
    gPad->SetTickx(2);
    gPad->SetTicky(2);
    // gPad->SetTopMargin(0);
    //gPad->SetRightMargin(0);
    grac->Draw("AEP");
    linea_rapporto->Draw("same");
    c3->SaveAs("./Plots/alice_over_cms.pdf");

    auto c4 = new TCanvas{"c4", "Plots",800,600};
    gPad->SetTickx(2);
    gPad->SetTicky(2);
    //gPad->SetTopMargin(0);
    //gPad->SetRightMargin(0);
    grc_a->Draw("AEP");
    linea_differenza->Draw("same");
    c4->SaveAs("./Plots/cms-alice_over_cms.pdf");





    // Cancello i puntatori che non mi servono più
    /*
    delete grMc;
    delete grData;
    delete multiGraph;
    delete grRapporto;
    delete grMcA;
    delete grDataA;
    delete grRapportoA;
    delete temp;
    delete tempA;
    delete leg;
    delete p1;
    delete p2;
    delete p3;
    delete c1;
    */
}
