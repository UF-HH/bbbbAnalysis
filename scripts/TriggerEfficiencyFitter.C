#include "Riostream.h"
#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"


TH1D* TriggerEfficiencyFitter(TFile *inputFile, std::string sampleName, std::string selectionName, 
    std::string histogramName, std::string normalizationSelectionName, TF1 *fittingFunction, double *parameters)
{
    std::string efficiencyTmpName = Form("%s/%s/%s_%s_%s",sampleName.data(),selectionName.data(),sampleName.data(),selectionName.data(),histogramName.data());
    std::cout<<efficiencyTmpName<<std::endl;
    TH1D *efficiencyTmp    = (TH1D*)inputFile->Get(efficiencyTmpName.data());
    TH1D *efficiencyHisto  = (TH1D*)efficiencyTmp->Clone(("Efficiency "+selectionName).data());
    efficiencyHisto->SetDirectory(0);


    std::string normalizationTmpName = Form("%s/%s/%s_%s_%s",sampleName.data(),normalizationSelectionName.data(),sampleName.data(),normalizationSelectionName.data(),histogramName.data());
    std::cout<<normalizationTmpName<<std::endl;
    TH1D *normalizationTmp    = (TH1D*)inputFile->Get(normalizationTmpName.data());
    TH1D *normalizationHisto  = (TH1D*)normalizationTmp->Clone(("Normalization "+selectionName).data());
    normalizationHisto->SetDirectory(0);
    
    efficiencyHisto->Divide(normalizationHisto);
    efficiencyHisto->GetYaxis()->SetTitle("efficiency");

    return efficiencyHisto;
}

void MeasureTriggerEfficiency(std::string inputFileName, std::string sampleName, std::string outputFileName)
{
    TF1 *dummyFormula = new TF1();
    double dummyParameters[1] = {0.};

    TFile *inputFile = new TFile(inputFileName.data());
    TFile *outputFile = new TFile(outputFileName.data(),"RECREATE");

    TH1D *efficiency;
    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "L1triggerAndPrevious_Full", 
        "SelectedJetPtSum", "Normalization_Full", dummyFormula, dummyParameters);
    efficiency->SetDirectory(0);
    efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "QuadCentralJet30AndPrevious_Full", 
        "ForthSelectedJetPt", "L1triggerAndPrevious_Full", dummyFormula, dummyParameters);
    efficiency->SetDirectory(0);
    efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "DoubleCentralJet90AndPrevious_Full", 
        "SecondSelectedJetPt", "QuadCentralJet30AndPrevious_Full", dummyFormula, dummyParameters);
    efficiency->SetDirectory(0);
    efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "QuadPFCentralJetLooseID30AndPrevious_Full", 
        "ForthSelectedJetPt", "DoubleCentralJet90AndPrevious_Full", dummyFormula, dummyParameters);
    efficiency->SetDirectory(0);
    efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "DoublePFCentralJetLooseID90AndPrevious_Full", 
        "SecondSelectedJetPt", "QuadPFCentralJetLooseID30AndPrevious_Full", dummyFormula, dummyParameters);
    efficiency->SetDirectory(0);
    efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, "BTagCaloCSVp087TripleAndPrevious_Full", 
        "ThirdSelectedJetDeepCSV", "DoublePFCentralJetLooseID90AndPrevious_Full", dummyFormula, dummyParameters);
    efficiency->SetDirectory(0);
    efficiency->Write();

    //outputFile->WriteObject(efficiency,"Efficiency");

    // inputFile->Close();
    // outputFile->Close();

    return;
}