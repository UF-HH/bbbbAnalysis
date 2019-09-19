#include "OfflineProducerHelper.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <stdlib.h>
#include <any>

#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CompositeCandidate.h"
#include "Jet.h"
#include "Electron.h"
#include "Muon.h"
#include "GenPart.h"
#include "HH4b_kinFit.h"
#include "TRandom.h"

#include "TMVA/GeneticAlgorithm.h"
#include "TMVA/GeneticFitter.h"
#include "TMVA/IFitterTarget.h"
#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Reader.h"

using namespace std;

// ----------------- Objects for cut - BEGIN ----------------- //

void OfflineProducerHelper::initializeObjectsForCuts(OutputTree &ot){

    string objectsForCut = any_cast<string>(parameterList_->at("ObjectsForCut"));

    if(objectsForCut == "None")
        save_objects_for_cut = [](NanoAODTree& nat, OutputTree &ot, EventInfo& ei) -> void {return;};

    else if(objectsForCut == "WandZleptonDecays"){
        save_objects_for_cut = [=](NanoAODTree& nat, OutputTree &ot, EventInfo& ei) -> void {this -> save_WAndZLeptonDecays(nat, ot, ei);};
        ot.declareUserFloatBranch("LeadingIsolatedZElectron_pt", -1.);
        ot.declareUserFloatBranch("LeadingIsolatedZMuon_pt", -1.);
        ot.declareUserFloatBranch("LeadingIsolatedWElectron_pt", -1.);
        ot.declareUserFloatBranch("LeadingIsolatedWMuon_pt", -1.);
    }

    else if(objectsForCut == "TriggerObjects"){
        save_objects_for_cut = [=] (NanoAODTree& nat, OutputTree &ot, EventInfo& ei) -> void {this -> save_TriggerObjects(nat, ot, ei);};
        for(const auto & triggerObject : any_cast< std::map< std::pair<int,int>, std::string > >(parameterList_->at("TriggerObjectsForStudies")))
        {
            if(mapTriggerObjectIdAndFilter_.find(triggerObject.first.first) == mapTriggerObjectIdAndFilter_.end())
            {
                mapTriggerObjectIdAndFilter_[triggerObject.first.first] = vector<int>();
            }
            //Adding only the ones not already in there
            if(std::find(mapTriggerObjectIdAndFilter_[triggerObject.first.first].begin(),mapTriggerObjectIdAndFilter_[triggerObject.first.first].end(),triggerObject.first.second) == mapTriggerObjectIdAndFilter_[triggerObject.first.first].end())
                mapTriggerObjectIdAndFilter_[triggerObject.first.first].emplace_back(triggerObject.first.second);
            ot.declareUserIntBranch(triggerObject.second, 0);
        }
        ot.declareUserFloatBranch("FirstSelectedJetPt", -1.);
        ot.declareUserFloatBranch("SecondSelectedJetPt", -1.);
        ot.declareUserFloatBranch("ThirdSelectedJetPt", -1.);
        ot.declareUserFloatBranch("ForthSelectedJetPt", -1.);
        ot.declareUserFloatBranch("SelectedJetPtSum", -1.);
        ot.declareUserFloatBranch("ThirdSelectedJetDeepCSV", -1.);
        ot.declareUserFloatBranch("HighestMuonPt", -1.);
        ot.declareUserFloatBranch("SmallestMuonJetDeltaR", -1.);

    }

    return;

}

// reject events with leptons that may come from W and Z decays
void OfflineProducerHelper::save_WAndZLeptonDecays (NanoAODTree& nat, OutputTree &ot, EventInfo& ei){

    std::vector<Electron> electronFromW;
    electronFromW.reserve(*(nat.nElectron));
    std::vector<Electron> electronFromZ;
    electronFromZ.reserve(*(nat.nElectron));
    std::vector<Muon> muonFromW;
    muonFromW.reserve(*(nat.nMuon));
    std::vector<Muon> muonFromZ;
    muonFromZ.reserve(*(nat.nMuon));

    for (uint eIt = 0; eIt < *(nat.nElectron); ++eIt){
        Electron theElectron(eIt, &nat);

        if( //theElectron.P4Regressed().Pt()                                 >25.0 &&
            get_property(theElectron,Electron_mvaSpring16GP_WP80) &&
            get_property(theElectron,Electron_pfRelIso03_all    ) < any_cast<float>(parameterList_->at("WElectronMaxPfIso")) )
            electronFromW.emplace_back(theElectron);

        if( //theElectron.P4().Pt()                                 >20.0 &&
            get_property(theElectron,Electron_mvaSpring16GP_WP90) &&
            get_property(theElectron,Electron_pfRelIso03_all    ) < any_cast<float>(parameterList_->at("ZElectronMaxPfIso")) )
            electronFromZ.emplace_back(theElectron);
    }

    if(electronFromZ.size()>0){
        stable_sort(electronFromZ.begin(), electronFromZ.end(), [](const Electron & a, const Electron & b) -> bool
        {
            return ( a.P4().Pt() > b.P4().Pt() );
        }); // sort electrons by pt (highest to lowest)

        ot.userFloat("LeadingIsolatedZElectron_pt") = electronFromZ.at(0).P4().Pt();
    }

    if(electronFromW.size()>0){
        stable_sort(electronFromW.begin(), electronFromW.end(), [](const Electron & a, const Electron & b) -> bool
        {
            return ( a.P4().Pt() > b.P4().Pt() );
        }); // sort electrons by pt (highest to lowest)

        ot.userFloat("LeadingIsolatedWElectron_pt") = electronFromW.at(0).P4().Pt();
    }

    for (uint eIt = 0; eIt < *(nat.nMuon); ++eIt){
        Muon theMuon(eIt, &nat);

        if( //theMuon.P4().Pt()                                     >25.0 &&
            get_property(theMuon,Muon_tightId                    )      &&
            get_property(theMuon,Muon_pfRelIso04_all             ) < any_cast<float>(parameterList_->at("WMuonMaxPfIso")) &&
            get_property(theMuon,Muon_dxy                        ) < any_cast<float>(parameterList_->at("MuonMaxDxy")) &&
            get_property(theMuon,Muon_dz                         ) < any_cast<float>(parameterList_->at("MuonMaxDz")) )
            muonFromW.emplace_back(theMuon);

        if( //theMuon.P4().Pt()                                     >20.0 &&
            get_property(theMuon,Muon_pfRelIso04_all             ) < any_cast<float>(parameterList_->at("ZMuonMaxPfIso")) &&
            get_property(theMuon,Muon_dxy                        ) < any_cast<float>(parameterList_->at("MuonMaxDxy")) &&
            get_property(theMuon,Muon_dz                         ) < any_cast<float>(parameterList_->at("MuonMaxDz")) )
            muonFromZ.emplace_back(theMuon);
    }

    if(muonFromZ.size()>0){
        stable_sort(muonFromZ.begin(), muonFromZ.end(), [](const Muon & a, const Muon & b) -> bool
        {
            return ( a.P4().Pt() > b.P4().Pt() );
        }); // sort muons by pt (highest to lowest)

        ot.userFloat("LeadingIsolatedZMuon_pt") = muonFromZ.at(0).P4().Pt();
    }

    if(muonFromW.size()>0){
        stable_sort(muonFromW.begin(), muonFromW.end(), [](const Muon & a, const Muon & b) -> bool
        {
            return ( a.P4().Pt() > b.P4().Pt() );
        }); // sort muons by pt (highest to lowest)

        ot.userFloat("LeadingIsolatedWMuon_pt") = muonFromW.at(0).P4().Pt();
    }

}

void OfflineProducerHelper::save_TriggerObjects (NanoAODTree& nat, OutputTree &ot, EventInfo& ei)
{


    for( const auto & triggerAndBranch : any_cast<std::map<std::pair<int,int>, std::string > >(parameterList_->at("TriggerObjectsForStudies")) )
    {
        ot.userInt(triggerAndBranch.second) = mapTriggerMatching_[triggerAndBranch.first];

    }

    std::vector<double> candidatePt =
    {
        ei.H1_b1->P4().Pt(),
        ei.H1_b2->P4().Pt(),
        ei.H2_b1->P4().Pt(),
        ei.H2_b2->P4().Pt()
    };
    stable_sort(candidatePt.begin(), candidatePt.end(), greater<double>());

    std::vector<double> candidateDeepCSV =
    {
        get_property(ei.H1_b1.get(),Jet_btagDeepB),
        get_property(ei.H1_b2.get(),Jet_btagDeepB),
        get_property(ei.H2_b1.get(),Jet_btagDeepB),
        get_property(ei.H2_b2.get(),Jet_btagDeepB)
    };
    stable_sort(candidateDeepCSV.begin(), candidateDeepCSV.end(), greater<double>());


    ot.userFloat("FirstSelectedJetPt")      = candidatePt[0];
    ot.userFloat("SecondSelectedJetPt")     = candidatePt[1];
    ot.userFloat("ThirdSelectedJetPt")      = candidatePt[2];
    ot.userFloat("ForthSelectedJetPt")      = candidatePt[3];
    ot.userFloat("SelectedJetPtSum")        = candidatePt[0] + candidatePt[1] + candidatePt[2] + candidatePt[3];
    ot.userFloat("ThirdSelectedJetDeepCSV") = candidateDeepCSV[2];
    if(*(nat.nMuon)>0){
        ot.userFloat("HighestMuonPt") = nat.Muon_pt.At(0);
        std::vector<double> muonJetDeltaR;
        muonJetDeltaR.emplace_back(sqrt(pow(ei.H1_b1->P4().Eta() - nat.Muon_eta.At(0),2) + pow(deltaPhi(ei.H1_b1->P4().Phi(), nat.Muon_phi.At(0)),2) ) );
        muonJetDeltaR.emplace_back(sqrt(pow(ei.H1_b2->P4().Eta() - nat.Muon_eta.At(0),2) + pow(deltaPhi(ei.H1_b2->P4().Phi(), nat.Muon_phi.At(0)),2) ) );
        muonJetDeltaR.emplace_back(sqrt(pow(ei.H2_b1->P4().Eta() - nat.Muon_eta.At(0),2) + pow(deltaPhi(ei.H2_b1->P4().Phi(), nat.Muon_phi.At(0)),2) ) );
        muonJetDeltaR.emplace_back(sqrt(pow(ei.H2_b2->P4().Eta() - nat.Muon_eta.At(0),2) + pow(deltaPhi(ei.H2_b2->P4().Phi(), nat.Muon_phi.At(0)),2) ) );
        stable_sort(muonJetDeltaR.begin(), muonJetDeltaR.end());
        ot.userFloat("SmallestMuonJetDeltaR") = muonJetDeltaR.at(0);
    }


    return;
}

// ----------------- Objects for cut - END ----------------- //


// ----------------- Compute scaleFactors - BEGIN ----------------- //

void OfflineProducerHelper::initializeObjectsBJetForScaleFactors(OutputTree &ot){

    string scaleFactorsMethod = any_cast<string>(parameterList_->at("BTagScaleFactorMethod"));

    if(scaleFactorsMethod == "None"){
        //do nothing
    }
    else if(scaleFactorsMethod == "FourBtag_ScaleFactor"){

        // b-tag scale factors
        ot.declareUserFloatBranch("bTagScaleFactor_central"          , 1.);
        ot.declareUserFloatBranch("bTagScaleFactor_bJets_up"         , 1.);
        ot.declareUserFloatBranch("bTagScaleFactor_bJets_down"       , 1.);
        ot.declareUserFloatBranch("bTagScaleFactor_cJets_up"         , 1.);
        ot.declareUserFloatBranch("bTagScaleFactor_cJets_down"       , 1.);
        ot.declareUserFloatBranch("bTagScaleFactor_lightJets_up"     , 1.);
        ot.declareUserFloatBranch("bTagScaleFactor_lightJets_down"   , 1.);

        // branchesAffectedByJetEnergyVariations_["bTagScaleFactor_central"] = 1.;
        BTagCalibration btagCalibration("DeepCSV",any_cast<string>(parameterList_->at("BJetScaleFactorsFile")));
        btagCalibrationReader_lightJets_ = new BTagCalibrationReader(BTagEntry::OP_MEDIUM,"central",{"up", "down"});
        btagCalibrationReader_cJets_     = new BTagCalibrationReader(BTagEntry::OP_MEDIUM,"central",{"up", "down"});
        btagCalibrationReader_bJets_     = new BTagCalibrationReader(BTagEntry::OP_MEDIUM,"central",{"up", "down"});
        btagCalibrationReader_lightJets_->load(btagCalibration, BTagEntry::FLAV_UDSG, "incl"  );
        btagCalibrationReader_cJets_    ->load(btagCalibration, BTagEntry::FLAV_C   , "mujets");
        btagCalibrationReader_bJets_    ->load(btagCalibration, BTagEntry::FLAV_B   , "mujets");

    }

    return;

}

void OfflineProducerHelper::compute_scaleFactors_fourBtag_eventScaleFactor (const std::vector<Jet> &jets, NanoAODTree& nat, OutputTree &ot){

    float tmpScaleFactor_bJets_central     = 1.;
    float tmpScaleFactor_bJets_up          = 1.;
    float tmpScaleFactor_bJets_down        = 1.;
    float tmpScaleFactor_cJets_central     = 1.;
    float tmpScaleFactor_cJets_up          = 1.;
    float tmpScaleFactor_cJets_down        = 1.;
    float tmpScaleFactor_lightJets_central = 1.;
    float tmpScaleFactor_lightJets_up      = 1.;
    float tmpScaleFactor_lightJets_down    = 1.;

    for(const auto &iJet : jets){
        int jetFlavour = abs(get_property(iJet,Jet_hadronFlavour));
        if(jetFlavour==5){
            tmpScaleFactor_bJets_central     *= btagCalibrationReader_bJets_    ->eval_auto_bounds("central", BTagEntry::FLAV_B   , iJet.P4().Eta(), iJet.P4().Pt());
            tmpScaleFactor_bJets_up          *= btagCalibrationReader_bJets_    ->eval_auto_bounds("up"     , BTagEntry::FLAV_B   , iJet.P4().Eta(), iJet.P4().Pt());
            tmpScaleFactor_bJets_down        *= btagCalibrationReader_bJets_    ->eval_auto_bounds("down"   , BTagEntry::FLAV_B   , iJet.P4().Eta(), iJet.P4().Pt());
        }
        else if(jetFlavour==4){
            tmpScaleFactor_cJets_central     *= btagCalibrationReader_cJets_    ->eval_auto_bounds("central", BTagEntry::FLAV_C   , iJet.P4().Eta(), iJet.P4().Pt());
            tmpScaleFactor_cJets_up          *= btagCalibrationReader_cJets_    ->eval_auto_bounds("up"     , BTagEntry::FLAV_C   , iJet.P4().Eta(), iJet.P4().Pt());
            tmpScaleFactor_cJets_down        *= btagCalibrationReader_cJets_    ->eval_auto_bounds("down"   , BTagEntry::FLAV_C   , iJet.P4().Eta(), iJet.P4().Pt());
        }
        else{
            tmpScaleFactor_lightJets_central *= btagCalibrationReader_lightJets_->eval_auto_bounds("central", BTagEntry::FLAV_UDSG, iJet.P4().Eta(), iJet.P4().Pt());
            tmpScaleFactor_lightJets_up      *= btagCalibrationReader_lightJets_->eval_auto_bounds("up"     , BTagEntry::FLAV_UDSG, iJet.P4().Eta(), iJet.P4().Pt());
            tmpScaleFactor_lightJets_down    *= btagCalibrationReader_lightJets_->eval_auto_bounds("down"   , BTagEntry::FLAV_UDSG, iJet.P4().Eta(), iJet.P4().Pt());
        }
    }

    ot.userFloat("bTagScaleFactor_central"          ) = tmpScaleFactor_bJets_central * tmpScaleFactor_cJets_central * tmpScaleFactor_lightJets_central ;
    ot.userFloat("bTagScaleFactor_bJets_up"         ) = tmpScaleFactor_bJets_up      * tmpScaleFactor_cJets_central * tmpScaleFactor_lightJets_central ;
    ot.userFloat("bTagScaleFactor_bJets_down"       ) = tmpScaleFactor_bJets_down    * tmpScaleFactor_cJets_central * tmpScaleFactor_lightJets_central ;
    ot.userFloat("bTagScaleFactor_cJets_up"         ) = tmpScaleFactor_bJets_central * tmpScaleFactor_cJets_up      * tmpScaleFactor_lightJets_central ;
    ot.userFloat("bTagScaleFactor_cJets_down"       ) = tmpScaleFactor_bJets_central * tmpScaleFactor_cJets_down    * tmpScaleFactor_lightJets_central ;
    ot.userFloat("bTagScaleFactor_lightJets_up"     ) = tmpScaleFactor_bJets_central * tmpScaleFactor_cJets_central * tmpScaleFactor_lightJets_up      ;
    ot.userFloat("bTagScaleFactor_lightJets_down"   ) = tmpScaleFactor_bJets_central * tmpScaleFactor_cJets_central * tmpScaleFactor_lightJets_down    ;

    return;
}

// ----------------- Compute scaleFactors - END ----------------- //


// ----------------- Compute weights - BEGIN ----------------- //

void OfflineProducerHelper::initializeObjectsForEventWeight(OutputTree &ot, SkimEffCounter &ec, std::string PUWeightFileName, float crossSection)
{
    sampleCrossSection_ = 1;//crossSection;

    string weightsMethod = any_cast<string>(parameterList_->at("WeightMethod"));

    if(weightsMethod == "None")
    {
        calculateEventWeight = [=](NanoAODTree& nat, EventInfo& ei, OutputTree &ot, SkimEffCounter &ec) -> float {return sampleCrossSection_;};
    }
    else if(weightsMethod == "StandardWeight")
    {
        calculateEventWeight = [=] (NanoAODTree& nat, EventInfo& ei, OutputTree &ot, SkimEffCounter &ec) -> float {return this -> calculateEventWeight_AllWeights (nat, ei, ot, ec);};
        std::string branchName;

        int weightBin = ec.binMap_.size();

        // PUWeight need to store histograms from pu files
        branchName = "PUWeight";
        ot.declareUserFloatBranch(branchName, 1.);
        weightMap_[branchName] = std::pair< float, std::map<std::string, float> >();
        weightMap_[branchName].first = 1.;
        std::vector<std::string> puWeightVariation = {"_up","_down"};
        TFile *PUWeightFile = TFile::Open(PUWeightFileName.data());
        std::map<std::string, TH1D*> PUWeightHistogramMap;
        if(PUWeightFile == NULL){
            cerr << "**  Pileup weight file " << PUWeightFileName << " not found, aborting" << endl;
            abort();
        }
        PUWeightHistogramMap[branchName] = (TH1D*) PUWeightFile->Get("PUweights");
        // PUWeightHistogramMap[branchName]->SetDirectory(0);

        for(unsigned int var = 0; var<puWeightVariation.size(); ++var)
        {
            std::string variationBranch = branchName + puWeightVariation[var];
            ot.declareUserFloatBranch(variationBranch, 1.);
            weightMap_[branchName].second[variationBranch] = 1.;
            ec.binMap_[variationBranch] = ++weightBin;
            ec.binEntries_[variationBranch] = 0.;
            PUWeightHistogramMap[variationBranch] = (TH1D*) PUWeightFile->Get(("PUweights"+puWeightVariation[var]).data());
            // PUWeightHistogramMap[variationBranch]->SetDirectory(0);
        }

        for(const auto & histogram : PUWeightHistogramMap)
        {
            for(int iBin=1; iBin<=histogram.second->GetNbinsX(); ++iBin)
            {
                PUWeightMap_[histogram.first][std::pair<float,float>(histogram.second->GetBinLowEdge(iBin),histogram.second->GetBinLowEdge(iBin+1))] = histogram.second->GetBinContent(iBin);
            }
        }

        PUWeightFile->Close();

        //genWeight (no weight variations)
        branchName = "genWeight";
        ot.declareUserFloatBranch(branchName, 1.);
        weightMap_[branchName] = std::pair< float, std::map<std::string, float> >();
        weightMap_[branchName].first = 1.;

        // LHEPdfWeight
        branchName = "LHEPdfWeight";
        ot.declareUserFloatBranch(branchName, 1.);
        weightMap_[branchName] = std::pair< float, std::map<std::string, float> >();
        weightMap_[branchName].first = 1.;
        // LHEPdfWeight weight variations branches are created during the event loop (number of variation varies between samples)

        // LHEScaleWeight
        branchName = "LHEScaleWeight";
        ot.declareUserFloatBranch(branchName, 1.);
        weightMap_[branchName] = std::pair< float, std::map<std::string, float> >();
        weightMap_[branchName].first = 1.;
        // LHEScaleWeight weight variations branches are created during the event loop (number of variation varies between samples)

        // PSWeight
        branchName = "PSWeight";
        ot.declareUserFloatBranch(branchName, 1.);
        weightMap_[branchName] = std::pair< float, std::map<std::string, float> >();
        weightMap_[branchName].first = 1.;
        // PSWeight weight variations branches are created during the event loop (number of variation varies between samples)
    }

    return;

}

