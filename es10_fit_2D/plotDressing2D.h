#ifndef PLOT_DRESSING_H
#define PLOT_DRESSING_H

TCanvas *canvasDressing(TString name = "c1") {
    /** 
     * Definisce tutte le opzioni grafiche del canvas, crea un canvas con quelle opzioni e lo restituisce. 
     * @param: TString name: nome del canva 
     * @return: TCanvas *: il canvas con le opzioni grafiche 
     */
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(0);
    gStyle->SetPalette(1);
    gStyle->SetFrameFillColor(0);
    gStyle->SetCanvasColor(0);
    gStyle->SetPadBorderSize(0);
    gStyle->SetCanvasBorderSize(0);
    gStyle->SetFrameLineColor(1);
    gStyle->SetFrameLineWidth(2.0);
    gStyle->SetCanvasColor(0);
    gStyle->SetCanvasBorderMode(0);
    gStyle->SetPadColor(0);
    gStyle->SetPadBorderMode(0);
    gStyle->SetPadTickX(1);
    gStyle->SetPadTickY(1);
    gStyle->SetLegendBorderSize(0);
    gStyle->SetErrorX(0.5);
    gStyle->SetTextFont(42);
    gStyle->SetLabelFont(42,"x");
    gStyle->SetLabelFont(42,"y");
    gStyle->SetLabelFont(42,"z");
    gStyle->SetTitleFont(42,"x");
    gStyle->SetTitleFont(42,"y");
    gStyle->SetTitleFont(42,"z");
    gStyle->SetEndErrorSize(1);
    
    TCanvas *c1 = new TCanvas(name,name,71,86,768,568);
    c1->Range(4.819967,-13.30288,6.070376,78.37912);
    c1->SetFillColor(0);
    c1->SetBorderMode(0);
    c1->SetBorderSize(2);
    c1->SetLeftMargin(0.1439791);
    c1->SetRightMargin(0.05628272);
    c1->SetTopMargin(0.075);
    c1->SetBottomMargin(0.145098);
    c1->SetFrameBorderMode(0);
    c1->SetFrameBorderMode(0);
    
    return c1;
}

void LegendMassProj() {
    /**
     * Definisce e disegna la legenda per il plot della massa sul canvas attivo. 
     */
    TPaveText *pt = new TPaveText(0.3347987,0.94,0.6652013,0.995,"blNDC");
    pt->SetName("title");
    pt->SetBorderSize(0);
    pt->SetFillColor(0);
    pt->SetFillStyle(0);
    pt->SetTextFont(42);
    pt->Draw();
    TLatex *   tex = new TLatex(5.780687,775.0,"38 pb^{-1}(13 TeV)");
    tex->SetLineWidth(2);
    tex->Draw();
    tex = new TLatex(5.774141,672.7936,"CMS");
    tex->SetTextFont(42);
    tex->SetLineWidth(2);
    tex->Draw();
    tex = new TLatex(5.657938,632.1172,"Preliminary");
    tex->SetTextFont(52);
    tex->SetLineWidth(2);
    tex->Draw();
    tex = new TLatex(5.657938,577.6243,"B^{#pm} #rightarrow J/#psi K^{#pm}");
    tex->SetLineWidth(2);
    tex->Draw();
    tex = new TLatex(5.657938,524.9479,"p_{T} > 18 GeV");
    tex->SetLineWidth(2);
    tex->Draw();
    TLine *line = new TLine(5.582651,470.455,5.620295,470.455);
    line->SetLineColor(4);
    line->SetLineWidth(3);
    line->Draw();
    tex = new TLatex(5.657938,455.9236,"Total fit");
    tex->SetTextFont(42);
    tex->SetLineWidth(2);
    tex->Draw();
    line = new TLine(5.584288,410.5129,5.625205,410.5129);
    line->SetLineColor(7);
    line->SetLineStyle(2);
    line->SetLineWidth(3);
    line->Draw();
    tex = new TLatex(5.657938,397.7979,"Combinatorial bkg");
    tex->SetTextFont(42);
    tex->SetTextSize(0.03703704);
    tex->SetLineWidth(2);
    tex->Draw();
    tex = new TLatex(5.657938,346.9379,"J/#psi K+X");
    tex->SetTextFont(42);
    tex->SetLineWidth(2);
    tex->Draw();
    line = new TLine(5.587561,359.6529,5.621931,359.6529);
    line->SetLineColor(kBlack);
    line->SetLineStyle(2);
    line->SetLineWidth(3);
    line->Draw();
    TPave *pave = new TPave(5.590835,290.6286,5.626841,317.875,4,"br");
    pave->SetFillColor(2);
    pave->SetFillStyle(3008);
    pave->SetLineColor(2);
    pave->Draw();
    tex = new TLatex(5.657938,294.2614,"Signal");
    tex->SetTextFont(42);
    tex->SetLineWidth(2);
    tex->Draw();
}

void LegendLifetimeProj() {
    /**
     * Definisce e disegna la legenda per il plot della lifetime sul canvas attivo. 
     */
    TPaveText *pt = new TPaveText(0.3347987,0.94,0.6652013,0.995,"blNDC");
    pt->SetName("title");
    pt->SetBorderSize(0);
    pt->SetFillColor(0);
    pt->SetFillStyle(0);
    pt->SetTextFont(42);
    pt->Draw();
    TLatex *   tex = new TLatex(0.175,7023.452,"38 pb^{-1}(13 TeV)");
    tex->SetLineWidth(2);
    tex->Draw();
    tex = new TLatex(0.2230442,2307.542,"CMS");
    tex->SetTextFont(42);
    tex->SetLineWidth(2);
    tex->Draw();
    tex = new TLatex(0.1881833,1082.523,"Preliminary");
    tex->SetTextFont(52);
    tex->SetLineWidth(2);
    tex->Draw();
    tex = new TLatex(0.1886743,555.1321,"B^{#pm} #rightarrow J/#psi K^{#pm}");
    tex->SetLineWidth(2);
    tex->Draw();
    tex = new TLatex(0.1886743,278.4119,"p_{T} > 18 GeV");
    tex->SetLineWidth(2);
    tex->Draw();
    tex = new TLatex(0.1886743,100.2767,"Total fit");
    tex->SetTextFont(42);
    tex->SetLineWidth(2);
    tex->Draw();
    tex = new TLatex(0.1886743,50.22088,"Combinatorial bkg");
    tex->SetTextFont(42);
    tex->SetTextSize(0.03703704);
    tex->SetLineWidth(2);
    tex->Draw();

    TLine *line = new TLine(0.178,117.0,0.185,117.0);
    line->SetLineColor(4);
    line->SetLineWidth(3);
    line->Draw();
    line = new TLine(0.178,61.4,0.185,61.4);
    line->SetLineColor(7);
    line->SetLineStyle(2);
    line->SetLineWidth(3);
    line->Draw();
    tex = new TLatex(0.1886743,26.29928,"J/#psi K+X");
    tex->SetTextFont(42);
    tex->SetLineWidth(2);
    tex->Draw();
    line = new TLine(0.178,32.0,0.185,32.0);
    line->SetLineColor(kBlack);
    line->SetLineStyle(2);
    line->SetLineWidth(3);
    line->Draw();
    TPave *pave = new TPave(0.178,15.0,0.185,21.0,4,"br");
    pave->SetFillColor(2);
    pave->SetFillStyle(3008);
    pave->SetLineColor(2);
    pave->Draw();
    tex = new TLatex(0.1886743,14.0,"Signal");
    tex->SetTextFont(42);
    tex->SetLineWidth(2);
    tex->Draw();
}

#endif