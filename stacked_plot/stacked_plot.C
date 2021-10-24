#include <iostream>
#include <string>
#include <ctime>
#include <array>
#include "TStyle.h"
#include "TFile.h"
#include "TH1.h"

using std::string;

const string currentDate() {
    /* Funzione che restituisce la data corrente */
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    string date{buf};
    return date.substr(0, 10);
}


// Perchè usi le TString e non le stringhe normali ? Vedi appunti su metodo Get di TFile.
// Il valore di default della data è settato alla data di oggi.
void stacked_plot(const TString &estensione, const TString &tipoMuone, const TString &osservabile = "qtimespt", bool logScale = true, const TString &scaleType = "unif") {

    // ------------------------- Operazioni preparatorie ------------------------
    const TString data{currentDate()};
    const TString parametro{"1gpv"}; // A quanto pare per ragioni storiche

    const TString outPath{"./Plots/"};
    const TString nomeBasePlots{osservabile + "_" + tipoMuone + "_" + data + "." +estensione};

    // Resetto la memoria
    gROOT->Reset();
    gROOT->Clear();

    // Resetto lo stile
    gROOT->SetStyle("Plain");

    // Cancello le statistiche
    gStyle->SetOptStat(0000);

    // Se necessario setto la scala logaritmica
    if (logScale) gStyle->SetOptLogy();
    else gStyle->SetOptLogy(0);

    TCanvas *c1 = new TCanvas{"c1", "plots", 500, 500};
    c1->SetBorderSize(2);
    c1->cd();


    // ------------------------- Lettura dei file e costruzione istogrammi ------------------------
    // Apro i file
    TFile mc_file{"./root_files/Histos_Mc_MinBias_1aprnew_goodZB_last.root", "read"};
    TFile data_file{"./root_files/Histos_Data_ZeroBias_1aprnew_goodZB_last_OK.root","read"};

    // Array di istogrammi, contengono le diverse componenti delle MC
    array<TH1D *, 5> listaHistMc;
    // Nomi delle componenti delle MC
    array<TString, 5> listaTipiMuoni = {
        "_fake_",
        "_ghost_",
        "_light_",
        "_charm_",
        "_beauty_"
    };

    // Il casting è necessario perchè Get restituisce un TObject *
    // Get vuole come argomento un const char *, se tutti gli argomenti li passi come string
    // lui non riesce a fare la conversione in const char*. Probabilmente se uno degli argomenti è una TString
    // l'operazione di somma restituisce una const char *.
    // La cosa giusta da fare sarebbe o inizializzare la stringa c-like a partire dalla std::string con la libreria
    // oppure usa la TString e vaffanculo
    TH1D *hData1gpv{dynamic_cast<TH1D *>(data_file.Get("hMuons_"+osservabile+"_"+tipoMuone+"_"+parametro))};
    TH1D *hData2gpv{dynamic_cast<TH1D *>(data_file.Get("hMuons_"+osservabile+"_"+tipoMuone+"_2gpv"))};

    // Costruisco l'istogramma somma dei dati
    TH1D *dataSomma{static_cast<TH1D *>(hData1gpv->Clone("dataSomma"))};
    dataSomma->Add(hData1gpv, hData2gpv, 1, 1);
    dataSomma->Sumw2();

    // Gestisco overflow e underflow per i dati veri
    // Overflow
    int numeroBins{dataSomma->GetNbinsX()};
    auto numeroEventiOverflow{dataSomma->GetBinContent(numeroBins + 1)};
    dataSomma->AddBinContent(numeroBins, numeroEventiOverflow);
    // Underflow
    auto numeroEventiUnderflow{dataSomma->GetBinContent(0)};
    dataSomma->AddBinContent(1, numeroEventiUnderflow);

    // A questo punto posso cancellare gli istogrammi con i dati singoli
    delete hData1gpv, hData2gpv;

    // Fattore di scaling
    Double_t ScaleLumi = 1.0;
    if (scaleType == "unif") {
        ScaleLumi = 0.64904;
        cout << "Absolute Normalization Scale Real/Simulated =" << ScaleLumi << endl;
    }

    for (int i{0}; i < listaHistMc.size(); ++i) {
        // Riempio il vettore con le componenti delle monte carlo
        listaHistMc[i] = dynamic_cast<TH1D *>(mc_file.Get("hMuons_" + osservabile + "_" + tipoMuone + listaTipiMuoni[i] + parametro));

        // Gestisco Overflow e underflow
        // ----- Overflow -----------
        // Prendo il numero di bin
        numeroBins = listaHistMc[i]->GetNbinsX();
        // Prendo gli eventi overflow (che sono nel bin n + 1)
        numeroEventiOverflow = listaHistMc[i]->GetBinContent(numeroBins + 1);
        // Li metto nel bin n-simo
        listaHistMc[i]->AddBinContent(numeroBins, numeroEventiOverflow);

        // ----- Underflow -----------
        numeroEventiUnderflow = listaHistMc[i]->GetBinContent(0);
        listaHistMc[i]->AddBinContent(1, numeroEventiUnderflow);

        // ----- Faccio lo scaling --------------
        listaHistMc[i]->Scale(ScaleLumi);
    }

    // ---------------------------  Faccio lo stacking dei mc
    TH1D * histoDueComponenti{dynamic_cast<TH1D *>(listaHistMc[0]->Clone("histoDueComponenti"))};
    histoDueComponenti->Add(listaHistMc[1], 1); // Contiene due componenti (fake + ghost)
    TH1D * histoTreComponenti{dynamic_cast<TH1D *>(histoDueComponenti->Clone("histoTreComponenti"))};
    histoTreComponenti->Add(listaHistMc[2], 1); // Tre componenti (fake + ghost + light)
    TH1D * histoQuattroComponenti{dynamic_cast<TH1D *>(histoTreComponenti->Clone("histoQuattroComponenti"))};
    histoQuattroComponenti->Add(listaHistMc[3], 1); // Quattro componenti (fake + ghost + light + charm)
    TH1D * histoMcTotale{dynamic_cast<TH1D *>(histoQuattroComponenti->Clone("histoMcTotale"))};
    histoMcTotale->Add(listaHistMc[4], 1); // Tutte le componenti

    // ---------------------------- Disegno -------------------------
    // Colori per il riempimento
    listaHistMc[0]->SetFillColor(kGray);
    histoDueComponenti->SetFillColor(kGreen);
    histoTreComponenti->SetFillColor(kRed);
    histoQuattroComponenti->SetFillColor(kYellow);
    histoMcTotale->SetFillColor(kCyan);

    if (logScale) histoMcTotale->SetMinimum(0.5); // Per tenere sotto controllo la coda di destra

    // Disegno gli isto con le varie componenti sommate una sopra l'altra
    // Devo disegnare dal più alto al più basso altrimenti si coprono a vicenda e si vede solo l'ultima
    histoMcTotale->Draw("histsame");
    histoQuattroComponenti->Draw("histsame");
    histoTreComponenti->Draw("histsame");
    histoDueComponenti->Draw("histsame");
    listaHistMc[0]->Draw("histsame");

    // Disegno l'isto con i dati
    dataSomma->SetMarkerStyle(20);
    dataSomma->SetMarkerColor(kBlack);
    dataSomma->Draw("esame");

    // Ridisegno gli assi perchè potrebbero essere coperti
    gPad->RedrawAxis();

    // --------------------------- Titoli --------------------------
    histoMcTotale->SetTitle("");

    // Setto i titoli agli assi
    const TString titoloX{osservabile}; // Da cambiare in base alla variabile !
    histoMcTotale->GetXaxis()->SetTitle(titoloX);

    // Bisogna farlo in stile C perchè altrimenti non puoi fare il print con una sola cifra significativa
    // Con sprintf puoi usare %.1f
    char titoloY[80];
    auto binWidth{dataSomma->GetBinWidth(1.)};
    sprintf(titoloY, tipoMuone + " muons/%.1f", binWidth);
    histoMcTotale->GetYaxis()->SetTitle(titoloY);
    histoMcTotale->GetYaxis()->SetTitleOffset(1.25); // Necessario perchè si sovrappone il titolo ai numeri



    // ----------------------- Legenda -------------------------------------
    // Setto il margine del grafico a destra e sinistra per essere sicuro di disegnare la
    // legenda nel posto giusto
    gPad->SetMargin(0.12, 0.9, 0.1, 0.9); // sinistra, destra, sotto, sopra

    // Disegno la legenda
    double xMin{0.6}, yMin{0.9}, xMax{0.9}, yMax{0.7};
    TLegend leg{xMin, yMin, xMax, yMax};
    leg.SetTextSize(0.03);
    leg.AddEntry(dataSomma, "Data", "LP"); // Il terzo argomento sono le opzioni di drawing, con LP, disegni la linea con il pallino
    leg.AddEntry(histoMcTotale, "b (+ c cascade)", "F"); // Con l'opzione F disegan il quadratino sulla legenda
    leg.AddEntry(histoQuattroComponenti, "charm", "F");
    leg.AddEntry(histoTreComponenti, "light", "F");
    leg.AddEntry(histoDueComponenti, "duplicate", "F");
    leg.AddEntry(listaHistMc[0], "punch-through", "F");
    leg.Draw();


    // Salvo lo stacked plot
    c1->SaveAs(outPath + "stacked_plot_" + nomeBasePlots);

    // Cancello i puntatori di cui non ho più bisogno
    delete c1;
    for (int i{0}; i < listaHistMc.size(); ++i) {
        delete listaHistMc[i];
    }
    delete dataSomma;
}