float OfflineProducerHelper::calculateEventWeight_AllWeights(NanoAODTree& nat, EventInfo& ei, OutputTree &ot, SkimEffCounter &ec)
{

    for(auto & weight : weightMap_)
    {
        weight.second.first = 1.;
        for(auto & correction : weight.second.second)
        {
            correction.second = 1.;
        }
    }

    int weightBin = ec.binMap_.size();

    float eventWeight = 1.; //sampleCrossSection_;
    float tmpWeight = 1.;
    std::string branchName;

    // PUWeight need get pu from histograms
    branchName = "PUWeight";
    float eventPU = *(nat.Pileup_nTrueInt);
    for(const auto & puWeightBin : PUWeightMap_[branchName])
    {
        if(eventPU >= puWeightBin.first.first && eventPU < puWeightBin.first.second)
        {
            tmpWeight = puWeightBin.second;
            break;
        }
    }
    tmpWeight = tmpWeight==0 ? 1 : tmpWeight; //set to 1 if weight is 0
    ot.userFloat(branchName) = tmpWeight;
    weightMap_[branchName].first = tmpWeight;
    eventWeight *= tmpWeight;
    std::vector<std::string> puWeightVariation = {"_up","_down"};
    for(unsigned int var = 0; var<puWeightVariation.size(); ++var)
    {
        std::string variationBranch = branchName + puWeightVariation[var];
        for(const auto & puWeightBin : PUWeightMap_[variationBranch])
        {
            if(eventPU >= puWeightBin.first.first && eventPU < puWeightBin.first.second)
            {
                tmpWeight = puWeightBin.second;
                break;
            }
        }
        tmpWeight = tmpWeight==0 ? 1 : tmpWeight; //set to 1 if weight is 0
        ot.userFloat(variationBranch) = tmpWeight;
        weightMap_[branchName].second[variationBranch] = tmpWeight;
    }

    //genWeight (no weight variations)
    branchName = "genWeight";
    float genWeight = *(nat.genWeight);
    //float originalXWGTUP = *(nat.LHEWeight_originalXWGTUP);
    //if (originalXWGTUP != genWeight) genWeight *= originalXWGTUP; //Mail from Roberto Covarelli
    tmpWeight = genWeight;
    tmpWeight = tmpWeight==0 ? 1 : tmpWeight; //set to 1 if weight is 0
    ot.userFloat(branchName) = tmpWeight;
    weightMap_[branchName].first = tmpWeight;
    eventWeight *= tmpWeight;


    // LHEPdfWeight
    branchName = "LHEPdfWeight";
    tmpWeight = 1.;
    tmpWeight = tmpWeight==0 ? 1 : tmpWeight; //set to 1 if weight is 0
    ot.userFloat(branchName) = tmpWeight;
    weightMap_[branchName].first = tmpWeight;
    eventWeight *= tmpWeight;
    // LHEPdfWeight weight variations
    for(unsigned int var = 0; var<=*(nat.nLHEPdfWeight); ++var)
    {
        tmpWeight = nat.LHEPdfWeight.At(var);
        tmpWeight = tmpWeight==0 ? 1 : tmpWeight; //set to 1 if weight is 0
        std::string variationBranch = branchName + "_var" + std::to_string(var);
        if(weightMap_[branchName].second.find(variationBranch) == weightMap_[branchName].second.end()) //branch does not exist, creating:
        {
            ot.declareUserFloatBranch(variationBranch, 1.);
            ec.binMap_[variationBranch] = ++weightBin;
            ec.binEntries_[variationBranch] = 0.;
        }
        ot.userFloat(variationBranch) = tmpWeight;
        weightMap_[branchName].second[variationBranch] = tmpWeight;
    }

    // LHEScaleWeight
    branchName = "LHEScaleWeight";
    tmpWeight = 1;
    tmpWeight = tmpWeight==0 ? 1 : tmpWeight; //set to 1 if weight is 0
    ot.userFloat(branchName) = tmpWeight;
    weightMap_[branchName].first = tmpWeight;
    eventWeight *= tmpWeight;
    // LHEScaleWeight weight variations
    for(unsigned int var = 0; var<*(nat.nLHEScaleWeight); ++var)
        {
        tmpWeight = nat.LHEScaleWeight.At(var);
        tmpWeight = tmpWeight==0 ? 1 : tmpWeight; //set to 1 if weight is 0
        std::string variationBranch = branchName + "_var" + std::to_string(var);
        if(weightMap_[branchName].second.find(variationBranch) == weightMap_[branchName].second.end()) //branch does not exist, creating:
        {
            ot.declareUserFloatBranch(variationBranch, 1.);
            ec.binMap_[variationBranch] = ++weightBin;
            ec.binEntries_[variationBranch] = 0.;
        }
        ot.userFloat(variationBranch) = tmpWeight;
        weightMap_[branchName].second[variationBranch] = tmpWeight;
    }

    // PSWeight
    branchName = "PSWeight";
    tmpWeight = 1;
    tmpWeight = tmpWeight==0 ? 1 : tmpWeight; //set to 1 if weight is 0
    ot.userFloat(branchName) = tmpWeight;
    weightMap_[branchName].first = tmpWeight;
    eventWeight *= tmpWeight;
    // PSWeight weight variations
    for(unsigned int var = 0; var<*(nat.nPSWeight); ++var)
    {
        tmpWeight = nat.PSWeight.At(var);
        tmpWeight = tmpWeight==0 ? 1 : tmpWeight; //set to 1 if weight is 0
        std::string variationBranch = branchName + "_var" + std::to_string(var);
        if(weightMap_[branchName].second.find(variationBranch) == weightMap_[branchName].second.end()) //branch does not exist, creating:
        {
            ot.declareUserFloatBranch(variationBranch, 1.);
            ec.binMap_[variationBranch] = ++weightBin;
            ec.binEntries_[variationBranch] = 0.;
        }
        ot.userFloat(variationBranch) = tmpWeight;
        weightMap_[branchName].second[variationBranch] = tmpWeight;
    }

    // hh reweight if needed
    if (hhreweighter_)
    {

        float kl = hhreweighter_kl_;
        TLorentzVector vSum = ei.gen_H1->P4() + ei.gen_H2->P4();
        
        // boost to CM
        TLorentzVector vH1_cm = ei.gen_H1->P4();
        vH1_cm.Boost(-vSum.BoostVector());

        float gen_mHH          = vSum.M();
        float gen_costh_H1_cm  = vH1_cm.CosTheta();

        double w = hhreweighter_->getWeight(kl, 1.0, gen_mHH, gen_costh_H1_cm);
        ot.userFloat("HH_reweight") = w;
        eventWeight *= w;
    }

    //calculate bins for weights variations

    for(auto & weight : weightMap_)
    {
        for(auto & correction : weight.second.second)
        {
            ec.binEntries_[correction.first] += (eventWeight/weight.second.first*correction.second);
        }
    }

    return eventWeight;

}

// ----------------- Compute weights - END ----------------- //


// ----------------- Compute JER - BEGIN ------------------- //

void OfflineProducerHelper::initializeJERsmearingAndVariations(OutputTree &ot)
{

    string JERmethod = any_cast<string>(parameterList_->at("JetEnergyResolution"));

    if (JERmethod == "None")
    {
        JERsmearing   = [](NanoAODTree& nat, std::vector<Jet> &jets) -> std::vector<Jet>  {return jets;};
        JERvariations = [](NanoAODTree& nat, std::vector<Jet> &jets, std::vector< std::pair<std::string, std::vector<Jet> > > &jetEnergyVariationsMap) -> void {return;};
    }
    else if (JERmethod == "StandardJER")
    {
        // JERsmearing = &OfflineProducerHelper::standardJERsmearing;
        JERsmearing = [=] (NanoAODTree& nat, std::vector<Jet> &jets) -> std::vector<Jet> {return this -> standardJERsmearing(nat, jets);};
        jetResolutionScaleFactor_ = new JME::JetResolutionScaleFactor(any_cast<string>(parameterList_->at("JERScaleFactorFile")));
        jetResolution_            = new JME::JetResolution           (any_cast<string>(parameterList_->at("JERResolutionFile" )));
        gRandom->SetSeed(any_cast<int>(parameterList_->at("RandomGeneratorSeed")));

        //set up variations
        if(any_cast<bool>(parameterList_->at("JERComputeVariations")))
        {
            mapJERNamesAndVariation_["JER_up"  ] = Variation::UP  ;
            mapJERNamesAndVariation_["JER_down"] = Variation::DOWN;
            // JERvariations = &OfflineProducerHelper::standardJERVariations;
            JERvariations = [=] (NanoAODTree& nat, std::vector<Jet> &jets, std::vector< std::pair<std::string, std::vector<Jet> > > &jetEnergyVariationsMap)
                {this -> standardJERVariations (nat, jets, jetEnergyVariationsMap);};
            for(const auto & branch : branchesAffectedByJetEnergyVariations_)
            {
                for(const auto & variation : mapJERNamesAndVariation_)
                {
                    ot.declareUserFloatBranch(branch.first + "_" + variation.first, branch.second);
                }
            }
        }
        else
        {
            JERvariations = [](NanoAODTree& nat, std::vector<Jet> &jets, std::vector< std::pair<std::string, std::vector<Jet> > > &jetEnergyVariationsMap) -> void {return;};
        }

    }

    return;
}

std::vector<Jet> OfflineProducerHelper::standardJERsmearing(NanoAODTree& nat, std::vector<Jet> &jets)
{
    return applyJERsmearing(nat, jets);
}

void OfflineProducerHelper::standardJERVariations(NanoAODTree& nat, std::vector<Jet> &jets, std::vector< std::pair<std::string, std::vector<Jet> > > &jetEnergyVariationsMap)
{

    for (auto const & variation : mapJERNamesAndVariation_)
    {
        jetEnergyVariationsMap.push_back (std::pair<std::string, std::vector<Jet> >(variation.first, applyJERsmearing(nat, jets, variation.second)));
    }

    return;
}


std::vector<Jet> OfflineProducerHelper::applyJERsmearing(NanoAODTree& nat, std::vector<Jet> jets, Variation variation)
{

    JME::JetParameters jetParameters;
    jetParameters.setRho(*(nat.fixedGridRhoFastjetAll));
    int numberOfGenJets = *(nat.nGenJet);

    for(auto &iJet : jets){
        //same method of https://github.com/cms-sw/cmssw/blob/CMSSW_8_0_25/PhysicsTools/PatUtils/interface/SmearedJetProducerT.h
        jetParameters.setJetEta(iJet.P4().Eta());
        jetParameters.setJetPt(iJet.P4().Pt());

        float tmpJER_ScaleFactor = jetResolutionScaleFactor_->getScaleFactor(jetParameters, variation  );
        float tmpJER_Resolution  = jetResolution_->getResolution(jetParameters);

        int genJetId = abs(get_property(iJet,Jet_genJetIdx));

        float smearFactor;
        if(genJetId>=0 && genJetId < numberOfGenJets) //generated jet was found
        {
            smearFactor = 1. + (tmpJER_ScaleFactor - 1.) * (iJet.P4().Pt() - nat.GenJet_pt.At(genJetId))/iJet.P4().Pt();
        }
        else if(tmpJER_ScaleFactor>1.)
        {
            float sigma = tmpJER_Resolution * std::sqrt(tmpJER_ScaleFactor * tmpJER_ScaleFactor - 1);
            smearFactor = 1. + gRandom->Gaus(0., sigma);
        }
        else
        {
            smearFactor = 1.;
        }

        float MIN_JET_ENERGY = 1e-2;

        if (iJet.P4().Energy() * smearFactor < MIN_JET_ENERGY)
        {
            smearFactor = MIN_JET_ENERGY / iJet.P4().Energy();
        }

        iJet.setP4(iJet.P4()*smearFactor);
        iJet.buildP4Regressed();

    }


    return jets;
}

// ----------------- Compute JER - END --------------------- //


// ----------------- Compute JER - BEGIN ------------------- //

void OfflineProducerHelper::initializeJECVariations(OutputTree &ot)
{

    string JECmethod = any_cast<string>(parameterList_->at("JetEnergyCorrection"));

    if (JECmethod == "None")
    {
        JECvariations = [](NanoAODTree& nat, std::vector<Jet> &jets, std::vector< std::pair<std::string, std::vector<Jet> > > &jetEnergyVariationsMap) -> void {return;};
    }
    else if (JECmethod == "StandardJEC")
    {
        // JECvariations = &OfflineProducerHelper::standardJECVariations;
        JECvariations = [=] (NanoAODTree& nat, std::vector<Jet> &jets, std::vector< std::pair<std::string, std::vector<Jet> > > &jetEnergyVariationsMap) -> void
            {this -> standardJECVariations(nat, jets, jetEnergyVariationsMap);};
        mapJECNamesAndVariation_["_up"]   = true  ;
        mapJECNamesAndVariation_["_down"] = false ;

        string JECFileName = any_cast<string>(parameterList_->at("JECFileName"));
        for( const auto & JECVariationName : any_cast<std::vector<std::string> >(parameterList_->at("JECListOfCorrections")))
        {
            // JetCorrectorParameters *parameter = new JetCorrectorParameters(JECFileName, JECVariationName);
            // mapForJECuncertanties_[JECVariationName] = new JetCorrectionUncertainty(*parameter);
            mapForJECuncertanties_[JECVariationName] = new JetCorrectionUncertainty(*(new JetCorrectorParameters(JECFileName, JECVariationName)));

            for(const auto & branch : branchesAffectedByJetEnergyVariations_)
            {
                for(const auto & variation : mapJECNamesAndVariation_)
                {
                    ot.declareUserFloatBranch(branch.first + "_" + JECVariationName + variation.first, branch.second);
                }
            }
        }
    }

    return;
}

void OfflineProducerHelper::standardJECVariations(NanoAODTree& nat, std::vector<Jet> &jetsUnsmeared, std::vector< std::pair<std::string, std::vector<Jet> > > &jetEnergyVariationsMap)
{

    //Uncertanties are calculated according to https://twiki.cern.ch/twiki/bin/view/CMS/JECUncertaintySources#Code_example

    for (auto const & variation : mapForJECuncertanties_)
    {
        for (auto const & direction : mapJECNamesAndVariation_)
        {
            std::string variationName = variation.first + direction.first;
            jetEnergyVariationsMap.push_back (std::pair<std::string, std::vector<Jet> >(variationName, applyJECVariation(nat, jetsUnsmeared, variation.first, direction.second)));
        }
    }

    return;
}


std::vector<Jet> OfflineProducerHelper::applyJECVariation(NanoAODTree& nat, std::vector<Jet> jetsUnsmeared, std::string variationName, bool direction)
{

    // calculation derived from the following twikis:
    // https://twiki.cern.ch/twiki/bin/view/CMS/JECUncertaintySources#Example_implementation
    // https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#JetCorUncertainties

    double shift = direction ? 1. : -1. ;
    for(size_t iJet=0; iJet<jetsUnsmeared.size(); ++iJet)
    {
        mapForJECuncertanties_[variationName]->setJetPt(jetsUnsmeared[iJet].P4().Pt());
        mapForJECuncertanties_[variationName]->setJetEta(jetsUnsmeared[iJet].P4().Eta());
        double correctionFactor = mapForJECuncertanties_[variationName]->getUncertainty(direction);

        jetsUnsmeared[iJet].setP4(jetsUnsmeared[iJet].P4()*(1+shift*correctionFactor));
        jetsUnsmeared[iJet].buildP4Regressed();  //I have to recompute it in case JERsmearing is not doing anything
    }

    // JER smearing has to be applied after JEC:
    // https://hypernews.cern.ch/HyperNews/CMS/get/jes/439/1.html
    return JERsmearing(nat,jetsUnsmeared);;
}

// ----------------- Compute JEC - END --------------------- //


void OfflineProducerHelper::fillJetEnergyVariationBranch(OutputTree &ot, std::string branchName, std::string variation, float value)
{

    if(branchesAffectedByJetEnergyVariations_.find(branchName) == branchesAffectedByJetEnergyVariations_.end())
    {
        throw std::runtime_error("OfflineProducerHelper::select_bbbb_jets -> branch " + branchName + " doesn't exist, impossible to calculate the variations");
    }

    ot.userFloat(branchName + "_" + variation) = value;

    return;
}

bool OfflineProducerHelper::select_bbbb_jets(NanoAODTree& nat, EventInfo& ei, OutputTree &ot, std::vector<std::string> listOfPassedTriggers)
{

    if (*(nat.nJet) < 4)
        return false;
    std::vector<Jet> unsmearedJets;
    unsmearedJets.reserve(*(nat.nJet));

    for (uint ij = 0; ij < *(nat.nJet); ++ij){
        unsmearedJets.emplace_back(Jet(ij, &nat));
    }
// std::cout<<"culo1\n";

    //if some montecarlo weight are applied via a reshaping of the jets variables, they must be applied here

    //Apply preselection cuts
    const string preselectionCutStrategy = any_cast<string>(parameterList_->at("PreselectionCut"));

    if(preselectionCutStrategy=="bJetCut"){
        bJets_PreselectionCut(unsmearedJets);
    }
    else if(preselectionCutStrategy=="None"){
        //do nothing
    }
    else throw std::runtime_error("cannot recognize cut strategy --" + preselectionCutStrategy + "--");
// std::cout<<"culo2\n";

    //at least 4 jets required
    if(unsmearedJets.size()<4) return false;

    // sort by deepCSV (highest to lowest)
    stable_sort(unsmearedJets.begin(), unsmearedJets.end(), [](const Jet & a, const Jet & b) -> bool
    {
        return ( get_property(a, Jet_btagDeepB) > get_property(b, Jet_btagDeepB) );
    });

    std::vector<Jet> jetsOriginal;
    std::vector< std::pair<std::string, std::vector<Jet> > > jetEnergyVariationsMap;
    std::string originalSampleName = "NominalObjects";

    // Apply JER corrections and variations
    if(parameterList_->find("JetEnergyResolution") != parameterList_->end())
    { //is it a MC event
        jetsOriginal = JERsmearing(nat,unsmearedJets);
        JERvariations(nat,unsmearedJets,jetEnergyVariationsMap);
    }
    else
    {
        jetsOriginal = unsmearedJets;
    }
    //need to be done before all the variations
    jetEnergyVariationsMap.insert(jetEnergyVariationsMap.begin(),std::pair<std::string, std::vector<Jet> >(originalSampleName,jetsOriginal));

    if(parameterList_->find("JetEnergyCorrection") != parameterList_->end())
    { //is it a MC event
        JECvariations(nat,unsmearedJets,jetEnergyVariationsMap);
    }


// std::cout<<"culo3\n";
    int H1b1_idx(-1), H1b2_idx(-1), H2b1_idx(-1), H2b2_idx(-1); //Jet indexes for variations

    for(auto & jets : jetEnergyVariationsMap)
    {
        // std::cout<<jets.first<<" size = "<<jets.second.size()<<std::endl;
        if(jets.first == originalSampleName) //Original sample without variations
        {
            // calculate scaleFactors after preselection cuts
            if(parameterList_->find("BTagScaleFactorMethod") != parameterList_->end()){ //is it a MC event
                const string BJetcaleFactorsMethod = any_cast<string>(parameterList_->at("BTagScaleFactorMethod"));

                if(BJetcaleFactorsMethod == "FourBtag_ScaleFactor"){
                    compute_scaleFactors_fourBtag_eventScaleFactor(jets.second,nat,ot);
                }
            }

// std::cout<<"culo4\n";
            // now need to pair the jets
            std::vector<Jet> presel_jets = {{
                (jets.second[0]),
                (jets.second[1]),
                (jets.second[2]),
                (jets.second[3])
            }};

            std::vector<Jet> ordered_jets;
            string strategy = any_cast<string>(parameterList_->at("bbbbChoice"));

            //Select the fouf b jets
            if(strategy == "OneClosestToMh")
                ordered_jets = bbbb_jets_idxs_OneClosestToMh(&presel_jets);
            else if(strategy == "BothClosestToMh")
                ordered_jets = bbbb_jets_idxs_BothClosestToMh(&presel_jets);
            else if(strategy == "MostBackToBack")
                ordered_jets = bbbb_jets_idxs_MostBackToBack(&presel_jets);
            else if(strategy == "HighestCSVandClosestToMh"){
                ordered_jets = bbbb_jets_idxs_HighestCSVandClosestToMh(&jets.second);
            }
            else throw std::runtime_error("cannot recognize bbbb choice strategy " + strategy);

// std::cout<<"culo5\n";
            if(ordered_jets.size()!=4)
            {
                if(!any_cast<bool>(parameterList_->at("UseAntiTagOnOneBjet")) && strategy == "HighestCSVandClosestToMh")
                {
                    throw std::runtime_error("OfflineProducerHelper::select_bbbb_jets -> candidates not found -> this should never happen to use jet energy variations");
                }
                return false;
            }

            if(debug) std::cout<< "Event " << *(nat.run) << " - " << *(nat.luminosityBlock) << " - " << *(nat.event) << std::endl;

// std::cout<<"culo6\n";
            if(jets.first == originalSampleName)
            {
                std::vector< std::unique_ptr<Candidate> > selectedCandidates;
// std::cout<<"culo6.1\n";
                for(auto jet : ordered_jets)
                {
                    selectedCandidates.emplace_back(std::make_unique<Jet>(jet));
                    //std::cout<<jet.P4().Eta()<<" "<<selectedCandidates.back()->P4().Eta()<<" "<<selectedCandidates.at(0)->P4().Eta()<<std::endl;
                }

                if(*(nat.nMuon)>0)
                {
                    //Candidate *theHighestPtMuon = new Muon(0, &nat);
                    selectedCandidates.emplace_back(std::make_unique<Muon>(Muon(0, &nat)));
                }

                calculateTriggerMatching(selectedCandidates,nat);

// std::cout<<"culo6.2\n";
            }

            if(jets.first == originalSampleName)
            {
// std::cout<<"culo6.3\n";
                for(auto & triggerFired : listOfPassedTriggers)
                {
                    ot.userInt(triggerFired+"_Fired") = 1;
                }

                if(!checkTriggerObjectMatching(listOfPassedTriggers,ot))
                {
                    if(debug) std::cout<<"TriggerObjects not matched, Printing jets:\n";
                    if(debug) std::cout<<"Id\t\tPt\t\tEta\t\tPhi\t\tCSV\t\tSelected\n";
                    for(const auto & jet : jetsOriginal)
                    {
                        if(debug) std::cout<< std::fixed << std::setprecision(3) <<jet.getIdx()<<"\t\t"<<jet.P4().Pt()<<"\t\t"<<jet.P4().Eta()<<"\t\t"<<jet.P4().Phi()<<"\t\t"<<get_property((jet),Jet_btagDeepB)<<"\t\t";
                        bool selected=false;
                        for(const auto & selectedJet : ordered_jets)
                        {
                            if(selectedJet.getIdx() == jet.getIdx())
                            {
                                selected=true;
                                break;
                            }
                        }
                        if(selected) if(debug) std::cout<<"*";
                        if(debug) std::cout<<std::endl;
                    }
                    if(debug)
                    {
                        Muon *theHighestPtMuon = new Muon(0, &nat);
                        std::cout<< std::fixed << std::setprecision(3) <<theHighestPtMuon->getIdx()<<"\t\t"<<theHighestPtMuon->P4().Pt()<<"\t\t"<<theHighestPtMuon->P4().Eta()<<"\t\t"<<theHighestPtMuon->P4().Phi()<<"\n";
                    }

                }
// std::cout<<"culo6.4\n";
            }

// std::cout<<"culo7\n";

            // order H1, H2 by pT: pT(H1) > pT (H2)
            CompositeCandidate H1 = CompositeCandidate(ordered_jets.at(0), ordered_jets.at(1));
            H1.rebuildP4UsingRegressedPt(true,true);

            CompositeCandidate H2 = CompositeCandidate(ordered_jets.at(2), ordered_jets.at(3));
            H2.rebuildP4UsingRegressedPt(true,true);

// std::cout<<"culo8\n";
            //Do a random swap to be sure that the m1 and m2 are simmetric
            bool swapped = (int(H1.P4().Pt()*100.) % 2 == 1);

            if (!swapped)
            {
                ei.H1 = H1;
                ei.H2 = H2;
                ei.H1_b1 = ordered_jets.at(0);
                ei.H1_b2 = ordered_jets.at(1);
                ei.H2_b1 = ordered_jets.at(2);
                ei.H2_b2 = ordered_jets.at(3);
            }
            else
            {
                ei.H1 = H2;
                ei.H2 = H1;
                ei.H1_b1 = ordered_jets.at(2);
                ei.H1_b2 = ordered_jets.at(3);
                ei.H2_b1 = ordered_jets.at(0);
                ei.H2_b2 = ordered_jets.at(1);
            }

            H1b1_idx = ei.H1_b1->getIdx();
            H1b2_idx = ei.H1_b2->getIdx();
            H2b1_idx = ei.H2_b1->getIdx();
            H2b2_idx = ei.H2_b2->getIdx();

            ei.H1_bb_DeltaR = sqrt(pow(ei.H1_b1->P4Regressed().Eta() - ei.H1_b2->P4Regressed().Eta(),2) + pow(deltaPhi(ei.H1_b1->P4Regressed().Phi(), ei.H1_b2->P4Regressed().Phi()),2));
            ei.H2_bb_DeltaR = sqrt(pow(ei.H2_b1->P4Regressed().Eta() - ei.H2_b2->P4Regressed().Eta(),2) + pow(deltaPhi(ei.H2_b1->P4Regressed().Phi(), ei.H2_b2->P4Regressed().Phi()),2));


            ei.HH = CompositeCandidate(ei.H1.get(), ei.H2.get());

            float targetHiggsMass;
            if(strategy == "HighestCSVandClosestToMh")
            {
                targetHiggsMass = any_cast<float>(parameterList_->at("HiggsMassLMR"));
                if(any_cast<float>(parameterList_->at("LMRToMMRTransition"))>=0. && ei.HH->P4().M() > any_cast<float>(parameterList_->at("LMRToMMRTransition"))) targetHiggsMass = any_cast<float>(parameterList_->at("HiggsMassMMR"));

            }
            else
            {
                targetHiggsMass = any_cast<float>(parameterList_->at("HiggsMass"));
            }

            ei.HH_2DdeltaM = pow(ei.H1->P4().M() - targetHiggsMass,2) + pow(ei.H2->P4().M() - targetHiggsMass,2);

            bool applyKineamticFit=true;
            if(applyKineamticFit)
            {
                HH4b_kinFit::constrainHH_signalMeasurement(&ordered_jets.at(0).p4Regressed_, &ordered_jets.at(1).p4Regressed_, &ordered_jets.at(2).p4Regressed_, &ordered_jets.at(3).p4Regressed_);
                CompositeCandidate H1kf = CompositeCandidate(ordered_jets.at(0), ordered_jets.at(1));
                H1kf.rebuildP4UsingRegressedPt(true,true);

                CompositeCandidate H2kf = CompositeCandidate(ordered_jets.at(2), ordered_jets.at(3));
                H2kf.rebuildP4UsingRegressedPt(true,true);

                ei.HH_m_kinFit = CompositeCandidate(H1kf, H2kf).P4().M();
            }

            ei.Run = *(nat.run);
            ei.LumiSec = *(nat.luminosityBlock);
            ei.Event = *(nat.event);

// std::cout<<"culo9\n";

        }
        else //Variations
        {
// std::cout<<"culo10\n";

            int H1b1(-1), H1b2(-1), H2b1(-1), H2b2(-1);
            for(size_t iJet=0; iJet<jets.second.size(); ++iJet){
                // std::cout<<"Index = "<<iJet<<std::endl;
                int currentJetId = jets.second[iJet].getIdx();
                // std::cout<<"Id = "<<currentJetId<<std::endl;
                if(H1b1_idx == currentJetId) H1b1 = iJet;
                if(H1b2_idx == currentJetId) H1b2 = iJet;
                if(H2b1_idx == currentJetId) H2b1 = iJet;
                if(H2b2_idx == currentJetId) H2b2 = iJet;
            }

            // order H1, H2 by pT: pT(H1) > pT (H2)
            CompositeCandidate H1 = CompositeCandidate(jets.second[H1b1], jets.second[H1b2]);
            H1.rebuildP4UsingRegressedPt(true,true);

            CompositeCandidate H2 = CompositeCandidate(jets.second[H2b1], jets.second[H2b2]);
            H2.rebuildP4UsingRegressedPt(true,true);

            CompositeCandidate HH = CompositeCandidate(H1, H2);

            string strategy = any_cast<string>(parameterList_->at("bbbbChoice"));
            float targetHiggsMass;
            if(strategy == "HighestCSVandClosestToMh")
            {
                targetHiggsMass = any_cast<float>(parameterList_->at("HiggsMassLMR"));
                if(any_cast<float>(parameterList_->at("LMRToMMRTransition"))>=0. && ei.HH->P4().M() > any_cast<float>(parameterList_->at("LMRToMMRTransition"))) targetHiggsMass = any_cast<float>(parameterList_->at("HiggsMassMMR"));

            }
            else
            {
                targetHiggsMass = any_cast<float>(parameterList_->at("HiggsMass"));
            }

            float HH_2DdeltaM = pow(H1.P4().M() - targetHiggsMass,2) + pow(H2.P4().M() - targetHiggsMass,2);

            bool applyKineamticFit=true;
            float HH_m_kinFit=-1;
            if(applyKineamticFit)
            {
                HH4b_kinFit::constrainHH_signalMeasurement(&jets.second[H1b1].p4Regressed_, &jets.second[H1b2].p4Regressed_, &jets.second[H2b1].p4Regressed_, &jets.second[H2b2].p4Regressed_);
                CompositeCandidate H1kf = CompositeCandidate(jets.second[H1b1], jets.second[H1b2]);
                H1kf.rebuildP4UsingRegressedPt(true,true);

                CompositeCandidate H2kf = CompositeCandidate(jets.second[H2b1], jets.second[H2b2]);
                H2kf.rebuildP4UsingRegressedPt(true,true);

                HH_m_kinFit = CompositeCandidate(H1kf, H2kf).P4().M();
            }

            fillJetEnergyVariationBranch(ot, "H1_b1_pt", jets.first, jets.second[H1b1].P4().Pt());
            fillJetEnergyVariationBranch(ot, "H1_b2_pt", jets.first, jets.second[H1b2].P4().Pt());
            fillJetEnergyVariationBranch(ot, "H2_b1_pt", jets.first, jets.second[H2b1].P4().Pt());
            fillJetEnergyVariationBranch(ot, "H2_b2_pt", jets.first, jets.second[H2b2].P4().Pt());
            fillJetEnergyVariationBranch(ot, "H1_b1_ptRegressed", jets.first, jets.second[H1b1].P4Regressed().Pt());
            fillJetEnergyVariationBranch(ot, "H1_b2_ptRegressed", jets.first, jets.second[H1b2].P4Regressed().Pt());
            fillJetEnergyVariationBranch(ot, "H2_b1_ptRegressed", jets.first, jets.second[H2b1].P4Regressed().Pt());
            fillJetEnergyVariationBranch(ot, "H2_b2_ptRegressed", jets.first, jets.second[H2b2].P4Regressed().Pt());
            fillJetEnergyVariationBranch(ot, "H1_m", jets.first, H1.P4().M());
            fillJetEnergyVariationBranch(ot, "H2_m", jets.first, H2.P4().M());
            fillJetEnergyVariationBranch(ot, "HH_m", jets.first, HH.P4().M());
            fillJetEnergyVariationBranch(ot, "HH_m_kinFit", jets.first, HH_m_kinFit);
            fillJetEnergyVariationBranch(ot, "HH_2DdeltaM", jets.first, HH_2DdeltaM);

        }
    }


    return true;
}


