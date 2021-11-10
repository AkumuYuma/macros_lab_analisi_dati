#include <TH1.h>
#include <TF1.h>
#include <TF2.h>
#include <TH2.h>
#include <TFormula.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TProfile.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TString.h>
#include <TLine.h>
#include <TPad.h>
//
// to execute
// .x rocmacro.C("LeptIP3D_worst","LeptSTIP_SLIP_worst","4nov2021","png")
//
void rocmacro(TString var1, TString var2, TString date, TString extens){
  //
  //var1==AllLeptIP3D, Ele(Mu)IP_best(worst), Ele(Mu)SLIP_best(worst), Ele(Mu)STIP_best(worst), LeptIP3D_worst, LeptSTIP_SLIP_worst, etc.
  //var2=="",AllLeptIP3D, Ele(Mu)IP_best(worst), Ele(Mu)SLIP_best(worst), Ele(Mu)STIP_best(worst), LeptIP3D_worst, LeptSTIP_SLIP_worst, etc.

  gROOT->Reset();
  gROOT->Clear();
  //
  /////////////////////////////////// configurazione stile grafico
  gStyle->SetCanvasColor(0);
  gStyle->SetPadColor(0);
  //gStyle->SetHistFillColor(0);
  //gStyle->SetHistLineStyle(1);
  //gStyle->SetHistLineWidth(1);
  //gStyle->SetHistLineColor(1);
  gStyle->SetTitleXOffset(0.9);
  gStyle->SetTitleYOffset(1.15);
  //gStyle->SetOptStat(1110);
  gStyle->SetOptStat(kFALSE);
  gStyle->SetOptFit(0111);
  gStyle->SetStatH(0.1);
  gStyle->SetPadTopMargin(0.09);
  gStyle->SetPadBottomMargin(0.13);
  gStyle->SetPadLeftMargin(0.12);
  gStyle->SetPadRightMargin(0.10);
  gStyle->SetPadTickX(1);  // To get tick marks on the opposite side of the frame
  gStyle->SetPadTickY(1);
  gStyle->SetOptTitle(1);
  gStyle->SetStatFont(42);
  gStyle->SetTitleFont(42);
  gStyle->SetTitleSize(1);
  //
  gROOT->SetStyle("Plain"); // cambio di stile grafico
  gStyle->SetNdivisions(10);
  gStyle->SetCanvasBorderMode(0); 
  gStyle->SetPadBorderMode(0);
  //
  /////////////////////////////////////////////////////////////////////
  //
  TCanvas *MyC = new TCanvas("MyC","Plots",900,700);
  //
  // Vettore dei tre files 
  TFile *f[3];   //// i==0 signal; i==1 bkg TT; i==2 bkg Zbb
  f[0] = new TFile("./rootfiles/H150_ZZ_4l_10TeV_GEN_HLT_Presel_glb_2e2mu_2e2mu_merged.root","read"); // Signal H->ZZ->2e2m (mass=150GeV)
  f[1] = new TFile("./rootfiles/TT_4l_10TeV_Presel_4l_2e2mu_merged.root","read");                     // BKG ttbar
  f[2] = new TFile("./rootfiles/LLBB_4l_10TeV_Presel_4l_2e2mu_merged.root","read");                   // BKG Zbbar->2lbbar
  //
  TGraph *gr_h150_TT_vtx;
  TGraph *gr_h150_Zbb_vtx;
  //
  TGraph *gr_h150_TT_wrkgPt;
  //
  TGraph *gr_h150_Zbb_var2;
  TGraph *gr_h150_TT_var2;


  ///////////////////////////////////////////////////////////////////// VARIABLE-1 //
  TH1D *hVar1[3];
  //TH1D *hVar1Cut[3];
  TH1D *hVarRatio[3];
  //
  Double_t TotIntegral1[3];
  Double_t PartIntegral1[3];
  Double_t Eff1[3];
  Double_t IntegralCounts1[3][500];
  //
  //const char histoname[128];
  char histoname[128];
  //
  int nBins1=100;           // just a dummy inizialization
  //
  for (int i=0; i<3; i++)
     {
      if (var1=="AllLeptIP3D")     
	{ 
	  hVar1[i]=(TH1D*)f[i]->Get("h"+var1+"_large");
	}
      else                        
	{
	  hVar1[i]=(TH1D*)f[i]->Get("h"+var1);
	  //hVar1Cut[i]=(TH1D*)f[i]->Get("h"+var1+"_cut");
	}
      //
      //hVar1Cut[i]=(TH1D*)f[i]->Get("h"+var1+"_cut");

      // //////---------- Integrals //
      // Qua calcolo gli integrali totali (sotto tutta la curva) 
      TotIntegral1[i] = hVar1[i]->Integral(1,201); // or 1001
      nBins1 = hVar1[i]->GetNbinsX();
      cout << "# of bins of hVar1 = " << nBins1 << endl;
      sprintf(histoname,"hVarRatio[%d]",i);
      hVarRatio[i]=new TH1D(histoname,"Partial integral over total",200,0.,1.);
      // Ora vario il cut. Praticamente sposto j sul numero di bin e calcolo gli integrali fino a quel bin scelto. 
      // E per ogni valore di j ho una certa area e quindi un certo cut. 
      for(int j=1; j<nBins1+1; j++)
	{
	  PartIntegral1[i] = hVar1[i]->Integral(1,j); // Integro dal 1 bin a j 
	  Eff1[i] = PartIntegral1[i]/TotIntegral1[i]; // Calcolo il rapporto con l'area totale
	  IntegralCounts1[i][j-1]=Eff1[i]; 
	  // Nota che metto j-1 perchè questo loop parte da j=1 (perchè il primo integrale che faccio è da 1 a 1, ma quando devo metterlo nell'array, gli indici partono da 1
	  //
	  if (i==0 && j==200 ) cout << "i = " << i << " , j = " << j << " : eff1 = " << IntegralCounts1[i][j-1] << endl;
	  if (i==1 && j==200 ) cout << "i = " << i << " , j = " << j << " : eff1 = " << IntegralCounts1[i][j-1] << endl;
	  if (i==2 && j==200 ) cout << "i = " << i << " , j = " << j << " : eff1 = " << IntegralCounts1[i][j-1] << endl;
	  //
	  // riempio l'istogramma con i valori
	  hVarRatio[i]->Fill(Eff1[i]); // non estremamente significativo; per i=0/segnale picca ad 1
	}
    }
  //
  ///
  MyC->Divide(2,2);
  MyC->cd(1);
  hVar1[0]->Draw();
  MyC->cd(2);
  hVar1[1]->Draw();
  MyC->cd(3);
  hVar1[2]->Draw();
  MyC->cd(4);
  hVar1[0]->Draw();
  hVar1[1]->Draw("same");
  hVar1[2]->Draw("same");
  MyC->Update();
  MyC->SaveAs("./Plots/Histo"+var1+"_"+date+"."+extens);
  MyC->Clear();
  //
  ///////////////////////////////////////////////////////////////////// VARIABLE-2
  //
  //if (var2!="")
  //  {
      TH1D *hVar2[3];
      //TH1D *hVar2Cut[3];
      Double_t TotIntegral2[3];
      Double_t PartIntegral2[3];
      Double_t Eff2[3];
      Double_t IntegralCounts2[3][500];

      int nBins2=100;                       // just a dummy inizialization
      //
      // Loop sui tre file 
      for(int i=0; i<3; i++)
	{
	  // Inizializzo l'istogramma
	  if(var2=="AllLeptIP3D") 	 
	    {
	      hVar2[i]=(TH1D*)f[i]->Get("h"+var2+"_large");
	    }
	  else  
	    {
	      hVar2[i]=(TH1D*)f[i]->Get("h"+var2);
	      //hVar2Cut[i]=(TH1D*)f[i]->Get("h"+var2+"_cut");
	    }

	  // /////----------- Integrals ---------------------- //
	  TotIntegral2[i] = hVar2[i]->Integral(1,201); // or 1001
	  nBins2 = hVar2[i]->GetNbinsX();
	  for(int j=1; j<nBins2+1; j++)
	    {
	      PartIntegral2[i] = hVar2[i]->Integral(1,j); // Integro la variabile dal primo bin al j-simo 
	      Eff2[i] = PartIntegral2[i]/TotIntegral2[i]; // Faccio il rapporto tra questo integrale e l'area totale 
	      IntegralCounts2[i][j-1]=Eff2[i]; // IntegralCounts2[i][j - 1] è il valore del rapporto per un certo fondo(i) e un certo cut (j) 
	    }
	}
      
  // --------------------------------- sono arrivato qua

      //  }
  //
  ///////////// Draw ratios  PartIntegral[i]/TotIntegral[i]
  // non estremamente significativo; per i=0/segnale picca ad 1
  //
  gStyle->SetOptStat(0);
  MyC->SetGridx();
  MyC->SetGridy();
  MyC->Divide(0,3);
  MyC->cd(1); 
  hVarRatio[0]->SetLineColor(2);
  hVarRatio[0]->Draw();
  TLegend *leg1 = new TLegend(.1,.8,.2,.9);
  leg1->SetTextFont(42);
  leg1->SetFillColor(0);
  leg1->SetTextSize(0.05);
  leg1->AddEntry("","signal", "");
  leg1->Draw();
  MyC->cd(2); 
  hVarRatio[1]->SetLineColor(3);
  hVarRatio[1]->Draw();
  TLegend *leg2 = new TLegend(.1,.8,.2,.9);
  leg2->SetTextFont(42);
  leg2->SetFillColor(0);
  leg2->SetTextSize(0.05);
  leg2->AddEntry("","t#bar{t}", "");
  leg2->Draw();
  MyC->cd(3); 
  hVarRatio[2]->SetLineColor(4);
  hVarRatio[2]->Draw();
  TLegend *leg3 = new TLegend(.1,.8,.2,.9);
  leg3->SetTextFont(42);
  leg3->SetFillColor(0);
  leg3->SetTextSize(0.05);
  leg3->AddEntry("","Zb#bar{b}", "");
  leg3->Draw();
  //
  MyC->Update();
  MyC->SaveAs("./Plots/Integral-over-Total_"+var1+"_"+date+"."+extens);
  MyC->Clear();
  //
  ///////////////////////////////////////////////////////////////////////////////////////////
  //
  ///////////// Draw figure of merit for each bkg - 1 VARIABLE
  MyC->Divide(0,2);
  MyC->cd(1); 
  MyC->SetGridx();
  MyC->SetGridy();
  //
  TH2F *hframe1 = new TH2F("hframe1","Signal(H150) survival efficiency VS Bkg(t#bar{t}) survival efficiency after FULL preselection",100,0.01,1.,100,0.1,1.05);
  //
  hframe1->Draw();
  //
  hframe1->SetYTitle("H150 survival efficiency");
  hframe1->SetXTitle("t#bar{t} survival efficiency");
  // segnale vs primo fondo 
  gr_h150_TT_vtx = new TGraph(nBins1,IntegralCounts1[1],IntegralCounts1[0]);
  gr_h150_TT_vtx->SetMarkerColor(3);
  gr_h150_TT_vtx->SetMarkerSize(0.6);
  gr_h150_TT_vtx->SetMarkerStyle(20);
  gr_h150_TT_vtx->Draw("P");
  //
  TLegend *txt1 = new TLegend(.65,.1,.9,.3);
  txt1->SetTextFont(42);
  txt1->SetFillColor(0);
  txt1->SetTextSize(0.05);
  txt1->AddEntry("",var1, "");
  txt1->Draw();
  ////////////
  MyC->cd(2); 
  TH2F *hframe2 = new TH2F("hframe2","Signal(H150) survival efficiency VS Bkg(Zb#bar{b}) survival efficiency after FULL preselection",100,0.01,1.,100,0.1,1.05);
  //
  hframe2->Draw();
  //
  hframe2->SetYTitle("H150 survival efficiency");
  hframe2->SetXTitle("Zb#bar{b} survival efficiency");
  // Segnale vs secondo fondo 
  gr_h150_Zbb_vtx = new TGraph(nBins1,IntegralCounts1[2],IntegralCounts1[0]);
  gr_h150_Zbb_vtx->SetMarkerColor(2);
  gr_h150_Zbb_vtx->SetMarkerSize(0.6);
  gr_h150_Zbb_vtx->SetMarkerStyle(20);
  gr_h150_Zbb_vtx->Draw("P");
  //
  TLegend *txt2 = new TLegend(.65,.1,.9,.3);
  txt2->SetTextFont(42);
  txt2->SetFillColor(0);
  txt2->SetTextSize(0.05);
  txt2->AddEntry("", var1, "");
  txt2->Draw();
  //
  MyC->Update();
  MyC->SaveAs("./Plots/scatt-effic_H150-vsBkgs_"+var1+"_"+date+"."+extens);
  MyC->Clear();

  //////////// Overlap figures of merit for both bkgs - 1 VARIABLE
  MyC->SetGridx();
  MyC->SetGridy();
  MyC->Divide(0,2);
  MyC->cd(1); 
  TH2F *hframe3 = new TH2F("hframe3","Signal(H150) survival efficiency VS Bkgs survival efficiency after FULL preselection",100,0.,1.,100,0.4,1.05);
  //
  hframe3->Draw();
  //
  hframe3->SetYTitle("H150 survival efficiency");
  hframe3->SetXTitle("bkgs survival efficiency");
  gr_h150_TT_vtx->Draw("P");
  gr_h150_Zbb_vtx->Draw("Psame");
  //
  TLegend *leg4 = new TLegend(.1,.75,.2,.9);
  leg4->SetTextFont(42);
  leg4->SetFillColor(0);
  leg4->SetTextSize(0.05);
  leg4->AddEntry(gr_h150_TT_vtx,"t#bar{t}", "P");
  leg4->AddEntry(gr_h150_Zbb_vtx,"Zb#bar{b}", "P");
  leg4->Draw();
  ////// Zoom
  MyC->cd(2); 
  TH2F *hframe4 = new TH2F("hframe4","Signal(H150) survival efficiency VS Bkgs survival efficiency after FULL preselection",100,0.,1.,100,0.85,1.05);
  //
  hframe4->Draw();
  //
  hframe4->SetYTitle("H150 survival efficiency");
  hframe4->SetXTitle("bkgs survival efficiency");
  //
  gr_h150_TT_vtx->Draw("P");
  gr_h150_Zbb_vtx->Draw("Psame");
  //
  TLegend *leg5 = new TLegend(.1,.75,.2,.9);
  leg5->SetTextFont(42);
  leg5->SetFillColor(0);
  leg5->SetTextSize(0.05);
  leg5->AddEntry(gr_h150_TT_vtx,"t#bar{t}", "P");
  leg5->AddEntry(gr_h150_Zbb_vtx,"Zb#bar{b}", "P");
  leg5->Draw();
  //
  MyC->Update();
  MyC->SaveAs("./Plots/scatt-effic_H150-vsBkgs-overlap_"+var1+"_"+date+"."+extens);
  MyC->Clear();
  //
  ///////////////////////////////////////////////////////////////////////////////////////// COMPARISON WITH 2nd VARIABLE
  //
  //if(var2!="")
  //  {
      //////////// Overlap 2 figures of merit for both bkgs
      TH2F *hframe6 = new TH2F("hframe6","Signal(H150) survival efficiency VS Bkgs survival efficiency after FULL preselection",100,0.,1.,100,0.4,1.05);
      //
      hframe6->Draw();
      //
      hframe6->SetYTitle("H150 survival efficiency");
      hframe6->SetXTitle("bkgs survival efficiency");
      //
      gr_h150_TT_var2 = new TGraph(nBins2,IntegralCounts2[1],IntegralCounts2[0]);
      gr_h150_TT_var2->SetMarkerColor(6);
      gr_h150_TT_var2->SetMarkerSize(0.6);
      gr_h150_TT_var2->SetMarkerStyle(21);
      //
      gr_h150_Zbb_var2 = new TGraph(nBins2,IntegralCounts2[2],IntegralCounts2[0]);
      gr_h150_Zbb_var2->SetMarkerColor(4);
      gr_h150_Zbb_var2->SetMarkerSize(0.6);
      gr_h150_Zbb_var2->SetMarkerStyle(21);
      //
      gr_h150_TT_vtx->Draw("P");
      gr_h150_Zbb_vtx->Draw("Psame");
      gr_h150_TT_var2->Draw("P");
      gr_h150_Zbb_var2->Draw("Psame");
      //
      TLegend *leg6 = new TLegend(.35,.1,.9,.37);
      leg6->SetTextFont(42);
      leg6->SetFillColor(0);
      leg6->SetTextSize(0.05);
      leg6->AddEntry(gr_h150_TT_vtx,"t#bar{t} "+var1, "P");
      leg6->AddEntry(gr_h150_Zbb_vtx,"Zb#bar{b} "+var1, "P");
      leg6->AddEntry(gr_h150_TT_var2,"t#bar{t} "+var2, "P");
      leg6->AddEntry(gr_h150_Zbb_var2,"Zb#bar{b} "+var2, "P");
      leg6->Draw();
      //
      MyC->Update();
      MyC->SaveAs("./Plots/scatt-effic_H150-vsBkgs-overlap-"+var1+"-"+var2+"_"+date+"."+extens);
      MyC->Clear();
      //   }


 delete MyC;

  for(int i=0; i<3; i++)
    {
      f[i]->Close();
      f[i]->Delete();
    }
 //
 //
 gROOT->Reset();
 gROOT->Clear();

}
