#include "TH1.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TNtuple.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "RooAbsData.h"
#include "RooBernstein.h"
#include "RooRealVar.h"
#include "RooAbsPdf.h"
#include "RooChebychev.h"
#include "RooExponential.h"
#include "RooGaussian.h"
#include "RooJohnson.h"
#include "RooAbsPdf.h"
#include "RooPlot.h"
#include "RooAddPdf.h"
#include "RooDataHist.h"
#include "RooArgSet.h"
#include "RooArgList.h"
#include "RooVoigtian.h"
#include "RooFit.h"
#include "RooDataSet.h"
#include "RooFormulaVar.h"
#include "RooLinkedList.h"
#include "TFile.h"
#include "RooStats/SPlot.h"
#include "RooAbsDataStore.h"
#include "TLine.h"
#include "TLegend.h"
#include <iostream>

using namespace RooFit;

////////////////////////////////
//  root[0] .L B0s_bkgsub-1.C+
//  root[1] sidesub()
////////////////////////////////

int sidesub() {
  //--Reading the file
  TFile inputfile("./root_files/b0sData.root","READ");

  // Definisco un canvas 
  TCanvas myc("canvas","canvas",1200,800);

  //--Loading the dataset
  RooDataSet* b0sData = (RooDataSet*) inputfile.Get("alldata");

  //-Printing the tuple content
  b0sData->Print();


  //-set scales
  //...for B signal
  auto massmin = 5.0; //auto deduct the type directly from the content
  auto massmax = 5.7;
  auto massbins = (5.7 - 5.0)/0.007; // 7 MeV bins

  //... for phi signal 
  auto phimass = 1.019, dphi = 0.025;
  auto phimin = 0.995;
  auto phimax = 1.045;

  auto nentries = b0sData->sumEntries();

  std::cout << ">> Number of entries: " << nentries << std::endl;

  // Definisco le variabili (Hanno lo stesso nome di quelle nel file)

  RooRealVar masskk ("ditrak_m","M(KK) [GeV]",phimin,phimax);
  int masskk_bins = 100;
  masskk.setBins(masskk_bins);

  RooRealVar dimuonditrk_m_rf_c ("dimuonditrk_m_rf_c","M(#mu#muKK)[GeV]",4.0,5.7);
  int massb0s_bins = 100;  // or 200 
  dimuonditrk_m_rf_c.setBins(massb0s_bins);

  RooRealVar dimuonditrk_ctauPV ("dimuonditrk_ctauPV","dimuonditrk_ctauPV",-1000.0,1000.0);

  RooRealVar dimuonditrk_ctauErrPV ("dimuonditrk_ctauErrPV","dimuonditrk_ctauErrPV",-1000.0,1000.0);

  RooRealVar dimuon_pt ("dimuon_pt","dimuon_pt",0.0,1000.0);

  RooRealVar ditrak_pt ("ditrak_pt","ditrak_pt",0.0,1000.0);

  RooRealVar dimuon_m("dimuon_m","dimuon_m",2.5,3.5);
  int massJpsi_bins = 100;
  dimuon_m.setBins(massJpsi_bins);

  //----plotting histogram
  //
  myc.Divide(2,2);
  myc.cd(1);
  TH1D* histo_Jpsimass = (TH1D*)b0sData->createHistogram("histo_Jpsimass", dimuon_m, Binning(massJpsi_bins,2.9,3.2));
  histo_Jpsimass->SetMarkerStyle(20);
  histo_Jpsimass->Draw("EP"); 

  myc.cd(2);
  TH1D* histo_phimass_beforecut = (TH1D*)b0sData->createHistogram("histo_phimass_beforecut", masskk, Binning(masskk_bins,phimin,phimax));
  histo_phimass_beforecut->SetMinimum(0);
  histo_phimass_beforecut->SetMaximum(500);
  histo_phimass_beforecut->SetMarkerStyle(20);
  histo_phimass_beforecut->Draw("EP");

  myc.cd(3);
  TH1D* histo_b0smass_beforecut = (TH1D*)b0sData->createHistogram("histo_b0smass_beforecut", dimuonditrk_m_rf_c, Binning(massb0s_bins,5.0,5.7));
  histo_b0smass_beforecut->SetMinimum(0);
  histo_b0smass_beforecut->SetMarkerStyle(20);
  histo_b0smass_beforecut->Draw("EP");

  myc.cd(4);
  TH1D* histo_b0s_displacement = (TH1D*)b0sData->createHistogram("histo_b0s_displacement", dimuonditrk_ctauPV, Binning(100, 0., 0.10));
  histo_b0s_displacement->SetMinimum(0);
  histo_b0s_displacement->SetMarkerStyle(20);
  histo_b0s_displacement->Draw("EP");

  myc.SaveAs("./Plots/B0s_and_Phi_Mass_before_cut.png");

  //- Create a filtered dataset for only DISPLACED ("non-prompt") B0s candidates  
  RooDataSet* b0sDataNonPrompt = (RooDataSet*) b0sData->reduce(RooFormulaVar("displacement","dimuonditrk_ctauPV/dimuonditrk_ctauErrPV>3.0",RooArgList(dimuonditrk_ctauErrPV,dimuonditrk_ctauPV)));
  // - note : cut on ctau significance (ctau/error) is applied

  auto nNewEntries = b0sDataNonPrompt->sumEntries();
  std::cout << ">> Number of non-prompt entries: " << nNewEntries << std::endl;

  //- Write this reduced dataset to an external rootuple file 
  TFile datasetfile("b0sData_nonprompt.root","RECREATE"); //and writing it in a ROOT file for future uses
  datasetfile.cd();
  b0sDataNonPrompt->Write();
  datasetfile.Close(); //remember always to close it

  //- Create a reduced variable collection for displaced candidates (reduced/skimmed rootuple)
  //
  RooDataSet* b0sDataNonPromptMass = (RooDataSet*) b0sDataNonPrompt->reduce(SelectVars(RooArgSet(dimuonditrk_m_rf_c,masskk))); 

  //- Histograms (for easier manipulation)
  // Inserisce i dati dopo il cut in istogrammi di ROOT 
  TH1D* histo_b0smass_aftercut = (TH1D*)b0sDataNonPrompt->createHistogram("histo_b0smass_aftercut", dimuonditrk_m_rf_c, Binning(massb0s_bins,5.0,5.7));
  TH1D* histo_b0smass_aftercut_200bins = (TH1D*)b0sDataNonPrompt->createHistogram("histo_b0smass_aftercut_200bins", dimuonditrk_m_rf_c, Binning(200,5.0,5.7));  
  TH1D* histo_phimass_aftercut = (TH1D*)b0sDataNonPrompt->createHistogram("histo_phimass_aftercut", masskk, Binning(masskk_bins,phimin,phimax));

  // Questo sarà la somma dell'isto prima del cut con l'isto dopo il cut 
  TH1D* histo_phimass_rejected = (TH1D*)histo_phimass_aftercut->Clone("histo_phimass_rejected");
  histo_phimass_rejected->Add(histo_phimass_beforecut,histo_phimass_aftercut,1,-1);
  histo_phimass_rejected->Sumw2();
 
  // Stessa cosa per b0s
  TH1D* histo_b0smass_rejected = (TH1D*)histo_b0smass_aftercut->Clone("histo_b0smass_rejected");
  histo_b0smass_rejected->Add(histo_b0smass_beforecut,histo_b0smass_aftercut,1,-1);
  histo_b0smass_rejected->Sumw2();

  // Plotto gli isto cuttati 
  TCanvas myc2("canvas","canvas",1200,800);
  myc2.Divide(2,2);

  // B0s
  myc2.cd(1); // Prima quelli dopo il cut 
  histo_b0smass_aftercut->SetMinimum(0);
  histo_b0smass_aftercut->SetMarkerStyle(20);
  histo_b0smass_aftercut->SetMarkerStyle(20);
  histo_b0smass_aftercut->Draw("EP");

  myc2.cd(2); // Poi il confronto tra prima del cut e rejected 
  histo_b0smass_beforecut->Draw("EP");
  histo_b0smass_rejected->SetFillColor(kYellow);
  histo_b0smass_rejected->SetLineColor(1);
  histo_b0smass_rejected->Draw("histsame");   // "hist" needed to get the filled colorr
  histo_b0smass_rejected->Draw("Esame");      // Needed to get seen full error bar
  histo_b0smass_aftercut->SetMarkerColor(2);
  histo_b0smass_aftercut->Draw("EPsame");
  
  // Phi 
  myc2.cd(3);
  histo_phimass_aftercut->SetMinimum(0);
  histo_phimass_aftercut->SetMaximum(300);
  histo_phimass_aftercut->SetMarkerColor(2);
  histo_phimass_aftercut->SetMarkerStyle(20);
  histo_phimass_aftercut->Draw("EP");

  myc2.cd(4);
  histo_phimass_beforecut->Draw("EP");
  histo_phimass_rejected->SetFillColor(kYellow);
  histo_phimass_rejected->SetLineColor(1);
  histo_phimass_rejected->Draw("histsame"); 
  histo_phimass_rejected->Draw("Esame");    
  histo_phimass_aftercut->SetMarkerColor(2);
  histo_phimass_aftercut->Draw("EPsame");
  myc2.SaveAs("./Plots/B0s_and_Phi_Masses_after_displacement-cut.png");

  //
  //== let's fit the B0s NONprompt signal
  //
  TCanvas myc1("canvas","canvas",1200,800);
  myc1.Divide(2,2);

  myc1.cd(1);
  dimuonditrk_m_rf_c.setRange("fitRange",massmin,massmax);
  dimuonditrk_m_rf_c.setBins(200);
  RooPlot* massFrame1 = dimuonditrk_m_rf_c.frame(Range(massmin,massmax));
  b0sDataNonPromptMass->plotOn(massFrame1);
  massFrame1->Draw("EP");

  myc1.cd(2);
  histo_b0smass_aftercut->Draw("EP");

  myc1.SaveAs("./Plots/b0sMass_NONprompt_only.png");


  // ----------------------------- B0s
  RooRealVar mVar("mVar","m(J/psiKK)",5.0,5.7);
  mVar.setBins(200);
  RooPlot* mVarFrame = mVar.frame();
  RooDataHist* B0sMass_aftercut_200 = new RooDataHist(histo_b0smass_aftercut_200bins->GetName(), histo_b0smass_aftercut_200bins->GetTitle(), RooArgSet(mVar), RooFit::Import(*histo_b0smass_aftercut_200bins, kFALSE));
  B0sMass_aftercut_200->plotOn(mVarFrame, MarkerSize(0.2));

  //// -- Sig
  // Gaus 
  RooRealVar mg("mg","Gaussian's mean",5.35,5.32,5.38);
  RooRealVar wg("wg","Gaussian's width",0.012,0.005,0.025);
  RooGaussian gauss1("gauss1","Gauss(mVar,mg,wg)",mVar,mg,wg); // NON USATA!
  // Johnson 
  RooRealVar mu("mu","Johnson's mean",5.35,5.32,5.38);
  RooRealVar lambda("lambda","Johnson's Lambda",0.012,0.005,0.03);
  RooRealVar gamma("gamma","Johnson's Gamma",-0.05,-0.3,0.15);
  RooRealVar delta("delta","Jhonson's delta",1.2,0.,5.);
  RooJohnson john("john","Johnson",mVar,mu,lambda,gamma,delta);

  // -- Bkg 
  // Cheby a 3
  RooRealVar c0("c0","1st coeff",-0.5,-100.,100.);
  RooRealVar c1("c1","2nd coeff",0.5,-100.,100.);
  RooRealVar c2("c2","3rd coeff",-0.5,-1000.,1000.);
  RooChebychev cheby("cheby","Chebyshev",mVar,RooArgList(c0,c1,c2));

  // Totale 
  RooRealVar nsig("nsig","sig yield",1500.,500.,6000.);
  RooRealVar nbkg("nbkg","bkg yield",5500.,1000.,10000.);
  // pdf totale 
  RooAddPdf* B0s_model_ext = new RooAddPdf("B0s_model_ext","gauss/john+cheby EXT",RooArgList(john,cheby),RooArgList(nsig,nbkg));   

  // Fitting 
  RooFitResult* fitB0sResult = B0s_model_ext->fitTo(*B0sMass_aftercut_200, Extended(kTRUE), Minos(kTRUE), Save(kTRUE));  

  // Plot 
  B0s_model_ext->plotOn(mVarFrame, LineColor(kRed));
  B0s_model_ext->plotOn(mVarFrame, Components(RooArgSet(cheby)), LineColor(kBlue), LineStyle(kDashed));
  B0s_model_ext->paramOn(mVarFrame, Parameters(RooArgSet(mu,lambda,nsig)), Layout(0.6,0.99,0.9));

  B0sMass_aftercut_200->plotOn(mVarFrame, MarkerSize(0.2));

  // Linee e integrali (Boh!)
  auto sigDW = 5.3416;    // 5.3656-2*0.012
  TLine *lineSigDW = new TLine(5.3416,0.,5.3416,300.);
  auto sigUP = 5.3896;    // 5.3656+2*0.012
  TLine *lineSigUp = new TLine(5.3896,0.,5.3896,300.);
  Double_t integral_below_peak = histo_b0smass_aftercut_200bins->Integral(sigDW,sigUP);
  cout << "################ Integral under the peak in 2sigma-mass-window = S+B = " << integral_below_peak << endl; 

  auto sdbDWdw = 5.2576;  // 5.3656-9*0.012
  TLine *lineSdbDWdw = new TLine(5.2576,0.,5.2576,300.);
  auto sdbDWup = 5.4256;  // 5.3656-5*0.012
  TLine *lineSdbDWup = new TLine(5.4256,0.,5.4256,300.);
  Double_t integral_sdbDW = histo_b0smass_aftercut_200bins->Integral(sdbDWdw,sdbDWup);
  cout << "################ Integral inside Down/Left SideBand = " << integral_sdbDW << endl;
  
  auto sdbUPdw = 5.4136;  // 5.3656+4*0.012
  TLine *lineSdbUPdw = new TLine(5.4136,0.,5.4136,300.);
  auto sdbUPup = 5.831;   // 5.3656+7*0.012 
  TLine *lineSdbUPup = new TLine(5.831,0.,5.831,300.);
  Double_t integral_sdbUP = histo_b0smass_aftercut_200bins->Integral(sdbUPdw,sdbUPup);
  cout << "integral inside Up/Right SideBand" << integral_sdbUP << endl;

  // integrate the background under the peak ( i need the PDF of the bkg component)
  mVar.setRange("peakRange",5.3416,5.3896);
  RooArgSet nset1(mVar);
  RooAbsReal* bkg_peak = cheby.createIntegral(nset1,nset1,"peakRange");
  RooFormulaVar yield_bkg_peak("yield_bkg_peak","(@0*@1)",RooArgList(*bkg_peak,nbkg));
  cout << "################ Integral of the BKG under the signal peak (normalized to 1) = " << bkg_peak->getVal() << endl;  
  cout << "################ Integral of the BKG in the full spectrum = " << nbkg.getVal() << endl;                                   cout << "################ Integral of the BKG under the signal peak = " << yield_bkg_peak.getVal() << endl;

  RooAbsReal* sig_peak = john.createIntegral(nset1,nset1,"peakRange");
  RooFormulaVar yield_sig_peak("yield_sig_peak","(@0*@1)",RooArgList(*sig_peak,nsig));
  cout << "################ Integral of the SIGNAL under the signal peak (normalized to 1) = " << sig_peak->getVal() << endl;
  cout << "################ Integral of the SIGNAL in the full spectrum = " << nsig.getVal() << endl;
  cout << "################ Integral of the SIGNAL under the signal peak = " << yield_sig_peak.getVal() << endl;
  RooFormulaVar total_peak("total_peak","(@0+@1)",RooArgList(yield_sig_peak,yield_bkg_peak));
  RooFormulaVar purity_peak("purity","(@0/@1)",RooArgList(yield_sig_peak,total_peak));

  cout << "################ TOTAL = SIGNAL+BKG = " << total_peak.getVal() << endl;
  cout << "################ PURITY = S/S+B = SIGNAL/SIGNAL+BKG = " << purity_peak.getVal() << endl;

  // Disegno la variabile 
  mVarFrame->Draw();
  lineSdbUPup->SetLineColor(2);
  lineSdbUPup->Draw("same");
  //
  myc1.cd(3);
  RooPlot* massPhiFrame1 = masskk.frame(Range(phimin,phimax));
  b0sDataNonPromptMass->plotOn(massPhiFrame1);
  massPhiFrame1->Draw("EP"); 
  //

  // ---------------- PHI 
  myc1.cd(4);    
  RooRealVar mPhiVar("mPhiVar","m(KK)",phimin,phimax);
  RooPlot* mPhiVarFrame = mPhiVar.frame();
  RooDataHist* PhiMass_aftercut = new RooDataHist(histo_phimass_aftercut->GetName(), histo_phimass_aftercut->GetTitle(), RooArgSet(mPhiVar), RooFit::Import(*histo_phimass_aftercut, kFALSE));
  //
  PhiMass_aftercut->plotOn(mPhiVarFrame, MarkerSize(0.2));
  //                   
  // -- Sig
  RooRealVar mean("mean","mean of voigtian", 1.0197, 1.019, 1.0205);
  RooRealVar gammaBW("gammaBW","width of BW", 0.0045, 0.001, 0.01);
  RooRealVar sigma("#sigma", "mass resolution", 0.0013); // to set at the MC value of 1.3MeV
  // 
  RooVoigtian sigPDF("sigPDF","sigPDF", mPhiVar, mean, gammaBW, sigma);
  //
  // -- Bkg
  RooRealVar cc0("cc_{0}", "cc0", 0.1 ,-5, 5);
  RooRealVar cc1("cc_{1}", "cc1", 0.05 ,-5, 5);
  RooRealVar cc2("cc_{2}", "cc2", -0.01 ,-5, 5);
  RooChebychev bkgPDF("bkgPDF", "bkgPDF", mPhiVar, RooArgSet(cc0,cc1,cc2));
  //
  RooRealVar nSigPhi("nSigPhi", "Number of signal candidates ", 5e+4, 100., 5e+6);
  RooRealVar nBkgPhi("nBkgPhi", "Bkg component", 2e+5, 100., 5e+7); 
  //
  RooAddPdf* Phi_model_ext = new RooAddPdf("Phi_model_ext", "totalPDF", RooArgList(sigPDF, bkgPDF), RooArgList(nSigPhi, nBkgPhi));     
  Phi_model_ext->fitTo(*PhiMass_aftercut, Extended(kTRUE), Minos(kTRUE));     
  Phi_model_ext->plotOn(mPhiVarFrame, LineColor(kRed));
  Phi_model_ext->plotOn(mPhiVarFrame, Components(RooArgSet(bkgPDF)), LineColor(kBlue), LineStyle(kDashed));
  Phi_model_ext->paramOn(mPhiVarFrame, Parameters(RooArgSet(mean,gammaBW,nSigPhi)), Layout(0.6,0.99,0.9));
  //
  PhiMass_aftercut->plotOn(mPhiVarFrame, MarkerSize(0.2));
  //
  mPhiVarFrame->Draw();
  //
  myc1.SaveAs("./Plots/B0sPhiMass_NONpromptOnly.png");
  //
  return 0;
}
