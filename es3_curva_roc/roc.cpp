#include <TH1.h>
#include <string> 
#include "../funzioni_utili/funzioni.h"

void roc(const TString var1 = "LeptIP3D_worst", const TString var2 = "LeptSTIP_SLIP_worst", const TString extension = ".png") {

    std::string today{utils::currentDate()};

    // Vettore di file
    //// i==0 signal; i==1 bkg TT; i==2 bkg Zbb
    TFile f[3] = {
        {"./root_files/H150_ZZ_4l_10TeV_GEN_HLT_Presel_glb_2e2mu_2e2mu_merged.root","read"}, // Signal H->ZZ->2e2m (mass=150GeV)
        {"./root_files/TT_4l_10TeV_Presel_4l_2e2mu_merged.root","read"},                    // BKG ttbar
        {"./root_files/LLBB_4l_10TeV_Presel_4l_2e2mu_merged.root","read"}                   // BKG Zbbar->2lbbar
    };



    TH1D *histoVars[3][2]; // Metterò gli histo per la var1 
    // il punto i,j contiene l'isto per il file i, della variabile j

    TH1D *histoVarsRapporto[3][2]; // Meterò gli histo con i rapporti 


    int jMax{100}; // Capire perchè 
    // Forse perchè entrambe le variabili ha valori più o meno fino a 100 bin, nei successivi c'è solo coda. 
    // Infatti se vedi l'efficienza a j = 100, è più di 0.99

    // valoriEfficienzeVar1 servirà dopo per plottare le efficienze
    double valoriEfficienzeVars[3][jMax][2]; // Valori delle efficienze 
    // il punto valoriEfficienze[i][j] contiene il rapporto 
    // integraleTotaleVar1[i]/integraleParzialeFinoA(j+1)[i]
    // Il terzo indice rappresenta la variabile (0 = var1, 1 = var2)


    TString var{}; // Contiene il valore della variabile che sto guardando  
    for (int varValue{0}; varValue < 2; ++varValue) {
        // varValue = 0 -> var1
        // varValue = 1 -> var2

        // Prendo la variabile giusta
        if (varValue == 0) var = var1; // Se sono al primo loop prendo var1
        else var = var2; // altrimenti prendo var2

        std::cout << " -------------------------------------------- \n"; 
        std::cout << "Operazioni su var" << varValue + 1 << ": " << var << std::endl;

        // Loop sui file 
        for (int i{0}; i<3; ++i) {
            // Prendo l'istogramma giusto (a seconda di var) 
            if (var == "AllLeptIP3D") histoVars[i][varValue] = dynamic_cast<TH1D *>(f[i].Get("h" + var + "_large")); 
            else histoVars[i][varValue] = dynamic_cast<TH1D *>(f[i].Get("h" + var)); 

            // Calcolo gli integrali 

            // Valuto il numero di bin 
            auto numeroBin = histoVars[i][varValue]->GetNbinsX();

            // Scritta di debug (sono 200 bin per var1 e 500 per var2)
            std::cout << "Numero di bin dell'histo histoVars[" << i << "]: " << numeroBin << std::endl; 

            // calcolo l'integrale totale 
            // Metto n + 1 perchè considero il bin di overflow 
            double integraleTotaleVar{histoVars[i][varValue]->Integral(1, numeroBin + 1)}; // Perchè ho 200 bin in ogni istogramma 


            // NOTA: sulla conversione tra stringhe c++ e stringhe c-like. 
            // Qui vogliamo creare una stringa fatta così "hVar1Ratio[i]" dove al posto della i devo mettere il numero.  
            // Lui faceva così:  
            // char histoname[128]; 
            // sprintf(histoname,"hVarRatio[%d]",i);
            // In c++ è sempre meglio usare le stringhe built-in, quint il modo più giusto di creare la stringa è questo: 
            std::string nomeHisto{"hVar" + std::to_string(varValue + 1) + "Ratio[" + std::to_string(i) + "]"}; 
            // Il costruttore di TH1D (e in generale tutti i metodi che in root vogliono le stringhe), però, prende un tipo const char *, cioè una stringa c-like, o al più una TString. 
            // Quindi effettuo la conversione usando il metodo delle stringhe "c_str()". Che restituisce un const char*. 
            const char *nomeHistoChar = nomeHisto.c_str(); // Converte la stringa c++ in stringa c-like
            histoVarsRapporto[i][varValue] = new TH1D(nomeHistoChar, "Partial integration over total", numeroBin, 0., 1.);

            for (int j{0}; j < jMax + 1; ++j) {
                // Calcolo l'integrale parziale 
                double integraleParziale{histoVars[i][varValue]->Integral(1, j)};
                // Calcolo il rapporto tra parziale e totale
                double efficienza{integraleParziale/integraleTotaleVar}; 
                // Lo stampo per debug 
                if (j%10 == 0) std::cout << "Valore efficienza per i = " << i << " j = " << j << " : " << efficienza << std::endl;
                // Salvo il valore nell'array delle efficienze
                valoriEfficienzeVars[i][j - 1][varValue] = efficienza; 
                // Inserisco il valore nell'istogramma
                histoVarsRapporto[i][varValue]->Fill(efficienza); 
            }
        }

        //    -------------------------- Disegno per var1 e var2 
            TCanvas myCanvas{"myCanvas", "Plots", 800, 600}; 
            myCanvas.Divide(2, 2); 
            // Disegno i primi 3
            for (int i{0}; i<=3; ++i) {
                myCanvas.cd(i + 1); 
                if (i != 3) histoVars[i][varValue]->Draw();
                else {
                    histoVars[0][varValue]->Draw(); 
                    histoVars[1][varValue]->Draw("same"); 
                    histoVars[2][varValue]->Draw("same"); 
                }
            }
            myCanvas.Update(); 
            myCanvas.SaveAs("./Plots/Histo_"+ var + "_" + today + "." + extension); 
            myCanvas.Clear(); // Pulisco il canvas prima di finire il loop, così per la var2 si trova il canvas pulito

        std::cout << "Fine operazioni su var" << varValue + 1 << "\n"; 
        std::cout << " --------------------------------------------" << std::endl; 
    }


    
}
