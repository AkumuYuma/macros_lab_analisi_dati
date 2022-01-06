#include "RooPolynomial.h"
#include "RooRealVar.h"
#include "RooBreitWigner.h"
#include "RooNumConvPdf.h"
#include "RooGaussian.h"
#include "RooExponential.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooAbsData.h"
#include "RooMinuit.h"
#include "RooPlot.h"
#include "RooChebychev.h"
#include "RooAddPdf.h"
#include "RooArgList.h"
#include "TH1F.h"
#include <vector>
#include "TCanvas.h"

#include "RooRandom.h" // needed for Randomizer

#include <sys/time.h>
#include <sys/times.h>

using namespace RooFit; //Working in RooFit//

///////////////////////////////////////////////////////////////////////////////////
//root [0] .L RooConvolutionExpNew.C+
//root [1]  RooConvolutionExpNew("10000","yes",80)
//
// otherwise (if you do not want CPU time counting):
// root [1]  RooConvolutionExpNew("10000",80)
//
// try at least 10K, 100K, 1M (it takes from about 2 to 10 and 20 minutes!)
// make a plot TIME (in linear scale) vs #EVENTS (in log scale) taking more points
//////////////////////////////////////////////////////////////////////////////////

timeval startTime, stopTime, totalTime;
timeval startTimeRead, stopTimeRead, totalTimeRead;
clock_t startCPU, stopCPU;
clock_t startCPURead, stopCPURead;
tms startProc, stopProc;  //Struct time intervals in clock ticks//
tms startProcRead, stopProcRead;

//  gROOT->Reset();
//  gROOT->Clear();

//////////////////////////////////////////////////////////////////
////
//void RooConvolutionExpNew(int events=1000,int bins=200) {
//
void RooConvolutionExpNew(TString argv, TString calctime, int bins=200) {

  int events = atoi(argv.Data());  // converte string "numero" in numero intero
  TString name = argv;

  char bufferstring[256];

  RooRealVar xvar("xvar", "", -10, 10);
  xvar.setBins(bins);

  // Breit Wigner Signal //
  RooRealVar mean("m", "mean", 0.2, -1, 1);                    //Breit Wigner mean//
  RooRealVar gamma("#Gamma", "gamma", 2, 0.1, 5);              //Breit Wigner width//
  RooBreitWigner signal("BW", "BW signal", xvar, mean, gamma); //Breit Wigner pdf//

  // Gaussian Resolution Function //
  RooRealVar zero("zero","Gaussian resolution mean", 0.);               // offset from mean
  RooRealVar sigma("#sigma", "sigma", 1.5, 0.1, 5);                     //Gaussian sigma//
  RooGaussian resol("resol", "Gaussian resolution", xvar, zero, sigma); //Gaussian pdf//

  // Background //
  RooRealVar alpha("#alpha","Exponential Parameter",-0.05,-2.0,0.0);
  RooExponential bkg("Bkg","Bkg",xvar,alpha);

  // Gaussian + BW convolution //
  RooNumConvPdf convolution("convolution", "BW (X) gauss", xvar, signal , resol);

  // TotalPdf = Gaussian + Bkg //
  RooRealVar sigfrac("sig1frac","fraction of component 1 in signal",0.5,0.,1.) ;
  RooAddPdf total("totalPDF", "totalPDF", RooArgList(convolution, bkg),sigfrac);

  cout <<"\nGenerating " << name << " events\n" << endl ;

  ////////////////////////////////////////////////////////////////////////
  // Generating data
  ////////////////////////////////////////////////////////////////////////

  timeval trand;
  gettimeofday(&trand,NULL);

  long int msRand = trand.tv_sec * 1000 + trand.tv_usec / 1000;
  cout << "\n------" << endl;
  cout << "msRand = " << msRand ;
  cout << "\n------" << endl;
  RooRandom::randomGenerator()->SetSeed(msRand);
  //
  RooDataSet* data = total.generate(xvar,events);
  //
  sprintf(bufferstring,"./txt_files/%d_events.txt",events);
  data->write(bufferstring);
  //
  cout <<"\nFitting " << name << " events\n" << endl ;

  ////////////////////////////////////////////////////////////////////////
  // Fitting data
  ////////////////////////////////////////////////////////////////////////


  RooAbsReal* nll = total.createNLL(*data);

  //Declare null (pointer) and assign -log(Likelyhood) to it, Likelihood -> convolution and *data//
  RooMinuit min(*nll);

  if (calctime == "yes")
    {
     gettimeofday(&startTime, NULL);
     startCPU = times(&startProc);
    }
  //--Migrad Fit
  min.migrad();  // execute fit
  //
  if (calctime == "yes")
    {
     stopCPU = times(&stopProc);
     gettimeofday(&stopTime, NULL);
    }
  //
  //
  ////// Print total fitting time
  cout << "\n--------------------------------------------------------------" << endl ;
  //
  string total_time;
  if (calctime == "yes")
    {
     double myCPUc = (stopCPU - startCPU)*10000;
     total_time = to_string(myCPUc / CLOCKS_PER_SEC);
     cout << "Total CPU time: " << total_time << endl;
    }
//
  cout << "\n--------------------------------------------------------------" << endl ;
  cout << endl ;

  ////////////////////////////////////////////////////////////////////
  // Fit result and data representation
  ////////////////////////////////////////////////////////////////////////

  TCanvas *myC = new TCanvas("RooCanvas","Roofit Canvas", 1200, 800);

  RooPlot *frame = xvar.frame("") ;
  sprintf(bufferstring," RooFit : %d events",events);
  frame->SetTitle(bufferstring) ;
  frame->SetYTitle("# of events") ;
  data->plotOn(frame);
  total.plotOn(frame,LineColor(kGreen));
  total.plotOn(frame,Components(RooArgSet(convolution)),LineColor(kRed));
  total.plotOn(frame,Components(RooArgSet(bkg)),LineColor(kBlue),LineStyle(kDashed));
  total.paramOn(frame, Layout(0.75,0.99,0.99));
  frame->getAttText()->SetTextSize(0.028);

  frame->Draw() ;
  //myC->SaveAs("plots/RooConvGen_"+name+".eps");
  myC->SaveAs("plots/RooConvGen_"+name+".png");
  //
  if(myC) {
           myC->Close();
//	   gSystem->ProcessEvents();
           delete myC;
//	   myC = 0;
          }
  //

  // Scrivo il numero di eventi generato e il tempo passato su un file
  if (calctime == "yes") {
      ofstream outputFile{"./txt_files/emanuele_times.txt", ios::app};
      outputFile << name << "\t" << total_time << "\n";
      outputFile.close();
  }

}
