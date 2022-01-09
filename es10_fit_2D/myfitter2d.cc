#include <TStyle.h>
#include <TAxis.h>
#include <TLatex.h>
#include <TPaveText.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TNtupleD.h>
#include <TH1D.h>
//
#include <RooRealVar.h>
#include <RooDataSet.h>
#include <RooGaussian.h>
#include <RooChebychev.h>
#include <RooExponential.h>
#include <RooAddPdf.h>
#include <RooProdPdf.h>
#include <RooDecay.h>
#include <RooGaussModel.h>
#include <RooAddModel.h>
#include <RooPlot.h>
//
#include "myloop.h"
#include "plotDressing2D.h"

using namespace RooFit;

// General fitting options
#define NUMBER_OF_CPU       1
#define DO_MINOS            kTRUE
// 0 - w/o DISPLAY
// 1 - w/  DISPLAY
#define DISPLAY             1

#define MASS_MIN            5.0
#define MASS_MAX            6.0
#define MASS_PEAK           BP_MASS
#define SOURCE              "myloop.root" 

void myfitter2d()
{
    // define variables: mass, proper time and error on proper tim:
    RooRealVar mass("mass","mass",MASS_MIN,MASS_MAX); 
    RooRealVar ct("ct","ct",-0.02,0.28);
    RooRealVar cterr("cterr","cterr",0.0001,0.008);
    
    // Creo output file () e NTupla
    TFile *fout = new TFile("myfitter2d.root","recreate");     // output file    
    TNtupleD *_nt = new TNtupleD("_nt","_nt","mass:ct:cterr"); // output ntuple
    
    // input 
    TFile *fin = new TFile(SOURCE); 
    TTree *tin = (TTree*)fin->Get("ntkp");
    
    // setting up rootuple for reading
    ReducedBranches br; // Creo un oggetto della classe 
    br.setbranchadd(tin); // Chiamo il metodo per riempire le branch con gli indirizzi di memoria.
    
    // reading rootuple
    for (int evt=0; evt < tin->GetEntries(); evt++) {

        tin->GetEntry(evt);

        // cuts to select events/cands (non prendo alcuni eventi)
        if (br.hltbook[HLT_Dimuon16_Jpsi_v1]!=1) continue;
        if (br.vtxprob<=0.15) continue;
        if (br.tk1pt<=2.0) continue;

        // filling the 3D vector in the output ntuple
        double var[3];
        var[0] = br.mass;
        var[1] = br.ctau2d;
        var[2] = br.ctau2derr;
        _nt->Fill(var);
       }

    fin->Close(); // Chiuso il file di input 


    // the dataset contains only the 3 variables of interest
    // Creo un dataset dalla ntupla _nt (tridimensionale) su tre variabili (mass, ct, cterr)
    RooDataSet *data = new RooDataSet("data","data",_nt,RooArgSet(mass,ct,cterr));


    // initialization
    // Prendo il numero di valori ottenuti dalla differenza tra i due cut. 
    double n_signal_initial = data->sumEntries(TString::Format("abs(mass-%g)<0.015",MASS_PEAK))
    - data->sumEntries(TString::Format("abs(mass-%g)<0.030&&abs(mass-%g)>0.015",MASS_PEAK,MASS_PEAK));

    // Eventi totali - valori ottenuti prima 
    double n_combinatorial_initial = data->sumEntries() - n_signal_initial;


    //-----------------------------------------------------------------

    // signal PDF 

    // MASSA
    // double gaussian for the signal in mass
    RooRealVar m_mean("m_mean","m_mean",MASS_PEAK,MASS_MIN,MASS_MAX);
    RooRealVar m_sigma1("m_sigma1","m_sigma1",0.016,0.001,0.045);
    RooRealVar m_sigma2("m_sigma2","m_sigma2",0.035,0.001,0.090);
    RooRealVar m_fraction("m_fraction","m_fraction",0.5);
    // Stessa media ma diversa sigma 
    RooGaussian m_gaussian1("m_gaussian1","m_gaussian1",mass,m_mean,m_sigma1);
    RooGaussian m_gaussian2("m_gaussian2","m_gaussian2",mass,m_mean,m_sigma2);

    // Pdf 
    RooAddPdf pdf_m_signal("pdf_m_signal","pdf_m_signal",RooArgList(m_gaussian1,m_gaussian2),RooArgList(m_fraction));

    // TEMPO 
    // exponential convoluted with gaussian resolution for the signal in ct
    RooRealVar res_sig_mean("res_sig_mean","res_sig_mean",0.0,-1.,1.);
    RooRealVar res_sig_sigma("res_sig_sigma","res_sig_sigma",1.0,0.3,2.0);
    
    // E' un modello di risoluzione (da convolvere con il RooDecay) 
    RooGaussModel res_signal("res_signal","res_signal",ct,res_sig_mean,res_sig_sigma,cterr);

    // Modello di decay single sided (da convolvere con modello di risoluzione)
    // Pdf 
    RooRealVar ctau("ctau","ctau",0.04911,0.010,0.090);
    RooDecay pdf_t_signal("pdf_t_signal","pdf_t_signal",ct,ctau,res_signal,RooDecay::SingleSided);

    // bidimensional signal pdf
    // Prodotto delle pdf di massa e tempo 
    RooProdPdf pdf_signal("pdf_signal","pdf_signal", RooArgSet(pdf_m_signal, pdf_t_signal));


    // Background combinatoriale  
    // combinatorial background PDF (prompt or non-prompt J/psi + random track)

    // MASSA 
    // exponential for the combinatorial background in mass
    RooRealVar m_par1("m_par1","m_par1",-0.3,-2.,+2.);
    RooExponential pdf_m_combinatorial("pdf_m_combinatorial","pdf_m_combinatorial",mass,m_par1);

    // TEMPO 

    // Non-prompt 
    // exponential convoluted with gaussian resolution for the non-prompt background in ct
    RooRealVar ctau_nonprompt("ctau_nonprompt","ctau_nonprompt",0.0500, 0.0010, 0.1000);
    // pdf 
    RooDecay pdf_t_nonprompt("pdf_t_nonprompt","pdf_t_nonprompt",ct,ctau_nonprompt,res_signal,RooDecay::SingleSided);

    // prompt 
    // Sum of gaussian resolution function (res_signal) for prompt background in ct and the previous exponential for NP-bkg
    RooRealVar prompt_fraction("prompt_fraction","prompt_fraction",0.5,0.0,1.0); 
    // pdf 
    RooAddPdf pdf_t_combinatorial("pdf_t_combinatorial","pdf_t_combinatorial",RooArgList(res_signal,pdf_t_nonprompt),RooArgList(prompt_fraction));

    // Background totale (prodotto)
    // bidimensional combinatorial-bkg pdf 
    RooProdPdf pdf_combinatorial("pdf_combinatorial","pdf_combinatorial",RooArgSet(pdf_m_combinatorial,pdf_t_combinatorial));


    // Background fisico 
    // B->J/psi+track+X background PDF
    // 
    // single gaussian for the physical background in mass
    //
    RooRealVar m_jpsix_mean("m_jpsix_mean","m_jpsix_mean",5.1,5.0,5.3);
    RooRealVar m_jpsix_sigma("m_jpsix_sigma","m_jpsix_sigma",0.05,0.01,0.10);
    RooGaussian pdf_m_jpsix("pdf_m_jpsix","pdf_m_jpsix",mass,m_jpsix_mean,m_jpsix_sigma);
    //
    // exponential convoluted with gaussian resolution for the physical background in ct
    //
    RooRealVar ctau_jpsix("ctau_jpsix","ctau_jpsix",0.0500, 0.0010, 0.1000);
    RooDecay pdf_t_jpsix("pdf_t_jpsix","pdf_t_jpsix",ct,ctau_jpsix,res_signal,RooDecay::SingleSided); 
    //
    // bidimensional physical-bkg pdf
    //
    RooProdPdf pdf_jpsix("pdf_jpsix","pdf_jpsix",RooArgSet(pdf_m_jpsix, pdf_t_jpsix));


    // FULL MODEL (SIGNAL + 2 BKGS)
    //
    // define coefficients for addition of the 3 bidimentional pdfs
    //
    RooRealVar n_signal("n_signal","n_signal",n_signal_initial,0.,data->sumEntries());
    RooRealVar n_combinatorial("n_combinatorial","n_combinatorial",n_combinatorial_initial,0.,data->sumEntries());
    RooRealVar n_jpsix("n_jpsix","n_jpsix",200.,0.,data->sumEntries());
    //
    RooAddPdf model("model","model",
                    RooArgList(pdf_signal, pdf_combinatorial, pdf_jpsix),
                    RooArgList(n_signal, n_combinatorial, n_jpsix));


    // finally go for fitting !
    //
    model.fitTo(*data,Minos(DO_MINOS),NumCPU(NUMBER_OF_CPU),Offset(kTRUE));
    //
    // go to display plots with fits superimposed on data distributions

// Plot 
#if DISPLAY
    //
    // Display mass plots
    //-------------------
    //
    TCanvas *c1 = canvasDressing("c1");
    //
    RooPlot* frame_m = mass.frame();
    //
    TH1D* histo_data_m = (TH1D*)data->createHistogram("histo_data_m", mass, Binning(50,mass.getMin(),mass.getMax()));
    //
    histo_data_m->Sumw2(false);
    histo_data_m->SetBinErrorOption(TH1::kPoisson);
    histo_data_m->SetMarkerStyle(20);
    histo_data_m->SetMarkerSize(0.8);
    histo_data_m->SetLineColor(kBlack);
    for (int i=1; i<=50; i++)
        if (histo_data_m->GetBinContent(i)==0) histo_data_m->SetBinError(i,0.);
    //
    data->plotOn(frame_m,Binning(50),Invisible());
    model.plotOn(frame_m,Precision(5E-4));
    model.plotOn(frame_m,Precision(5E-4),Components(pdf_signal),LineColor(kRed),LineWidth(2),LineStyle(kSolid),FillStyle(3008),FillColor(2), VLines(), DrawOption("F"));
    model.plotOn(frame_m,Precision(5E-4),Components(pdf_combinatorial),LineColor(kCyan+1),LineWidth(2),LineStyle(2));
    model.plotOn(frame_m,Precision(5E-4),Components(pdf_jpsix),LineColor(kBlack),LineWidth(2),LineStyle(7));
    //
    frame_m->SetTitle("");
    frame_m->GetXaxis()->SetTitle("M_{J/#psi K^{#pm}} [GeV]");
    frame_m->GetXaxis()->SetLabelFont(42);
    frame_m->GetXaxis()->SetLabelOffset(0.01);
    frame_m->GetXaxis()->SetTitleSize(0.06);
    frame_m->GetXaxis()->SetTitleOffset(1.09);
    frame_m->GetXaxis()->SetLabelFont(42);
    frame_m->GetXaxis()->SetLabelSize(0.055);
    frame_m->GetXaxis()->SetTitleFont(42);
    frame_m->GetYaxis()->SetTitle("Events / 20 MeV");
    frame_m->GetYaxis()->SetLabelFont(42);
    frame_m->GetYaxis()->SetLabelOffset(0.01);
    frame_m->GetYaxis()->SetTitleOffset(1.14);
    frame_m->GetYaxis()->SetTitleSize(0.06);
    frame_m->GetYaxis()->SetTitleFont(42);
    frame_m->GetYaxis()->SetLabelFont(42);
    frame_m->GetYaxis()->SetLabelSize(0.055);
    //
    frame_m->Draw();
    histo_data_m->Draw("Esame");
    LegendMassProj();
    //
    c1->SaveAs("massPlot_13TeV.png");
    //
    //c1->Delete();
    //
    /////////////////////////////////////////////////////
    //
    // Display c*proper-time plots 
    //----------------------------
    //
    TCanvas *c2 = canvasDressing("c2");
    //
    RooPlot* frame_t = ct.frame();
    //
    TH1D* histo_data_t = (TH1D*)data->createHistogram("histo_data_t", ct, Binning(120,ct.getMin(),ct.getMax()));
    //
    histo_data_t->Sumw2(false);
    histo_data_t->SetBinErrorOption(TH1::kPoisson);
    histo_data_t->SetMarkerStyle(20);
    histo_data_t->SetMarkerSize(0.8);
    histo_data_t->SetLineColor(kBlack);
    for (int i=1; i<=120; i++)
        if (histo_data_t->GetBinContent(i)==0) histo_data_t->SetBinError(i,0.);
    //
    data->plotOn(frame_t,Binning(120),Invisible());
    model.plotOn(frame_t,Precision(5E-4),ProjWData(cterr,*data));
    model.plotOn(frame_t,Precision(5E-4),ProjWData(cterr,*data),Components(pdf_signal),LineColor(kRed),LineWidth(2),LineStyle(kSolid),FillStyle(3008),FillColor(2), VLines(), DrawOption("F"));
    model.plotOn(frame_t,Precision(5E-4),ProjWData(cterr,*data),Components(pdf_combinatorial),LineColor(kCyan+1),LineWidth(2),LineStyle(2));
    model.plotOn(frame_t,Precision(5E-4),ProjWData(cterr,*data),Components(pdf_jpsix),LineColor(kBlack),LineWidth(2),LineStyle(7));
    //
    frame_t->SetTitle("");
    frame_t->GetXaxis()->SetTitle("ct [cm]");
    frame_t->GetXaxis()->SetLabelFont(42);
    frame_t->GetXaxis()->SetLabelOffset(0.01);
    frame_t->GetXaxis()->SetTitleSize(0.06);
    frame_t->GetXaxis()->SetTitleOffset(1.09);
    frame_t->GetXaxis()->SetLabelFont(42);
    frame_t->GetXaxis()->SetLabelSize(0.055);
    frame_t->GetXaxis()->SetTitleFont(42);
    frame_t->GetYaxis()->SetTitle("Events / 25 #mum");
    frame_t->GetYaxis()->SetLabelFont(42);
    frame_t->GetYaxis()->SetLabelOffset(0.01);
    frame_t->GetYaxis()->SetTitleOffset(1.14);
    frame_t->GetYaxis()->SetTitleSize(0.06);
    frame_t->GetYaxis()->SetTitleFont(42);
    frame_t->GetYaxis()->SetLabelFont(42);
    frame_t->GetYaxis()->SetLabelSize(0.055);
    //
    frame_t->GetYaxis()->SetRangeUser(0.5,histo_data_t->GetMaximum()*2.);
    //
    frame_t->Draw();
    histo_data_t->Draw("Esame");
    LegendLifetimeProj();
    //
    c2->SetLogy();
    //
    c2->SaveAs("ctPlot_13TeV.png");
    //
    c1->Delete();
    c2->Delete();
    //
#endif
    //
    fout->Write();
    fout->Close();
}