//functions fo apply preselection cuts:
void OfflineProducerHelper::bJets_PreselectionCut(std::vector<Jet> &jets)
{

    float minimumDeepCSVaccepted            = any_cast<float>(parameterList_->at("MinDeepCSV"          ));
    float maximumPtAccepted                 = any_cast<float>(parameterList_->at("MinPt"               ));
    float maximumAbsEtaCSVaccepted          = any_cast<float>(parameterList_->at("MaxAbsEta"           ));
    if(any_cast<bool>(parameterList_->at("UseAntiTagOnOneBjet")))
    {
        minimumDeepCSVaccepted = -1;
    }

    if(minimumDeepCSVaccepted<=0. && maximumPtAccepted<=0. && maximumAbsEtaCSVaccepted<=0.) return;

    auto it = jets.begin();
    while (it != jets.end()){
        if(minimumDeepCSVaccepted>=0.){
            if(get_property((*it),Jet_btagDeepB)<minimumDeepCSVaccepted){
                it=jets.erase(it);
                continue;
            }
        }
        if(maximumPtAccepted>=0.){
            if(it->P4().Pt()<maximumPtAccepted){
                it=jets.erase(it);
                continue;
            }
        }
        if(maximumAbsEtaCSVaccepted>=0.){
            if(abs(it->P4().Eta())>maximumAbsEtaCSVaccepted){
                it=jets.erase(it);
                continue;
            }
        }
        ++it;
    }

    return;

}


// one pair is closest to the Higgs mass, the other follows
std::vector<Jet> OfflineProducerHelper::bbbb_jets_idxs_OneClosestToMh(const std::vector<Jet> *presel_jets)
{
    float targetHiggsMass = any_cast<float>(parameterList_->at("HiggsMass"));
    std::vector<pair <float, pair<int,int>>> mHs_and_jetIdx; // each entry is the mH and the two idxs of the pair

    for (uint i = 0; i < presel_jets->size(); ++i)
        for (uint j = i+1; j < presel_jets->size(); ++j)
        {
            TLorentzVector p4sum = (presel_jets->at(i).P4Regressed() + presel_jets->at(j).P4Regressed());
            float dmh = fabs(p4sum.Mag() - targetHiggsMass);
            mHs_and_jetIdx.emplace_back(make_pair(dmh, make_pair(i,j)));
        }

    // sort to get the pair closest to mH
    stable_sort (mHs_and_jetIdx.begin(), mHs_and_jetIdx.end());

    std::vector<Jet> outputJets = *presel_jets;
    int ij0 = mHs_and_jetIdx.begin()->second.first;
    int ij1 = mHs_and_jetIdx.begin()->second.second;

    // get the other two jets. The following creates a vector with idxs 0/1/2/3, and then removes ij1 and ij2
    std::vector<int> vres;
    for (uint i = 0; i < presel_jets->size(); ++i) vres.emplace_back(i);
    vres.erase(std::remove(vres.begin(), vres.end(), ij0), vres.end());
    vres.erase(std::remove(vres.begin(), vres.end(), ij1), vres.end());

    int ij2 = vres.at(0);
    int ij3 = vres.at(1);

    outputJets.at(0) = presel_jets->at(ij0);
    outputJets.at(1) = presel_jets->at(ij1);
    outputJets.at(2) = presel_jets->at(ij2);
    outputJets.at(3) = presel_jets->at(ij3);

    return outputJets;
}

// minimize the distance from (targetHiggsMass, targetHiggsMass) in the 2D plane
std::vector<Jet> OfflineProducerHelper::bbbb_jets_idxs_BothClosestToMh(const std::vector<Jet> *presel_jets)
{

    float targetHiggsMass = any_cast<float>(parameterList_->at("HiggsMass"));
    std::vector<float> mHs;

    for (uint i = 0; i < presel_jets->size(); ++i)
        for (uint j = i+1; j < presel_jets->size(); ++j)
        {
            TLorentzVector p4sum = (presel_jets->at(i).P4() + presel_jets->at(j).P4());
            mHs.emplace_back(p4sum.Mag());
        }

    std::pair<float, float> m_12_34 = make_pair (mHs.at(0), mHs.at(5));
    std::pair<float, float> m_13_24 = make_pair (mHs.at(1), mHs.at(4));
    std::pair<float, float> m_14_23 = make_pair (mHs.at(2), mHs.at(3));

    float r12_34 = sqrt (pow(m_12_34.first - targetHiggsMass, 2) + pow(m_12_34.second - targetHiggsMass, 2) );
    float r13_24 = sqrt (pow(m_13_24.first - targetHiggsMass, 2) + pow(m_13_24.second - targetHiggsMass, 2) );
    float r14_23 = sqrt (pow(m_14_23.first - targetHiggsMass, 2) + pow(m_14_23.second - targetHiggsMass, 2) );

    float the_min = std::min({r12_34, r13_24, r14_23});

    std::vector<Jet> outputJets = *presel_jets;

    if (the_min == r12_34){
        outputJets.at(0) = presel_jets->at(1 - 1);
        outputJets.at(1) = presel_jets->at(2 - 1);
        outputJets.at(2) = presel_jets->at(3 - 1);
        outputJets.at(3) = presel_jets->at(4 - 1);
    }

    else if (the_min == r13_24){
        outputJets.at(0) = presel_jets->at(1 - 1);
        outputJets.at(1) = presel_jets->at(3 - 1);
        outputJets.at(2) = presel_jets->at(2 - 1);
        outputJets.at(3) = presel_jets->at(4 - 1);
    }

    else if (the_min == r14_23){
        outputJets.at(0) = presel_jets->at(1 - 1);
        outputJets.at(1) = presel_jets->at(4 - 1);
        outputJets.at(2) = presel_jets->at(2 - 1);
        outputJets.at(3) = presel_jets->at(3 - 1);
    }

    else
        cout << "** [WARNING] : bbbb_jets_idxs_BothClosestToMh : something went wrong with finding the smallest radius" << endl;

    return outputJets;
}

// make the pairs that maximize their dR separation
std::vector<Jet> OfflineProducerHelper::bbbb_jets_idxs_MostBackToBack(const std::vector<Jet> *presel_jets)
{
    std::pair<CompositeCandidate,CompositeCandidate> H1H2 = get_two_best_jet_pairs (
        *presel_jets,
        std::function<float (std::pair<CompositeCandidate,CompositeCandidate>)> ([](pair<CompositeCandidate,CompositeCandidate> x)-> float {return x.first.P4().DeltaR(x.second.P4());}),
        false);

    std::vector<Jet> output_jets;
    output_jets.at(0) = static_cast<Jet&> (H1H2.first.getComponent1());
    output_jets.at(1) = static_cast<Jet&> (H1H2.first.getComponent2());
    output_jets.at(2) = static_cast<Jet&> (H1H2.second.getComponent1());
    output_jets.at(3) = static_cast<Jet&> (H1H2.second.getComponent2());

    return output_jets;
}


std::vector<Jet> OfflineProducerHelper::bbbb_jets_idxs_HighestCSVandClosestToMh(const std::vector<Jet> *jets){

    float targetHiggsMassLMR                = any_cast<float>(parameterList_->at("HiggsMassLMR"        ));
    float targetHiggsMassMMR                = any_cast<float>(parameterList_->at("HiggsMassMMR"        ));
    float LMRToMMRTransition                = any_cast<float>(parameterList_->at("LMRToMMRTransition" ));

    std::vector<Jet> output_jets;
    unsigned int numberOfJets = jets->size();
    if(numberOfJets  < 4)
    {
        throw std::runtime_error("OfflineProducerHelper::bbbb_jets_idxs_HighestCSVandClosestToMh -> numberOfJets  < 4 -> this should never happen to use jet energy variations, please check the number of Jets provived to this functions");
    }

    std::map< const std::array<unsigned int,4>, float> candidateMap;
    for(unsigned int h1b1it = 0; h1b1it< numberOfJets-1; ++h1b1it){
        for(unsigned int h1b2it = h1b1it+1; h1b2it< numberOfJets; ++h1b2it)
        {
            for(unsigned int h2b1it = h1b1it+1; h2b1it< numberOfJets-1; ++h2b1it)
            {
                if(h2b1it == h1b2it) continue;
                for(unsigned int h2b2it = h2b1it+1; h2b2it< numberOfJets; ++h2b2it)
                {
                    if(h2b2it == h1b2it) continue;
                    if(any_cast<bool>(parameterList_->at("UseAntiTagOnOneBjet")))
                    {
                        int numberOfBJets = 0;
                        float minimumDeepCSVaccepted = any_cast<float>(parameterList_->at("MinDeepCSV"));
                        if(get_property(jets->at(h1b1it),Jet_btagDeepB)>=minimumDeepCSVaccepted) ++numberOfBJets;
                        if(get_property(jets->at(h1b2it),Jet_btagDeepB)>=minimumDeepCSVaccepted) ++numberOfBJets;
                        if(get_property(jets->at(h2b1it),Jet_btagDeepB)>=minimumDeepCSVaccepted) ++numberOfBJets;
                        if(get_property(jets->at(h2b2it),Jet_btagDeepB)>=minimumDeepCSVaccepted) ++numberOfBJets;
                        if(numberOfBJets != 3) continue; //antiTag requires that 1 of the jets has deepCSV < MdeepCSV
                    }
                    float candidateMass = (jets->at(h1b1it).P4Regressed() + jets->at(h1b2it).P4Regressed() + jets->at(h2b1it).P4Regressed() + jets->at(h2b2it).P4Regressed()).M();
                    float targetHiggsMass = targetHiggsMassLMR;
                    if(LMRToMMRTransition>=0. && candidateMass > LMRToMMRTransition) targetHiggsMass = targetHiggsMassMMR; //use different range for mass
                    float squareDeltaMassH1 = pow((jets->at(h1b1it).P4Regressed() + jets->at(h1b2it).P4Regressed()).M()-targetHiggsMass,2);
                    float squareDeltaMassH2 = pow((jets->at(h2b1it).P4Regressed() + jets->at(h2b2it).P4Regressed()).M()-targetHiggsMass,2);
                    candidateMap.emplace((std::array<unsigned int,4>){h1b1it,h1b2it,h2b1it,h2b2it},squareDeltaMassH1+squareDeltaMassH2);
                }
            }
        }
    }

    if(candidateMap.size()==0)
    {
        if(!any_cast<bool>(parameterList_->at("UseAntiTagOnOneBjet")))
        {
            throw std::runtime_error("OfflineProducerHelper::bbbb_jets_idxs_HighestCSVandClosestToMh -> number of candidates=0 -> this should never happen to use jet energy variations");
        }
        return output_jets;
    }

    const std::pair< const std::array<unsigned int,4>, float> *itCandidateMap=NULL;
    //find candidate with both Higgs candidates cloasest to the true Higgs mass
    for(const auto & candidate : candidateMap){
        if(itCandidateMap==NULL) itCandidateMap = &candidate;
        else if(itCandidateMap->second > candidate.second) itCandidateMap = &candidate;
    }

    for(const auto & jetPosition : itCandidateMap->first){
        output_jets.emplace_back(jets->at(jetPosition));
    }

    return output_jets;
}

// Minimizes distance to the diagonal (a la ATLAS)
std::vector<Jet> OfflineProducerHelper::bbbb_jets_idxs_BothClosestToDiagonal(const std::vector<Jet> *presel_jets)
{

    float targetHiggsMass1 = any_cast<float>(parameterList_->at("LeadingHiggsMass"));
    float targetHiggsMass2 = any_cast<float>(parameterList_->at("SubleadingHiggsMass"));

    std::vector<TLorentzVector> p4s;

    for (uint i = 0; i < presel_jets->size(); ++i)
        for (uint j = i+1; j < presel_jets->size(); ++j)
        {
            TLorentzVector p4sum = (presel_jets->at(i).P4() + presel_jets->at(j).P4());
            p4s.emplace_back(p4sum);
        }

    float m1,m2,m3,m4,m5,m6;

    if(p4s.at(0).Pt() > p4s.at(5).Pt())
    {
      m1 = p4s.at(0).M(); m2 = p4s.at(5).M();
    }
    else
    {
      m1 = p4s.at(5).M(); m2 = p4s.at(0).M();        
    }

    if(p4s.at(1).Pt() > p4s.at(4).Pt())
    {
      m3 = p4s.at(1).M(); m4 = p4s.at(4).M();
    }
    else
    {
      m3 = p4s.at(4).M(); m4 = p4s.at(1).M();       
    }

    if(p4s.at(2).Pt() > p4s.at(3).Pt())
    {
      m5 = p4s.at(2).M(); m6 = p4s.at(3).M();
    }
    else
    {
      m5 = p4s.at(3).M(); m6 = p4s.at(2).M();       
    }

    std::pair<float, float> m_12_34 = make_pair(m1,m2);
    std::pair<float, float> m_13_24 = make_pair(m3,m4);
    std::pair<float, float> m_14_23 = make_pair(m5,m6);

    float d12_34 = fabs( m_12_34.first - ((targetHiggsMass1/targetHiggsMass2)*m_12_34.second) );
    float d13_24 = fabs( m_13_24.first - ((targetHiggsMass1/targetHiggsMass2)*m_13_24.second) );
    float d14_23 = fabs( m_14_23.first - ((targetHiggsMass1/targetHiggsMass2)*m_14_23.second) );

    float the_min = std::min({d12_34, d13_24, d14_23});

    std::vector<Jet> outputJets = *presel_jets;

    if (the_min == d12_34){
        outputJets.at(0) = presel_jets->at(1 - 1);
        outputJets.at(1) = presel_jets->at(2 - 1);
        outputJets.at(2) = presel_jets->at(3 - 1);
        outputJets.at(3) = presel_jets->at(4 - 1);
    }

    else if (the_min == d13_24){
        outputJets.at(0) = presel_jets->at(1 - 1);
        outputJets.at(1) = presel_jets->at(3 - 1);
        outputJets.at(2) = presel_jets->at(2 - 1);
        outputJets.at(3) = presel_jets->at(4 - 1);
    }

    else if (the_min == d14_23){
        outputJets.at(0) = presel_jets->at(1 - 1);
        outputJets.at(1) = presel_jets->at(4 - 1);
        outputJets.at(2) = presel_jets->at(2 - 1);
        outputJets.at(3) = presel_jets->at(3 - 1);
    }

    else
        cout << "** [WARNING] : bbbb_jets_idxs_BothClosestToDiagonal : something went wrong with finding the smallest Dhh" << endl;

    return outputJets;
}

// Minimizes mass difference (2016 Published Analysis)
std::vector<Jet> OfflineProducerHelper::bbbb_jets_idxs_MinMassDifference(const std::vector<Jet> *presel_jets)
{
    std::vector<float> mHs;

    for (uint i = 0; i < presel_jets->size(); ++i)
        for (uint j = i+1; j < presel_jets->size(); ++j)
        {
            TLorentzVector p4sum = (presel_jets->at(i).P4() + presel_jets->at(j).P4());
            mHs.emplace_back(p4sum.Mag());
        }

    std::pair<float, float> m_12_34 = make_pair (mHs.at(0), mHs.at(5));
    std::pair<float, float> m_13_24 = make_pair (mHs.at(1), mHs.at(4));
    std::pair<float, float> m_14_23 = make_pair (mHs.at(2), mHs.at(3));

    float d12_34 = fabs( m_12_34.first - m_12_34.second );
    float d13_24 = fabs( m_13_24.first - m_13_24.second );
    float d14_23 = fabs( m_14_23.first - m_14_23.second );

    float the_min = std::min({d12_34, d13_24, d14_23});

    std::vector<Jet> outputJets = *presel_jets;

    if (the_min == d12_34){
        outputJets.at(0) = presel_jets->at(1 - 1);
        outputJets.at(1) = presel_jets->at(2 - 1);
        outputJets.at(2) = presel_jets->at(3 - 1);
        outputJets.at(3) = presel_jets->at(4 - 1);
    }

    else if (the_min == d13_24){
        outputJets.at(0) = presel_jets->at(1 - 1);
        outputJets.at(1) = presel_jets->at(3 - 1);
        outputJets.at(2) = presel_jets->at(2 - 1);
        outputJets.at(3) = presel_jets->at(4 - 1);
    }

    else if (the_min == d14_23){
        outputJets.at(0) = presel_jets->at(1 - 1);
        outputJets.at(1) = presel_jets->at(4 - 1);
        outputJets.at(2) = presel_jets->at(2 - 1);
        outputJets.at(3) = presel_jets->at(3 - 1);
    }

    else
        cout << "** [WARNING] : bbbb_jets_idxs_MinMassDifference : something went wrong with finding the smallest Dhh" << endl;

    return outputJets;
}

////----------------------------------------TT-EMU SELECTION STARTS-------------------------------------------------////

bool OfflineProducerHelper::select_bbemu(NanoAODTree& nat, EventInfo& ei, OutputTree &ot)
{
    //Event variables
    ei.Run = *(nat.run);
    ei.LumiSec = *(nat.luminosityBlock);
    ei.Event = *(nat.event);
    ei.TT_nPV=*(nat.PV_npvs);
    ei.TT_nPVgood=*(nat.PV_npvsGood);
    //Get the jets in the event and fill the jet multiplicity
    std::vector<Jet> jets; int njets=0;
    jets.reserve(*(nat.nJet));
    for (uint ij = 0; ij < *(nat.nJet); ++ij){
        jets.emplace_back(Jet(ij, &nat));
        if(get_property(Jet(ij, &nat),Jet_pt) > 25 && abs( get_property(Jet(ij, &nat),Jet_eta) )<2.4){njets++;}
    }
    if(jets.size()<2) return false;
    //---Preselect two 'b-jets' (pt, eta, btagging)
    std::vector<Jet> ordered_jets = bbJets_PreselectionCut(jets);
    if(ordered_jets.size()<2) return false;
    order_by_pT(ordered_jets.at(0), ordered_jets.at(1));
    ei.TT_b1=ordered_jets.at(0);
    ei.TT_b2=ordered_jets.at(1);
    //---Calculate scaleFactors for preselected two b-jets
    if(parameterList_->find("BTagScaleFactorMethod") != parameterList_->end()){ //is it a MC event
        const string BJetcaleFactorsMethod = any_cast<string>(parameterList_->at("BTagScaleFactorMethod"));
        if(BJetcaleFactorsMethod == "FourBtag_ScaleFactor"){
            compute_scaleFactors_fourBtag_eventScaleFactor(ordered_jets,nat,ot);
        }
    }
    //--Preselect the electron & muon
    std::vector<std::tuple <Electron,Muon>> ordered_emu = emu_PreselectionCut(nat,ei,ot);
    if(ordered_emu.size()<1) return false;
    ei.TT_e = get<0>(ordered_emu[0]);
    ei.TT_mu= get<1>(ordered_emu[0]);
    ei.TT_m= (ei.TT_b1->P4() + ei.TT_b2->P4() + ei.TT_e->P4() + ei.TT_mu->P4()).M();
    ei.TT_nJet=njets;
    return true;
}

