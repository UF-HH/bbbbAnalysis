#include "Riostream.h"
#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"
#include "TGraphAsymmErrors.h"


TGraphAsymmErrors* TriggerEfficiencyFitter(TFile *inputFile, std::string sampleName, std::string histogramName, 
    std::string selectionName, std::string normalizationSelectionName, TF1 *fittingFunction, double *parameters,
    std::string histogramTitle, std::string xAxisTitle)
{
    std::string cutTmpName = Form("%s/%s/%s_%s_%s",sampleName.data(),selectionName.data(),sampleName.data(),selectionName.data(),histogramName.data());
    std::cout<<cutTmpName<<std::endl;
    TH1D *cutTmp    = (TH1D*)inputFile->Get(cutTmpName.data());
    TH1D *cutHisto  = (TH1D*)cutTmp->Clone(("cut "+selectionName).data());
    cutHisto->SetDirectory(0);

    std::string normalizationTmpName = Form("%s/%s/%s_%s_%s",sampleName.data(),normalizationSelectionName.data(),sampleName.data(),normalizationSelectionName.data(),histogramName.data());
    std::cout<<normalizationTmpName<<std::endl;
    TH1D *normalizationTmp    = (TH1D*)inputFile->Get(normalizationTmpName.data());
    TH1D *normalizationHisto  = (TH1D*)normalizationTmp->Clone(("Normalization "+selectionName).data());
    normalizationHisto->SetDirectory(0);
    
    TGraphAsymmErrors *g_efficiency = new TGraphAsymmErrors();
    g_efficiency->Divide(cutHisto,normalizationHisto,"cl=0.683 b(1,1) mode");
    g_efficiency->SetTitle(histogramTitle.data());
    g_efficiency->GetXaxis()->SetTitle(xAxisTitle.data());
    g_efficiency->GetYaxis()->SetTitle("efficiency");
    g_efficiency->SetNameTitle(("Efficiency_"+selectionName).data(), ("Efficiency "+selectionName).data());
    g_efficiency->GetYaxis()->SetRangeUser(0., 1.2);
    g_efficiency->SetLineColor(kBlue);

    return g_efficiency;
}

void MeasureTriggerEfficiency(std::string inputFileName, std::string sampleName, std::string outputFileName)
{
    TF1 *dummyFormula = new TF1();
    double dummyParameters[1] = {0.};

    TFile *inputFile = new TFile(inputFileName.data());
    TFile *outputFile = new TFile(outputFileName.data(),"RECREATE");
    TGraphAsymmErrors *efficiency;


    // HLT_DoubleJet90_Double30_TripleBTagCSV_p087
    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "SelectedJetPtSum", 
        "L1triggerDouble90Double30AndPrevious_Full", "Normalization_Full", dummyFormula, dummyParameters,
        "L1 Trigger", "p_{1}^{T} + p_{2}^{T} + p_{3}^{T} + p_{4}^{T}");
    // efficiency->SetDirectory(0);
    efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "ForthSelectedJetPt", 
        "QuadCentralJet30AndPrevious_Full", "L1triggerDouble90Double30AndPrevious_Full", dummyFormula, dummyParameters,
        "Quad Central Jet 30 GeV", "p_{4}^{T}");
    // efficiency->SetDirectory(0);
    efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "SecondSelectedJetPt", 
        "DoubleCentralJet90AndPrevious_Full", "QuadCentralJet30AndPrevious_Full", dummyFormula, dummyParameters,
        "Double Central Jet 90 GeV", "p_{2}^{T}");
    // efficiency->SetDirectory(0);
    efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "ThirdSelectedJetDeepCSV", 
        "BTagCaloCSVp087TripleDouble90Double30AndPrevious_Full", "DoubleCentralJet90AndPrevious_Full", dummyFormula, dummyParameters,
        "BTag CaloCSV p087 Triple", "DeepCSV_{3}");
    // efficiency->SetDirectory(0);
    efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "ForthSelectedJetPt", 
        "QuadPFCentralJetLooseID30AndPrevious_Full", "BTagCaloCSVp087TripleDouble90Double30AndPrevious_Full", dummyFormula, dummyParameters,
        "Quad PF Central Jet Loose ID 30 GeV", "p_{4}^{T}");
    // efficiency->SetDirectory(0);
    efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "SecondSelectedJetPt", 
        "DoublePFCentralJetLooseID90AndPrevious_Full", "QuadPFCentralJetLooseID30AndPrevious_Full", dummyFormula, dummyParameters,
        "Double PF Central Jet Loose ID 90 GeV", "p_{2}^{T}");
    // efficiency->SetDirectory(0);
    efficiency->Write();


    // HLT_QuadJet45_TripleBTagCSV_p087
    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "SelectedJetPtSum", 
        "L1triggerQuad45AndPrevious_Full", "Normalization_Full", dummyFormula, dummyParameters,
        "L1 Trigger", "p_{1}^{T} + p_{2}^{T} + p_{3}^{T} + p_{4}^{T}");
    // efficiency->SetDirectory(0);
    efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "ForthSelectedJetPt", 
        "QuadCentralJet45AndPrevious_Full", "L1triggerQuad45AndPrevious_Full", dummyFormula, dummyParameters,
        "Quad Central Jet 45 GeV", "p_{4}^{T}");
    // efficiency->SetDirectory(0);
    efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "ThirdSelectedJetDeepCSV", 
        "BTagCaloCSVp087TripleQuad45AndPrevious_Full", "QuadCentralJet45AndPrevious_Full", dummyFormula, dummyParameters,
        "BTag CaloCSV p087 Triple", "DeepCSV_{3}");
    // efficiency->SetDirectory(0);
    efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "ForthSelectedJetPt", 
        "QuadPFCentralJetLooseID45AndPrevious_Full", "BTagCaloCSVp087TripleQuad45AndPrevious_Full", dummyFormula, dummyParameters,
        "Quad PF Central Jet Loose ID 45 GeV", "p_{4}^{T}");
    // efficiency->SetDirectory(0);
    efficiency->Write();

    //outputFile->WriteObject(efficiency,"Efficiency");

    // inputFile->Close();
    // outputFile->Close();

    return;
}