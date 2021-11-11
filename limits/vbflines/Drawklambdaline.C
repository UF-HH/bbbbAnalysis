#include <iostream>
#include <TH1F.h>
#include <TROOT.h>
#include <TFile.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TProfile.h>
#include <TStyle.h>
#include <TPaveText.h>
#include <THStack.h>
#include <TF1.h>
#include <TSystem.h>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TLine.h>

void Drawklambdaline()
{
   gROOT->SetStyle("Plain");gStyle->SetOptStat(0000);
   gStyle->SetPadGridX(0);gStyle->SetPadGridY(0);
   gStyle->SetPadTickX(1);gStyle->SetPadTickY(1);
   gStyle->SetLineWidth(3); 
   gStyle->SetOptTitle(0); 
   gStyle->SetErrorX(0);
   TCanvas c1("c1", "First canvas", 1000, 1000); 
   TPad *p_2=new TPad("p_2", "p_2", 0, 0, 1, 1);
   p_2->SetFillStyle(4000);p_2->SetFrameFillColor(0);
   p_2->SetTopMargin(0.10);p_2->SetBottomMargin(0.20);
   p_2->SetLeftMargin(0.16);p_2->SetRightMargin(0.05);  
   p_2->Draw();     
   p_2->cd();

   TGraph *plot = new TGraph("config/ggfhhc3line.txt"); 
   plot->GetYaxis()->SetTitle("#sigma_{ggF}(pp#rightarrowHH) [pb]");
   plot->GetXaxis()->SetTitle("#kappa_{#lambda}");
   //plot->GetYaxis()->SetTitle("#sigma_{VBF}(pp#rightarrowHH) X B(HH#rightarrow4b) [pb]");
   //plot->GetXaxis()->SetTitle("C_{3}");
   //plot->SetTitle("#kappa_{#lambda}=C_{3} couplings");
   plot->SetMaximum(4.0);
   //plot->SetMinimum(0);
   plot->GetXaxis()->SetLabelFont(42);
   plot->GetYaxis()->SetLabelFont(42);
   plot->GetYaxis()->SetTitleFont(42);
   plot->GetXaxis()->SetTitleFont(42);     
   plot->GetYaxis()->SetTitleOffset(1.4);
   plot->GetYaxis()->SetTitleSize(0.055);
   plot->GetYaxis()->SetLabelSize(0.040);
   plot->GetXaxis()->SetTitleSize(0.065);
   plot->GetXaxis()->SetLabelSize(0.040);
   plot->SetMarkerColor(kBlack);
   plot->SetMarkerStyle(20);
   plot->SetLineColor(kBlue);
   plot->Draw("ALP"); 
   c1.SaveAs("kl_line.png");
}