std::vector <Jet> OfflineProducerHelper::bbJets_PreselectionCut(std::vector<Jet> &jets)
{

    float minimumDeepCSVaccepted            = any_cast<float>(parameterList_->at("bMinDeepCSV"          ));
    float minimumPtAccepted                 = any_cast<float>(parameterList_->at("bMinPt"               ));
    float maximumAbsEtaCSVaccepted          = any_cast<float>(parameterList_->at("bMaxAbsEta"           ));

    std::vector <Jet> outputJets;
    auto it = jets.begin();
    while (it != jets.end()){
        if(minimumDeepCSVaccepted>=0.){
            if(get_property((*it),Jet_btagDeepB)<minimumDeepCSVaccepted){
                it=jets.erase(it);
                continue;
            }
        }
        if(minimumPtAccepted>=0.){
            if(it->P4().Pt()<minimumPtAccepted){
                it=jets.erase(it);
                continue;
            }
        }
        if(maximumAbsEtaCSVaccepted>=0.){
            if(abs(it->P4().Eta())>maximumAbsEtaCSVaccepted){
                it=jets.erase(it);
                continue;
            }
        }
        ++it;
    }
    if(jets.size()<2) return outputJets;
    // sort by deepCSV (highest to lowest)
    stable_sort(jets.begin(), jets.end(), [](const Jet & a, const Jet & b) -> bool
    {
        return ( get_property(a, Jet_btagDeepB) > get_property(b, Jet_btagDeepB) );
    });
    //Select only the two most b-tagged jets
    return outputJets = {{*(jets.begin()+0),*(jets.begin()+1)}};
}

std::vector<std::tuple <Electron,Muon>> OfflineProducerHelper::emu_PreselectionCut(NanoAODTree& nat, EventInfo& ei, OutputTree &ot)
{
    float eminimumPtAccepted                 = any_cast<float>(parameterList_->at("eMinPt"               ));
    float emaximumAbsEtaCSVaccepted          = any_cast<float>(parameterList_->at("eMaxAbsEta"           ));
    float muminimumPtAccepted                = any_cast<float>(parameterList_->at("muMinPt"               ));
    float mumaximumAbsEtaCSVaccepted         = any_cast<float>(parameterList_->at("muMaxAbsEta"           ));
    float emuminimunMass                     = any_cast<float>(parameterList_->at("emuMass"              ));
    float muMaxPfIso                         = any_cast<float>(parameterList_->at("muMaxPfIso"           ));
    float eMaxPfIso                          = any_cast<float>(parameterList_->at("eMaxPfIso"              ));

    //Fill muons and electrons
    std::vector<Electron> electrons;
    std::vector<Muon> muons;

    for (uint eIt = 0; eIt < *(nat.nElectron); ++eIt){
        Electron theElectron(eIt, &nat);
        if( get_property(theElectron,Electron_pt) > eminimumPtAccepted
            && abs(get_property(theElectron,Electron_eta)) < emaximumAbsEtaCSVaccepted
            && get_property(theElectron,Electron_pfRelIso03_all) < eMaxPfIso)
            electrons.emplace_back(theElectron);
    }
    for (uint eIt = 0; eIt < *(nat.nMuon); ++eIt){
        Muon theMuon(eIt, &nat);
        if( get_property(theMuon,Muon_pt) > muminimumPtAccepted
            && abs(get_property(theMuon,Muon_eta)) < mumaximumAbsEtaCSVaccepted
            && get_property(theMuon,Muon_pfRelIso04_all) < muMaxPfIso)
            muons.emplace_back(theMuon);
    }
    //Pick-up Opposite charge pair that maximizes the sum of PT
    std::vector<std::tuple <Electron,Muon>> emupair = OppositeChargeEMUPair(electrons,muons,emuminimunMass);
    return emupair;
}

std::vector<std::tuple <Electron,Muon>> OfflineProducerHelper::OppositeChargeEMUPair(std::vector<Electron> Electrons, std::vector<Muon> Muons, float mass)
{
    std::vector<std::tuple<Electron,Muon>> emupair;
    std::vector<std::tuple<float,int,int>> pairinfo;
    for (uint i = 0; i < Electrons.size(); ++i){
        for (uint j = 0; j < Muons.size(); ++j)
        {
            if(  get_property(Electrons.at(i),Electron_charge)*get_property(Muons.at(j),Muon_charge)< 0 && (Electrons.at(i).P4() + Muons.at(j).P4()).M() > mass)
            {
                float sumEMUPt = get_property(Electrons.at(i),Electron_pt) + get_property(Muons.at(j),Muon_pt);
                pairinfo.emplace_back(make_tuple(sumEMUPt,i,j));
            }
        }
    }
    //Pick up the one that maximizes the sumEMUPT
    if(pairinfo.size()<1) return emupair;
    std::sort (pairinfo.rbegin(), pairinfo.rend());
    emupair.emplace_back(make_pair( Electrons.at(get<1>(pairinfo[0])), Muons.at(get<2>(pairinfo[0]))));
    return emupair;
}
////----------------------------------------TTEMU SELECTION ENDS-------------------------------------------------////

////////////-----FUNCTIONS FOR NON-RESONANT ANALYSIS - START
void OfflineProducerHelper::CalculateBtagScaleFactor(const std::vector<Jet> presel_bjets,NanoAODTree& nat,OutputTree &ot){
    if(parameterList_->find("BTagScaleFactorMethod") != parameterList_->end()){ //is it a MC event
        const string BJetcaleFactorsMethod = any_cast<string>(parameterList_->at("BTagScaleFactorMethod"));
        if(BJetcaleFactorsMethod == "FourBtag_ScaleFactor"){
            compute_scaleFactors_fourBtag_eventScaleFactor(presel_bjets,nat,ot);
        }
    }
    return;
}


bool OfflineProducerHelper::select_bbbbjj_jets(NanoAODTree& nat, EventInfo& ei, OutputTree &ot)
{
    //Get jet collection
    std::vector<Jet> jets;
    jets.reserve(*(nat.nJet));
    for (uint ij = 0; ij < *(nat.nJet); ++ij){jets.emplace_back(Jet(ij, &nat));}
    //Preselect four most b-tagged jets (pt, eta, btagging) & 'two vbfjets'(pt,eta,deltaEta). Note: BTagSF is calculated for events with 3 or 4 b-tags
    std::vector<Jet> presel_jets = bjJets_PreselectionCut(nat,ei,ot,jets); //TO BE RETURNED bjJets_PreselectionCut/bjJets_2016_PreselectionCut
    if(presel_jets.size()<4) return false;
    //Pick up the four preselected b-jets, pair them, ordered them into a new vector
    std::vector<Jet> presel_bjets={{*(presel_jets.begin()+0),*(presel_jets.begin()+1),*(presel_jets.begin()+2),*(presel_jets.begin()+3)}};
    std::vector<Jet> ordered_jets;
    if(any_cast<string>(parameterList_->at("bbbbChoice"))=="BothClosestToMh"){
        ordered_jets = bbbb_jets_idxs_BothClosestToMh(&presel_bjets);
    }
    else if(any_cast<string>(parameterList_->at("bbbbChoice"))=="MinMassDifference"){
        ordered_jets = bbbb_jets_idxs_MinMassDifference(&presel_bjets);
    }
    else{
        ordered_jets = bbbb_jets_idxs_BothClosestToDiagonal(&presel_bjets);
    }
    if(presel_jets.size()==6){ordered_jets.push_back(presel_jets.at(4)); ordered_jets.push_back(presel_jets.at(5));}
    //Add objects (H1,H2,HH) and variables (dEta,dPhi) that are common for both GGF and VBF Categories
    AddInclusiveCategoryVariables(nat,ei,ordered_jets);
    //If the VBF jet pair exists, then create the VBF category variables
    if(presel_jets.size()==6){AddVBFCategoryVariables(nat,ei,ordered_jets);}
    return true;
}


float OfflineProducerHelper::MindRToAJet(const GenPart quark,const std::vector<Jet> jets)
{
float dR=999999,dRmin=0;
for (uint x=0; x < jets.size(); x++ )
{
      dRmin = jets.at(x).P4().DeltaR(quark.P4());
      if(dRmin < dR){dR=dRmin;}
}

return dR;
}

float OfflineProducerHelper::MindRToAGenJet(const GenPart quark,const std::vector<GenJet> jets)
{
float dR=999999,dRmin=0;
for (uint x=0; x < jets.size(); x++ )
{
      dRmin = jets.at(x).P4().DeltaR(quark.P4());
      if(dRmin < dR){dR=dRmin;}
}

return dR;
}

float OfflineProducerHelper::GenJetToPartonPt(const GenPart quark,const std::vector<GenJet> jets)
{
float dR=999999,dRmin=0,ptratio=0;
for (uint x=0; x < jets.size(); x++ )
{
      dRmin = jets.at(x).P4().DeltaR(quark.P4());
      if(dRmin < dR){dR=dRmin; ptratio = jets.at(x).P4().Pt() / quark.P4().Pt();}
}

return ptratio;
}



void OfflineProducerHelper::AddVBFGenMatchVariables(NanoAODTree& nat, EventInfo& ei)
{
std::vector<Jet> jets;
std::vector<int> quarkID;
jets.reserve(*(nat.nJet));
for (uint ij = 0; ij < *(nat.nJet); ++ij){
    jets.emplace_back(Jet(ij, &nat));
}
quarkID = AddGenMatchingInfo(nat,ei,jets);
ei.gen_H1_b1_mindRjet  = MindRToAJet(ei.gen_H1_b1.get() ,jets);
ei.gen_H1_b2_mindRjet  = MindRToAJet(ei.gen_H1_b2.get() ,jets);
ei.gen_H2_b1_mindRjet  = MindRToAJet(ei.gen_H2_b1.get() ,jets);
ei.gen_H2_b2_mindRjet  = MindRToAJet(ei.gen_H2_b2.get() ,jets);
ei.gen_q1_out_mindRjet = MindRToAJet(ei.gen_q1_out.get(),jets);
ei.gen_q2_out_mindRjet = MindRToAJet(ei.gen_q2_out.get(),jets);
ei.VBFGeneratedEtaSign = (ei.gen_q1_out.get().P4().Eta()*ei.gen_q2_out.get().P4().Eta() )/ abs(ei.gen_q1_out.get().P4().Eta()*ei.gen_q2_out.get().P4().Eta());
ei.VBFGeneratedDeltaEta= abs(  ei.gen_q1_out.get().P4().Eta()-ei.gen_q2_out.get().P4().Eta() );
//GenJet variables
std::vector<GenJet> genjets;
genjets.reserve(*(nat.nGenJet));
for (uint ij = 0; ij < *(nat.nGenJet); ++ij){
    genjets.emplace_back(GenJet(ij, &nat));
}
ei.gen_H1_b1_mindRgenjet  = MindRToAGenJet(ei.gen_H1_b1.get() ,genjets);
ei.gen_H1_b2_mindRgenjet  = MindRToAGenJet(ei.gen_H1_b2.get() ,genjets);
ei.gen_H2_b1_mindRgenjet  = MindRToAGenJet(ei.gen_H2_b1.get() ,genjets);
ei.gen_H2_b2_mindRgenjet  = MindRToAGenJet(ei.gen_H2_b2.get() ,genjets);
ei.gen_q1_out_mindRgenjet = MindRToAGenJet(ei.gen_q1_out.get(),genjets);
ei.gen_q2_out_mindRgenjet = MindRToAGenJet(ei.gen_q2_out.get(),genjets);
ei.gen_H1_b1_ptratio  = GenJetToPartonPt(ei.gen_H1_b1.get() ,genjets);
ei.gen_H1_b2_ptratio  = GenJetToPartonPt(ei.gen_H1_b2.get() ,genjets);
ei.gen_H2_b1_ptratio  = GenJetToPartonPt(ei.gen_H2_b1.get() ,genjets);
ei.gen_H2_b2_ptratio  = GenJetToPartonPt(ei.gen_H2_b2.get() ,genjets);
ei.gen_q1_out_ptratio = GenJetToPartonPt(ei.gen_q1_out.get(),genjets);
ei.gen_q2_out_ptratio = GenJetToPartonPt(ei.gen_q2_out.get(),genjets);
ei.gen_H1_b1_pz  = ei.gen_H1_b1.get().P4().Pz();
ei.gen_H1_b2_pz  = ei.gen_H1_b2.get().P4().Pz();
ei.gen_H2_b1_pz  = ei.gen_H2_b1.get().P4().Pz();
ei.gen_H2_b2_pz  = ei.gen_H2_b2.get().P4().Pz();
ei.gen_q1_out_pz = ei.gen_q1_out.get().P4().Pz();
ei.gen_q2_out_pz = ei.gen_q2_out.get().P4().Pz();
//Fill quarkID flag
ei.H1_b1_quarkID = quarkID.at(ei.H1_b1.get().getIdx());
ei.H1_b2_quarkID = quarkID.at(ei.H1_b2.get().getIdx());
ei.H2_b1_quarkID = quarkID.at(ei.H2_b1.get().getIdx());
ei.H2_b2_quarkID = quarkID.at(ei.H2_b2.get().getIdx());
ei.JJ_j1_quarkID = quarkID.at(ei.JJ_j1.get().getIdx());
ei.JJ_j2_quarkID = quarkID.at(ei.JJ_j2.get().getIdx());
//Fill genJetIdx flag
ei.H1_b1_genJetIdx = get_property(ei.H1_b1.get(),Jet_genJetIdx);
ei.H1_b2_genJetIdx = get_property(ei.H1_b2.get(),Jet_genJetIdx);
ei.H2_b1_genJetIdx = get_property(ei.H2_b1.get(),Jet_genJetIdx);
ei.H2_b2_genJetIdx = get_property(ei.H2_b2.get(),Jet_genJetIdx);
ei.JJ_j1_genJetIdx = get_property(ei.JJ_j1.get(),Jet_genJetIdx);
ei.JJ_j2_genJetIdx = get_property(ei.JJ_j2.get(),Jet_genJetIdx);
//Fill partonFlavour flag
ei.H1_b1_partonFlavour = get_property(ei.H1_b1.get(),Jet_partonFlavour);
ei.H1_b2_partonFlavour = get_property(ei.H1_b2.get(),Jet_partonFlavour);
ei.H2_b1_partonFlavour = get_property(ei.H2_b1.get(),Jet_partonFlavour);
ei.H2_b2_partonFlavour = get_property(ei.H2_b2.get(),Jet_partonFlavour);
ei.JJ_j1_partonFlavour = get_property(ei.JJ_j1.get(),Jet_partonFlavour);
ei.JJ_j2_partonFlavour = get_property(ei.JJ_j2.get(),Jet_partonFlavour);
//Fill hadronFlavour flag
ei.H1_b1_hadronFlavour = get_property(ei.H1_b1.get(),Jet_hadronFlavour);
ei.H1_b2_hadronFlavour = get_property(ei.H1_b2.get(),Jet_hadronFlavour);
ei.H2_b1_hadronFlavour = get_property(ei.H2_b1.get(),Jet_hadronFlavour);
ei.H2_b2_hadronFlavour = get_property(ei.H2_b2.get(),Jet_hadronFlavour);
ei.JJ_j1_hadronFlavour = get_property(ei.JJ_j1.get(),Jet_hadronFlavour);
ei.JJ_j2_hadronFlavour = get_property(ei.JJ_j2.get(),Jet_hadronFlavour);
//Fill branches with selected b-jets ordered by pt and also their quarkID flag
std::vector<Jet> presel_bjets={{*(ei.H1_b1),*(ei.H1_b2),*(ei.H2_b1),*(ei.H2_b2)}};
stable_sort(presel_bjets.begin(), presel_bjets.end(), [](const Jet & a, const Jet & b) -> bool
{return ( get_property(a, Jet_pt) > get_property(b, Jet_pt) );});
ei.HH_b1_quarkID = quarkID.at(presel_bjets.at(0).getIdx());
ei.HH_b2_quarkID = quarkID.at(presel_bjets.at(1).getIdx());
ei.HH_b3_quarkID = quarkID.at(presel_bjets.at(2).getIdx());
ei.HH_b4_quarkID = quarkID.at(presel_bjets.at(3).getIdx());
ei.HH_b1_genJetIdx = get_property(ei.HH_b1.get(),Jet_genJetIdx);
ei.HH_b2_genJetIdx = get_property(ei.HH_b2.get(),Jet_genJetIdx);
ei.HH_b3_genJetIdx = get_property(ei.HH_b3.get(),Jet_genJetIdx);
ei.HH_b4_genJetIdx = get_property(ei.HH_b4.get(),Jet_genJetIdx);
ei.HH_b1_partonFlavour = get_property(ei.HH_b1.get(),Jet_partonFlavour);
ei.HH_b2_partonFlavour = get_property(ei.HH_b2.get(),Jet_partonFlavour);
ei.HH_b3_partonFlavour = get_property(ei.HH_b3.get(),Jet_partonFlavour);
ei.HH_b4_partonFlavour = get_property(ei.HH_b4.get(),Jet_partonFlavour);
ei.HH_b1_hadronFlavour = get_property(ei.HH_b1.get(),Jet_hadronFlavour);
ei.HH_b2_hadronFlavour = get_property(ei.HH_b2.get(),Jet_hadronFlavour);
ei.HH_b3_hadronFlavour = get_property(ei.HH_b3.get(),Jet_hadronFlavour);
ei.HH_b4_hadronFlavour = get_property(ei.HH_b4.get(),Jet_hadronFlavour);
//Fill number of jets with a match at gen level in the event
int Nmatches=0,Nq1=0,Nq2=0,Nh1b1=0,Nh1b2=0,Nh2b1=0,Nh2b2=0;
for (uint x=0;x < quarkID.size();x++ )
{
    if(quarkID.at(x)>0) Nmatches++;
    if(quarkID.at(x)==1) Nh1b1++;
    if(quarkID.at(x)==2) Nh1b2++;
    if(quarkID.at(x)==3) Nh2b1++;
    if(quarkID.at(x)==4) Nh2b2++;
    if(quarkID.at(x)==5) Nq1++;
    if(quarkID.at(x)==6) Nq2++;
}
ei.NPartonsMatchedToJets = Nmatches;
ei.gen_H1_b1_matched  = Nh1b1;
ei.gen_H1_b2_matched  = Nh1b2;
ei.gen_H2_b1_matched  = Nh2b1;
ei.gen_H2_b2_matched  = Nh2b2;
ei.gen_q1_out_matched = Nq1;
ei.gen_q2_out_matched = Nq2;
//Fill the flavor of the quark
ei.gen_H1_b1_pdgId  = get_property(ei.gen_H1_b1.get(), GenPart_pdgId);
ei.gen_H1_b2_pdgId  = get_property(ei.gen_H1_b2.get(), GenPart_pdgId);
ei.gen_H2_b1_pdgId  = get_property(ei.gen_H2_b1.get(), GenPart_pdgId);
ei.gen_H2_b2_pdgId  = get_property(ei.gen_H2_b2.get(), GenPart_pdgId);
ei.gen_q1_out_pdgId = get_property(ei.gen_q1_out.get(), GenPart_pdgId);
ei.gen_q2_out_pdgId = get_property(ei.gen_q2_out.get(), GenPart_pdgId);
//Fill gen-level information
int h1_flag,h1_wpflag,h1_ukflag,h2_flag,h2_wpflag,h2_ukflag,hh_flag,hh_1wflag,hh_2wflag,hh_3wflag,hh_4wflag,jj_flag,jj_1wflag,jj_2wflag, hhjj_flag;
int h1b1 = quarkID.at(ei.H1_b1.get().getIdx());
int h1b2 = quarkID.at(ei.H1_b2.get().getIdx());
int h2b1 = quarkID.at(ei.H2_b1.get().getIdx());
int h2b2 = quarkID.at(ei.H2_b2.get().getIdx());
int jjj1 = quarkID.at(ei.JJ_j1.get().getIdx());
int jjj2 = quarkID.at(ei.JJ_j2.get().getIdx());
int h1bs=0; int h2bs=0; int js=0;
if(h1b1>0 && h1b1<5){h1bs++;} if(h2b1>0 && h2b1<5){h2bs++;}
if(h1b2>0 && h1b2<5){h1bs++;} if(h2b2>0 && h2b2<5){h2bs++;}
if(jjj1>4){js++;} if(jjj2>4){js++;}
//Check if H1 originates from a Higgs boson
if( (h1b1==1 && h1b2==2) || (h1b1==2 && h1b2==1) || (h1b1==3 && h1b2==4) || (h1b1==4 && h1b2==3)){h1_flag=1;h1_ukflag=0;h1_wpflag=0;}
else
{
    h1_flag=0;
    if(h1bs==2){h1_wpflag=1;h1_ukflag=0;}
    else{h1_wpflag=0;h1_ukflag=1;}
}
//Check if H2 originates from a Higgs boson
if( (h2b1==1 && h2b2==2) || (h2b1==2 && h2b2==1) || (h2b1==3 && h2b2==4) || (h2b1==4 && h2b2==3)){h2_flag=1;h2_ukflag=0;h2_wpflag=0;}
else
{
    h2_flag=0;
    if(h2bs==2){h2_wpflag=1;h2_ukflag=0;}
    else{h2_wpflag=0;h2_ukflag=1;}
}
//Check if VBF-jets originate from VBF-partons
if(js==2){jj_flag=1; jj_1wflag=0;jj_2wflag=0;}
else
{
    jj_flag=0;
    if(js==1){jj_1wflag=1;jj_2wflag=0;}
    if(js==0){jj_1wflag=0;jj_2wflag=1;}
}
//Check if HH originate from both Higgs bosons
//if( h1_flag==1 && h2_flag==1){hh_flag=1;}
if( (h1bs + h2bs) == 4){hh_flag=1;hh_1wflag=0;hh_2wflag=0;hh_3wflag=0;hh_4wflag=0;}
else
{
    hh_flag=0;
    if((h1bs + h2bs) == 3){hh_1wflag=1;hh_2wflag=0;hh_3wflag=0;hh_4wflag=0;}
    if((h1bs + h2bs) == 2){hh_1wflag=0;hh_2wflag=1;hh_3wflag=0;hh_4wflag=0;}
    if((h1bs + h2bs) == 1){hh_1wflag=0;hh_2wflag=0;hh_3wflag=1;hh_4wflag=0;}
    if((h1bs + h2bs) == 0){hh_1wflag=0;hh_2wflag=0;hh_3wflag=0;hh_4wflag=1;}
}
//Check if HH and JJ originate from the partons
if(hh_flag==1 && jj_flag==1){hhjj_flag=1;}
else{hhjj_flag=0;}

//Fill how good (quality) H1,H2, JJ ad HH are selected
ei.H1_quality =  h1_flag;
ei.H2_quality =  h2_flag;
ei.H1_wpflag  =  h1_wpflag;
ei.H2_wpflag  =  h2_wpflag;
ei.H1_ukflag  =  h1_ukflag;
ei.H2_ukflag  =  h2_ukflag;
ei.HH_quality =  hh_flag;
ei.HH_1wflag  =  hh_1wflag;
ei.HH_2wflag  =  hh_2wflag;
ei.HH_3wflag  =  hh_3wflag;
ei.HH_4wflag  =  hh_4wflag;
ei.JJ_quality =  jj_flag;
ei.JJ_1wflag  =  jj_1wflag;
ei.JJ_2wflag  =  jj_2wflag;
ei.HHJJ_quality = hhjj_flag;

return;
}

