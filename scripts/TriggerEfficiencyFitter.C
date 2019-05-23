#include "Riostream.h"
#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"
#include "TGraphAsymmErrors.h"


TGraphAsymmErrors* TriggerEfficiencyFitter(TFile *inputFile, std::string sampleName, std::string histogramName, 
    std::string selectionName, std::string normalizationSelectionName, TF1 *fittingFunction, double *parameters,
    std::string histogramTitle, std::string xAxisTitle, Color_t lineColor)
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
    g_efficiency->SetLineColor(lineColor);

    return g_efficiency;
}

void MeasureTriggerEfficiency(std::string inputFileName, std::string sampleName, std::string referenceTriggerName, std::string outputFileName, Color_t lineColor)
{
    TF1 *dummyFormula = new TF1();
    double dummyParameters[1] = {0.};

    TFile *inputFile = new TFile(inputFileName.data());
    TFile *outputFile = new TFile(outputFileName.data(),"RECREATE");
    TGraphAsymmErrors *efficiency;


    // HLT_DoubleJet90_Double30_TripleBTagCSV_p087
    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "SelectedJetPtSum", 
        "L1triggerDouble90Double30AndPrevious_"+referenceTriggerName, "Normalization_"+referenceTriggerName, dummyFormula, dummyParameters,
        "L1 Trigger", "p_{1}^{T} + p_{2}^{T} + p_{3}^{T} + p_{4}^{T}", lineColor);
    // efficiency->SetDirectory(0);
    efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "ForthSelectedJetPt", 
        "QuadCentralJet30AndPrevious_"+referenceTriggerName, "L1triggerDouble90Double30AndPrevious_"+referenceTriggerName, dummyFormula, dummyParameters,
        "Quad Central Jet 30 GeV", "p_{4}^{T}", lineColor);
    // efficiency->SetDirectory(0);
    efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "SecondSelectedJetPt", 
        "DoubleCentralJet90AndPrevious_"+referenceTriggerName, "QuadCentralJet30AndPrevious_"+referenceTriggerName, dummyFormula, dummyParameters,
        "Double Central Jet 90 GeV", "p_{2}^{T}", lineColor);
    // efficiency->SetDirectory(0);
    efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "ThirdSelectedJetDeepCSV", 
        "BTagCaloCSVp087TripleDouble90Double30AndPrevious_"+referenceTriggerName, "DoubleCentralJet90AndPrevious_"+referenceTriggerName, dummyFormula, dummyParameters,
        "BTag CaloCSV p087 Triple", "DeepCSV_{3}", lineColor);
    // efficiency->SetDirectory(0);
    efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "ForthSelectedJetPt", 
        "QuadPFCentralJetLooseID30AndPrevious_"+referenceTriggerName, "BTagCaloCSVp087TripleDouble90Double30AndPrevious_"+referenceTriggerName, dummyFormula, dummyParameters,
        "Quad PF Central Jet Loose ID 30 GeV", "p_{4}^{T}", lineColor);
    // efficiency->SetDirectory(0);
    efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "SecondSelectedJetPt", 
        "DoublePFCentralJetLooseID90AndPrevious_"+referenceTriggerName, "QuadPFCentralJetLooseID30AndPrevious_"+referenceTriggerName, dummyFormula, dummyParameters,
        "Double PF Central Jet Loose ID 90 GeV", "p_{2}^{T}", lineColor);
    // efficiency->SetDirectory(0);
    efficiency->Write();

/*
    // not per-object efficiency HLT_DoubleJet90_Double30_TripleBTagCSV_p087
    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "SelectedJetPtSum", 
        "HLT_Double90Double30AndPrevious_"+referenceTriggerName, "Normalization_"+referenceTriggerName, dummyFormula, dummyParameters,
        "L1 Trigger", "p_{1}^{T} + p_{2}^{T} + p_{3}^{T} + p_{4}^{T}", lineColor);
    // efficiency->SetDirectory(0);
    efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "ForthSelectedJetPt", 
        "HLT_Double90Double30AndPrevious_"+referenceTriggerName, "Normalization_"+referenceTriggerName, dummyFormula, dummyParameters,
        "Forth Jet efficiency", "p_{4}^{T}", lineColor);
    // efficiency->SetDirectory(0);
    efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "SecondSelectedJetPt", 
        "HLT_Double90Double30AndPrevious_"+referenceTriggerName, "Normalization_"+referenceTriggerName, dummyFormula, dummyParameters,
        "Second Jet efficiency", "p_{2}^{T}", lineColor);
    // efficiency->SetDirectory(0);
    efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "ThirdSelectedJetDeepCSV", 
        "HLT_Double90Double30AndPrevious_"+referenceTriggerName, "Normalization_"+referenceTriggerName, dummyFormula, dummyParameters,
        "Third BTag jet efficiency", "DeepCSV_{3}", lineColor);
    // efficiency->SetDirectory(0);
    efficiency->Write();
*/

    // _QuadJet45_TripleBTagCSV_p087
    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "SelectedJetPtSum", 
        "L1triggerQuad45AndPrevious_"+referenceTriggerName, "Normalization_"+referenceTriggerName, dummyFormula, dummyParameters,
        "L1 Trigger", "p_{1}^{T} + p_{2}^{T} + p_{3}^{T} + p_{4}^{T}", lineColor);
    // efficiency->SetDirectory(0);
    efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "ForthSelectedJetPt", 
        "QuadCentralJet45AndPrevious_"+referenceTriggerName, "L1triggerQuad45AndPrevious_"+referenceTriggerName, dummyFormula, dummyParameters,
        "Quad Central Jet 45 GeV", "p_{4}^{T}", lineColor);
    // efficiency->SetDirectory(0);
    efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "ThirdSelectedJetDeepCSV", 
        "BTagCaloCSVp087TripleQuad45AndPrevious_"+referenceTriggerName, "QuadCentralJet45AndPrevious_"+referenceTriggerName, dummyFormula, dummyParameters,
        "BTag CaloCSV p087 Triple", "DeepCSV_{3}", lineColor);
    // efficiency->SetDirectory(0);
    efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "ForthSelectedJetPt", 
        "QuadPFCentralJetLooseID45AndPrevious_"+referenceTriggerName, "BTagCaloCSVp087TripleQuad45AndPrevious_"+referenceTriggerName, dummyFormula, dummyParameters,
        "Quad PF Central Jet Loose ID 45 GeV", "p_{4}^{T}", lineColor);
    // efficiency->SetDirectory(0);
    efficiency->Write();

