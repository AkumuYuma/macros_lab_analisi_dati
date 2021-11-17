#include <TH1D.h> 
#include <string>

namespace rf = RooFit; 
using namespace rf; 

void macro() {
    gROOT->Clear(); 
    gROOT->Reset(); 

    TFile f{"./root_files/hlt_5_newSoftMuon_alsoInPsiPrimeWind.root", "READ"}; 


    // Numero di istogrammi 
    int numeroIstogrammi{23}; 

    RooDataHist * histoList[numeroIstogrammi];
    
    double meanList[numeroIstogrammi]; 
    double meanErrorList[numeroIstogrammi]; 
    double sigmaList[numeroIstogrammi];
    double sigmaErrorList[numeroIstogrammi]; 


    // Carico tutti gli istogrammi in un array 
    for (int i{1}; i < numeroIstogrammi; ++i) {

        std::string nomeStringa{"PsiPrimeMass_bin" + std::to_string(i + 1)}; 
        const char * nome{nomeStringa.c_str()};

        RooRealVar x{"x", "x", 3.4, 3.9}; 

        TCanvas myC{"myC", "myC", 800, 600}; 
        myC.cd();

        RooPlot * xFrame{x.frame(Title(""))}; 
        xFrame->SetTitle(nome); 

        // Segnale CB 
        RooRealVar meanCB{"meanCB", "meanCB", 3.7, 3.64, 3.72}; // Media
        RooRealVar sigmaCB{"sigmaCB", "sigmaCB", 0.05, 0.001, 0.2}; // Sigma
        RooRealVar alpha{"alpha", "alpha", 5, 0.01, 50};
        RooRealVar nCB{"nCB", "nCB", 5, 0.01, 50};
        // Pdf
        RooCBShape cbPdf{"cbPdf", "cbPdf", x, meanCB, sigmaCB, alpha, nCB};

        // Bkg cheby 
        RooRealVar c0{"c0", "1st coeff", -0.3, -1e4, 1e4};
        RooRealVar c1{"c1", "2nd coeff", 0.01, -1e4, 1e4};
        RooChebychev chebyPdf{"cheby", "Chebychev", x, RooArgList(c0, c1)};

        // Total pdf 
        // Componenti
        RooRealVar nSig{"nSig", "Number of signal cands", 4e5, 1e3, 1e7};
        RooRealVar nBkg{"nBjg", "Number of bkg component", 60e3, 1e3, 1e7};

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
        xFrame->Draw(); 

        std::string nomeFileStringa{"./Plots/fit_" + std::to_string(i + 1) + ".png"};
        const char * nomeFile{nomeFileStringa.c_str()};
        myC.SaveAs(nomeFile); 
        

        delete xFrame; 
        delete daAggiungere;
    }


    // TGraph con i risultati 
    TCanvas myC{"myC", "myC", 800, 600}; 
    TGraphErrors graph{22, meanList, sigmaList, meanErrorList, sigmaErrorList}; 
    graph.SetTitle("Sigma vs Rapidity"); 
    graph.SetMarkerStyle(20); 
    graph.SetMarkerColor(kRed); 
    graph.GetXaxis()->SetLimits(3.665, 3.69); 
    graph.Draw("ap"); 
    myC.SaveAs("./Plots/Graph.png"); 

}