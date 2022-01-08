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
#include <chrono> 
#include <fstream>

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


//  gROOT->Reset();
//  gROOT->Clear();

//////////////////////////////////////////////////////////////////
////
//void RooConvolutionExpNew(int events=1000,int bins=200) {
//
void RooConvolutionExpNew(int events = 1000, TString calctime = "yes", int bins=200) {

  TString name  = std::to_string(events).c_str();

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

  // Gaussian + BW convolution //
  RooNumConvPdf convolution("convolution", "BW (X) gauss", xvar, signal , resol);

  // Background //
  RooRealVar alpha("#alpha","Exponential Parameter",-0.05,-2.0,0.0);
  RooExponential bkg("Bkg","Bkg",xvar,alpha);

  // TotalPdf = Gaussian + Bkg //
  RooRealVar sigfrac("sig1frac","fraction of component 1 in signal",0.5,0.,1.) ;
  RooAddPdf total("totalPDF", "totalPDF", RooArgList(convolution, bkg),sigfrac);

  std::cout <<"\nGenerating " << events << " events\n" << std::endl ;

  ////////////////////////////////////////////////////////////////////////
  // Generating data
  ////////////////////////////////////////////////////////////////////////

  // Uso come seed per la generazione casuale l'istante presente
  timeval trand;
  gettimeofday(&trand,NULL);

  // Uso secondi e microsecondi come seme (alla fine è uguale potevo usare un valore qualsiasi)
  long int msRand = trand.tv_sec * 1000 + trand.tv_usec / 1000;
  cout << "\n------" << endl;
  cout << "msRand = " << msRand ;
  cout << "\n------" << endl;
  // Setto il seme random 
  RooRandom::randomGenerator()->SetSeed(msRand);

  // Creo un dataset generando eventi random dalla pdf totale. 
  // generate prende la variabile su cui generare e il numero di eventi da generare
  RooDataSet* data = total.generate(xvar,events);

  // Nome del file su cui salvare gli eventi random 
  // Tutto questo è perchè ho bisogno di concatenare le stringhe e poi usare stringhe c-like
  std::string nomeFile{"./txt_files/" + name + "_events.txt"};
  const char * nomeFileC = nomeFile.c_str();
  // Tolgo il qualificatore const
  char * bufferstring = const_cast<char *>(nomeFileC); 
  // Scrivo il dataset nel file con nome contenuto in bufferstring 
  data->write(bufferstring);

  cout <<"\nFitting " << events << " events\n" << endl ;

  ////////////////////////////////////////////////////////////////////////
  // Fitting data
  ////////////////////////////////////////////////////////////////////////

  // Creo la negative log likelihood dei dati rispetto alla pdf total
  RooAbsReal* nll = total.createNLL(*data);

  //Declare null (pointer) and assign -log(Likelyhood) to it, Likelihood -> convolution and *data//
  RooMinuit min(*nll); 

  std::cout << "--------------------------------------------------------------" << std::endl ;
  // Tempo iniziale 
  if (calctime == "yes") {
    std::chrono::steady_clock::time_point tempoIniziale{std::chrono::steady_clock::now()};
    min.migrad();  // execute fit
    std::cout << "--------------------------------------------------------------" << std::endl ;
    std::chrono::steady_clock::time_point tempoFinale{std::chrono::steady_clock::now()};
    std::string deltaTime{
      std::to_string(
        std::chrono::duration_cast<std::chrono::seconds>(tempoFinale - tempoIniziale).count()
        )};
    std::cout << "Total CPU time: " << deltaTime << std::endl;

  } else min.migrad();

  std::cout << "--------------------------------------------------------------" << std::endl ;

  ////////////////////////////////////////////////////////////////////
  // Fit result and data representation
  ////////////////////////////////////////////////////////////////////////

  TCanvas *myC = new TCanvas("RooCanvas","Roofit Canvas", 1200, 800);

  RooPlot *frame = xvar.frame("") ;
  // Titolo
  std::string titolo{"RooFit: " + std::to_string(events) + " events"}; 
  TString titoloTstring = titolo.c_str(); 
  frame->SetTitle(titoloTstring) ;
  frame->SetYTitle("# of events") ;
  // Plot data
  data->plotOn(frame);
  // Plot pdf 
  total.plotOn(frame,LineColor(kGreen));
  total.plotOn(frame,Components(RooArgSet(convolution)),LineColor(kRed));
  total.plotOn(frame,Components(RooArgSet(bkg)),LineColor(kBlue),LineStyle(kDashed));
  // Parameters 
  total.paramOn(frame, Layout(0.75,0.99,0.99));
  frame->getAttText()->SetTextSize(0.028);


  frame->Draw() ;

  myC->SaveAs("plots/RooConvGen_"+name+".png");
  delete myC; 


}