void OfflineProducerHelper::AddInclusiveCategoryVariables(NanoAODTree& nat, EventInfo& ei,std::vector<Jet> ordered_jets){
       //Event variables
       ei.Run = *(nat.run);
       ei.LumiSec = *(nat.luminosityBlock);
       ei.Event = *(nat.event);
       ei.nPVgood=*(nat.PV_npvsGood);
       if(ordered_jets.size()==6){ ei.VBFEvent = 1;}
       else{ei.VBFEvent = 0;}
       // order H1, H2 by pT: pT(H1) > pT (H2)
       order_by_pT(ordered_jets.at(0), ordered_jets.at(1));
       order_by_pT(ordered_jets.at(2), ordered_jets.at(3));
       CompositeCandidate H1 = CompositeCandidate(ordered_jets.at(0), ordered_jets.at(1) );
       CompositeCandidate H2 = CompositeCandidate(ordered_jets.at(2), ordered_jets.at(3) );
       ei.H1=H1;
       ei.H2=H2;
       bool swapped0 = order_by_pT(ei.H1.get(), ei.H2.get());
       if (!swapped0)
       {
           ei.H1_b1 = ordered_jets.at(0);ei.H1_b2 = ordered_jets.at(1);
           ei.H2_b1 = ordered_jets.at(2);ei.H2_b2 = ordered_jets.at(3);
       }
       else
       {
           ei.H1_b1 = ordered_jets.at(2);ei.H1_b2 = ordered_jets.at(3);
           ei.H2_b1 = ordered_jets.at(0);ei.H2_b2 = ordered_jets.at(1);
       }
       //cout<<" H1 pt "<<ei.H1.get().P4().Pt()<<" H2 pt "<<ei.H2.get().P4().Pt()<<endl;
       //HH variables
       ei.HH = CompositeCandidate(ei.H1.get(), ei.H2.get());
       //Fill branches with selected b-jets ordered by pt
       std::vector<Jet> presel_bjets, presel_bjets_btags;
       uint m=0; while(m<4){presel_bjets.push_back(ordered_jets.at(m));presel_bjets_btags.push_back(ordered_jets.at(m)); m++;}
       stable_sort(presel_bjets.begin(), presel_bjets.end(), [](const Jet & a, const Jet & b) -> bool
       {return ( get_property(a, Jet_pt) > get_property(b, Jet_pt) );});
       ei.HH_b1 = presel_bjets.at(0);
       ei.HH_b2 = presel_bjets.at(1);
       ei.HH_b3 = presel_bjets.at(2);
       ei.HH_b4 = presel_bjets.at(3);
       stable_sort(presel_bjets_btags.begin(), presel_bjets_btags.end(), [](const Jet & a, const Jet & b) -> bool
       {return ( get_property(a, Jet_btagCMVA) > get_property(b, Jet_btagCMVA) );});
       ei.HH_btag_cmva_b1 = presel_bjets_btags.at(0);
       ei.HH_btag_cmva_b2 = presel_bjets_btags.at(1);
       ei.HH_btag_cmva_b3 = presel_bjets_btags.at(2);
       ei.HH_btag_cmva_b4 = presel_bjets_btags.at(3);
       stable_sort(presel_bjets_btags.begin(), presel_bjets_btags.end(), [](const Jet & a, const Jet & b) -> bool
       {return ( get_property(a, Jet_btagDeepB) > get_property(b, Jet_btagDeepB) );});
       ei.HH_btag_b1 = presel_bjets_btags.at(0);
       ei.HH_btag_b2 = presel_bjets_btags.at(1);
       ei.HH_btag_b3 = presel_bjets_btags.at(2);
       ei.HH_btag_b4 = presel_bjets_btags.at(3);
       //Mass cut variables with a random swap to be sure that the m1 and m2 are simmetric when selecting the signal region
       bool swapped = (int(ei.H1->P4().Pt()*100.) % 2 == 1);
       if (!swapped){ei.H1rand = ei.H1.get();ei.H2rand = ei.H2.get();}
       else{ei.H1rand = ei.H2.get();ei.H2rand = ei.H1.get();}
       //dR,dPhi and dEta between pair b's
       ei.H1_bb_deltaR = ei.H1_b1->P4().DeltaR(ei.H1_b2->P4());
       ei.H1_bb_deltaPhi = ei.H1_b1->P4().DeltaPhi(ei.H1_b2->P4());
       ei.H1_bb_deltaEta = abs(ei.H1_b1->P4().Eta() - ei.H1_b2->P4().Eta() );
       ei.H2_bb_deltaR = ei.H2_b1->P4().DeltaR(ei.H2_b2->P4());
       ei.H2_bb_deltaPhi = ei.H2_b1->P4().DeltaPhi(ei.H2_b2->P4());
       ei.H2_bb_deltaEta = abs(ei.H2_b1->P4().Eta() - ei.H2_b2->P4().Eta() );
       //Special variables:deltaR between jets
       ei.b1b2_deltaR = ei.HH_b1->P4().DeltaR(ei.HH_b2->P4());
       ei.b1b3_deltaR = ei.HH_b1->P4().DeltaR(ei.HH_b3->P4());
       ei.b1b4_deltaR = ei.HH_b1->P4().DeltaR(ei.HH_b4->P4());
       ei.b2b3_deltaR = ei.HH_b2->P4().DeltaR(ei.HH_b3->P4());
       ei.b2b4_deltaR = ei.HH_b2->P4().DeltaR(ei.HH_b4->P4());
       ei.b3b4_deltaR = ei.HH_b3->P4().DeltaR(ei.HH_b4->P4());
       ei.b1b2_deltaPhi = ei.HH_b1->P4().DeltaPhi(ei.HH_b2->P4());
       ei.b1b3_deltaPhi = ei.HH_b1->P4().DeltaPhi(ei.HH_b3->P4());
       ei.b1b4_deltaPhi = ei.HH_b1->P4().DeltaPhi(ei.HH_b4->P4());
       ei.b2b3_deltaPhi = ei.HH_b2->P4().DeltaPhi(ei.HH_b3->P4());
       ei.b2b4_deltaPhi = ei.HH_b2->P4().DeltaPhi(ei.HH_b4->P4());
       ei.b3b4_deltaPhi = ei.HH_b3->P4().DeltaPhi(ei.HH_b4->P4());
       ei.b1b2_deltaEta = abs(ei.HH_b1->P4().Eta() - ei.HH_b2->P4().Eta() );
       ei.b1b3_deltaEta = abs(ei.HH_b1->P4().Eta() - ei.HH_b3->P4().Eta() );
       ei.b1b4_deltaEta = abs(ei.HH_b1->P4().Eta() - ei.HH_b4->P4().Eta() );
       ei.b2b3_deltaEta = abs(ei.HH_b2->P4().Eta() - ei.HH_b3->P4().Eta() );
       ei.b2b4_deltaEta = abs(ei.HH_b2->P4().Eta() - ei.HH_b4->P4().Eta() );
       ei.b3b4_deltaEta = abs(ei.HH_b3->P4().Eta() - ei.HH_b4->P4().Eta() );
       //Special variables between Higgses
       ei.h1h2_deltaR = ei.H1->P4().DeltaR(ei.H2->P4());
       ei.h1h2_deltaPhi = ei.H1->P4().DeltaPhi(ei.H2->P4());
       ei.h1h2_deltaEta = abs( ei.H1->P4().Eta() - ei.H2->P4().Eta());
       //Number of jets
       int njall=0,njbarrel=0,njendcap=0,njhf=0;
       for (uint ij = 0; ij < *(nat.nJet); ++ij)
       {
        if ( abs(get_property(Jet(ij, &nat), Jet_pt)) < 25) continue;
        if ( abs(get_property(Jet(ij, &nat), Jet_eta)) < 5){njall++;}
        if ( abs(get_property(Jet(ij, &nat), Jet_eta)) < 1.2){njbarrel++;}
        if ( abs(get_property(Jet(ij, &nat), Jet_eta)) >= 1.2 && abs(get_property(Jet(ij, &nat), Jet_eta)) < 2.4){njendcap++;}
        if ( abs(get_property(Jet(ij, &nat), Jet_eta)) >= 2.4 && abs(get_property(Jet(ij, &nat), Jet_eta)) < 5){njhf++;}
       }
       ei.nJet = njall;
       ei.nJetbarrel = njbarrel;
       ei.nJetendcap = njendcap;
       ei.nJethf = njhf;
       //Raw pt
       ei.HH_b1_rawpt = get_property(presel_bjets.at(0), Jet_pt) * (1-get_property(presel_bjets.at(0), Jet_rawFactor));
       ei.HH_b2_rawpt = get_property(presel_bjets.at(1), Jet_pt) * (1-get_property(presel_bjets.at(1), Jet_rawFactor));
       ei.HH_b3_rawpt = get_property(presel_bjets.at(2), Jet_pt) * (1-get_property(presel_bjets.at(2), Jet_rawFactor));
       ei.HH_b4_rawpt = get_property(presel_bjets.at(3), Jet_pt) * (1-get_property(presel_bjets.at(3), Jet_rawFactor));
       ei.H1_b1_rawpt = get_property( ei.H1_b1.get(), Jet_pt) * (1 - get_property(ei.H1_b1.get(), Jet_rawFactor) );
       ei.H1_b2_rawpt = get_property( ei.H1_b2.get(), Jet_pt) * (1 - get_property(ei.H1_b2.get(), Jet_rawFactor) );
       ei.H2_b1_rawpt = get_property( ei.H2_b1.get(), Jet_pt) * (1 - get_property(ei.H2_b1.get(), Jet_rawFactor) );
       ei.H2_b2_rawpt = get_property( ei.H2_b2.get(), Jet_pt) * (1 - get_property(ei.H2_b2.get(), Jet_rawFactor) );
       //COSTHETA VARIABLES in CM FRAME
       TLorentzVector Sum = ei.HH_b1->P4() + ei.HH_b2->P4() + ei.HH_b3->P4() + ei.HH_b4->P4();
       auto boost_vctr = Sum.BoostVector();
       TLorentzVector H1_cm = ei.H1->P4(); H1_cm.Boost(-boost_vctr);
       TLorentzVector H2_cm = ei.H2->P4(); H2_cm.Boost(-boost_vctr);
       TLorentzVector HH_b1_cm = ei.HH_b1->P4(); HH_b1_cm.Boost(-boost_vctr);
       TLorentzVector HH_b2_cm = ei.HH_b2->P4(); HH_b2_cm.Boost(-boost_vctr);
       TLorentzVector HH_b3_cm = ei.HH_b3->P4(); HH_b3_cm.Boost(-boost_vctr);
       TLorentzVector HH_b4_cm = ei.HH_b4->P4(); HH_b4_cm.Boost(-boost_vctr);
       ei.costh_H1_cm = H1_cm.CosTheta();
       ei.costh_H2_cm = H2_cm.CosTheta();
       ei.costh_HH_b1_cm = HH_b1_cm.CosTheta();
       ei.costh_HH_b2_cm = HH_b2_cm.CosTheta();
       ei.costh_HH_b3_cm = HH_b3_cm.CosTheta();
       ei.costh_HH_b4_cm = HH_b4_cm.CosTheta();
       
       //GGF-QCDKiller
       ei.BDT3 = GetBDT3Score(ei);

       // string strategy = any_cast<string>(parameterList_->at("bbbbChoice"));
       // if(strategy=="BothClosestToMh")
       // {
       // string weights= "weights/TMVAClassification_BDT_GGFQCDKiller_case1.weights.xml";
       // ei.BDT3 = GetBDT3Score(ei,weights);
       // }
       // else if(strategy=="BothClosestToDiagonal")
       // {
       // string weights= "weights/TMVAClassification_BDT_GGFQCDKiller_case2.weights.xml";
       // ei.BDT3 = GetBDT3Score(ei,weights);
       // }
       // else if(strategy=="MinMassDifference")
       // {
       // string weights= "weights/TMVAClassification_BDT_GGFQCDKiller_case2.weights.xml";
       // ei.BDT3 = GetBDT3Score(ei,weights);
       // }
       // else
       // {
       // ei.BDT3 = 0;
       // }
       return;
}

void OfflineProducerHelper::AddVBFCategoryVariables(NanoAODTree& nat, EventInfo& ei,std::vector<Jet> presel_jets)
{
       ei.JJ_j1 = presel_jets.at(4);
       ei.JJ_j2 = presel_jets.at(5);
       ei.JJ = CompositeCandidate(ei.JJ_j1.get(), ei.JJ_j2.get());
       //Special variables:deltaR between jets
       ei.b1j1_deltaR = ei.HH_b1->P4().DeltaR(ei.JJ_j1->P4());
       ei.b1j2_deltaR = ei.HH_b1->P4().DeltaR(ei.JJ_j2->P4());
       ei.b2j1_deltaR = ei.HH_b2->P4().DeltaR(ei.JJ_j1->P4());
       ei.b2j2_deltaR = ei.HH_b2->P4().DeltaR(ei.JJ_j2->P4());
       ei.b3j1_deltaR = ei.HH_b3->P4().DeltaR(ei.JJ_j1->P4());
       ei.b3j2_deltaR = ei.HH_b3->P4().DeltaR(ei.JJ_j2->P4());
       ei.b4j1_deltaR = ei.HH_b4->P4().DeltaR(ei.JJ_j1->P4());
       ei.b4j2_deltaR = ei.HH_b4->P4().DeltaR(ei.JJ_j2->P4());
       ei.j1j2_deltaR = ei.JJ_j1->P4().DeltaR(ei.JJ_j2->P4());
       //Special variables:deltaPhi between jets
       ei.b1j1_deltaPhi = ei.HH_b1->P4().DeltaPhi(ei.JJ_j1->P4());
       ei.b1j2_deltaPhi = ei.HH_b1->P4().DeltaPhi(ei.JJ_j2->P4());
       ei.b2j1_deltaPhi = ei.HH_b2->P4().DeltaPhi(ei.JJ_j1->P4());
       ei.b2j2_deltaPhi = ei.HH_b2->P4().DeltaPhi(ei.JJ_j2->P4());
       ei.b3j1_deltaPhi = ei.HH_b3->P4().DeltaPhi(ei.JJ_j1->P4());
       ei.b3j2_deltaPhi = ei.HH_b3->P4().DeltaPhi(ei.JJ_j2->P4());
       ei.b4j1_deltaPhi = ei.HH_b4->P4().DeltaPhi(ei.JJ_j1->P4());
       ei.b4j2_deltaPhi = ei.HH_b4->P4().DeltaPhi(ei.JJ_j2->P4());
       ei.j1j2_deltaPhi = ei.JJ_j1->P4().DeltaPhi(ei.JJ_j2->P4());
       //Special variables:deltaEta between jets
       ei.b1j1_deltaEta = abs(ei.HH_b1->P4().Eta() - ei.JJ_j1->P4().Eta() );
       ei.b1j2_deltaEta = abs(ei.HH_b1->P4().Eta() - ei.JJ_j2->P4().Eta() );
       ei.b2j1_deltaEta = abs(ei.HH_b2->P4().Eta() - ei.JJ_j1->P4().Eta() );
       ei.b2j2_deltaEta = abs(ei.HH_b2->P4().Eta() - ei.JJ_j2->P4().Eta() );
       ei.b3j1_deltaEta = abs(ei.HH_b3->P4().Eta() - ei.JJ_j1->P4().Eta() );
       ei.b3j2_deltaEta = abs(ei.HH_b3->P4().Eta() - ei.JJ_j2->P4().Eta() );
       ei.b4j1_deltaEta = abs(ei.HH_b4->P4().Eta() - ei.JJ_j1->P4().Eta() );
       ei.b4j2_deltaEta = abs(ei.HH_b4->P4().Eta() - ei.JJ_j2->P4().Eta() );
       ei.j1j2_deltaEta = abs(ei.JJ_j1->P4().Eta() - ei.JJ_j2->P4().Eta() );
       ei.VBFSelectedDeltaEta = abs(ei.JJ_j1->P4().Eta() - ei.JJ_j2->P4().Eta()) ;
       ei.VBFSelectedEtaSign = (ei.JJ_j1->P4().Eta() * ei.JJ_j2->P4().Eta())/ abs( ei.JJ_j1->P4().Eta() * ei.JJ_j2->P4().Eta()  ) ;
       //Special variables: deltaR between h1,h2,j1,j2,HH,JJ
       ei.h1j1_deltaR = ei.H1->P4().DeltaR(ei.JJ_j1->P4());
       ei.h1j2_deltaR = ei.H1->P4().DeltaR(ei.JJ_j2->P4());
       ei.h2j1_deltaR = ei.H2->P4().DeltaR(ei.JJ_j1->P4());
       ei.h2j2_deltaR = ei.H2->P4().DeltaR(ei.JJ_j2->P4());
       ei.h1jj_deltaR = ei.H1->P4().DeltaR(ei.JJ->P4());
       ei.h2jj_deltaR = ei.H2->P4().DeltaR(ei.JJ->P4());
       ei.hhj1_deltaR = ei.HH->P4().DeltaR(ei.JJ_j1->P4());
       ei.hhj2_deltaR = ei.HH->P4().DeltaR(ei.JJ_j2->P4());
       ei.hhjj_deltaR = ei.HH->P4().DeltaR(ei.JJ->P4());
       //Special variables: deltaPhi between h1,h2,j1,j2,HH,JJ
       ei.h1j1_deltaPhi = ei.H1->P4().DeltaPhi(ei.JJ_j1->P4());
       ei.h1j2_deltaPhi = ei.H1->P4().DeltaPhi(ei.JJ_j2->P4());
       ei.h2j1_deltaPhi = ei.H2->P4().DeltaPhi(ei.JJ_j1->P4());
       ei.h2j2_deltaPhi = ei.H2->P4().DeltaPhi(ei.JJ_j2->P4());
       ei.h1jj_deltaPhi = ei.H1->P4().DeltaPhi(ei.JJ->P4());
       ei.h2jj_deltaPhi = ei.H2->P4().DeltaPhi(ei.JJ->P4());
       ei.hhj1_deltaPhi = ei.HH->P4().DeltaPhi(ei.JJ_j1->P4());
       ei.hhj2_deltaPhi = ei.HH->P4().DeltaPhi(ei.JJ_j2->P4());
       ei.hhjj_deltaPhi = ei.HH->P4().DeltaPhi(ei.JJ->P4());
       //Special variables: deltaEta between h1,h2,j1,j2,HH,JJ
       ei.h1j1_deltaEta = abs( ei.H1->P4().Eta() - ei.JJ_j1->P4().Eta());
       ei.h1j2_deltaEta = abs( ei.H1->P4().Eta() - ei.JJ_j2->P4().Eta());
       ei.h2j1_deltaEta = abs( ei.H2->P4().Eta() - ei.JJ_j1->P4().Eta());
       ei.h2j2_deltaEta = abs( ei.H2->P4().Eta() - ei.JJ_j2->P4().Eta());
       ei.h1jj_deltaEta = abs( ei.H1->P4().Eta() - ei.JJ->P4().Eta());
       ei.h2jj_deltaEta = abs( ei.H2->P4().Eta() - ei.JJ->P4().Eta());
       ei.hhj1_deltaEta = abs( ei.HH->P4().Eta() - ei.JJ_j1->P4().Eta());
       ei.hhj2_deltaEta = abs( ei.HH->P4().Eta() - ei.JJ_j2->P4().Eta());
       ei.hhjj_deltaEta = abs( ei.HH->P4().Eta() - ei.JJ->P4().Eta());
       //VBF-jet location categories
       if ( abs( ei.JJ_j1->P4().Eta()) < 1.4){ei.JJ_j1_location=1;}
       else if ( abs( ei.JJ_j1->P4().Eta()) >= 1.4 && abs( ei.JJ_j1->P4().Eta()) < 2.4){ei.JJ_j1_location=2;}
       else{ei.JJ_j1_location=3;}
       if ( abs( ei.JJ_j2->P4().Eta()) < 1.4){ei.JJ_j2_location=1;}
       else if ( abs( ei.JJ_j2->P4().Eta()) >= 1.4 && abs( ei.JJ_j2->P4().Eta()) < 2.4){ei.JJ_j2_location=2;}
       else{ei.JJ_j2_location=3;}
       //VBF Event location categories
       if(     ei.JJ_j1_location==1 && ei.JJ_j2_location==1){ei.VBFEventLocation=1;}
       else if(ei.JJ_j1_location==2 && ei.JJ_j2_location==2){ei.VBFEventLocation=2;}
       else if(ei.JJ_j1_location==3 && ei.JJ_j2_location==3){ei.VBFEventLocation=3;}
       else if(ei.JJ_j1_location==1 && ei.JJ_j2_location==2){ei.VBFEventLocation=4;}
       else if(ei.JJ_j1_location==2 && ei.JJ_j2_location==1){ei.VBFEventLocation=4;}
       else if(ei.JJ_j1_location==1 && ei.JJ_j2_location==3){ei.VBFEventLocation=5;}
       else if(ei.JJ_j1_location==3 && ei.JJ_j2_location==1){ei.VBFEventLocation=5;}
       else {ei.VBFEventLocation=6;}
       //Extra and special variables
       ei.j1etaj2eta = ei.JJ_j1->P4().Eta()*ei.JJ_j2->P4().Eta();
       ei.maxj1etaj2eta = max( abs(ei.JJ_j1->P4().Eta() ), abs(ei.JJ_j2->P4().Eta() ) );
       ei.JJ_j1_rawpt = get_property(ei.JJ_j1.get(), Jet_pt) * (1-get_property(ei.JJ_j1.get(), Jet_rawFactor));
       ei.JJ_j2_rawpt = get_property(ei.JJ_j2.get(), Jet_pt) * (1-get_property(ei.JJ_j2.get(), Jet_rawFactor));
       //COSTHETA VARIABLES in CM FRAME
       TLorentzVector Sum = ei.HH_b1->P4() + ei.HH_b2->P4() + ei.HH_b3->P4() + ei.HH_b4->P4() + ei.JJ_j1->P4() + ei.JJ_j2->P4();
       auto boost_vctr = Sum.BoostVector();
       TLorentzVector H1_cm = ei.H1->P4(); H1_cm.Boost(-boost_vctr);
       TLorentzVector H2_cm = ei.H2->P4(); H2_cm.Boost(-boost_vctr);
       TLorentzVector HH_cm = ei.HH->P4(); HH_cm.Boost(-boost_vctr);
       TLorentzVector JJ_cm = ei.JJ->P4(); JJ_cm.Boost(-boost_vctr);
       TLorentzVector HH_b1_cm = ei.HH_b1->P4(); HH_b1_cm.Boost(-boost_vctr);
       TLorentzVector HH_b2_cm = ei.HH_b2->P4(); HH_b2_cm.Boost(-boost_vctr);
       TLorentzVector HH_b3_cm = ei.HH_b3->P4(); HH_b3_cm.Boost(-boost_vctr);
       TLorentzVector HH_b4_cm = ei.HH_b4->P4(); HH_b4_cm.Boost(-boost_vctr);
       TLorentzVector JJ_j1_cm = ei.JJ_j1->P4(); JJ_j1_cm.Boost(-boost_vctr);
       TLorentzVector JJ_j2_cm = ei.JJ_j2->P4(); JJ_j2_cm.Boost(-boost_vctr);
       ei.costh_H1_cm = H1_cm.CosTheta();
       ei.costh_H2_cm = H2_cm.CosTheta();
       ei.costh_HH_cm = HH_cm.CosTheta();
       ei.costh_JJ_cm = JJ_cm.CosTheta();
       ei.costh_HH_b1_cm = HH_b1_cm.CosTheta();
       ei.costh_HH_b2_cm = HH_b2_cm.CosTheta();
       ei.costh_HH_b3_cm = HH_b3_cm.CosTheta();
       ei.costh_HH_b4_cm = HH_b4_cm.CosTheta();
       ei.costh_JJ_j1_cm = JJ_j1_cm.CosTheta();
       ei.costh_JJ_j2_cm = JJ_j2_cm.CosTheta();
       
       // //The BDT always at the end, so the variables are defined for the calculation
       ei.BDT1 = GetBDT1Score(ei);

       // string strategy = any_cast<string>(parameterList_->at("bbbbChoice"));
       // // GGF-HHKiller
       // if(strategy=="BothClosestToMh")
       // {
       //     string weights= "weights/TMVAClassification_BDT_GGFKiller_case1.weights.xml";
       //     // ei.BDT1 = GetBDT1Score(ei,weights);
       //     ei.BDT1 = GetBDT1Score(ei);
       // }
       // else if(strategy=="BothClosestToDiagonal")
       // {
       //     string weights= "weights/TMVAClassification_BDT_GGFKiller_case2.weights.xml";
       //     // ei.BDT1 = GetBDT1Score(ei,weights); // GGF-HHKiller
       //     ei.BDT1 = GetBDT1Score(ei); // GGF-HHKiller
       // }
       // else if(strategy=="MinMassDifference")
       // {
       //     string weights= "weights/TMVAClassification_BDT_GGFKiller_case2.weights.xml";
       //     // ei.BDT1 = GetBDT1Score(ei,weights); // GGF-HHKiller
       //     ei.BDT1 = GetBDT1Score(ei); // GGF-HHKiller
       // }
       // else
       // {
       //     ei.BDT1 = 0;
       // }

       //VBF-QCDKiller
       ei.BDT2 = GetBDT2Score(ei);

       // if(strategy=="BothClosestToMh")
       // {
       //     string weights= "weights/TMVAClassification_BDT_VBFQCDKiller_case1.weights.xml";
       //     ei.BDT2 = GetBDT2Score(ei,weights);
       // }
       // else if(strategy=="BothClosestToDiagonal")
       // {
       //     string weights= "weights/TMVAClassification_BDT_VBFQCDKiller_case2.weights.xml";
       //     ei.BDT2 = GetBDT2Score(ei,weights); // GGF-HHKiller
       // }
       // else if(strategy=="MinMassDifference")
       // {
       //     string weights= "weights/TMVAClassification_BDT_VBFQCDKiller_case2.weights.xml";
       //     ei.BDT2 = GetBDT2Score(ei,weights); // GGF-HHKiller
       // }
       // else
       // {
       //     ei.BDT2 = 0;
       // }
       // //If VBF-HH Signal MC, then fill branches with information for gen-level studies
       // if(any_cast<bool>(parameterList_->at("is_VBF_sig")) ) AddVBFGenMatchVariables(nat, ei);

       return;
}

