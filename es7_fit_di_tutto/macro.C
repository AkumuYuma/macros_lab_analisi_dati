#include <TH1D.h> 
#include <string>

namespace rf = RooFit; 
using namespace rf; 

void macro() {
    gROOT->Clear(); 
    gROOT->Reset(); 

    TFile f{"./root_files/hlt_5_newSoftMuon_alsoInPsiPrimeWind.root", "READ"}; 


    // Numero di istogrammi 
    const int numeroIstogrammi{21}; 

    RooDataHist * histoList[numeroIstogrammi];
    
    double meanList[numeroIstogrammi]; 
    double meanErrorList[numeroIstogrammi]; 
    double sigmaList[numeroIstogrammi];
    double sigmaErrorList[numeroIstogrammi]; 


    // Carico tutti gli istogrammi in un array 
    for (int i{0}; i < numeroIstogrammi - 1; ++i) {

        std::string nomeStringa{"PsiPrimeMass_bin" + std::to_string(i + 2)}; 
        const char * nome{nomeStringa.c_str()};

        RooRealVar x{"x", "x", 3.4, 3.9}; 

        TCanvas myC{"myC", "myC", 800, 600}; 
        myC.cd();

        RooPlot * xFrame{x.frame(Title(""))}; 
        xFrame->SetTitle(nome); 

        // Segnale CB 
        RooRealVar meanCB{"meanCB", "meanCB", 3.675, 3.64, 3.72}; // Media
        RooRealVar sigmaCB{"sigmaCB", "sigmaCB", 0.04, 0.001, 0.7}; // Sigma
        RooRealVar alpha{"alpha", "alpha", 5.0, 0.01, 50.};
        RooRealVar nCB{"nCB", "nCB", 3.0, 0.001, 20.};
        // Pdf
        RooCBShape cbPdf{"cbPdf", "cbPdf", x, meanCB, sigmaCB, alpha, nCB};

        // Bkg cheby 
        RooRealVar c0{"c0", "1st coeff", -0.3, -1000, 100};
        RooRealVar c1{"c1", "2nd coeff", 0.1, -1000, 100};
        RooChebychev chebyPdf{"cheby", "Chebychev", x, RooArgList(c0, c1)};

        // Total pdf 
        // Componenti
        RooRealVar nSig{"nSig", "Number of signal cands", 4e5, 1e2, 1e8};
        RooRealVar nBkg{"nBjg", "Number of bkg component", 60e3, 1e2, 1e8};

        RooAddPdf totalPdf{"totalPdf", "totalPdf", RooArgList(cbPdf, chebyPdf), RooArgList(nSig, nBkg)};

        TH1D * daAggiungere{(TH1D *)f.Get(nome)}; 
        RooDataHist rooDaAggiungere{daAggiungere->GetName(), daAggiungere->GetTitle(), RooArgSet(x), rf::Import(*daAggiungere, kFALSE)};
        totalPdf.fitTo(rooDaAggiungere, Extended(kTRUE)); 

        meanList[i] = meanCB.getValV(); 
        meanErrorList[i] = meanCB.getError(); 
        sigmaList[i] = sigmaCB.getValV();
        sigmaErrorList[i] = sigmaCB.getError();


        // Plotto 
        // Istogramma
        rooDaAggiungere.plotOn(xFrame); 
        // Pdf 
        totalPdf.plotOn(xFrame, rf::LineColor(kRed));
        totalPdf.plotOn(xFrame, rf::Components(RooArgSet(cbPdf)), rf::LineColor(kGreen));
        totalPdf.plotOn(xFrame, rf::Components(chebyPdf), rf::LineStyle(kDashed));
        totalPdf.plotOn(xFrame, rf::LineColor(kRed));
        xFrame->Draw(); 

        std::string nomeFileStringa{"./Plots/fit_" + std::to_string(i + 2) + ".png"};
        const char * nomeFile{nomeFileStringa.c_str()};
        myC.SaveAs(nomeFile); 
        
        delete xFrame; 
        delete daAggiungere;
    }


    // Rapidità (sta scritto nel titolo dell'histo nel file root)
    double rapidity[numeroIstogrammi]; 
    rapidity[0] = -2.1; 
    double errorR[numeroIstogrammi]; 
    for (int i{1}; i < numeroIstogrammi; ++i) {
        rapidity[i] = rapidity[i - 1] + 0.2;
        errorR[i] = 0.1;
    }

    // TGraph con i risultati 
    TCanvas myC{"myC", "myC", 800, 600}; 
    
    TGraphErrors graph{21, rapidity, sigmaList, errorR, sigmaErrorList}; 
    graph.SetTitle("Sigma vs Rapidity"); 
    graph.SetMarkerStyle(20); 
    graph.SetMarkerColor(kRed); 
    graph.SetMinimum(0.02); 
    graph.SetMaximum(0.1); 
    // TF1 f1{"f1", "pol2", -2.5, 2.5}; 
    // f1.SetParameters(0.08, 0.08, 0); 
    // graph.Fit("pol2"); 
    // gStyle->SetOptFit(1111); 
    graph.Draw("ap"); 
    myC.SaveAs("./Plots/Graph.png"); 

}