/*
    // not per-object efficiency _QuadJet45_TripleBTagCSV_p087
    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "SelectedJetPtSum", 
        "HLT_Quad45AndPrevious_"+referenceTriggerName, "Normalization_"+referenceTriggerName, dummyFormula, dummyParameters,
        "L1 Trigger", "p_{1}^{T} + p_{2}^{T} + p_{3}^{T} + p_{4}^{T}", lineColor);
    // efficiency->SetDirectory(0);
    efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "ForthSelectedJetPt", 
        "HLT_Quad45AndPrevious_"+referenceTriggerName, "Normalization_"+referenceTriggerName, dummyFormula, dummyParameters,
        "Forth Jet efficiency", "p_{4}^{T}", lineColor);
    // efficiency->SetDirectory(0);
    efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "ThirdSelectedJetDeepCSV", 
        "HLT_Quad45AndPrevious_"+referenceTriggerName, "Normalization_"+referenceTriggerName, dummyFormula, dummyParameters,
        "Third BTag jet efficiency", "DeepCSV_{3}", lineColor);
    // efficiency->SetDirectory(0);
    efficiency->Write();
*/

    //outputFile->WriteObject(efficiency,"Efficiency");

    // inputFile->Close();
    // outputFile->Close();

    return;
}

void PlotAll()
{
    // MeasureTriggerEfficiency("2016DataPlots_SingleMuon_Trigger_MuPt40/outPlotter.root","tmp_SingleMuon","HLT_IsoMu24","TriggerEfficiency_SingleMuon_HLT_IsoMu24_MuPt40.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_SingleMuon_Trigger_MuPt40/outPlotter.root","tmp_SingleMuon","HLT_IsoMu24_Matched","TriggerEfficiency_SingleMuon_HLT_IsoMu24_Matched_MuPt40.root",kRed);

    // MeasureTriggerEfficiency("2016DataPlots_SingleMuon_Trigger_MuPt40/outPlotter.root","tmp_SingleMuon","HLT_IsoMu20","TriggerEfficiency_SingleMuon_HLT_IsoMu20_MuPt40.root",kGreen);
    // MeasureTriggerEfficiency("2016DataPlots_SingleMuon_Trigger_MuPt40/outPlotter.root","tmp_SingleMuon","HLT_IsoMu20_Matched","TriggerEfficiency_SingleMuon_HLT_IsoMu20_Matched_MuPt40.root",kOrange);

    // MeasureTriggerEfficiency("2016DataPlots_SingleMuon_Trigger_MuPt20/outPlotter.root","tmp_SingleMuon","HLT_IsoMu24","TriggerEfficiency_SingleMuon_HLT_IsoMu24_MuPt20.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_SingleMuon_Trigger_MuPt20/outPlotter.root","tmp_SingleMuon","HLT_IsoMu24_Matched","TriggerEfficiency_SingleMuon_HLT_IsoMu24_Matched_MuPt20.root",kRed);

    // MeasureTriggerEfficiency("2016DataPlots_SingleMuon_Trigger_MuPt20/outPlotter.root","tmp_SingleMuon","HLT_IsoMu20","TriggerEfficiency_SingleMuon_HLT_IsoMu20_MuPt20.root",kGreen);
    // MeasureTriggerEfficiency("2016DataPlots_SingleMuon_Trigger_MuPt20/outPlotter.root","tmp_SingleMuon","HLT_IsoMu20_Matched","TriggerEfficiency_SingleMuon_HLT_IsoMu20_Matched_MuPt20.root",kOrange);

    // MeasureTriggerEfficiency("2016DataPlots_SingleMuon_Trigger_MuPt40/outPlotter.root","tmp_SingleMuon","HLT_IsoMu20","TriggerEfficiency_SingleMuon_HLT_IsoMu20_MuPt40_best.root",kRed);
    // MeasureTriggerEfficiency("2016DataPlots_SingleMuon_Trigger_MuPt20/outPlotter.root","tmp_SingleMuon","HLT_IsoMu20","TriggerEfficiency_SingleMuon_HLT_IsoMu20_MuPt20_best.root",kBlue);

    // MeasureTriggerEfficiency("2016DataPlots_SingleMuon_Trigger_MuPt40/outPlotter.root","tmp_SingleMuon","HLT_IsoMu24","TriggerEfficiency_SingleMuon_HLT_IsoMu24_MuPt40.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_SingleMuon_Trigger_MuPt40/outPlotter.root","tmp_SingleMuon","HLT_IsoMu24_Matched","TriggerEfficiency_SingleMuon_HLT_IsoMu24_Matched_MuPt40.root",kRed);

    // MeasureTriggerEfficiency("2016DataPlots_SingleMuon_Trigger_MuPt40/outPlotter.root","tmp_SingleMuon","HLT_IsoMu20","TriggerEfficiency_SingleMuon_HLT_IsoMu20_MuPt40.root",kGreen);
    // MeasureTriggerEfficiency("2016DataPlots_SingleMuon_Trigger_MuPt40/outPlotter.root","tmp_SingleMuon","HLT_IsoMu20_Matched","TriggerEfficiency_SingleMuon_HLT_IsoMu20_Matched_MuPt40.root",kOrange);

    // MeasureTriggerEfficiency("2016DataPlots_SingleMuon_Trigger_MuPt20/outPlotter.root","tmp_SingleMuon","HLT_IsoMu24","TriggerEfficiency_SingleMuon_HLT_IsoMu24_MuPt20.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_SingleMuon_Trigger_MuPt20/outPlotter.root","tmp_SingleMuon","HLT_IsoMu24_Matched","TriggerEfficiency_SingleMuon_HLT_IsoMu24_Matched_MuPt20.root",kRed);

    // MeasureTriggerEfficiency("2016DataPlots_SingleMuon_Trigger_MuPt20/outPlotter.root","tmp_SingleMuon","HLT_IsoMu20","TriggerEfficiency_SingleMuon_HLT_IsoMu20_MuPt20.root",kGreen);
    // MeasureTriggerEfficiency("2016DataPlots_SingleMuon_Trigger_MuPt20/outPlotter.root","tmp_SingleMuon","HLT_IsoMu20_Matched","TriggerEfficiency_SingleMuon_HLT_IsoMu20_Matched_MuPt20.root",kOrange);


    // MeasureTriggerEfficiency("2016DataPlots_SingleMuon_Matching_Trigger_MuPt20/outPlotter.root","tmp_SingleMuon","HLT_IsoMu24","TriggerEfficiency_SingleMuon_HLT_IsoMu24_MuPt20.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_SingleMuon_Matching_Trigger_MuPt20/outPlotter.root","tmp_SingleMuon","HLT_IsoMu24_Matched","TriggerEfficiency_SingleMuon_HLT_IsoMu24_Matched_MuPt20.root",kRed);

    // MeasureTriggerEfficiency("2016DataPlots_SingleMuon_Matching_Trigger_MuPt20/outPlotter.root","tmp_SingleMuon","HLT_IsoMu20","TriggerEfficiency_SingleMuon_HLT_IsoMu20_MuPt20.root",kGreen);
    // MeasureTriggerEfficiency("2016DataPlots_SingleMuon_Matching_Trigger_MuPt20/outPlotter.root","tmp_SingleMuon","HLT_IsoMu20_Matched","TriggerEfficiency_SingleMuon_HLT_IsoMu20_Matched_MuPt20.root",kOrange);


    MeasureTriggerEfficiency("2016DataPlots_SingleMuon_JetSelected_Trigger_MuPt20/outPlotter.root","tmp_SingleMuon","HLT_IsoMu24","TriggerEfficiency_SingleMuon_HLT_IsoMu24_MuPt20.root",kBlue);
    MeasureTriggerEfficiency("2016DataPlots_SingleMuon_JetSelected_Trigger_MuPt20/outPlotter.root","tmp_SingleMuon","HLT_IsoMu24_Matched","TriggerEfficiency_SingleMuon_HLT_IsoMu24_Matched_MuPt20.root",kRed);

    MeasureTriggerEfficiency("2016DataPlots_SingleMuon_JetSelected_Trigger_MuPt20/outPlotter.root","tmp_SingleMuon","HLT_IsoMu20","TriggerEfficiency_SingleMuon_HLT_IsoMu20_MuPt20.root",kGreen);
    MeasureTriggerEfficiency("2016DataPlots_SingleMuon_JetSelected_Trigger_MuPt20/outPlotter.root","tmp_SingleMuon","HLT_IsoMu20_Matched","TriggerEfficiency_SingleMuon_HLT_IsoMu20_Matched_MuPt20.root",kOrange);

}