void OfflineProducerHelper::init_BDT_evals()
{
   // string strategy = any_cast<string>(parameterList_->at("bbbbChoice"));
   // string weights_BDT1 = "weights/TMVAClassification_BDT_GGFKiller_case1.weights.xml";    // FIXME: OK as default?
   // string weights_BDT2 = "weights/TMVAClassification_BDT_VBFQCDKiller_case1.weights.xml"; // FIXME: OK as default?
   // string weights_BDT3 = "weights/TMVAClassification_BDT_GGFQCDKiller_case1.weights.xml"; // FIXME: OK as default?
   
   // // GGF-HHKiller
   // if(strategy=="BothClosestToMh")
   //     weights_BDT1 = "weights/TMVAClassification_BDT_GGFKiller_case1.weights.xml";
   // else if(strategy=="BothClosestToDiagonal")
   //     weights_BDT1 = "weights/TMVAClassification_BDT_GGFKiller_case2.weights.xml";
   // else if(strategy=="MinMassDifference")
   //     string weights_BDT1 = "weights/TMVAClassification_BDT_GGFKiller_case2.weights.xml";
   // // else
   //      // some fallback?

   // //VBF-QCDKiller
   // if(strategy=="BothClosestToMh")
   //     weights_BDT2 = "weights/TMVAClassification_BDT_VBFQCDKiller_case1.weights.xml";
   // else if(strategy=="BothClosestToDiagonal")
   //     weights_BDT2 = "weights/TMVAClassification_BDT_VBFQCDKiller_case2.weights.xml";
   // else if(strategy=="MinMassDifference")
   //     weights_BDT2 = "weights/TMVAClassification_BDT_VBFQCDKiller_case2.weights.xml";
   // // else
   //      // some fallback?

   // //GGF-QCDKiller
   // if(strategy=="BothClosestToMh")
   //     weights_BDT3 = "weights/TMVAClassification_BDT_GGFQCDKiller_case1.weights.xml";
   // else if(strategy=="BothClosestToDiagonal")
   //     weights_BDT3 = "weights/TMVAClassification_BDT_GGFQCDKiller_case2.weights.xml";
   // else if(strategy=="MinMassDifference")
   //     weights_BDT3 = "weights/TMVAClassification_BDT_GGFQCDKiller_case2.weights.xml";
   // // else
   //      // some fallback?

   string strategy = any_cast<string>(parameterList_->at("bbbbChoice"));

   string method = "BDT method";

   string weights_BDT1 = "weights/TMVAClassification_BDT_GGFKiller_case1.weights.xml";    // FIXME: OK as default?
   string weights_BDT2 = "weights/TMVAClassification_BDT_VBFQCDKiller_case1.weights.xml"; // FIXME: OK as default?
   string weights_BDT3 = "weights/TMVAClassification_BDT_GGFQCDKiller_case1.weights.xml"; // FIXME: OK as default?
   
    // GGF-HHKiller
    if(strategy=="BothClosestToMh"){
        weights_BDT1 = "weights/TMVAClassification_BDT_GGFKiller_case1.weights.xml";
        weights_BDT2 = "weights/TMVAClassification_BDT_VBFQCDKiller_case1.weights.xml";
        weights_BDT3 = "weights/TMVAClassification_BDT_GGFQCDKiller_case1.weights.xml";
    }
    else if(strategy=="BothClosestToDiagonal"){
        weights_BDT1 = "weights/TMVAClassification_BDT_GGFKiller_case2.weights.xml";
        weights_BDT2 = "weights/TMVAClassification_BDT_VBFQCDKiller_case2.weights.xml";
        weights_BDT3 = "weights/TMVAClassification_BDT_GGFQCDKiller_case2.weights.xml";
    }
    else if(strategy=="MinMassDifference"){
       weights_BDT1 = "weights/TMVAClassification_BDT_GGFKiller_case2.weights.xml";
       weights_BDT2 = "weights/TMVAClassification_BDT_VBFQCDKiller_case2.weights.xml";
       weights_BDT3 = "weights/TMVAClassification_BDT_GGFQCDKiller_case2.weights.xml";
    }
    // else
        // some fallback?

    //std::cout << "[INFO] ... initialising BDT evals" << std::endl;
    //std::cout << "   - BDT 1 . method : " << method << ", weight_file : " << weights_BDT1 << std::endl;
    //std::cout << "   - BDT 2 . method : " << method << ", weight_file : " << weights_BDT2 << std::endl;
    //std::cout << "   - BDT 3 . method : " << method << ", weight_file : " << weights_BDT3 << std::endl;

    eval_BDT1_->init(method, weights_BDT1);
    eval_BDT2_->init(method, weights_BDT2);
    eval_BDT3_->init(method, weights_BDT3);
}


void OfflineProducerHelper::init_HH_reweighter(OutputTree& ot, std::string coeffFile, std::string hhreweighterInputMap, std::string histoName)
{
    // initialise the reweighter
    //std::cout << "[INFO] ... initialising HH reweighter" << std::endl;
    //std::cout << "   - coefficient file       : " << coeffFile            << std::endl;
    //std::cout << "   - input sample map file  : " << hhreweighterInputMap << std::endl;
    //std::cout << "   - input sample map hisot : " << histoName            << std::endl;

    TFile* fIn = TFile::Open(hhreweighterInputMap.c_str());
    TH2D*  hIn = (TH2D*) fIn->Get(histoName.c_str());
    hhreweighter_ = std::unique_ptr<HHReweight5D> (new HHReweight5D (coeffFile, hIn));
    fIn->Close();

    // create the needed branches for the reweigh
    ot.declareUserFloatBranch("HH_reweight", 1.0);
}

// float OfflineProducerHelper::GetBDT1Score(EventInfo& ei, string weights){
float OfflineProducerHelper::GetBDT1Score(EventInfo& ei){

       // TMVA::Reader *reader0 = new TMVA::Reader( "!Color:Silent" );
       // float abs_H1_eta_bdt=abs(ei.H1->P4().Eta());
       // float abs_H2_eta_bdt=abs(ei.H2->P4().Eta());
       // float H1_pt_bdt=ei.H1->P4().Pt();
       // float H2_pt_bdt=ei.H2->P4().Pt();
       // float JJ_j1_pt_bdt=ei.JJ_j1->P4().Pt();
       // float JJ_j2_pt_bdt=ei.JJ_j2->P4().Pt();
       // float abs_JJ_eta_bdt = abs(ei.JJ->P4().Eta());
       // float h1h2_deltaEta_bdt = ei.h1h2_deltaEta.get();
       // float h1j1_deltaR_bdt = ei.h1j1_deltaR.get();
       // float h1j2_deltaR_bdt = ei.h1j2_deltaR.get();
       // float h2j1_deltaR_bdt = ei.h2j1_deltaR.get();
       // float h2j2_deltaR_bdt = ei.h2j2_deltaR.get();
       // float abs_j1etaj2eta_bdt = abs( ei.j1etaj2eta.get() );
       // float abs_costh_HH_b1_cm_bdt = abs(ei.costh_HH_b1_cm.get());
       // float abs_costh_HH_b2_cm_bdt = abs(ei.costh_HH_b2_cm.get());
       // reader0->AddVariable("abs_H1_eta:=abs(H1_eta)", &(abs_H1_eta_bdt) );
       // reader0->AddVariable("abs_H2_eta:=abs(H2_eta)", &(abs_H2_eta_bdt) );
       // reader0->AddVariable("H1_pt", &(H1_pt_bdt) );
       // reader0->AddVariable("H2_pt", &(H2_pt_bdt) );
       // reader0->AddVariable("JJ_j1_pt", &(JJ_j1_pt_bdt) );
       // reader0->AddVariable("JJ_j2_pt", &(JJ_j2_pt_bdt) );
       // reader0->AddVariable("abs_JJ_eta:=abs(JJ_eta)", &(abs_JJ_eta_bdt));
       // reader0->AddVariable("h1h2_deltaEta", &(h1h2_deltaEta_bdt));
       // reader0->AddVariable("h1j1_deltaR", &(h1j1_deltaR_bdt));
       // reader0->AddVariable("h1j2_deltaR", &(h1j2_deltaR_bdt));
       // reader0->AddVariable("h2j1_deltaR", &(h2j1_deltaR_bdt));
       // reader0->AddVariable("h2j2_deltaR", &(h2j2_deltaR_bdt));
       // reader0->AddVariable("abs_j1etaj2eta:=abs(j1etaj2eta)", &(abs_j1etaj2eta_bdt));
       // reader0->AddVariable("abs_costh_HH_b1_cm:=abs(costh_HH_b1_cm)", &(abs_costh_HH_b1_cm_bdt));
       // reader0->AddVariable("abs_costh_HH_b2_cm:=abs(costh_HH_b2_cm)", &(abs_costh_HH_b2_cm_bdt));
       // TString method =  "BDT method";
       // reader0->BookMVA( "BDT method", Form("%s",weights.c_str()) );
       // float BDT = reader0->EvaluateMVA(method);
       // delete reader0;
       // return BDT;

        float abs_H1_eta_bdt                  = abs(ei.H1->P4().Eta());
        float abs_H2_eta_bdt                  = abs(ei.H2->P4().Eta());
        float H1_pt_bdt                       = ei.H1->P4().Pt();
        float H2_pt_bdt                       = ei.H2->P4().Pt();
        float JJ_j1_pt_bdt                    = ei.JJ_j1->P4().Pt();
        float JJ_j2_pt_bdt                    = ei.JJ_j2->P4().Pt();
        float abs_JJ_eta_bdt                  = abs(ei.JJ->P4().Eta());
        float h1h2_deltaEta_bdt               = ei.h1h2_deltaEta.get();
        float h1j1_deltaR_bdt                 = ei.h1j1_deltaR.get();
        float h1j2_deltaR_bdt                 = ei.h1j2_deltaR.get();
        float h2j1_deltaR_bdt                 = ei.h2j1_deltaR.get();
        float h2j2_deltaR_bdt                 = ei.h2j2_deltaR.get();
        float abs_j1etaj2eta_bdt              = abs( ei.j1etaj2eta.get() );
        float abs_costh_HH_b1_cm_bdt          = abs(ei.costh_HH_b1_cm.get());
        float abs_costh_HH_b2_cm_bdt          = abs(ei.costh_HH_b2_cm.get());

        eval_BDT1_ -> floatVarsMap["abs_H1_eta:=abs(H1_eta)"]                  = abs_H1_eta_bdt;
        eval_BDT1_ -> floatVarsMap["abs_H2_eta:=abs(H2_eta)"]                  = abs_H2_eta_bdt;
        eval_BDT1_ -> floatVarsMap["H1_pt"]                                    = H1_pt_bdt;
        eval_BDT1_ -> floatVarsMap["H2_pt"]                                    = H2_pt_bdt;
        eval_BDT1_ -> floatVarsMap["JJ_j1_pt"]                                 = JJ_j1_pt_bdt;
        eval_BDT1_ -> floatVarsMap["JJ_j2_pt"]                                 = JJ_j2_pt_bdt;
        eval_BDT1_ -> floatVarsMap["abs_JJ_eta:=abs(JJ_eta)"]                  = abs_JJ_eta_bdt;
        eval_BDT1_ -> floatVarsMap["h1h2_deltaEta"]                            = h1h2_deltaEta_bdt;
        eval_BDT1_ -> floatVarsMap["h1j1_deltaR"]                              = h1j1_deltaR_bdt;
        eval_BDT1_ -> floatVarsMap["h1j2_deltaR"]                              = h1j2_deltaR_bdt;
        eval_BDT1_ -> floatVarsMap["h2j1_deltaR"]                              = h2j1_deltaR_bdt;
        eval_BDT1_ -> floatVarsMap["h2j2_deltaR"]                              = h2j2_deltaR_bdt;
        eval_BDT1_ -> floatVarsMap["abs_j1etaj2eta:=abs(j1etaj2eta)"]          = abs_j1etaj2eta_bdt;
        eval_BDT1_ -> floatVarsMap["abs_costh_HH_b1_cm:=abs(costh_HH_b1_cm)"]  = abs_costh_HH_b1_cm_bdt;
        eval_BDT1_ -> floatVarsMap["abs_costh_HH_b2_cm:=abs(costh_HH_b2_cm)"]  = abs_costh_HH_b2_cm_bdt;

        return eval_BDT1_ -> eval();
}

float OfflineProducerHelper::GetBDT2Score(EventInfo& ei){
       // TMVA::Reader *reader0 = new TMVA::Reader( "!Color:Silent" );
       // float HH_b3_pt_bdt=ei.HH_b3->P4().Pt();
       // float HH_b4_pt_bdt=ei.HH_b4->P4().Pt();
       // float JJ_j1_qgl_bdt = get_property(ei.JJ_j1.get(), Jet_qgl);
       // float JJ_j2_qgl_bdt = get_property(ei.JJ_j2.get(), Jet_qgl);
       // float H1_m_bdt = ei.H1->P4().M();
       // float H2_m_bdt = ei.H2->P4().M();
       // float H1_bb_deltaR_bdt = ei.H1_bb_deltaR.get();
       // float H2_bb_deltaR_bdt = ei.H2_bb_deltaR.get();
       // float JJ_m_bdt = ei.JJ->P4().M();
       // float j1j2_deltaEta_bdt = ei.j1j2_deltaEta.get();
       // float abs_costh_JJ_j1_cm_bdt = abs(ei.costh_JJ_j1_cm.get());
       // reader0->AddVariable("HH_b3_pt", &(HH_b3_pt_bdt));
       // reader0->AddVariable("HH_b4_pt", &(HH_b4_pt_bdt));
       // reader0->AddVariable("JJ_j1_qgl", &(JJ_j1_qgl_bdt));
       // reader0->AddVariable("JJ_j2_qgl", &(JJ_j2_qgl_bdt));
       // reader0->AddVariable("H1_m",&(H1_m_bdt));
       // reader0->AddVariable("H2_m",&(H2_m_bdt));
       // reader0->AddVariable("H1_bb_deltaR",&(H1_bb_deltaR_bdt));
       // reader0->AddVariable("H2_bb_deltaR",&(H2_bb_deltaR_bdt));
       // reader0->AddVariable("JJ_m",&(JJ_m_bdt));
       // reader0->AddVariable("j1j2_deltaEta",&(j1j2_deltaEta_bdt));
       // reader0->AddVariable("abs_costh_JJ_j1_cm:=abs(costh_JJ_j1_cm)",&(abs_costh_JJ_j1_cm_bdt));
       // TString method =  "BDT method";
       // reader0->BookMVA( "BDT method", Form("%s",weights.c_str()) );
       // float BDT = reader0->EvaluateMVA(method);
       // delete reader0;
       // return BDT;
    
       float HH_b3_pt_bdt               = ei.HH_b3->P4().Pt();
       float HH_b4_pt_bdt               = ei.HH_b4->P4().Pt();
       float JJ_j1_qgl_bdt              = get_property(ei.JJ_j1.get(), Jet_qgl);
       float JJ_j2_qgl_bdt              = get_property(ei.JJ_j2.get(), Jet_qgl);
       float H1_m_bdt                   = ei.H1->P4().M();
       float H2_m_bdt                   = ei.H2->P4().M();
       float H1_bb_deltaR_bdt           = ei.H1_bb_deltaR.get();
       float H2_bb_deltaR_bdt           = ei.H2_bb_deltaR.get();
       float JJ_m_bdt                   = ei.JJ->P4().M();
       float j1j2_deltaEta_bdt          = ei.j1j2_deltaEta.get();
       float abs_costh_JJ_j1_cm_bdt     = abs(ei.costh_JJ_j1_cm.get());

        eval_BDT2_ -> floatVarsMap["HH_b3_pt"]                                = HH_b3_pt_bdt;
        eval_BDT2_ -> floatVarsMap["HH_b4_pt"]                                = HH_b4_pt_bdt;
        eval_BDT2_ -> floatVarsMap["JJ_j1_qgl"]                               = JJ_j1_qgl_bdt;
        eval_BDT2_ -> floatVarsMap["JJ_j2_qgl"]                               = JJ_j2_qgl_bdt;
        eval_BDT2_ -> floatVarsMap["H1_m"]                                    = H1_m_bdt;
        eval_BDT2_ -> floatVarsMap["H2_m"]                                    = H2_m_bdt;
        eval_BDT2_ -> floatVarsMap["H1_bb_deltaR"]                            = H1_bb_deltaR_bdt;
        eval_BDT2_ -> floatVarsMap["H2_bb_deltaR"]                            = H2_bb_deltaR_bdt;
        eval_BDT2_ -> floatVarsMap["JJ_m"]                                    = JJ_m_bdt;
        eval_BDT2_ -> floatVarsMap["j1j2_deltaEta"]                           = j1j2_deltaEta_bdt;
        eval_BDT2_ -> floatVarsMap["abs_costh_JJ_j1_cm:=abs(costh_JJ_j1_cm)"] = abs_costh_JJ_j1_cm_bdt;

        return eval_BDT2_ -> eval();
}

float OfflineProducerHelper::GetBDT3Score(EventInfo& ei){
       // TMVA::Reader *reader0 = new TMVA::Reader( "!Color:Silent" );
       // float HH_b3_pt_bdt=ei.HH_b3->P4().Pt();
       // float HH_b4_pt_bdt=ei.HH_b4->P4().Pt();
       // float abs_HH_b3_eta_bdt=abs(ei.HH_b3->P4().Eta());
       // float abs_HH_b4_eta_bdt=abs(ei.HH_b4->P4().Eta());
       // float H1_m_bdt = ei.H1->P4().M();
       // float H2_m_bdt = ei.H2->P4().M();
       // float H1_bb_deltaR_bdt = ei.H1_bb_deltaR.get();
       // float H2_bb_deltaR_bdt = ei.H2_bb_deltaR.get();
       // float H1_bb_deltaPhi_bdt = ei.H1_bb_deltaPhi.get();
       // float H2_bb_deltaPhi_bdt = ei.H2_bb_deltaPhi.get();
       // float abs_costh_HH_b1_cm_bdt = abs(ei.costh_HH_b1_cm.get());
       // float abs_costh_HH_b2_cm_bdt = abs(ei.costh_HH_b2_cm.get());
       // reader0->AddVariable("HH_b3_pt", &(HH_b3_pt_bdt));
       // reader0->AddVariable("HH_b4_pt", &(HH_b4_pt_bdt));
       // reader0->AddVariable("abs_HH_b3_eta:=abs(HH_b3_eta)", &(abs_HH_b3_eta_bdt));
       // reader0->AddVariable("abs_HH_b4_eta:=abs(HH_b4_eta)", &(abs_HH_b4_eta_bdt));
       // reader0->AddVariable("H1_m",&(H1_m_bdt));
       // reader0->AddVariable("H2_m",&(H2_m_bdt));
       // reader0->AddVariable("H1_bb_deltaR",&(H1_bb_deltaR_bdt));
       // reader0->AddVariable("H2_bb_deltaR",&(H2_bb_deltaR_bdt));
       // reader0->AddVariable("H1_bb_deltaPhi",&(H1_bb_deltaPhi_bdt));
       // reader0->AddVariable("H2_bb_deltaPhi",&(H2_bb_deltaPhi_bdt));
       // reader0->AddVariable("abs_costh_HH_b1_cm:=abs(costh_HH_b1_cm)",&(abs_costh_HH_b1_cm_bdt));
       // reader0->AddVariable("abs_costh_HH_b2_cm:=abs(costh_HH_b2_cm)",&(abs_costh_HH_b2_cm_bdt));
       // TString method =  "BDT method";
       // reader0->BookMVA( "BDT method", Form("%s",weights.c_str()) );
       // float BDT = reader0->EvaluateMVA(method);
       // delete reader0;
       // return BDT;

        float HH_b3_pt_bdt           =  ei.HH_b3->P4().Pt();
        float HH_b4_pt_bdt           =  ei.HH_b4->P4().Pt();
        float abs_HH_b3_eta_bdt      =  abs(ei.HH_b3->P4().Eta());
        float abs_HH_b4_eta_bdt      =  abs(ei.HH_b4->P4().Eta());
        float H1_m_bdt               =  ei.H1->P4().M();
        float H2_m_bdt               =  ei.H2->P4().M();
        float H1_bb_deltaR_bdt       =  ei.H1_bb_deltaR.get();
        float H2_bb_deltaR_bdt       =  ei.H2_bb_deltaR.get();
        float H1_bb_deltaPhi_bdt     =  ei.H1_bb_deltaPhi.get();
        float H2_bb_deltaPhi_bdt     =  ei.H2_bb_deltaPhi.get();
        float abs_costh_HH_b1_cm_bdt =  abs(ei.costh_HH_b1_cm.get());
        float abs_costh_HH_b2_cm_bdt =  abs(ei.costh_HH_b2_cm.get());

        eval_BDT3_ -> floatVarsMap["HH_b3_pt"]                                 = HH_b3_pt_bdt;
        eval_BDT3_ -> floatVarsMap["HH_b4_pt"]                                 = HH_b4_pt_bdt;
        eval_BDT3_ -> floatVarsMap["abs_HH_b3_eta:=abs(HH_b3_eta)"]            = abs_HH_b3_eta_bdt;
        eval_BDT3_ -> floatVarsMap["abs_HH_b4_eta:=abs(HH_b4_eta)"]            = abs_HH_b4_eta_bdt;
        eval_BDT3_ -> floatVarsMap["H1_m"]                                     = H1_m_bdt;
        eval_BDT3_ -> floatVarsMap["H2_m"]                                     = H2_m_bdt;
        eval_BDT3_ -> floatVarsMap["H1_bb_deltaR"]                             = H1_bb_deltaR_bdt;
        eval_BDT3_ -> floatVarsMap["H2_bb_deltaR"]                             = H2_bb_deltaR_bdt;
        eval_BDT3_ -> floatVarsMap["H1_bb_deltaPhi"]                           = H1_bb_deltaPhi_bdt;
        eval_BDT3_ -> floatVarsMap["H2_bb_deltaPhi"]                           = H2_bb_deltaPhi_bdt;
        eval_BDT3_ -> floatVarsMap["abs_costh_HH_b1_cm:=abs(costh_HH_b1_cm)"]  = abs_costh_HH_b1_cm_bdt;
        eval_BDT3_ -> floatVarsMap["abs_costh_HH_b2_cm:=abs(costh_HH_b2_cm)"]  = abs_costh_HH_b2_cm_bdt;

        return eval_BDT3_ -> eval();
}

