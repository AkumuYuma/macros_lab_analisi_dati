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

using namespace RooFit;

int sidesub()
{
  //--Reading the file
  char rootfile[] = "b0sData.root"; //input file
  TFile inputfile(rootfile,"READ");
  //
  //////////////////
  //
  //--Loading the dataset
  RooDataSet* b0sData = (RooDataSet*) inputfile.Get("alldata");
  //
  //-Printing the tuple content
  b0sData->Print();
  //
  //-set scales
  //
  //...for B signal
  auto massmin = 5.0; //auto deduct the type directly from the content
  auto massmax = 5.7;
  auto massbins = (6.0 - 5.0)/0.005; // 5 MeV bins
  //
  //... for phi signal
  auto phimass = 1.019, dphi = 0.025;
  auto phimin = 0.995;
  auto phimax = 1.045;
  auto nentries = b0sData->sumEntries();
  //
  // what "auto" does? at compilation time deduces the type of the object based on what you assign:
  // example: auto f = 10.0 is a double,  auto f = 10.0f is a float, auto f = 10 is an int.
  //
  std::cout << ">> Number of entries: " << nentries << std::endl;
  //
  //== if I execute only till here I get:
  //
  //  RooDataSet::alldata[ditrak_m,dimuonditrk_m_rf_c,dimuonditrk_ctauPV,dimuonditrk_ctauErrPV,dimuon_pt,ditrak_pt,dimuon_m] = 25158 entries
  //  >> Number of entries: 25158
  //
  //  note that there are 7 variables
  //  - ditrack_m               inv. mass (Phi candidates)
  //  - dimuonditrack_m         inv. mass (B0s candidates)
  //  - dimuon_m                inv. mass(J/psi candidates)
  //  - ditrack_pt              transv. mom. of Phi cands
  //  - dimuon_pt               transv. mom. of J/psi cands
  //  - dimuonditrack_ctauPV    ctau of B0s cands w.r.t. PV
  //  - dimuonditrack_ctauErrPV uncertainty on the ctau of B0s cands (w.r.t. PV)
  //
  //======================================
  //
  //- Get variables
  //
  RooRealVar masskk ("ditrak_m","M(KK) [GeV]",phimin,phimax);
  RooRealVar dimuonditrk_m_rf_c ("dimuonditrk_m_rf_c","M(#mu#muKK)[GeV]",4.0,6.0);
  RooRealVar dimuonditrk_ctauPV ("dimuonditrk_ctauPV","dimuonditrk_ctauPV",-1000.0,1000.0);
  RooRealVar dimuonditrk_ctauErrPV ("dimuonditrk_ctauErrPV","dimuonditrk_ctauErrPV",-1000.0,1000.0);
  RooRealVar dimuon_pt ("dimuon_pt","dimuon_pt",0.0,1000.0);
  RooRealVar ditrak_pt ("ditrak_pt","ditrak_pt",0.0,1000.0);
  RooRealVar dimuon_m("dimuon_m","dimuon_m",2.5,3.5);
  //
  massbins = 100;
  masskk.setBins(int(massbins));
  //
  TCanvas myc("canvas","canvas",1200,800);
  dimuonditrk_m_rf_c.setRange("fitRange",massmin,massmax);
  dimuonditrk_m_rf_c.setBins(200);
  RooPlot* massFrame = dimuonditrk_m_rf_c.frame(Range(massmin,massmax));
  b0sData->plotOn(massFrame);
  massFrame->Draw();
  myc.SaveAs("b0sMass_all.png");
  //
  /////////////////////////////////////////////////////////////////////////////
  //
  //- Create a filtered dataset for only DISPLACED ("non-prompt") B0s candidates
  //
  RooDataSet* b0sDataNonPrompt = (RooDataSet*) b0sData->reduce(RooFormulaVar("displacement","dimuonditrk_ctauPV/dimuonditrk_ctauErrPV>3.0",RooArgList(dimuonditrk_ctauErrPV,dimuonditrk_ctauPV)));


  RooDataSet* fondo = (RooDataSet*) b0sData->reduce(RooFormulaVar("displacement","dimuonditrk_ctauPV/dimuonditrk_ctauErrPV<3.0",RooArgList(dimuonditrk_ctauErrPV,dimuonditrk_ctauPV)));


  // - note : cut on ctau significance (ctau/error) is applied
  //

  auto nNewEntries = b0sDataNonPrompt->sumEntries();
  std::cout << ">> Number of non-prompt entries: " << nNewEntries << std::endl;
  //
  //- Write this reduced dataset to an external rootuple file
  //
  TFile datasetfile("b0sData_nonprompt.root","RECREATE"); //and writing it in a ROOT file for future uses
  datasetfile.cd();
  b0sDataNonPrompt->Write();
  datasetfile.Close(); //remember always to close it
  //
  //- Create a reduced variable collection for displaced candidates (reduced/skimmed rootuple)
  //
  RooDataSet* b0sDataNonPromptMass = (RooDataSet*) b0sDataNonPrompt->reduce(SelectVars(RooArgSet(dimuonditrk_m_rf_c,masskk)));
  //skimming it to the two variables we will need for background studies (sPlot etc...)

  TCanvas myc1("canvas","canvas",1200,800);
  dimuonditrk_m_rf_c.setRange("fitRange",massmin,massmax);
  dimuonditrk_m_rf_c.setBins(200);
  RooPlot* massFrame1 = dimuonditrk_m_rf_c.frame(Range(massmin,massmax));
  b0sDataNonPromptMass->plotOn(massFrame1);
  fondo->plotOn(massFrame1);
  massFrame1->Draw();
  myc1.SaveAs("b0sMass_prompt_only.png");
  //
  //
  //
  return 0;
}
