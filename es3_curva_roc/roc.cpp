#include <TH1.h>
#include <string> 
#include "../funzioni_utili/funzioni.h"

void roc(const TString var1 = "LeptIP3D_worst", const TString var2 = "LeptSTIP_SLIP_worst", const TString extension = "png") {
    gROOT->Reset(); 
    gROOT->Clear(); 

    std::string today{utils::currentDate()};

    // -------------------- Variabili 

    const int numeroFile{3}; // Numero di file
    const int numeroVariabili{2}; // Numero di variabili 
    const int maxBin{200}; // Indice di bin massimo (Calcolo l'integrale al massimo tra 1 e maxBin) 
    // Capire perchè 
    // Forse perchè entrambe le variabili ha valori più o meno fino a 100 bin, nei successivi c'è solo coda. 
    // Infatti se vedi l'efficienza a j = 100, è più di 0.99
    // In realtà lui comunque poi usa tutti i bin. All'inizio mettevo 100 ma poi ho aumentato a 200 perchè vengono valori diversi dai suoi sennò

    // Vettore di file
    //// i==0 signal; i==1 bkg TT; i==2 bkg Zbb
    TFile f[] = {
        {"./root_files/H150_ZZ_4l_10TeV_GEN_HLT_Presel_glb_2e2mu_2e2mu_merged.root","read"}, // Signal H->ZZ->2e2m (mass=150GeV)
        {"./root_files/TT_4l_10TeV_Presel_4l_2e2mu_merged.root","read"},                    // BKG ttbar
        {"./root_files/LLBB_4l_10TeV_Presel_4l_2e2mu_merged.root","read"}                   // BKG Zbbar->2lbbar
    };

    // Definisco un canvas nello scope più esterno così uso solo questo per tutti i disegni 
    TCanvas myCanvas{"myCanvas", "Plots", 800, 600}; 

    // il punto i,j contiene l'istogramma per il file i, della variabile j
    TH1D *histoVars[numeroFile][numeroVariabili]; 

    // Isto con i rapporti
    TH1D *histoVarsRapporto[numeroFile][numeroVariabili]; 

    // Valori delle efficienze 
    // Punto [i][j][k] = IntegraleParzialeFinoAk/IntegraleTotale per file i, variabile j
    double valoriEfficienzeVars[numeroFile][numeroVariabili][maxBin]; 

    TString var{}; // Valore della variabile che sto guardando  
    // Operazioni da fare sulle due variabili: 
    // 1. Calcolo per ogni file il valore dell'efficienza per i diversi cut (da 0 a maxBin) 
    // 2. Disegno gli istogrammi per ogni variabile 
    // 3. Disegno gli istogrammi dei rapporti 
    for (int varValue{0}; varValue < numeroVariabili; ++varValue) {
        // varValue = 0 -> var1
        // varValue = 1 -> var2

        // Prendo la variabile giusta
        if (varValue == 0) var = var1; // Se sono al primo loop prendo var1
        else var = var2; // altrimenti prendo var2

        // Scritta di debug
        std::cout << " -------------------------------------------- \n"; 
        std::cout << "Operazioni su var" << varValue + 1 << ": " << var << std::endl;


        // Loop sui file 
        for (int i{0}; i < numeroFile; ++i) {

            std::cout << "Variabile: " << varValue  + 1 << " ---- File: " << i << std::endl;
            // Prendo l'istogramma giusto (a seconda di var) 
            if (var == "AllLeptIP3D") histoVars[i][varValue] = dynamic_cast<TH1D *>(f[i].Get("h" + var + "_large")); 
            else histoVars[i][varValue] = dynamic_cast<TH1D *>(f[i].Get("h" + var)); 


            // Calcolo gli integrali 

            // Poi il numero di bin lo uso per definire il binnaggio di histoVarsRapporto[i][varValue]
            // Valuto il numero di bin 
            auto numeroBin = histoVars[i][varValue]->GetNbinsX();
            // Scritta di debug (sono 200 bin per var1 e 500 per var2)
            std::cout << "Numero di bin dell'histo histoVars[" << i << "]: " << numeroBin << std::endl; 

            // calcolo l'integrale totale 
            // Metto n + 1 perchè considero il bin di overflow 
            double integraleTotaleVar{histoVars[i][varValue]->Integral(1, numeroBin + 1)}; // Perchè ho 200 bin in ogni istogramma 

            // Nome istogramma
            std::string nomeHisto{"hVar" + std::to_string(varValue + 1) + "Ratio[" + std::to_string(i) + "]"}; 
            const char *nomeHistoChar = nomeHisto.c_str(); // Converte la stringa c++ in stringa c-like
            histoVarsRapporto[i][varValue] = new TH1D(nomeHistoChar, "Partial integration over total", numeroBin, 0., 1.);

            // Calcolo le efficienze
            for (int j{1}; j < maxBin; ++j) {
                // Integrale parziale fino a j
                double integraleParziale{histoVars[i][varValue]->Integral(1, j)};
                // Calcolo il rapporto tra parziale e totale
                double efficienza{integraleParziale/integraleTotaleVar}; 
                // Lo stampo per debug 
                if (j%10 == 0) std::cout << "Valore efficienza per i = " << i << " j = " << j << " : " << efficienza << std::endl;
                // Salvo il valore nell'array delle efficienze
                valoriEfficienzeVars[i][varValue][j - 1] = efficienza; 

                // Inserisco il valore nell'istogramma
                histoVarsRapporto[i][varValue]->Fill(efficienza); 
            }

        }

        //  -------------------------- Disegno istogrammi  
        myCanvas.Divide(2, 2); 

        // Disegno i tre istogrammi in tre pad diversi e nel quarto li disegno sovrapposti 
        for (int i{0}; i<=numeroFile; ++i) {
            myCanvas.cd(i + 1); 
            // Per i primi 3 disegno solo il valore dell'istogramma per la variabile
            if (i != 3) histoVars[i][varValue]->Draw();
            // Nel quarto pad disegno i tre sovrapposti
            else {
                histoVars[0][varValue]->Draw(); 
                histoVars[1][varValue]->Draw("same"); 
                histoVars[2][varValue]->Draw("same"); 
            }
        }
        // Salvo i plot
        myCanvas.Update(); 
        myCanvas.SaveAs("./Plots/Histo_"+ var + "_" + today + "." + extension); 
        myCanvas.Clear(); // Pulisco il canvas prima di finire il loop, così per la var2 si trova il canvas pulito

        // ---------------------------- Disegno istogrammi rapporti 
        gStyle->SetOptStat(0); 
        myCanvas.SetGridx();
        myCanvas.SetGridy();
        myCanvas.Divide(0, 3); 

        // Vettore di legende
        TLegend legende[3] = {
            {.1, .8, .2, .9},
            {.1, .8, .2, .9},
            {.1, .8, .2, .9}
        }; 
        // Nota che ne ho bisogno perchè quando chiamo l'operazione SaveAs() sul canvas, devono essere presenti in memoria
        // tutti gli oggetti che sono sul canvas. Quindi servono tutte e 3 le legende.

        // Loop sui file
        for (int i{0}; i<numeroFile; ++i) {
            myCanvas.cd(i + 1); 
            histoVarsRapporto[i][varValue]->SetLineColor(i + 2); // Così il primo ha colore linea 2, il secondo 3 ecc
            // Disegno l'isto del rapporto 
            histoVarsRapporto[i][varValue]->Draw(); 

            // Opzioni grafiche legenda 
            legende[i].SetTextFont(42);
            legende[i].SetFillColor(0);
            legende[i].SetTextSize(0.05);

            // Metto la scritta giusta a seconda del file
            if (i==0) legende[i].AddEntry("","signal", "");
            else if (i==1) legende[i].AddEntry("","t#bar{t}", "");
            else if (i==2) legende[i].AddEntry("","Zb#bar{b}", "");
            
            // Disegno la legenda
            legende[i].Draw();
        }

        myCanvas.Update();
        myCanvas.SaveAs("./Plots/Integral-over-total_" + var + "_" + today + "." + extension); 
        myCanvas.Clear(); 

        std::cout << "Fine operazioni su var" << varValue + 1 << "\n"; 
        std::cout << " --------------------------------------------" << std::endl; 
    }

    // --------------------- Disegno le survival efficiency 

    myCanvas.Divide(0, 2); 
    // grafici[i][j] contiene il grafico del background i (ttbar - zbbar) per la variabile j
    TGraph *grafici[2][numeroVariabili]; // Vettore di grafici  
    TLegend legende[] = {
        {.65, .1, .9, .3},
        {.65, .1, .9, .3}
    }; // Vettore di legende 

    // modo più veloce per fare un colore unico quando faccio il loop con due indici.
    int colori[2][2] = {
        {1, 2}, 
        {3, 4}
    }; 
    // Loop sulle variabili 
    for (int varValue{0}; varValue < numeroVariabili; ++varValue) {
        if (varValue == 0) var = var1; // Se sono al primo loop prendo var1
        else var = var2; // altrimenti prendo var2

        myCanvas.cd(varValue + 1); 
        for (int i{0}; i < 2; ++i) {
            // Faccio segnale contro i vari background 
            // Creo un nuovo grafico e poi lo uso per fare l'assegnazione al vettore di grafici 
            TGraph *grafico = new TGraph{maxBin, valoriEfficienzeVars[i + 1][varValue], valoriEfficienzeVars[0][varValue]}; 
            grafici[i][varValue] = grafico;
            grafici[i][varValue]->SetTitle("Signal(H150) survival efficiency VS Bkgs survival efficiency after FULL preselection"); 
            grafici[i][varValue]->SetMarkerColor(colori[i][varValue]);
            grafici[i][varValue]->SetMarkerSize(0.6);
            grafici[i][varValue]->SetMarkerStyle(20);
            grafici[i][varValue]->GetYaxis()->SetTitle("H150 survival efficiency");
            grafici[i][varValue]->GetXaxis()->SetTitle("Bkgs survival efficiency");
            if (i == 0) grafici[i][varValue]->Draw("aP"); // Se il primo ridisegno gli assi (opzione a)
            else grafici[i][varValue]->Draw("Psame"); // Altrimenti disegno sopra
        }
        // Disegno la legenda
        legende[varValue].SetTextFont(42);
        legende[varValue].SetFillColor(0);
        legende[varValue].SetTextSize(0.05);
        legende[varValue].AddEntry(grafici[0][varValue],"t#bar{t}" + var, "P");
        legende[varValue].AddEntry(grafici[1][varValue],"Zb#bar{b}" + var, "P");
        legende[varValue].Draw();
    }

    // Salvo il canvas 
    myCanvas.Update();
    myCanvas.SaveAs("./Plots/Efficiency-due-variabili_" + today + "." + extension); 
    myCanvas.Clear(); 

    // --------------------------- Ora disegno l'overlap di tutto 
    myCanvas.Divide(1, 0); 
    TLegend legenda{.65, .1, .9, .3}; 
    for (int i{0}; i < numeroVariabili; ++i) {
        if (i == 0) var = var1; // Se sono al primo loop prendo var1
        else var = var2; // altrimenti prendo var2

        for (int j{0}; j < 2; ++j) {
            if ((i == 0) && (j == 0)) grafici[i][j]->Draw("ap"); 
            else grafici[i][j]->Draw("psame"); 
        }
        legenda.AddEntry(grafici[0][i],"t#bar{t}" + var, "P");
        legenda.AddEntry(grafici[1][i],"Zb#bar{b}" + var, "P");
    }
    legenda.Draw();

    myCanvas.Update();
    myCanvas.SaveAs("./Plots/Efficiency-overlap-tutto_" + today + "." + extension); 
    myCanvas.Clear(); 


    // Poi distruggo gli oggetti associati ai puntatori 
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < numeroVariabili; ++j) {
            delete grafici[i][j];
        }
    }
     
    // Chiudo i file 
    for (int i{0}; i<numeroFile; ++i) {
        f[i].Close(); 
        f[i].Delete();
    }
}