std::vector<Jet> OfflineProducerHelper::bjJets_PreselectionCut(NanoAODTree& nat, EventInfo& ei,OutputTree &ot,std::vector<Jet> jets)

{
    float bminimumDeepCSVAccepted           = any_cast<float>(parameterList_->at("bMinDeepCSV"          ));
    float bminimumPtAccepted                = any_cast<float>(parameterList_->at("bMinPt"               ));
    float bmaximumAbsEtaAccepted            = any_cast<float>(parameterList_->at("bMaxAbsEta"           ));
    float jminimumPtAccepted                = any_cast<float>(parameterList_->at("jMinPt"               ));
    float jmaximumAbsEtaAccepted            = any_cast<float>(parameterList_->at("jMaxAbsEta"           ));
    std::vector<Jet> nobjets,outputJets,btaggedjets;

    /////////////--------Select four b-jet candidates
    ///////////// Apply PT and ETA requirements
    auto it = jets.begin();
    while (it != jets.end()){
        if(bminimumPtAccepted>0.){
            if(get_property( (*it),Jet_pt)<bminimumPtAccepted){
                nobjets.emplace_back(*it);
                it=jets.erase(it);
                continue;
            }
        }
        if(bmaximumAbsEtaAccepted>0.){
            if(abs(get_property( (*it),Jet_eta))>bmaximumAbsEtaAccepted){
                nobjets.emplace_back(*it);
                it=jets.erase(it);
                continue;
            }
        }
        ++it;
    }
    if(jets.size() < 4) return outputJets;
    ///////////// Apply BTAGGING requirements
    //Do we want antibtag information or not?
    int MinimumNumberOfBTags=0;
    if(any_cast<bool>(parameterList_->at("FourthAntiBTagInformation"))){MinimumNumberOfBTags= 3;}
    else{MinimumNumberOfBTags= 4;}
    //Count number of b-tagged jets. If btags < 3, then reject event (not important for us)
    int btags=0; for (uint i=0;i<jets.size();i++ ){ if( get_property( jets.at(i) ,Jet_btagDeepB) > bminimumDeepCSVAccepted) btags++;}
    if(btags<MinimumNumberOfBTags) return outputJets;
    //Select the four most b-tagged
    stable_sort(jets.begin(), jets.end(), [](const Jet & a, const Jet & b) -> bool
    { return ( get_property(a, Jet_btagDeepB) > get_property(b, Jet_btagDeepB) );});
    outputJets = {{*(jets.begin()+0),*(jets.begin()+1),*(jets.begin()+2),*(jets.begin()+3)}};
    int c=0; while (c<4){jets.erase(jets.begin());c++;}
    //Calculate the associated b-tagging scale factor depending on b-tag or anti-btag selection
    if(btags==3){btaggedjets={{*(outputJets.begin()+0),*(outputJets.begin()+1),*(outputJets.begin()+2)}};CalculateBtagScaleFactor(btaggedjets,nat,ot);}
    else{btaggedjets={{*(outputJets.begin()+0),*(outputJets.begin()+1),*(outputJets.begin()+2),*(outputJets.begin()+3)}};CalculateBtagScaleFactor(btaggedjets,nat,ot);}

    /////////////--------Select two VBF-jet candidates based on PT and ETA1*ETA2<0
    ///////////// Apply PT and ETA requirements
    jets.insert(jets.end(), nobjets.begin(), nobjets.end());
    auto jt = jets.begin();
    while (jt != jets.end()){
        if(jminimumPtAccepted>0.){
            if(get_property( (*jt),Jet_pt)<jminimumPtAccepted){
                jt=jets.erase(jt);
                continue;
            }
        }
        if(jmaximumAbsEtaAccepted>0.){
            if(abs(get_property( (*jt),Jet_eta))>jmaximumAbsEtaAccepted){
                jt=jets.erase(jt);
                continue;
            }
        }
        ++jt;
    }
    if(jets.size() < 2) return outputJets;
    //Check if event has at least two VBF-jet candidates (OppositeEtaJetPair), otherwise returns only b-jets candidates
    std::vector<Jet> VBFjets = OppositeEtaJetPair(jets);
    if(VBFjets.size() < 2)return outputJets;
    /////////////--------Fill information of preselected jets: outputJets = (4 b-jets, 2 VBF jets) and return it
    outputJets.insert(outputJets.end(),VBFjets.begin(),VBFjets.end());
    return outputJets;
}

std::vector<Jet> OfflineProducerHelper::bjJets_2016_PreselectionCut(NanoAODTree& nat, EventInfo& ei,OutputTree &ot,std::vector<Jet> jets)

{
    float bminimumDeepCSVAccepted           = any_cast<float>(parameterList_->at("bMinDeepCSV"          ));
    float bminimumPtAccepted                = any_cast<float>(parameterList_->at("bMinPt"               ));
    float bmaximumAbsEtaAccepted            = any_cast<float>(parameterList_->at("bMaxAbsEta"           ));
    float jminimumPtAccepted                = any_cast<float>(parameterList_->at("jMinPt"               ));
    float jmaximumAbsEtaAccepted            = any_cast<float>(parameterList_->at("jMaxAbsEta"           ));
    std::vector<Jet> nobjets,outputJets,btaggedjets;

    /////////////--------Select four b-jet candidates
    ///////////// Apply PT and ETA requirements
    auto it = jets.begin();
    while (it != jets.end()){
        if(bminimumPtAccepted>0.){
            if(get_property( (*it),Jet_pt)<bminimumPtAccepted){
                nobjets.emplace_back(*it);
                it=jets.erase(it);
                continue;
            }
        }
        if(bmaximumAbsEtaAccepted>0.){
            if(abs(get_property( (*it),Jet_eta))>bmaximumAbsEtaAccepted){
                nobjets.emplace_back(*it);
                it=jets.erase(it);
                continue;
            }
        }
        ++it;
    }
    if(jets.size() < 4) return outputJets;
    ///////////// Apply BTAGGING requirements
    //Do we want antibtag information or not?
    int MinimumNumberOfBTags=0;
    if(any_cast<bool>(parameterList_->at("FourthAntiBTagInformation"))){MinimumNumberOfBTags= 3;}
    else{MinimumNumberOfBTags= 4;}
    //Count number of b-tagged jets. If btags < 3, then reject event (not important for us)
    int btags=0; for (uint i=0;i<jets.size();i++ ){ if( get_property( jets.at(i) ,Jet_btagCMVA) > bminimumDeepCSVAccepted) btags++;}
    if(btags<MinimumNumberOfBTags) return outputJets;
    //Select the four most b-tagged
    stable_sort(jets.begin(), jets.end(), [](const Jet & a, const Jet & b) -> bool
    { return ( get_property(a, Jet_btagCMVA) > get_property(b, Jet_btagCMVA) );});
    outputJets = {{*(jets.begin()+0),*(jets.begin()+1),*(jets.begin()+2),*(jets.begin()+3)}};
    int c=0; while (c<4){jets.erase(jets.begin());c++;}
    //Calculate the associated b-tagging scale factor depending on b-tag or anti-btag selection
    if(btags==3){btaggedjets={{*(outputJets.begin()+0),*(outputJets.begin()+1),*(outputJets.begin()+2)}};CalculateBtagScaleFactor(btaggedjets,nat,ot);}
    else{btaggedjets={{*(outputJets.begin()+0),*(outputJets.begin()+1),*(outputJets.begin()+2),*(outputJets.begin()+3)}};CalculateBtagScaleFactor(btaggedjets,nat,ot);}

    /////////////--------Select two VBF-jet candidates based on PT and ETA1*ETA2<0
    ///////////// Apply PT and ETA requirements
    jets.insert(jets.end(), nobjets.begin(), nobjets.end());
    auto jt = jets.begin();
    while (jt != jets.end()){
        if(jminimumPtAccepted>0.){
            if(get_property( (*jt),Jet_pt)<jminimumPtAccepted){
                jt=jets.erase(jt);
                continue;
            }
        }
        if(jmaximumAbsEtaAccepted>0.){
            if(abs(get_property( (*jt),Jet_eta))>jmaximumAbsEtaAccepted){
                jt=jets.erase(jt);
                continue;
            }
        }
        ++jt;
    }
    if(jets.size() < 2) return outputJets;
    //Check if event has at least two VBF-jet candidates (OppositeEtaJetPair), otherwise returns only b-jets candidates
    std::vector<Jet> VBFjets = OppositeEtaJetPair(jets);
    if(VBFjets.size() < 2)return outputJets;
    /////////////--------Fill information of preselected jets: outputJets = (4 b-jets, 2 VBF jets) and return it
    outputJets.insert(outputJets.end(),VBFjets.begin(),VBFjets.end());
    return outputJets;
}

std::vector<Jet> OfflineProducerHelper::ExternalEtaJetPair(std::vector<Jet> jets, std::vector<Jet> bjets){
//Check Max abs(Eta) among the b-jets
std::vector<Jet> outputJets;
float maxbEta=-1;
for (uint i=0;i<bjets.size();i++ )
{
        float Eta = abs(bjets.at(i).P4().Eta());
        if(Eta>maxbEta){maxbEta=Eta;}
}
//Remove VBF-jet candidates with abs(eta) lower than the bjets abs(eta)
auto jt = jets.begin();
while (jt != jets.end()){
        if(abs(get_property( (*jt),Jet_eta))<maxbEta){
            jt=jets.erase(jt);
            continue;
        }
        ++jt;
}
//Ordered the VBF candidates by pt. Ff there are not two VBF-candidates, then return empty
if(jets.size()<2) return outputJets;
stable_sort(jets.begin(), jets.end(), [](const Jet & a, const Jet & b) -> bool
{ return ( get_property(a, Jet_pt ) >  get_property(b, Jet_pt)  );});
outputJets = {{*(jets.begin()+0),*(jets.begin()+1)}}; return outputJets;
}

std::vector<int> OfflineProducerHelper::AddGenMatchingInfo(NanoAODTree& nat, EventInfo& ei, std::vector<Jet> jets)
{
    //OUTPUT: Vector of quarkIDs for the jets
    //QuarkId: (0) Others, (1) gen_H1_b1, (2) gen_H1_b2, (3) gen_H2_b1, (4) gen_H2_b2, (5) gen_j1_out, (6) gen_j2_outs
    //Order the generator level information by specific PT
    order_by_pT(ei.gen_H1_b1.get(), ei.gen_H1_b2.get());
    order_by_pT(ei.gen_H2_b1.get(), ei.gen_H2_b2.get());
    order_by_pT(ei.gen_q1_out.get(), ei.gen_q2_out.get());
    std::vector<GenPart> bs = {{*(ei.gen_H1_b1),*(ei.gen_H1_b2),*(ei.gen_H2_b1),*(ei.gen_H2_b2)}};
    bool swapped = order_by_pT(ei.gen_H1.get(), ei.gen_H2.get());
    if (!swapped)
    {
        ei.gen_H1_b1 = bs.at(0);
        ei.gen_H1_b2 = bs.at(1);
        ei.gen_H2_b1 = bs.at(2);
        ei.gen_H2_b2 = bs.at(3);
    }
    else
    {
        ei.gen_H1_b1 = bs.at(2);
        ei.gen_H1_b2 = bs.at(3);
        ei.gen_H2_b1 = bs.at(0);
        ei.gen_H2_b2 = bs.at(1);
    }
    //Define a vector with the six ordered quarks
    std::vector<GenPart> quarks = {{
        *(ei.gen_H1_b1),*(ei.gen_H1_b2),
        *(ei.gen_H2_b1),*(ei.gen_H2_b2), *(ei.gen_q1_out),*(ei.gen_q2_out)}};
    std::vector<int> output = QuarkToJetMatcher(quarks,jets);
    return output;
 }

std::vector<int> OfflineProducerHelper::QuarkToJetMatcher(const std::vector<GenPart> quarks,const std::vector<Jet> jets){
// OUTPUT is a vector with nJet entries filled the QuarkId, ordered by jets index
//----QuarkId: (1) gen_H1_b1, (2) gen_H1_b2, (3) gen_H2_b1, (4) gen_H2_b2, (5) gen_j1_out, (6) gen_j2_out, (0) Others
//----JetId: Index of the jets in the input vector 'jets'
//Define and initialize variables
std::vector<std::tuple<TLorentzVector,int>> jetsinfo,quarksinfo;
std::vector<std::tuple<int,int>> quark_jet_link; uint id;
std::vector<int> output;
float dR=0.4, maxDeltaR, DeltaR; bool matched = false;
uint m=0; while(m<quarks.size()){quarksinfo.emplace_back( make_tuple(quarks.at(m).P4(), m)); m++;}
uint n=0; while(n<jets.size()  ){jetsinfo.emplace_back( make_tuple(jets.at(n).P4(), n)    ); n++;}
//Create all possible combinations and find the jet set that minimize the deltaR
for (uint x=0;x < quarksinfo.size();x++ )
{
 maxDeltaR = dR;
 for (uint y=0;y < jetsinfo.size(); y++ ){
      DeltaR = get<0>(quarksinfo[x]).DeltaR( get<0>(jetsinfo[y]) );
      if(DeltaR < maxDeltaR){maxDeltaR=DeltaR; matched=true; id = y ;}
 }
 if(matched) {quark_jet_link.emplace_back( make_tuple(x,get<1>(jetsinfo[id]))); jetsinfo.erase(jetsinfo.begin()+id);}
 else {quark_jet_link.emplace_back( make_tuple(x,-1) );}
 matched = false;
}
//Create output with QuarkID
bool found; int foundy;
for (uint x=0; x < jets.size(); x++ ){
      uint jetid=x;
      found = false;
      //Check if there is a quark that is linked to this jet
      for (uint y=0; y < quark_jet_link.size(); y++ ){
      uint m = get<1>(quark_jet_link[y]);
      if(jetid == m ){found = true; foundy=y;}
      }
      //Load jet information to the output: QuarkID
      if(found){output.emplace_back(get<0>(quark_jet_link[foundy])+1);}
      else{output.emplace_back(0);}
}
return output;
}

std::vector<Jet> OfflineProducerHelper::OppositeEtaJetPair(std::vector<Jet> jets){
//Initialize DEta
int id;
float Eta1Eta2= 0; bool foundpair=false;
std::vector<Jet> outputJets;
//Order the jets py pt
stable_sort(jets.begin(), jets.end(), [](const Jet & a, const Jet & b) -> bool
{ return ( get_property(a, Jet_pt ) >  get_property(b, Jet_pt)  );});
//Find the j1 (highest pt jet) and j2 (the jet with highest pt and opposite eta sign)
for (uint y=1;y<jets.size();y++ )
{
        Eta1Eta2 = jets.at(0).P4().Eta()*jets.at(y).P4().Eta();
        if (Eta1Eta2<0)
        {
            id = y; foundpair=true;
        }
        if(foundpair) break;
}
if(!foundpair) return outputJets;
outputJets = {{*(jets.begin()+0),*(jets.begin()+id)}}; return outputJets;
}

std::vector<Jet> OfflineProducerHelper::HighestPtJetPair(std::vector<Jet> jets){
std::vector<Jet> outputJets;
//Order the jets py pt
stable_sort(jets.begin(), jets.end(), [](const Jet & a, const Jet & b) -> bool
{ return ( get_property(a, Jet_pt ) >  get_property(b, Jet_pt)  );});
//Find the j1 (highest pt jet) and j2 (the jet with highest pt and opposite eta sign)
outputJets = {{*(jets.begin()+0),*(jets.begin()+1)}}; return outputJets;
}

////////////-----FUNCTIONS FOR PRESELECTION OF EVENTS FOR NON-RESONANT ANALYSIS - END


void OfflineProducerHelper::initializeTriggerMatching(OutputTree &ot)
{
    for( const auto & triggerRequirements : any_cast<std::map<std::string, std::map< std::pair<int,int>, int > > >(parameterList_->at("TriggerObjectAndMinNumberMap")) )
    {
        ot.declareUserIntBranch(triggerRequirements.first+"_ObjectMatched", 0);
        ot.declareUserIntBranch(triggerRequirements.first+"_Fired", 0);

        for(const auto & triggerObject : triggerRequirements.second)
        {
            if(mapTriggerObjectIdAndFilter_.find(triggerObject.first.first) == mapTriggerObjectIdAndFilter_.end())
            {
                mapTriggerObjectIdAndFilter_[triggerObject.first.first] = vector<int>();
            }
            //Adding only the ones not already in there
            if(std::find(mapTriggerObjectIdAndFilter_[triggerObject.first.first].begin(),mapTriggerObjectIdAndFilter_[triggerObject.first.first].end(),triggerObject.first.second) == mapTriggerObjectIdAndFilter_[triggerObject.first.first].end())
                mapTriggerObjectIdAndFilter_[triggerObject.first.first].emplace_back(triggerObject.first.second);
        }
    }
}


bool OfflineProducerHelper::checkTriggerObjectMatching(std::vector<std::string> listOfPassedTriggers, OutputTree &ot)
{

    std::map<std::string, std::map< std::pair<int,int>, int > > triggerObjectAndMinNumberMap;
    std::map<std::string, bool> triggerResult;
    bool triggerMatched = false;

    for( const auto & triggerRequirements : any_cast<std::map<std::string, std::map< std::pair<int,int>, int > > >(parameterList_->at("TriggerObjectAndMinNumberMap")) )
    {
        triggerResult[triggerRequirements.first] = true;
        if(std::find(listOfPassedTriggers.begin(),listOfPassedTriggers.end(),triggerRequirements.first) == listOfPassedTriggers.end()) // triggers not fired
        {
            triggerResult[triggerRequirements.first] = false;
            continue;
        }

        for ( const auto & requiredNumberOfObjects : triggerRequirements.second) // triggers fired
        {
            if(mapTriggerMatching_.find(requiredNumberOfObjects.first)==mapTriggerMatching_.end()) triggerResult[triggerRequirements.first] = false;   // Object not found
            if(mapTriggerMatching_[requiredNumberOfObjects.first] < requiredNumberOfObjects.second) triggerResult[triggerRequirements.first]  = false; // Number of object not enought
        }

        for(const auto & triggerChecked : triggerResult) //If at least one of the trigger was found return true
        {
            if(triggerChecked.second){
                ot.userInt(triggerChecked.first+"_ObjectMatched") = 1;
                triggerMatched = true;
            }
        }
    }

    return triggerMatched; // no matching trigger found among the fired ones
}

void OfflineProducerHelper::calculateTriggerMatching(const std::vector< std::unique_ptr<Candidate> > &candidateList, NanoAODTree& nat)
{
    if(debug) std::cout<<"Matching triggers, Objects found:\n";
    if(debug) std::cout<<"\t\tPt\t\tEta\t\tPhi\t\tBit\t\tMatchedJetId\n";

    mapTriggerMatching_.clear();
//std::cout<<"culo6.1.1\n";

    for (uint trigObjIt = 0; trigObjIt < *(nat.nTrigObj); ++trigObjIt) //for over all trigger objects
    {
        int triggerObjectId = nat.TrigObj_id.At(trigObjIt);
//std::cout<<"culo6.1.2\n";
//std::cout<<triggerObjectId<<std::endl;

        if(mapTriggerObjectIdAndFilter_.find(triggerObjectId) != mapTriggerObjectIdAndFilter_.end()) //check if the object particle id is needed
        {
//std::cout<<"culo6.1.3\n";
            bool newCandidate = true;
            bool isNeeded = false;

            int triggerFilterBitSum = nat.TrigObj_filterBits.At(trigObjIt);
            float triggerObjectPt   = nat.TrigObj_pt        .At(trigObjIt);
            float triggerObjectEta  = nat.TrigObj_eta       .At(trigObjIt);
            float triggerObjectPhi  = nat.TrigObj_phi       .At(trigObjIt);
            int candidateIdx=-1;

            for(const auto & filterBit : mapTriggerObjectIdAndFilter_[triggerObjectId]) //loop over all the needed filters
            {
//std::cout<<"culo6.1.4\n";
                //std::cout<<"!!! - "<<filterBit<<std::endl;
                if( (triggerFilterBitSum >> filterBit) & 0x1 ) //check object passes the filter
                {
                    if(!isNeeded) isNeeded=true;
                    if(newCandidate)
                    {
                        if(debug) std::cout<< std::fixed << std::setprecision(3) <<"\t\t"<<triggerObjectPt<<"\t\t"<<triggerObjectEta<<"\t\t"<<triggerObjectPhi<<"\t\t"<<filterBit;
                        newCandidate=false;
                    }
                    else if(debug) std::cout<<" "<<filterBit;

//std::cout<<"culo6.1.5\n";
                    //std::cout<<"!!!~~ - "<<filterBit<<std::endl;
                    float deltaR = 1024; //easy to do square root
                    int tmpCandidateIdx=-1;
                    for(const auto & candidate : candidateList) //loop to find best Candidate matching DeltaR
                    {
                        if(candidate->getCandidateTypeId() != triggerObjectId) continue; // Skip different particles
                        //std::cout<<"!!!~~~~ - "<<filterBit<<std::endl;

                        float candidateEta    = candidate->P4().Eta ();
                        float candidatePhi    = candidate->P4().Phi ();
                        float tmpdeltaR       = (candidateEta - triggerObjectEta)*(candidateEta - triggerObjectEta) + deltaPhi(candidatePhi,triggerObjectPhi)*deltaPhi(candidatePhi,triggerObjectPhi);
                        //std::cout<<candidate->getIdx()<< "!!!~~~~candidateEta - "<<candidateEta<<std::endl;

                        if(tmpdeltaR < deltaR)
                        {
                            deltaR = tmpdeltaR;
                            tmpCandidateIdx=candidate->getIdx();

                        }
                        //std::cout<<"!!!~~~~DeltaR - "<<deltaR<<std::endl;

                    }

                    if(sqrt(deltaR) < any_cast<float>(parameterList_->at("MaxDeltaR"))) // check if a matching was found
                    {
                        std::pair<int,int> particleAndFilter(triggerObjectId,filterBit);
                        if(mapTriggerMatching_.find(particleAndFilter) == mapTriggerMatching_.end())
                        {
                            mapTriggerMatching_[particleAndFilter] = 0;
                        }
                        ++mapTriggerMatching_[particleAndFilter];
                        candidateIdx=tmpCandidateIdx;
                        //std::cout<<triggerObjectId<<" "<<filterBit<<" "<<mapTriggerMatching_[particleAndFilter]<<std::endl;
                    }
                }
            }
            if(isNeeded) if(debug)  std::cout <<"\t\t"<<candidateIdx<<std::endl;
        }
    }

    return;
}


bool OfflineProducerHelper::select_gen_HH (NanoAODTree& nat, EventInfo& ei)
{
    bool all_ok = true;
    for (uint igp = 0; igp < *(nat.nGenPart); ++igp)
    {
        GenPart gp (igp, &nat);
        if (abs(get_property(gp, GenPart_pdgId)) != 25) continue;
        // bool isFirst = checkBit(get_property(gp, GenPart_statusFlags), 12); // 12: isFirstCopy
        // if (!isFirst) continue;
        if (gp.isFirstCopy())
        {
            if (!assign_to_uninit(gp, {&ei.gen_H1, &ei.gen_H2} )) {
                //cout << "** [WARNING] : select_gen_HH : more than two Higgs found" << endl;
                all_ok = false;
            }

            // // assign
            // if      (!ei.gen_H1) ei.gen_H1 = gp;
            // else if (!ei.gen_H2) ei.gen_H2 = gp;
            // else{
            //     cout << "** [WARNING] : select_gen_HH : more than two Higgs found" << endl;
            //     return false;
            // }
        }
        if (gp.isLastCopy())
        {
            if (!assign_to_uninit(gp, {&ei.gen_H1_last, &ei.gen_H2_last} )) {
                //cout << "** [WARNING] : select_gen_HH : more than two Higgs found (last copy)" << endl;
                all_ok = false;
            }

            // assign
            // if      (!ei.gen_H1_last) ei.gen_H1_last = gp;
            // else if (!ei.gen_H2_last) ei.gen_H2_last = gp;
            // else{
            //     cout << "** [WARNING] : select_gen_HH : more than two Higgs found (last copy)" << endl;
            //     return false;
            // }
        }
    }

    if (!ei.gen_H1 || !ei.gen_H2){
//        cout << "** [WARNING] : select_gen_HH : didn't find two Higgs : "
//             << std::boolalpha
//             << "H1 :" << ei.gen_H1.is_initialized()
//             << "H2 :" << ei.gen_H2.is_initialized()
//             << std::noboolalpha
//             << endl;
        all_ok = false;
    }
    return all_ok;
}


bool OfflineProducerHelper::select_gen_XYH (NanoAODTree& nat, EventInfo& ei)
{
    // pdgId : X = 45, Y = 35, H = 25

    bool all_ok = true;
    for (uint igp = 0; igp < *(nat.nGenPart); ++igp)
    {
        GenPart gp (igp, &nat);
        int apdgId = abs(get_property(gp, GenPart_pdgId));
        if (apdgId != 45 &&  apdgId != 35 && apdgId != 25) continue;
        // bool isFirst = checkBit(get_property(gp, GenPart_statusFlags), 12); // 12: isFirstCopy
        // if (!isFirst) continue;
        if (gp.isFirstCopy())
        {
            switch (apdgId) {
                case 45:
                    if (!ei.gen_X) ei.gen_X = gp;
                    else{
                       cout << "** [WARNING] : select_gen_XYH : more than one X found (first copy)" << endl; 
                       all_ok = false;
                    }
                    break;
                
                case 35:
                    if (!ei.gen_H1) ei.gen_H1 = gp;
                    else{
                       cout << "** [WARNING] : select_gen_XYH : more than one Y found (first copy)" << endl; 
                       all_ok = false;
                    }
                    break;
                
                case 25:
                    if (!ei.gen_H2) ei.gen_H2 = gp;
                    else{
                       cout << "** [WARNING] : select_gen_XYH : more than one H found (first copy)" << endl; 
                       all_ok = false;
                    }
                    break;
                
                default:
                    cout << "** [WARNING] : select_gen_XYH : wrong gen particle pdgId handled (first copy) (should never happen) : " << apdgId << endl; 
                    all_ok = false;
                    break;
            }
        }
        
        if (gp.isLastCopy())
        {
            switch (apdgId) {
                case 45:
                    if (!ei.gen_X_last) ei.gen_X_last = gp;
                    else{
                       cout << "** [WARNING] : select_gen_XYH : more than one X found (last copy)" << endl; 
                       all_ok = false;
                    }
                    break;
                
                case 35:
                    if (!ei.gen_H1_last) ei.gen_H1_last = gp;
                    else{
                       cout << "** [WARNING] : select_gen_XYH : more than one Y found (last copy)" << endl; 
                       all_ok = false;
                    }
                    break;
                
                case 25:
                    if (!ei.gen_H2_last) ei.gen_H2_last = gp;
                    else{
                       cout << "** [WARNING] : select_gen_XYH : more than one H found (last copy)" << endl; 
                       all_ok = false;
                    }
                    break;
                
                default:
                    cout << "** [WARNING] : select_gen_XYH : wrong gen particle pdgId handled (last copy) (should never happen) : " << apdgId << endl; 
                    all_ok = false;
                    break;
            }
        }
    }

    if (!ei.gen_X || !ei.gen_H1 || !ei.gen_H2){
       cout << "** [WARNING] : select_gen_XYH : didn't find the three gen cands (first copy): "
            << std::boolalpha
            << " X :" << ei.gen_X.is_initialized()
            << " Y :" << ei.gen_H1.is_initialized()
            << " H :" << ei.gen_H2.is_initialized()
            << std::noboolalpha
            << endl;
        all_ok = false;
    }

    if (!ei.gen_X_last || !ei.gen_H1_last || !ei.gen_H2_last){
       cout << "** [WARNING] : select_gen_XYH : didn't find the three gen cands (last copy): "
            << std::boolalpha
            << " X :" << ei.gen_X_last.is_initialized()
            << " Y :" << ei.gen_H1_last.is_initialized()
            << " H :" << ei.gen_H2_last.is_initialized()
            << std::noboolalpha
            << endl;
        all_ok = false;
    }

    // sometimes I get a -nan mass for mX, use this to debug
    // Solution: when the X is produced parallel to the beam line, it has pt = 0 and infinite eta
    // --> TLV gives nonsense results
    // --> directly access the properties from the NanoAOD with get_property
    // if ((int) ei.gen_X->P4().M() < 0) dump_gen_part (nat, false);

    return all_ok;
}


bool OfflineProducerHelper::select_gen_bb_bb (NanoAODTree& nat, EventInfo& ei)
{
    if (!ei.gen_H1 || !ei.gen_H2)
    {
        //cout << "** [WARNING] : select_gen_bb_bb : you need to search for H1 and H2 before" << endl;
        return false;
    }

    bool all_ok = true;
    for (uint igp = 0; igp < *(nat.nGenPart); ++igp)
    {
        GenPart gp (igp, &nat);
        if (abs(get_property(gp, GenPart_pdgId)) != 5) continue;
        // bool isFirst = checkBit(get_property(gp, GenPart_statusFlags), 12); // 12: isFirstCopy
        // if (!isFirst) continue;
        if (!gp.isFirstCopy()) continue;

        int idxMoth = get_property(gp, GenPart_genPartIdxMother);
        if (idxMoth == ei.gen_H1_last->getIdx())
        {
            if (!assign_to_uninit(gp, {&ei.gen_H1_b1, &ei.gen_H1_b2} )) {
                //cout << "** [WARNING] : select_gen_bb_bb : more than two b from H1 found" << endl;
                all_ok = false;
            }
            // if      (!ei.gen_H1_b1) ei.gen_H1_b1 = gp;
            // else if (!ei.gen_H1_b2) ei.gen_H1_b2 = gp;
            // else{
            //     cout << "** [WARNING] : select_gen_bb_bb : more than two b from H1 found" << endl;
            //     all_ok = false;
            // }
        }
        else if (idxMoth == ei.gen_H2_last->getIdx())
        {
            if (!assign_to_uninit(gp, {&ei.gen_H2_b1, &ei.gen_H2_b2} )) {
                //cout << "** [WARNING] : select_gen_bb_bb : more than two b from H2 found" << endl;
                all_ok = false;
            }

            // if      (!ei.gen_H2_b1) ei.gen_H2_b1 = gp;
            // else if (!ei.gen_H2_b2) ei.gen_H2_b2 = gp;
            // else{
            //     cout << "** [WARNING] : select_gen_bb_bb : more than two b from H2 found" << endl;
            //     all_ok = false;
            // }
        }
        else
        {
            // cout << "** [WARNING] : select_gen_bb_bb : found a b quark of idx " << gp.getIdx() << " and mother " << idxMoth
            //      << " that does not match last H1 H2 idx " << ei.gen_H1_last->getIdx() << " " << ei.gen_H2_last->getIdx()
            //      << endl;
            //      all_ok = false;
            // possibly something that has b --> b + g --> b + (other stuff)
        }
    }
    if (!all_ok)
    {
        cout << "** [DEBUG] : select_gen_bb_bb : something went wrong, dumping gen parts" << endl;
        dump_gen_part(nat, true);
    }

/*
// std::cout<<"culo1\n";
    //match generated
    std::vector<double> candidatePhi {ei.H1_b1->P4().Phi()    , ei.H1_b2->P4().Phi()    , ei.H2_b1->P4().Phi()    , ei.H2_b2->P4().Phi()    };
    std::vector<double> candidateEta {ei.H1_b1->P4().Eta()    , ei.H1_b2->P4().Eta()    , ei.H2_b1->P4().Eta()    , ei.H2_b2->P4().Eta()    };
    std::vector<double> genBJetPhi   {ei.gen_H1_b1->P4().Phi(), ei.gen_H1_b2->P4().Phi(), ei.gen_H2_b1->P4().Phi(), ei.gen_H2_b2->P4().Phi()};
    std::vector<double> genBJetEta   {ei.gen_H1_b1->P4().Eta(), ei.gen_H1_b2->P4().Eta(), ei.gen_H2_b1->P4().Eta(), ei.gen_H2_b2->P4().Eta()};
    std::vector<bool> isCandidaterMatched(4,false);
    std::vector<int>  matchedCandidate(4,-1);
// std::cout<<"culo2\n";

    for(uint8_t itGenBJet=0; itGenBJet<4; ++itGenBJet)
    {
// std::cout<<"culo3\n";
        double deltaR = 1024;
        int    candidateMatched=-1;
        for(uint8_t itCandidate=0; itCandidate<4; ++itCandidate)
        {
// std::cout<<"culo4\n";
            if(isCandidaterMatched[itCandidate]) continue;
            double tmpDeltaR = deltaPhi(candidatePhi[itCandidate],genBJetPhi[itGenBJet])*deltaPhi(candidatePhi[itCandidate],genBJetPhi[itGenBJet]) + (candidateEta[itCandidate]-genBJetEta[itGenBJet])*(candidateEta[itCandidate]-genBJetEta[itGenBJet]);
            if(tmpDeltaR<deltaR)
            {
// std::cout<<"culo5\n";
                deltaR = tmpDeltaR;
                candidateMatched = itCandidate;
            }
        }
        if(deltaR< (0.3*0.3))
        {
// std::cout<<"culo6\n";
            isCandidaterMatched[candidateMatched] = true;
            matchedCandidate[itGenBJet] = candidateMatched;
        }

    }

// std::cout<<"culo7\n";
    ei.gen_H1_b1_matchedflag = matchedCandidate[0];
    ei.gen_H1_b2_matchedflag = matchedCandidate[1];
    ei.gen_H2_b1_matchedflag = matchedCandidate[2];
    ei.gen_H2_b2_matchedflag = matchedCandidate[3];
    // std::cout<<ei.gen_H1_b1_matchedflag.get()<<"  "<<ei.gen_H1_b2_matchedflag.get()<<"  "<<ei.gen_H2_b1_matchedflag.get()<<"  "<<ei.gen_H2_b2_matchedflag.get()<<std::endl;
// std::cout<<"culo8\n";
*/

    return all_ok;
}

bool OfflineProducerHelper::select_gen_VBF_partons (NanoAODTree& nat, EventInfo& ei)
{

    bool all_ok = true;

    for (uint igp = 0; igp < *(nat.nGenPart); ++igp)
    {
        GenPart gp (igp, &nat);
        if (abs(get_property(gp, GenPart_pdgId)) >= 6) continue; // only light quarks + b
        // auto flags = get_property(gp, GenPart_statusFlags);
        // search for ingoing partons
        // if (!) continue; // 12: isFirstCopy
        // search for outgoing partons

        if (get_property(gp, GenPart_status) == 21) // incoming VBF parton
        {
            // if      (!ei.gen_q1_in) ei.gen_q1_in = gp;
            // else if (!ei.gen_q2_in) ei.gen_q2_in = gp;
            // else{
            //     cout << "** [WARNING] : select_gen_VBF_partons : more than two incoming partons found" << endl;
            //     all_ok = false;
            // }

            if (!assign_to_uninit(gp, {&ei.gen_q1_in, &ei.gen_q2_in} )) {
                cout << "** [WARNING] : select_gen_VBF_partons : more than two incoming partons found" << endl;
                all_ok = false;
            }
        }

        else
        {
            // it appears that in the VBF sample, the first objects in the list are the VBF ingoing partons
            // and that the next ones are the outgoing partons, 1st copy
            // so checking that idxMother == 0 should be enough for the VBF sample in use (checked on 80X on 19/01/2018)

            // FIXME: how to ensure the correspondence in_1 --> out_1, in_2 --> out_2 ?
            // unfortunately there is no valid gen info associated
            if (get_property(gp, GenPart_genPartIdxMother) == 0)
            {
                // if      (!ei.gen_q1_out) ei.gen_q1_out = gp;
                // else if (!ei.gen_q2_out) ei.gen_q2_out = gp;
                // else {
                //     cout << "** [WARNING] : select_gen_VBF_partons : more than two outgoing partons found" << endl;
                //     all_ok = false;
                // }
                if (!assign_to_uninit(gp, {&ei.gen_q1_out, &ei.gen_q2_out} )) {
                    cout << "** [WARNING] : select_gen_VBF_partons : more than two outgoing partons found" << endl;
                    all_ok = false;
                }
            }

            // the last copy of the outgoing parton can be checked by verifying two conditions:
            // 1. is last copy
            // 2. its mother is one of the outgoing VBF partons at ME. As they are before in the gen part list, at this point they should already have been identified
            // 23/01/2018: commenting out the outgoing partons, as it is not clear if this information makes sense
            else
            {
                // bool good_last = gp.isLastCopy();
                // if (ei.gen_q1_out && ei.gen_q2_out)
                // {
                //     // auto moth_idx = get_property(gp, GenPart_genPartIdxMother);
                //     auto moth_idx = recursive_gen_mother_idx(gp); // make a recursive search, if intermediate copies where radiated
                //     good_last = good_last && ((moth_idx == ei.gen_q1_out->getIdx()) || (moth_idx == ei.gen_q2_out->getIdx()));
                // }
                // else
                //     good_last = false;

                // // now check and save
                // if (good_last)
                // {
                //     if      (!ei.gen_q1_out_lastcopy) ei.gen_q1_out_lastcopy = gp;
                //     else if (!ei.gen_q2_out_lastcopy) ei.gen_q2_out_lastcopy = gp;
                //     else {
                //         cout << "** [WARNING] : select_gen_VBF_partons : more than two outgoing partons (last copy) found" << endl;
                //         all_ok = false;
                //     }
                // }
            }
        }
    }

    // quality checks

    if (!ei.gen_q1_in || !ei.gen_q2_in){
    cout << "** [WARNING] : select_gen_VBF_partons : didn't find two incoming partons : "
         << std::boolalpha
         << " q1_in :" << ei.gen_q1_in.is_initialized()
         << " q2_in :" << ei.gen_q2_in.is_initialized()
         << std::noboolalpha
         << endl;
        all_ok = false;
    }

    if (!ei.gen_q1_out || !ei.gen_q2_out){
    cout << "** [WARNING] : select_gen_VBF_partons : didn't find two outgoing partons : "
         << std::boolalpha
         << " q1_out :" << ei.gen_q1_out.is_initialized()
         << " q2_out :" << ei.gen_q2_out.is_initialized()
         << std::noboolalpha
         << endl;
        all_ok = false;
    }

    // if (!ei.gen_q1_out_lastcopy || !ei.gen_q2_out_lastcopy){
    // cout << "** [WARNING] : select_gen_VBF_partons : didn't find two outgoing partons (last copy) : "
    //      << std::boolalpha
    //      << " q1_out_lastcopy :" << ei.gen_q1_out_lastcopy.is_initialized()
    //      << " q2_out_lastcopy :" << ei.gen_q2_out_lastcopy.is_initialized()
    //      << std::noboolalpha
    //      << endl;
    //     all_ok = false;
    // }

    // // print a debug
    // if (!all_ok)
    // {
    //     cout << "** [DEBUG] select_gen_VBF_partons : something went wrong, dumping gen parts" << endl;
    //     for (uint igp = 0; igp < *(nat.nGenPart); ++igp)
    //     {
    //         GenPart gp (igp, &nat);
    //         // if (abs(get_property(gp, GenPart_pdgId)) >= 6) continue; // only light quarks + b
    //         cout << boolalpha;
    //         cout << igp << " -- "
    //              << " pdgId: "      << setw(4)  << get_property(gp, GenPart_pdgId)
    //              << " pt: "         << setw(10) << get_property(gp, GenPart_pt)
    //              << " eta: "        << setw(10) << get_property(gp, GenPart_eta)
    //              << " phi: "        << setw(10) << get_property(gp, GenPart_phi)
    //              << " status: "     << setw(4)  << get_property(gp, GenPart_status)
    //              << " moth_idx: "   << setw(4)  << get_property(gp, GenPart_genPartIdxMother)
    //              << endl;
    //         cout << "    . Flags :" << endl
    //             << "       isPrompt :                           " << gp.isPrompt() << endl
    //             << "       isDecayedLeptonHadron :              " << gp.isDecayedLeptonHadron() << endl
    //             << "       isTauDecayProduct :                  " << gp.isTauDecayProduct() << endl
    //             << "       isPromptTauDecayProduct :            " << gp.isPromptTauDecayProduct() << endl
    //             << "       isDirectTauDecayProduct :            " << gp.isDirectTauDecayProduct() << endl
    //             << "       isDirectPromptTauDecayProduct :      " << gp.isDirectPromptTauDecayProduct() << endl
    //             << "       isDirectHadronDecayProduct :         " << gp.isDirectHadronDecayProduct() << endl
    //             << "       isHardProcess :                      " << gp.isHardProcess() << endl
    //             << "       fromHardProcess :                    " << gp.fromHardProcess() << endl
    //             << "       isHardProcessTauDecayProduct :       " << gp.isHardProcessTauDecayProduct() << endl
    //             << "       isDirectHardProcessTauDecayProduct : " << gp.isDirectHardProcessTauDecayProduct() << endl
    //             << "       fromHardProcessBeforeFSR :           " << gp.fromHardProcessBeforeFSR() << endl
    //             << "       isFirstCopy :                        " << gp.isFirstCopy() << endl
    //             << "       isLastCopy :                         " << gp.isLastCopy() << endl
    //             << "       isLastCopyBeforeFSR :                " << gp.isLastCopyBeforeFSR() << endl
    //             << endl;
    //         cout << noboolalpha;
    //     }
    //     cout << endl << endl;
    // } // end of debug block

    return all_ok;
}

void OfflineProducerHelper::dump_gen_part (NanoAODTree& nat, bool printFlags)
{
    // print a debug
    cout << "** [DEBUG] dumping gen parts of event " << *(nat.event) << endl;
    cout << boolalpha;
    for (uint igp = 0; igp < *(nat.nGenPart); ++igp)
    {
        GenPart gp (igp, &nat);
        // if (abs(get_property(gp, GenPart_pdgId)) >= 6) continue; // only light quarks + b
        cout << igp << " -- "
             << " pdgId: "      << setw(4)  << get_property(gp, GenPart_pdgId)
             << " pt: "         << setw(10) << get_property(gp, GenPart_pt)
             << " eta: "        << setw(10) << get_property(gp, GenPart_eta)
             << " phi: "        << setw(10) << get_property(gp, GenPart_phi)
             << " status: "     << setw(4)  << get_property(gp, GenPart_status)
             << " moth_idx: "   << setw(4)  << get_property(gp, GenPart_genPartIdxMother)
             << endl;
        if (printFlags)
        {
            cout << "    . Flags :" << endl
                << "       isPrompt :                           " << gp.isPrompt() << endl
                << "       isDecayedLeptonHadron :              " << gp.isDecayedLeptonHadron() << endl
                << "       isTauDecayProduct :                  " << gp.isTauDecayProduct() << endl
                << "       isPromptTauDecayProduct :            " << gp.isPromptTauDecayProduct() << endl
                << "       isDirectTauDecayProduct :            " << gp.isDirectTauDecayProduct() << endl
                << "       isDirectPromptTauDecayProduct :      " << gp.isDirectPromptTauDecayProduct() << endl
                << "       isDirectHadronDecayProduct :         " << gp.isDirectHadronDecayProduct() << endl
                << "       isHardProcess :                      " << gp.isHardProcess() << endl
                << "       fromHardProcess :                    " << gp.fromHardProcess() << endl
                << "       isHardProcessTauDecayProduct :       " << gp.isHardProcessTauDecayProduct() << endl
                << "       isDirectHardProcessTauDecayProduct : " << gp.isDirectHardProcessTauDecayProduct() << endl
                << "       fromHardProcessBeforeFSR :           " << gp.fromHardProcessBeforeFSR() << endl
                << "       isFirstCopy :                        " << gp.isFirstCopy() << endl
                << "       isLastCopy :                         " << gp.isLastCopy() << endl
                << "       isLastCopyBeforeFSR :                " << gp.isLastCopyBeforeFSR() << endl
                << endl;
        }
    }
    cout << noboolalpha;
    cout << endl << endl;
}

// bool OfflineProducerHelper::checkBit (int number, int bitpos)
// {
//     bool res = number & (1 << bitpos);
//     return res;
// }

int OfflineProducerHelper::recursive_gen_mother_idx(const GenPart& gp, bool stop_at_moth_zero)
{
    int imoth = get_property(gp, GenPart_genPartIdxMother);
    if (imoth < 0)
        return gp.getIdx();
    if (imoth == 0 && stop_at_moth_zero)
        return gp.getIdx();
    GenPart gp_moth (imoth, gp.getNanoAODTree());
    return recursive_gen_mother_idx(gp_moth, stop_at_moth_zero);
}