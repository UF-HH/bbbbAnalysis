#include "OfflineProducerHelper.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <stdlib.h>
#include <any>

#include "CompositeCandidate.h"
#include "Jet.h"
#include "Electron.h"
#include "Muon.h"
#include "GenPart.h"
#include "HH4b_kinFit.h"
#include "TRandom.h"
#include "TMath.h"

#include "TMVA/GeneticAlgorithm.h"
#include "TMVA/GeneticFitter.h"
#include "TMVA/IFitterTarget.h"
#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Reader.h"

using namespace std;

const std::string OfflineProducerHelper::nominal_jes_syst_shift_name = "nominal";

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
    else if(objectsForCut == "IsolatedLeptons"){
        save_objects_for_cut = [=](NanoAODTree& nat, OutputTree &ot, EventInfo& ei) -> void {this -> save_IsolatedLeptons(nat, ot, ei);};
        ot.declareUserIntBranch("IsolatedElectron_Multiplicity", -1.);
        ot.declareUserFloatBranch("IsolatedElectron1_pt", -1.);
        ot.declareUserFloatBranch("IsolatedElectron1_MaxPfIso", -1.);
        ot.declareUserIntBranch("IsolatedElectron1_looseID", -1.);
        ot.declareUserIntBranch("IsolatedElectron1_mediumID",-1.);
        ot.declareUserIntBranch("IsolatedElectron1_tightID", -1.); 
        ot.declareUserFloatBranch("IsolatedElectron2_pt", -1.);
        ot.declareUserFloatBranch("IsolatedElectron2_MaxPfIso", -1.);
        ot.declareUserIntBranch("IsolatedElectron2_looseID", -1.);
        ot.declareUserIntBranch("IsolatedElectron2_mediumID",-1.);
        ot.declareUserIntBranch("IsolatedElectron2_tightID", -1.);
        ot.declareUserIntBranch("IsolatedMuon_Multiplicity", -1.);
        ot.declareUserFloatBranch("IsolatedMuon1_pt", -1.);
        ot.declareUserFloatBranch("IsolatedMuon1_MaxPfIso", -1.);
        ot.declareUserIntBranch("IsolatedMuon1_looseID", -1.);
        ot.declareUserIntBranch("IsolatedMuon1_mediumID",-1.);
        ot.declareUserIntBranch("IsolatedMuon1_tightID", -1.);  
        ot.declareUserFloatBranch("IsolatedMuon2_pt", -1.);
        ot.declareUserFloatBranch("IsolatedMuon2_MaxPfIso", -1.);
        ot.declareUserIntBranch("IsolatedMuon2_looseID", -1.);
        ot.declareUserIntBranch("IsolatedMuon2_mediumID",-1.);
        ot.declareUserIntBranch("IsolatedMuon2_tightID", -1.);      
    }

    return;

}

// objects to reject events with leptons 
void OfflineProducerHelper::save_IsolatedLeptons (NanoAODTree& nat, OutputTree &ot, EventInfo& ei){

    std::vector<Electron> IsolatedElectrons;
    IsolatedElectrons.reserve(*(nat.nElectron));
    std::vector<Muon> IsolatedMuons;
    IsolatedMuons.reserve(*(nat.nMuon));

    //Look for the good electrons in the event. 
    for (uint eIt = 0; eIt < *(nat.nElectron); ++eIt){
        Electron IsolatedElectron(eIt, &nat);
        if( abs(get_property(IsolatedElectron,Electron_eta)) < 1.2 && abs(get_property(IsolatedElectron,Electron_dz))   > any_cast<float>(parameterList_->at("ElectronMaxBarrelDz"))  ) continue;
        if( abs(get_property(IsolatedElectron,Electron_eta)) < 1.2 && abs(get_property(IsolatedElectron,Electron_dxy))  > any_cast<float>(parameterList_->at("ElectronMaxBarrelDxy")) ) continue;
        if( abs(get_property(IsolatedElectron,Electron_eta)) < 2.4 && abs(get_property(IsolatedElectron,Electron_dz))   > any_cast<float>(parameterList_->at("ElectronMaxEndcapDz"))  ) continue;
        if( abs(get_property(IsolatedElectron,Electron_eta)) < 2.4 && abs(get_property(IsolatedElectron,Electron_dxy))  > any_cast<float>(parameterList_->at("ElectronMaxEndcapDxy")) ) continue;
        if(     get_property(IsolatedElectron,Electron_pt)   < any_cast<float>(parameterList_->at("ElectronMinPt"))     ) continue;
        if( abs(get_property(IsolatedElectron,Electron_eta)) > any_cast<float>(parameterList_->at("ElectronMaxAbsEta")) ) continue;
        if( any_cast<int>(parameterList_->at("ElectronID"))==0 && !get_property(IsolatedElectron,Electron_mvaFall17V2Iso_WPL) ) continue;
        if( any_cast<int>(parameterList_->at("ElectronID"))==1 && !get_property(IsolatedElectron,Electron_mvaFall17V2Iso_WP90) ) continue;
        if( any_cast<int>(parameterList_->at("ElectronID"))==2 && !get_property(IsolatedElectron,Electron_mvaFall17V2Iso_WP80) ) continue;
        if(  get_property(IsolatedElectron,Electron_pfRelIso03_all    ) > any_cast<float>(parameterList_->at("ElectronMaxPfIso"))  ) continue;
        IsolatedElectrons.emplace_back(IsolatedElectron);
    }
    //Look for the good muons in the event. 
    for (uint eIt = 0; eIt < *(nat.nMuon); ++eIt){
        Muon IsolatedMuon(eIt, &nat);
        if( abs(get_property(IsolatedMuon,Muon_eta)) < 1.2 && abs(get_property(IsolatedMuon,Muon_dz))   > any_cast<float>(parameterList_->at("MuonMaxBarrelDz"))  ) continue;
        if( abs(get_property(IsolatedMuon,Muon_eta)) < 1.2 && abs(get_property(IsolatedMuon,Muon_dxy))  > any_cast<float>(parameterList_->at("MuonMaxBarrelDxy")) ) continue;
        if( abs(get_property(IsolatedMuon,Muon_eta)) < 2.4 && abs(get_property(IsolatedMuon,Muon_dz))   > any_cast<float>(parameterList_->at("MuonMaxEndcapDz"))  ) continue;
        if( abs(get_property(IsolatedMuon,Muon_eta)) < 2.4 && abs(get_property(IsolatedMuon,Muon_dxy))  > any_cast<float>(parameterList_->at("MuonMaxEndcapDxy")) ) continue;
        if(     get_property(IsolatedMuon,Muon_pt)    < any_cast<float>(parameterList_->at("MuonMinPt")) ) continue;
        if( abs(get_property(IsolatedMuon,Muon_eta))  > any_cast<float>(parameterList_->at("MuonMaxAbsEta")) ) continue;
        if( any_cast<int>(parameterList_->at("MuonID"))==0 && !get_property(IsolatedMuon,Muon_looseId)  ) continue;
        if( any_cast<int>(parameterList_->at("MuonID"))==1 && !get_property(IsolatedMuon,Muon_mediumId) ) continue;
        if( any_cast<int>(parameterList_->at("MuonID"))==2 && !get_property(IsolatedMuon,Muon_tightId)  ) continue;
        if( get_property(IsolatedMuon,Muon_pfRelIso04_all    ) > any_cast<float>(parameterList_->at("MuonMaxPfIso"))  ) continue;
        IsolatedMuons.emplace_back(IsolatedMuon);
    }

    //Save good electron properties
    ot.userInt("IsolatedElectron_Multiplicity")=IsolatedElectrons.size();
    if(IsolatedElectrons.size()>0){
        stable_sort(IsolatedElectrons.begin(), IsolatedElectrons.end(), [](const Electron & a, const Electron & b) -> bool
        {
            return ( a.P4().Pt() > b.P4().Pt() );
        });
        ot.userFloat("IsolatedElectron1_pt")=IsolatedElectrons.at(0).P4().Pt();
        ot.userFloat("IsolatedElectron1_MaxPfIso")=get_property(IsolatedElectrons.at(0),Electron_pfRelIso03_all);
        ot.userInt( "IsolatedElectron1_looseID")  = get_property(IsolatedElectrons.at(0),Electron_mvaFall17V2Iso_WPL        );
        ot.userInt( "IsolatedElectron1_mediumID") = get_property(IsolatedElectrons.at(0),Electron_mvaFall17V2Iso_WP90       );
        ot.userInt( "IsolatedElectron1_tightID")  = get_property(IsolatedElectrons.at(0),Electron_mvaFall17V2Iso_WP80       );
        if(IsolatedElectrons.size()>1){
             ot.userFloat("IsolatedElectron2_pt")=IsolatedElectrons.at(1).P4().Pt();
             ot.userFloat("IsolatedElectron2_MaxPfIso")=get_property(IsolatedElectrons.at(1),Electron_pfRelIso03_all);
             ot.userInt( "IsolatedElectron2_looseID")  = get_property(IsolatedElectrons.at(1),Electron_mvaFall17V2Iso_WPL        );
             ot.userInt( "IsolatedElectron2_mediumID") = get_property(IsolatedElectrons.at(1),Electron_mvaFall17V2Iso_WP90       );
             ot.userInt( "IsolatedElectron2_tightID")  = get_property(IsolatedElectrons.at(1),Electron_mvaFall17V2Iso_WP80       );
        }
    }
    //Save good muons properties
    ot.userInt("IsolatedMuon_Multiplicity")=IsolatedMuons.size();
    if(IsolatedMuons.size()>0){
        stable_sort(IsolatedMuons.begin(), IsolatedMuons.end(), [](const Muon & a, const Muon & b) -> bool
        {
            return ( a.P4().Pt() > b.P4().Pt() );
        }); 
        ot.userFloat("IsolatedMuon1_pt")       = IsolatedMuons.at(0).P4().Pt();
        ot.userFloat("IsolatedMuon1_MaxPfIso") = get_property(IsolatedMuons.at(0),Muon_pfRelIso04_all );
        ot.userInt( "IsolatedMuon1_looseID")  = get_property(IsolatedMuons.at(0),Muon_looseId        );
        ot.userInt( "IsolatedMuon1_mediumID") = get_property(IsolatedMuons.at(0),Muon_mediumId       );
        ot.userInt( "IsolatedMuon1_tightID")  = get_property(IsolatedMuons.at(0),Muon_tightId        );
        if(IsolatedMuons.size()>1){
             ot.userFloat("IsolatedMuon2_pt")       = IsolatedMuons.at(1).P4().Pt();
             ot.userFloat("IsolatedMuon2_MaxPfIso") = get_property(IsolatedMuons.at(1),Muon_pfRelIso04_all );
             ot.userInt( "IsolatedMuon2_looseID")   = get_property(IsolatedMuons.at(1),Muon_looseId        );
             ot.userInt( "IsolatedMuon2_mediumID")  = get_property(IsolatedMuons.at(1),Muon_mediumId       );
             ot.userInt( "IsolatedMuon2_tightID")   = get_property(IsolatedMuons.at(1),Muon_tightId        );
        }
    }

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

void OfflineProducerHelper::initializeObjectsL1PrefiringForScaleFactors(OutputTree &ot)
{
    //save_objects_for_l1prefiring = [=](NanoAODTree& nat, OutputTree &ot, EventInfo& ei) -> void {this -> compute_scaleFactors_l1prefiring(nat, ot, ei);};
    ot.declareUserFloatBranch("L1PreFiringWeight_Nom", 1.);
    ot.declareUserFloatBranch("L1PreFiringWeight_Up",  1.);
    ot.declareUserFloatBranch("L1PreFiringWeight_Dn",  1.);
    return;
}

void OfflineProducerHelper::initializeObjectsBJetForScaleFactors(OutputTree &ot){

    string scaleFactorsMethod = any_cast<string>(parameterList_->at("BTagScaleFactorMethod"));

    string eventselection = any_cast<string>(parameterList_->at("PreselectionCut"));

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
        string btaggingfile,btagger;
        if(eventselection=="VBFJetCut")
        {
          if( any_cast<bool>(parameterList_->at("NewestBtaggingAlgorithm"))  )
          {
            btagger= "DeepJet";btaggingfile = any_cast<string>(parameterList_->at("BJetScaleFactorsFileAlternative"));
          }
          else
          {
            btagger= "DeepCSV";btaggingfile = any_cast<string>(parameterList_->at("BJetScaleFactorsFile"));
          }
        }
        else{
            btagger= "DeepCSV";btaggingfile = any_cast<string>(parameterList_->at("BJetScaleFactorsFile"));
        }

        BTagCalibration btagCalibration(btagger,btaggingfile);
        btagCalibrationReader_lightJets_ = new BTagCalibrationReader(BTagEntry::OP_MEDIUM,"central",{"up", "down"});
        btagCalibrationReader_cJets_     = new BTagCalibrationReader(BTagEntry::OP_MEDIUM,"central",{"up", "down"});
        btagCalibrationReader_bJets_     = new BTagCalibrationReader(BTagEntry::OP_MEDIUM,"central",{"up", "down"});
        btagCalibrationReader_lightJets_->load(btagCalibration, BTagEntry::FLAV_UDSG, "incl"  );
        btagCalibrationReader_cJets_    ->load(btagCalibration, BTagEntry::FLAV_C   , "comb");//mujets is for top physics in dilepton channel
        btagCalibrationReader_bJets_    ->load(btagCalibration, BTagEntry::FLAV_B   , "comb");//mujets is for top physics in dilepton channel 

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

void OfflineProducerHelper::CalculateL1prefiringScaleFactor(NanoAODTree& nat,OutputTree &ot, EventInfo& ei)
{

   if( parameterList_->find("L1PrefiringSFMethod") != parameterList_->end()) //only defined when it is MC
   {
       if(any_cast<string>(parameterList_->at("L1PrefiringSFMethod")) == "Standard" && any_cast<string>(parameterList_->at("datasetname")) != "2018")
       {
       ot.userFloat("L1PreFiringWeight_Nom") = *(nat.L1PreFiringWeight_Nom);
       ot.userFloat("L1PreFiringWeight_Up")  = *(nat.L1PreFiringWeight_Up);
       ot.userFloat("L1PreFiringWeight_Dn")  = *(nat.L1PreFiringWeight_Dn);
       }
       else{
        //do nothing
       }
   }
   else{
        //do nothing
   }

   return;
}

void OfflineProducerHelper::CalculateBtagScaleFactor(const std::vector<Jet> presel_bjets,NanoAODTree& nat,OutputTree &ot){
    if(parameterList_->find("BTagScaleFactorMethod") != parameterList_->end()){ //is it a MC event
        const string BJetcaleFactorsMethod = any_cast<string>(parameterList_->at("BTagScaleFactorMethod"));
        if(BJetcaleFactorsMethod == "FourBtag_ScaleFactor")
        {
            float bminimumDeepCSVAccepted           = any_cast<float>(parameterList_->at("bMinDeepCSV"  ));
            float bminimumDeepJetAccepted           = any_cast<float>(parameterList_->at("bMinDeepJet"  ));
            std::vector<Jet> jetsforbSF;
            if(any_cast<bool>(parameterList_->at("NewestBtaggingAlgorithm")))
            {
                  for (uint i=0;i<presel_bjets.size();i++ ){ if( get_property( presel_bjets.at(i) ,Jet_btagDeepFlavB) >= bminimumDeepJetAccepted) jetsforbSF.push_back(presel_bjets.at(i));}
            }
            else
            {
                  for (uint i=0;i<presel_bjets.size();i++ ){ if( get_property( presel_bjets.at(i) ,Jet_btagDeepB) >= bminimumDeepCSVAccepted) jetsforbSF.push_back(presel_bjets.at(i));}
            }

            compute_scaleFactors_fourBtag_eventScaleFactor(jetsforbSF,nat,ot);
        }
    }
    return;
}
// ----------------- Compute scaleFactors - END ----------------- //


// ----------------- Compute shape weights - BEGIN ----------------- //

void OfflineProducerHelper::initializeObjectsForEventWeight(OutputTree &ot, SkimEffCounter &ec, std::string PUWeightFileName, float crossSection)
{
    sampleCrossSection_ = 1;

    string weightsMethod = any_cast<string>(parameterList_->at("WeightMethod"));

    string dataset  = any_cast<string>(parameterList_->at("datasetname"));

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

float OfflineProducerHelper::calculateEventWeight_AllWeights(NanoAODTree& nat, EventInfo& ei,  OutputTree &ot, SkimEffCounter &ec)
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

    float eventWeight = 1.; 
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


bool OfflineProducerHelper::checkEventWeight(NanoAODTree& nat, OutputTree &ot, SkimEffCounter &ec)
{

    float eventWeight = 1.; 
    float tmpWeight = 1.;
    bool pass;
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
    eventWeight *= tmpWeight;
    //genWeight (no weight variations)
    branchName = "genWeight";
    float genWeight = *(nat.genWeight);
    tmpWeight = genWeight;
    tmpWeight = tmpWeight==0 ? 1 : tmpWeight; //set to 1 if weight is 0
    eventWeight *= tmpWeight;
    // LHEPdfWeight
    branchName = "LHEPdfWeight";
    tmpWeight = 1.;
    tmpWeight = tmpWeight==0 ? 1 : tmpWeight; //set to 1 if weight is 0
    eventWeight *= tmpWeight;
    // LHEScaleWeight
    branchName = "LHEScaleWeight";
    tmpWeight = 1;
    tmpWeight = tmpWeight==0 ? 1 : tmpWeight; //set to 1 if weight is 0
    eventWeight *= tmpWeight;
    // PSWeight
    branchName = "PSWeight";
    tmpWeight = 1;
    tmpWeight = tmpWeight==0 ? 1 : tmpWeight; //set to 1 if weight is 0
    eventWeight *= tmpWeight;

    //Check only for NLO-HH samples
    if( abs(eventWeight) > 0.5) pass = false;
    else pass = true;

    return pass;

}



// ----------------- Compute shape weights - END ----------------- //


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
        jetResolutionScaleFactor_ = std::unique_ptr<JME::JetResolutionScaleFactor>(new JME::JetResolutionScaleFactor(any_cast<string>(parameterList_->at("JERScaleFactorFile"))));
        jetResolution_            = std::unique_ptr<JME::JetResolution>           (new JME::JetResolution           (any_cast<string>(parameterList_->at("JERResolutionFile" ))));
        gRandom->SetSeed(any_cast<int>(parameterList_->at("RandomGeneratorSeed")));

        //set up variations-> TODO: Consider the BregJERsmearing in the variations
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

        int genJetId = get_property(iJet,Jet_genJetIdx); //Before was bugged, int genJetId = abs(get_property(iJet,Jet_genJetIdx));

        //Get the unsmeared jetP4 (before any procedure)
        TLorentzVector unsmeared_jetP4 = iJet.P4();

        //JER smearfactor for normal jets
        float smearFactor;
        if(genJetId>=0 && genJetId < numberOfGenJets) //generated jet was found and saved
        {
            //JER smearfactor for normal jets
            smearFactor     = 1. + (tmpJER_ScaleFactor - 1.) * (iJet.P4().Pt() - nat.GenJet_pt.At(genJetId))   /iJet.P4().Pt();
        }
        else if(tmpJER_ScaleFactor>1.)
        {
            float sigma     = tmpJER_Resolution * std::sqrt(tmpJER_ScaleFactor * tmpJER_ScaleFactor - 1);
            smearFactor     = 1. + gRandom->Gaus(0., sigma);
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
        
        //Make standard smearing to the jet
        iJet.setP4(unsmeared_jetP4*smearFactor);

        //Procedure for b-regressed jets 
        //Step1: Check if genjet is found, then use the dedicated smearing and regression
        float bregcorr          = Get_bRegCorr(iJet);
        float smearedbreg_jetpt = unsmeared_jetP4.Pt()*bregcorr;
        float smearedbreg_jeten = unsmeared_jetP4.E()*bregcorr; 
        if(genJetId>=0 && genJetId < numberOfGenJets) //generated jet was found and saved
        {
            //Get the genjet P4
            GenJet genjet           = GenJet(genJetId, &nat);
            TLorentzVector genjetP4 = genjet.P4();
            TLorentzVector gennuP4, genjetwithnuP4;
            //Compute the nearby neutrinos p4
            for (uint igp = 0; igp < *(nat.nGenPart); ++igp)
            {
                GenPart nu (igp, &nat);
                if (     get_property(nu, GenPart_status) == 1  && 
                    (abs(get_property(nu, GenPart_pdgId)) == 12 || 
                     abs(get_property(nu, GenPart_pdgId)) == 14 ||
                     abs(get_property(nu, GenPart_pdgId)) == 16) )
                {
                    if( genjetP4.DeltaR( nu.P4() ) < 0.4) gennuP4 = gennuP4 + nu.P4();
                }
            }
            //Add the neutrinos to the genjet
            genjetwithnuP4 = genjetP4 + gennuP4;
            //Derive the new values of regressed/smeared pt and energy to build the dedicated regressed p4
            float resSmear = 1.1; //recommended nominal scale factor //TODO:Add the variations of this scale factor
            float dpt          = smearedbreg_jetpt - genjetwithnuP4.Pt();
            smearedbreg_jetpt  = genjetwithnuP4.Pt() + resSmear*dpt;
            float den          = smearedbreg_jeten - genjetwithnuP4.E();
            smearedbreg_jeten  = genjetwithnuP4.E()  + resSmear*den;
            //Save the correct p4 for b-regressed jets
            TLorentzVector smearedbreg_jetP4;
            smearedbreg_jetP4.SetPtEtaPhiE(smearedbreg_jetpt,unsmeared_jetP4.Eta(),unsmeared_jetP4.Phi(),smearedbreg_jeten);
            iJet.setP4Regressed(smearedbreg_jetP4);    
        }
        else // Step2: Keep standard smearing and build standard P4Regressed with it
        {
           iJet.buildP4Regressed();
        } 
  
    }

    return jets;
}


void OfflineProducerHelper::initializeApplyJERAndBregSmearing(std::string syst_and_direction)
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
        JERsmearing = [=] (NanoAODTree& nat, std::vector<Jet> &jets) -> std::vector<Jet> {return this -> applyJERAndBregSmearing(nat, jets);};
        jetResolutionScaleFactor_ = std::unique_ptr<JME::JetResolutionScaleFactor>(new JME::JetResolutionScaleFactor(any_cast<string>(parameterList_->at("JERScaleFactorFile"))));
        jetResolution_            = std::unique_ptr<JME::JetResolution>           (new JME::JetResolution           (any_cast<string>(parameterList_->at("JERResolutionFile" ))));
        gRandom->SetSeed(any_cast<int>(parameterList_->at("RandomGeneratorSeed")));

        if (syst_and_direction == "nominal")
        {
            jer_dir_      = Variation::NOMINAL;
            breg_jer_dir_ = Variation::NOMINAL;
        }
        
        else
        {
            // check that the input string is well formatted
            auto idelimiter = syst_and_direction.find(":");
            if (idelimiter == std::string::npos)
            {
                string msg = "OfflineProducerHelper::initializeApplyJERAndBregSmearing : input string [" + syst_and_direction + string("] is malformed : cannot find delimited (:) : aborting");
                throw std::runtime_error(msg);
            }

            string sname = syst_and_direction.substr(0, idelimiter);
            string sdir  = syst_and_direction.substr(idelimiter+1);

            // if one syst is not quoted, its default is nominal
            jer_dir_      = Variation::NOMINAL;
            breg_jer_dir_ = Variation::NOMINAL;

            // setting standard jer uncertainty
            if (sname == "jer")
            {
                if (sdir == "up")
                    jer_dir_ = Variation::UP;
                else if (sdir == "down")
                    jer_dir_ = Variation::DOWN;
                else{
                    string msg = "OfflineProducerHelper::initializeApplyJERAndBregSmearing : input string [" + syst_and_direction + string("] is malformed : cannot understand direction (up/down) : aborting");
                    throw std::runtime_error(msg);
                }
            }

            // setting b jet regression jer uncertainty
            else if (sname == "bjer")
            {
                if (sdir == "up")
                    breg_jer_dir_ = Variation::UP;
                else if (sdir == "down")
                    breg_jer_dir_ = Variation::DOWN;
                else{
                    string msg = "OfflineProducerHelper::initializeApplyJERAndBregSmearing : input string [" + syst_and_direction + string("] is malformed : cannot understand direction (up/down) : aborting");
                    throw std::runtime_error(msg);
                }
            }

            // error handling`
            else
            {
                std::string msg = "OfflineProducerHelper::initializeApplyJERAndBregSmearing : malformed input for JER syst variation name : " + sname;
                throw std::runtime_error(msg);
            }

        } // variations

        // make a summary
        std::string s_jer_var = (
            jer_dir_ == Variation::NOMINAL ? "nominal" : 
            jer_dir_ == Variation::UP      ? "up"      : 
            jer_dir_ == Variation::DOWN    ? "down"    : 
            "!! ERROR !!"
        );
        std::string s_breg_jer_var = (
            breg_jer_dir_ == Variation::NOMINAL ? "nominal" : 
            breg_jer_dir_ == Variation::UP      ? "up"      : 
            breg_jer_dir_ == Variation::DOWN    ? "down"    : 
            "!! ERROR !!"
        );

        cout << "[INFO] ... for the standard JER smearing the resolution systematics used is : " << s_jer_var << endl;
        cout << "[INFO] ... for the standard b jet regression JER smearing the resolution systematics used is : " << s_breg_jer_var << endl;
    } // handling StandardJER

    return;
}


std::vector<Jet> OfflineProducerHelper::applyJERAndBregSmearing(NanoAODTree& nat, std::vector<Jet> jets)
{

    JME::JetParameters jetParameters;
    jetParameters.setRho(*(nat.fixedGridRhoFastjetAll));
    int numberOfGenJets = *(nat.nGenJet);

    for(auto &iJet : jets){
        //same method of https://github.com/cms-sw/cmssw/blob/CMSSW_8_0_25/PhysicsTools/PatUtils/interface/SmearedJetProducerT.h
        jetParameters.setJetEta(iJet.P4().Eta());
        jetParameters.setJetPt(iJet.P4().Pt());

        float tmpJER_ScaleFactor = jetResolutionScaleFactor_->getScaleFactor(jetParameters, jer_dir_);
        float tmpJER_Resolution  = jetResolution_->getResolution(jetParameters);

        int genJetId = get_property(iJet,Jet_genJetIdx); //Before was bugged, int genJetId = abs(get_property(iJet,Jet_genJetIdx));

        //Get the unsmeared jetP4 (before any procedure)
        TLorentzVector unsmeared_jetP4 = iJet.P4();

        //JER smearfactor for normal jets
        float smearFactor;
        if(genJetId>=0 && genJetId < numberOfGenJets) //generated jet was found and saved
        {
            //JER smearfactor for normal jets
            smearFactor     = 1. + (tmpJER_ScaleFactor - 1.) * (iJet.P4().Pt() - nat.GenJet_pt.At(genJetId))   /iJet.P4().Pt();
        }
        else if(tmpJER_ScaleFactor>1.)
        {
            float sigma     = tmpJER_Resolution * std::sqrt(tmpJER_ScaleFactor * tmpJER_ScaleFactor - 1);
            smearFactor     = 1. + gRandom->Gaus(0., sigma);
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
        
        //Make standard smearing to the jet
        iJet.setP4(unsmeared_jetP4*smearFactor);

        //Procedure for b-regressed jets 
        //Step1: Check if genjet is found, then use the dedicated smearing and regression
        float bregcorr          = Get_bRegCorr(iJet);
        float smearedbreg_jetpt = unsmeared_jetP4.Pt()*bregcorr;
        float smearedbreg_jeten = unsmeared_jetP4.E()*bregcorr; 
        if(genJetId>=0 && genJetId < numberOfGenJets) //generated jet was found and saved
        {
            //Get the genjet P4
            GenJet genjet           = GenJet(genJetId, &nat);
            TLorentzVector genjetP4 = genjet.P4();
            TLorentzVector gennuP4, genjetwithnuP4;
            //Compute the nearby neutrinos p4
            for (uint igp = 0; igp < *(nat.nGenPart); ++igp)
            {
                GenPart nu (igp, &nat);
                if (     get_property(nu, GenPart_status) == 1  && 
                    (abs(get_property(nu, GenPart_pdgId)) == 12 || 
                     abs(get_property(nu, GenPart_pdgId)) == 14 ||
                     abs(get_property(nu, GenPart_pdgId)) == 16) )
                {
                    if( genjetP4.DeltaR( nu.P4() ) < 0.4) gennuP4 = gennuP4 + nu.P4();
                }
            }
            //Add the neutrinos to the genjet
            genjetwithnuP4 = genjetP4 + gennuP4;
            //Derive the new values of regressed/smeared pt and energy to build the dedicated regressed p4

            float resSmear;
            if (breg_jer_dir_      == Variation::NOMINAL)
                resSmear = 1.1;
            else if (breg_jer_dir_ == Variation::UP)
                resSmear = 1.2;
            else if (breg_jer_dir_ == Variation::DOWN)
                resSmear = 1.0;
            else
                throw std::runtime_error("applyJERAndBregSmearing : did not recognise b jet variation");

            float dpt          = smearedbreg_jetpt - genjetwithnuP4.Pt();
            smearedbreg_jetpt  = genjetwithnuP4.Pt() + resSmear*dpt;
            float den          = smearedbreg_jeten - genjetwithnuP4.E();
            smearedbreg_jeten  = genjetwithnuP4.E()  + resSmear*den;
            //Save the correct p4 for b-regressed jets
            TLorentzVector smearedbreg_jetP4;
            smearedbreg_jetP4.SetPtEtaPhiE(smearedbreg_jetpt,unsmeared_jetP4.Eta(),unsmeared_jetP4.Phi(),smearedbreg_jeten);
            iJet.setP4Regressed(smearedbreg_jetP4);    
        }
        else // Step2: Keep standard smearing and build standard P4Regressed with it
        {
           iJet.buildP4Regressed();
        } 
  
    }

    return jets;
}

// ----------------- Compute JER - END --------------------- //


// ----------------- Compute JEC - BEGIN ------------------- //

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

void OfflineProducerHelper::initializeApplyJESshift(std::string syst_and_direction)
{
    // parse the string to get the systematic name and the direction.
    // the string is expected to be formatted as systematicName:direction
    // if the name -nominal- is passed, no shift is applied

    if (syst_and_direction == nominal_jes_syst_shift_name)
    {
        jes_syst_shift_name_ = nominal_jes_syst_shift_name;
        cout << "[INFO] ... jets will not be shifted for JEC systematics because shift is set to : " << syst_and_direction << endl;
        return;
    }

    // check that the input string is well formatted
    auto idelimiter = syst_and_direction.find(":");
    if (idelimiter == std::string::npos)
    {
        string msg = "OfflineProducerHelper::initializeApplyJESshift : input string [" + syst_and_direction + string("] is malformed : cannot find delimited (:) : aborting");
        throw std::runtime_error(msg);
    }

    string sname = syst_and_direction.substr(0, idelimiter);
    string sdir  = syst_and_direction.substr(idelimiter+1);

    string JECFileName = any_cast<string>(parameterList_->at("JECFileName"));

    jcp_ = std::unique_ptr<JetCorrectorParameters> (new JetCorrectorParameters(JECFileName, sname));
    jcu_ = std::unique_ptr<JetCorrectionUncertainty> (new JetCorrectionUncertainty(*jcp_));

    jes_syst_shift_name_ = sname;
    if (sdir == "up")
        jes_syst_shift_dir_is_up_ = true;
    else if (sdir == "down")
        jes_syst_shift_dir_is_up_ = false;
    else
    {
        string msg = "OfflineProducerHelper::initializeApplyJESshift : input string [" + syst_and_direction + string("] is malformed : cannot understand direction (up/down) : aborting");
        throw std::runtime_error(msg);
    }

    cout << "[INFO] ... jets will be shifted for JEC systematics according to systematic : " << jes_syst_shift_name_ << " : " << (jes_syst_shift_dir_is_up_ ? "up" : "down") << endl;

    return;
}

std::vector<Jet> OfflineProducerHelper::applyJESshift(NanoAODTree &nat, const std::vector<Jet> &jets, bool direction_is_up)
{
    // calculation derived from the following twikis:
    // https://twiki.cern.ch/twiki/bin/view/CMS/JECUncertaintySources#Example_implementation
    // https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#JetCorUncertainties

    // NOTE : the input jets must be unsmeared

    std::vector<Jet> result;
    double shift = direction_is_up ? 1. : -1.;

    for(size_t ijet = 0; ijet < jets.size(); ++ijet)
    {
        jcu_->setJetPt  (jets.at(ijet).P4().Pt());
        jcu_->setJetEta (jets.at(ijet).P4().Eta());
        double corr_factor = jcu_->getUncertainty(direction_is_up);

        Jet jet = jets.at(ijet);
        jet.setP4(jet.P4() * (1 + shift * corr_factor) ); // set the shifted p4 ...
        jet.buildP4Regressed(); // ... and reset the regressed p4 to be recomputed
        result.emplace_back(jet);

    }

    return result;
}

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

// Minimizes distance to the diagonal (alla ATLAS)
std::vector<Jet> OfflineProducerHelper::bbbb_jets_idxs_BothClosestToDiagonal(const std::vector<Jet> *presel_jets)
{
    //Do you add b-jet regression into the pairing?
    bool breg = any_cast<bool>(parameterList_->at("BjetRegression"));
    float targetHiggsMass1Mass2 = any_cast<float>(parameterList_->at("TargetHiggsMass1Mass2"));
    int   option                = any_cast<int>(parameterList_->at("Option"));
    std::vector<TLorentzVector> p4s;

    for (uint i = 0; i < presel_jets->size(); ++i)
        for (uint j = i+1; j < presel_jets->size(); ++j)
        {
            TLorentzVector p4sum;
            if (breg) 
               p4sum= (presel_jets->at(i).P4Regressed() + presel_jets->at(j).P4Regressed());
            else
               p4sum= (presel_jets->at(i).P4() + presel_jets->at(j).P4());
            p4s.emplace_back(p4sum);
        }

    float m1,m2,m3,m4,m5,m6;

    //Ordering Option 1
    if(p4s.at(0).Pt() > p4s.at(5).Pt())
    {
      m1 = p4s.at(0).M(); m2 = p4s.at(5).M();
    }
    else
    {
      m1 = p4s.at(5).M(); m2 = p4s.at(0).M();        
    }
    //Ordering Option 2
    if(p4s.at(1).Pt() > p4s.at(4).Pt())
    {
      m3 = p4s.at(1).M(); m4 = p4s.at(4).M();
    }
    else
    {
      m3 = p4s.at(4).M(); m4 = p4s.at(1).M();       
    }
    //Ordering Option 3
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
     
    //Compute distances to diagonal
    float d12_34 =  (1/ TMath::Sqrt(1 + pow(targetHiggsMass1Mass2,2) ) )*fabs( m_12_34.first - ((targetHiggsMass1Mass2)*m_12_34.second) );
    float d13_24 =  (1/ TMath::Sqrt(1 + pow(targetHiggsMass1Mass2,2) ) )*fabs( m_13_24.first - ((targetHiggsMass1Mass2)*m_13_24.second) );
    float d14_23 =  (1/ TMath::Sqrt(1 + pow(targetHiggsMass1Mass2,2) ) )*fabs( m_14_23.first - ((targetHiggsMass1Mass2)*m_14_23.second) );

    //Order distances and compute their differences
    vector<float> distances = {d12_34, d13_24, d14_23};
    std::sort(distances.begin(), distances.end());
    float the_fst = distances.at(0);
    float the_snd = distances.at(1);
    float the_trd = distances.at(2);
    float dhh_delta12 = abs(the_fst-the_snd);

    //define the set of jets 
    std::vector<Jet> outputJets        = *presel_jets;
    std::vector<Jet> outputJets_thefst = *presel_jets;
    std::vector<Jet> outputJets_thesnd = *presel_jets;
    std::vector<Jet> outputJets_thetrd = *presel_jets;

    //Get the pair3ing closest
    if (the_fst == d12_34){
        outputJets_thefst.at(0) = presel_jets->at(1 - 1);outputJets_thefst.at(1) = presel_jets->at(2 - 1);
        outputJets_thefst.at(2) = presel_jets->at(3 - 1);outputJets_thefst.at(3) = presel_jets->at(4 - 1);
    }
    else if (the_fst == d13_24){
        outputJets_thefst.at(0) = presel_jets->at(1 - 1);outputJets_thefst.at(1) = presel_jets->at(3 - 1);
        outputJets_thefst.at(2) = presel_jets->at(2 - 1);outputJets_thefst.at(3) = presel_jets->at(4 - 1);
    }
    else if (the_fst == d14_23){
        outputJets_thefst.at(0) = presel_jets->at(1 - 1);outputJets_thefst.at(1) = presel_jets->at(4 - 1);
        outputJets_thefst.at(2) = presel_jets->at(2 - 1);outputJets_thefst.at(3) = presel_jets->at(3 - 1);
    }
    else
    {
        //Do nothing
    }
    //Get the pairing second closest
    if (the_snd == d12_34){
        outputJets_thesnd.at(0) = presel_jets->at(1 - 1);outputJets_thesnd.at(1) = presel_jets->at(2 - 1);
        outputJets_thesnd.at(2) = presel_jets->at(3 - 1);outputJets_thesnd.at(3) = presel_jets->at(4 - 1);
    }
    else if (the_snd == d13_24){
        outputJets_thesnd.at(0) = presel_jets->at(1 - 1);outputJets_thesnd.at(1) = presel_jets->at(3 - 1);
        outputJets_thesnd.at(2) = presel_jets->at(2 - 1);outputJets_thesnd.at(3) = presel_jets->at(4 - 1);
    }
    else if (the_snd == d14_23){
        outputJets_thesnd.at(0) = presel_jets->at(1 - 1);outputJets_thesnd.at(1) = presel_jets->at(4 - 1);
        outputJets_thesnd.at(2) = presel_jets->at(2 - 1);outputJets_thesnd.at(3) = presel_jets->at(3 - 1);
    }
    else
    {
        //Do nothing
    }
    //Get the pairing third closest
    if (the_trd == d12_34){
        outputJets_thetrd.at(0) = presel_jets->at(1 - 1);outputJets_thetrd.at(1) = presel_jets->at(2 - 1);
        outputJets_thetrd.at(2) = presel_jets->at(3 - 1);outputJets_thetrd.at(3) = presel_jets->at(4 - 1);
    }
    else if (the_trd == d13_24){
        outputJets_thetrd.at(0) = presel_jets->at(1 - 1);outputJets_thetrd.at(1) = presel_jets->at(3 - 1);
        outputJets_thetrd.at(2) = presel_jets->at(2 - 1);outputJets_thetrd.at(3) = presel_jets->at(4 - 1);
    }
    else if (the_trd == d14_23){
        outputJets_thetrd.at(0) = presel_jets->at(1 - 1);outputJets_thetrd.at(1) = presel_jets->at(4 - 1);
        outputJets_thetrd.at(2) = presel_jets->at(2 - 1);outputJets_thetrd.at(3) = presel_jets->at(3 - 1);
    }
    else
    {
        //Do nothing
    }

    //Compute important variables for the options
    float thefst_h1pt_cm = compute_higgsptcm(outputJets_thefst);
    float thesnd_h1pt_cm = compute_higgsptcm(outputJets_thesnd);

    //Choose depending on the predefined option
    if(option==0) //Nominal
    {
       outputJets = outputJets_thefst;
    }
    else if (option==1) // Hydrid
    {
        if (dhh_delta12 > 30) 
            outputJets = outputJets_thefst;
        else
        {
            if(thefst_h1pt_cm > thesnd_h1pt_cm) 
                outputJets = outputJets_thefst;
            else
                outputJets = outputJets_thesnd;
        }

    }
    else if (option==2) // By Higgs pT in the CM frame
    {
            if(thefst_h1pt_cm > thesnd_h1pt_cm) 
                outputJets = outputJets_thefst;
            else
                outputJets = outputJets_thesnd;
    }
    else
        cout << "** [WARNING] : Something went wrong with finding the correct option" << endl;

    return outputJets;
}

// Minimizes distance to the slope parameter
std::vector<Jet> OfflineProducerHelper::bbbb_jets_idxs_BothClosestToSlope(const std::vector<Jet> *presel_jets)
{
    //Do you add b-jet regression into the pairing?
    bool breg = any_cast<bool>(parameterList_->at("BjetRegression"));

    float target = any_cast<float>(parameterList_->at("SlopeParameter"));
    std::vector<TLorentzVector> p4s;

    for (uint i = 0; i < presel_jets->size(); ++i)
        for (uint j = i+1; j < presel_jets->size(); ++j)
        {
            TLorentzVector p4sum;
            if (breg) 
               p4sum= (presel_jets->at(i).P4Regressed() + presel_jets->at(j).P4Regressed());
            else
               p4sum= (presel_jets->at(i).P4() + presel_jets->at(j).P4());
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

    //NOTE: The formula below is equivalent (after some massaging :-) )
    float d12_34 = fabs( (m_12_34.first / m_12_34.second) - target );
    float d13_24 = fabs( (m_13_24.first / m_13_24.second) - target );
    float d14_23 = fabs( (m_14_23.first / m_14_23.second) - target );
 
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
 
    //Do you add b-jet regression into the pairing?
    bool breg = any_cast<bool>(parameterList_->at("BjetRegression"));

    std::vector<float> mHs;

    for (uint i = 0; i < presel_jets->size(); ++i)
        for (uint j = i+1; j < presel_jets->size(); ++j)
        {
            TLorentzVector p4sum;
            if(breg)
              p4sum = (presel_jets->at(i).P4Regressed() + presel_jets->at(j).P4Regressed());
            else    
              p4sum = (presel_jets->at(i).P4() + presel_jets->at(j).P4());
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
bool OfflineProducerHelper::select_bbbbjj_jets(NanoAODTree& nat, EventInfo& ei, OutputTree &ot)
{
    //Get the jet collection
    std::vector<Jet> unsmearedjets,jets;
    unsmearedjets.reserve(*(nat.nJet));jets.reserve(*(nat.nJet));

    for (uint ij = 0; ij < *(nat.nJet); ++ij){unsmearedjets.emplace_back(Jet(ij, &nat));}

    // if a shift was asked (shift name != "nominal"), run the systematics shift
    if (jes_syst_shift_name_ != nominal_jes_syst_shift_name){
        auto jets_tmp = applyJESshift(nat, unsmearedjets, jes_syst_shift_dir_is_up_);
        unsmearedjets = jets_tmp;
    }

    //If MC sample, then obtain the jet energy resolution correction strategy and apply it before any selection.
    if(parameterList_->find("JetEnergyResolution") != parameterList_->end())
    {   //It is MC
        jets = JERsmearing(nat, unsmearedjets);
    }
    else
    {   //It is data
        jets = unsmearedjets;
    }

    //Preselect four most b-tagged jets (pt, eta, btagging) & 'VBF-jet pair'(pt,eta,deltaEta). 
    std::vector<Jet> presel_jets = bjJets_PreselectionCut(nat, ei, ot, jets);
    if(presel_jets.size()<4) return false;
    //Pick up the four preselected b-jets
    std::vector<Jet> presel_bjets={{*(presel_jets.begin()+0),*(presel_jets.begin()+1),*(presel_jets.begin()+2),*(presel_jets.begin()+3)}};

    //Do the Higgs pairing based on bbbbChoice
    std::vector<Jet> ordered_jets;
    if(any_cast<string>(parameterList_->at("bbbbChoice"))=="BothClosestToMh"){
        ordered_jets = bbbb_jets_idxs_BothClosestToMh(&presel_bjets);
    }
    else if(any_cast<string>(parameterList_->at("bbbbChoice"))=="BothClosestToDiagonal"){
        ordered_jets = bbbb_jets_idxs_BothClosestToDiagonal(&presel_bjets);
    }
    else if(any_cast<string>(parameterList_->at("bbbbChoice"))=="BothClosestToSlope"){
        ordered_jets = bbbb_jets_idxs_BothClosestToSlope(&presel_bjets);
    }
    else{
        ordered_jets = bbbb_jets_idxs_MinMassDifference(&presel_bjets);
    }
    //Add higher-level objects (H1,H2,HH) and angular variables (dEta,dPhi,etc) 
    if(presel_jets.size()==6){
        ordered_jets.push_back(presel_jets.at(4)); ordered_jets.push_back(presel_jets.at(5));
        AddInclusiveCategoryVariables(nat,ei,ordered_jets);
        AddVBFCategoryVariables(nat,ei,ordered_jets);
    }
    else
    {
        AddInclusiveCategoryVariables(nat,ei,ordered_jets);
    }

    return true; 
}

void OfflineProducerHelper::CalculateEventbyEventScaleFactors(NanoAODTree& nat,OutputTree &ot, EventInfo& ei, float xs)
{
    //Store the cross section in the output tree
    ot.userFloat("XS")=xs;
    //Here calculate btag scale/L1prefiring factors for MC
    std::vector<Jet> presel_bjets = {*(ei.H1_b1),*(ei.H1_b2),*(ei.H2_b1),*(ei.H2_b2)};    
    //--Calculate the associated b-tagging scale factor
    CalculateBtagScaleFactor(presel_bjets,nat,ot);
    //--L1Prefiring weights
    CalculateL1prefiringScaleFactor(nat,ot,ei);

}

void OfflineProducerHelper::AddInclusiveCategoryVariables(NanoAODTree& nat, EventInfo& ei,std::vector<Jet> ordered_jets){
       //Event variables
       ei.Run = *(nat.run);
       ei.LumiSec = *(nat.luminosityBlock);
       ei.Event = *(nat.event);
       ei.nPVgood=*(nat.PV_npvsGood);
       ei.EventCount = 1;
       if(ordered_jets.size()==6){ ei.VBFEvent = 1;}
       else{ei.VBFEvent = 0;}
       // order H1, H2 by pT: pT(H1) > pT (H2)
       bool breg = any_cast<bool>(parameterList_->at("BjetRegression"));
       order_by_pT(ordered_jets.at(0), ordered_jets.at(1));
       order_by_pT(ordered_jets.at(2), ordered_jets.at(3));
       CompositeCandidate H1            = CompositeCandidate(ordered_jets.at(0), ordered_jets.at(1) );
       CompositeCandidate H2            = CompositeCandidate(ordered_jets.at(2), ordered_jets.at(3) );
       CompositeCandidate H1unregressed = CompositeCandidate(ordered_jets.at(0), ordered_jets.at(1) );
       CompositeCandidate H2unregressed = CompositeCandidate(ordered_jets.at(2), ordered_jets.at(3) );
       //Do you add b-jet regression into Higgs reconstruction?
       ei.H1unregressed=H1unregressed;
       ei.H2unregressed=H2unregressed;
       order_by_pT(ei.H1unregressed.get(), ei.H2unregressed.get());
       if(breg)
       {
           H1.rebuildP4UsingRegressedPt(true,true); 
           H2.rebuildP4UsingRegressedPt(true,true);
       }
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
       //HH variables
       ei.HHunregressed = CompositeCandidate(ei.H1unregressed.get(), ei.H2unregressed.get());       
       ei.HH            = CompositeCandidate(ei.H1.get(), ei.H2.get());
       //Fill branches with selected b-jets ordered by pt
       std::vector<Jet> presel_bjets, presel_bjets_btags;
       uint m=0; while(m<4)
       {
        presel_bjets.push_back(ordered_jets.at(m));
        presel_bjets_btags.push_back(ordered_jets.at(m)); 
        m++;
       }
       stable_sort(presel_bjets.begin(), presel_bjets.end(), [](const Jet & a, const Jet & b) -> bool
       {return ( a.P4().Pt() > b.P4().Pt() );});
       ei.HH_b1      = presel_bjets.at(0);
       ei.HH_b2      = presel_bjets.at(1);
       ei.HH_b3      = presel_bjets.at(2);
       ei.HH_b4      = presel_bjets.at(3);
       if(breg){ei.sum_4b_pt = ei.HH_b1.get().P4Regressed().Pt()+ei.HH_b2.get().P4Regressed().Pt()+ei.HH_b3.get().P4Regressed().Pt()+ei.HH_b4.get().P4Regressed().Pt();}
       else{ei.sum_4b_pt = ei.HH_b1.get().P4().Pt()+ei.HH_b2.get().P4().Pt()+ei.HH_b3.get().P4().Pt()+ei.HH_b4.get().P4().Pt();}  
       //Is it three or four b-tags? and branch with b-jets ordered by btag score
       if( any_cast<bool>(parameterList_->at("NewestBtaggingAlgorithm")) )
       {
           ei.btaggerID=1;
           stable_sort(presel_bjets_btags.begin(), presel_bjets_btags.end(), [](const Jet & a, const Jet & b) -> bool
           {return ( get_property(a, Jet_btagDeepFlavB) > get_property(b, Jet_btagDeepFlavB) );});
           ei.HH_btag_b1 = presel_bjets_btags.at(0);
           ei.HH_btag_b2 = presel_bjets_btags.at(1);
           ei.HH_btag_b3 = presel_bjets_btags.at(2);
           ei.HH_btag_b4 = presel_bjets_btags.at(3);
           ei.HH_btag_b1_bscore = get_property( presel_bjets_btags.at(0) , Jet_btagDeepFlavB );
           ei.HH_btag_b2_bscore = get_property( presel_bjets_btags.at(1) , Jet_btagDeepFlavB );
           ei.HH_btag_b3_bscore = get_property( presel_bjets_btags.at(2) , Jet_btagDeepFlavB );
           ei.HH_btag_b4_bscore = get_property( presel_bjets_btags.at(3) , Jet_btagDeepFlavB );           
           ei.HH_btag_b1_bres   = Get_bRegRes( presel_bjets_btags.at(0) ); 
           ei.HH_btag_b2_bres   = Get_bRegRes( presel_bjets_btags.at(1) );
           ei.HH_btag_b3_bres   = Get_bRegRes( presel_bjets_btags.at(2) );
           ei.HH_btag_b4_bres   = Get_bRegRes( presel_bjets_btags.at(3) );
           if( get_property( ei.HH_btag_b4.get() , Jet_btagDeepFlavB ) >= any_cast<float>(parameterList_->at("bMinDeepJet") ) ){ei.nBtag=4;}
           else{ei.nBtag=3;}
       }
       else
       {
           ei.btaggerID=0;
           stable_sort(presel_bjets_btags.begin(), presel_bjets_btags.end(), [](const Jet & a, const Jet & b) -> bool
           {return ( get_property(a, Jet_btagDeepB) > get_property(b, Jet_btagDeepB) );});
           ei.HH_btag_b1 = presel_bjets_btags.at(0);
           ei.HH_btag_b2 = presel_bjets_btags.at(1);
           ei.HH_btag_b3 = presel_bjets_btags.at(2);
           ei.HH_btag_b4 = presel_bjets_btags.at(3);
           ei.HH_btag_b1_bscore = get_property( presel_bjets_btags.at(0) , Jet_btagDeepB );
           ei.HH_btag_b2_bscore = get_property( presel_bjets_btags.at(1) , Jet_btagDeepB );
           ei.HH_btag_b3_bscore = get_property( presel_bjets_btags.at(2) , Jet_btagDeepB );
           ei.HH_btag_b4_bscore = get_property( presel_bjets_btags.at(3) , Jet_btagDeepB );           
           ei.HH_btag_b1_bres   = Get_bRegRes( presel_bjets_btags.at(0) );
           ei.HH_btag_b2_bres   = Get_bRegRes( presel_bjets_btags.at(1) );
           ei.HH_btag_b3_bres   = Get_bRegRes( presel_bjets_btags.at(2) );
           ei.HH_btag_b4_bres   = Get_bRegRes( presel_bjets_btags.at(3) );
           if( get_property( ei.HH_btag_b4.get() , Jet_btagDeepB ) >= any_cast<float>(parameterList_->at("bMinDeepCSV") ) ){ei.nBtag=4;}
           else{ei.nBtag=3;}
       }
       ei.sum_3b_bscore = (ei.HH_btag_b1_bscore.get() + ei.HH_btag_b2_bscore.get() + ei.HH_btag_b3_bscore.get());
       //Mass cut variables with a random swap to be sure that the m1 and m2 are simmetric when selecting the signal region
       bool swapped = (int(ei.H1->P4().Pt()*100.) % 2 == 1);
       if (!swapped){ei.H1rand = ei.H1.get();ei.H2rand = ei.H2.get();}
       else{ei.H1rand = ei.H2.get();ei.H2rand = ei.H1.get();}
       float mHHcenter;
       if(any_cast<string>(parameterList_->at("bbbbChoice"))=="BothClosestToDiagonal")
            mHHcenter = 125;
       else
            mHHcenter = 120;
       ei.HH_2DdeltaM  = pow(ei.H1rand->P4().M() - mHHcenter,2) + pow(ei.H2rand->P4().M() - mHHcenter,2);
       //dR,dPhi and dEta between pair b's
       ei.H1_bb_deltaR   = ei.H1_b1->P4().DeltaR(ei.H1_b2->P4());
       ei.H1_bb_deltaPhi = abs(ei.H1_b1->P4().DeltaPhi(ei.H1_b2->P4()));
       ei.H1_bb_deltaEta = abs(ei.H1_b1->P4().Eta() - ei.H1_b2->P4().Eta() );
       ei.H2_bb_deltaR   = ei.H2_b1->P4().DeltaR(ei.H2_b2->P4());
       ei.H2_bb_deltaPhi = abs(ei.H2_b1->P4().DeltaPhi(ei.H2_b2->P4()));
       ei.H2_bb_deltaEta = abs(ei.H2_b1->P4().Eta() - ei.H2_b2->P4().Eta() );
       //Special variables:deltaR between jets
       ei.b1b2_deltaR = ei.HH_b1->P4().DeltaR(ei.HH_b2->P4());
       ei.b1b3_deltaR = ei.HH_b1->P4().DeltaR(ei.HH_b3->P4());
       ei.b1b4_deltaR = ei.HH_b1->P4().DeltaR(ei.HH_b4->P4());
       ei.b2b3_deltaR = ei.HH_b2->P4().DeltaR(ei.HH_b3->P4());
       ei.b2b4_deltaR = ei.HH_b2->P4().DeltaR(ei.HH_b4->P4());
       ei.b3b4_deltaR = ei.HH_b3->P4().DeltaR(ei.HH_b4->P4());
       ei.b1b2_deltaPhi = abs(ei.HH_b1->P4().DeltaPhi(ei.HH_b2->P4()));
       ei.b1b3_deltaPhi = abs(ei.HH_b1->P4().DeltaPhi(ei.HH_b3->P4()));
       ei.b1b4_deltaPhi = abs(ei.HH_b1->P4().DeltaPhi(ei.HH_b4->P4()));
       ei.b2b3_deltaPhi = abs(ei.HH_b2->P4().DeltaPhi(ei.HH_b3->P4()));
       ei.b2b4_deltaPhi = abs(ei.HH_b2->P4().DeltaPhi(ei.HH_b4->P4()));
       ei.b3b4_deltaPhi = abs(ei.HH_b3->P4().DeltaPhi(ei.HH_b4->P4()));
       ei.b1b2_deltaEta = abs(ei.HH_b1->P4().Eta() - ei.HH_b2->P4().Eta() );
       ei.b1b3_deltaEta = abs(ei.HH_b1->P4().Eta() - ei.HH_b3->P4().Eta() );
       ei.b1b4_deltaEta = abs(ei.HH_b1->P4().Eta() - ei.HH_b4->P4().Eta() );
       ei.b2b3_deltaEta = abs(ei.HH_b2->P4().Eta() - ei.HH_b3->P4().Eta() );
       ei.b2b4_deltaEta = abs(ei.HH_b2->P4().Eta() - ei.HH_b4->P4().Eta() );
       ei.b3b4_deltaEta = abs(ei.HH_b3->P4().Eta() - ei.HH_b4->P4().Eta() );
       //Special variables between Higgses
       ei.h1h2_deltaR   = ei.H1->P4().DeltaR(ei.H2->P4());
       ei.h1h2_deltaPhi = abs(ei.H1->P4().DeltaPhi(ei.H2->P4()));
       ei.h1h2_deltaEta = abs( ei.H1->P4().Eta() - ei.H2->P4().Eta());
       //Number of jets (bMinPt,bMaxAbsEta)
       int njec=0,njhf=0;
       int njtot=*(nat.nJet);
       for (uint ij = 0; ij < *(nat.nJet); ++ij)
       {
        if ( Jet(ij, &nat).P4().Pt() >= any_cast<float>(parameterList_->at("bMinPt")) && abs(get_property(Jet(ij, &nat), Jet_eta)) <= any_cast<float>(parameterList_->at("bMaxAbsEta")) ) njec++;
        if ( Jet(ij, &nat).P4().Pt() >= any_cast<float>(parameterList_->at("jMinPt")) && abs(get_property(Jet(ij, &nat), Jet_eta)) <= any_cast<float>(parameterList_->at("jMaxAbsEta")) ) njhf++;
       }
       ei.nJet_ec = njec;
       ei.nJet_hf = njhf;
       ei.nJet    = njtot;
       //HT from jets passing the min trigger cut
       float HT_PT30=0;
       float HT_PT40=0;
       for (uint ij = 0; ij < *(nat.nJet); ++ij)
       {
        if ( Jet(ij, &nat).P4().Pt() >= 30 && abs(get_property(Jet(ij, &nat), Jet_eta)) <= any_cast<float>(parameterList_->at("bMaxAbsEta")) ) HT_PT30 += Jet(ij, &nat).P4().Pt();
        if ( Jet(ij, &nat).P4().Pt() >= 40 && abs(get_property(Jet(ij, &nat), Jet_eta)) <= any_cast<float>(parameterList_->at("bMaxAbsEta")) ) HT_PT40 += Jet(ij, &nat).P4().Pt();
       }
       ei.HT_PT30   = HT_PT30;
       ei.HT_PT40   = HT_PT40;
       //Raw pt
       ei.HH_b1_rawpt = get_property(presel_bjets.at(0), Jet_pt) * (1-get_property(presel_bjets.at(0), Jet_rawFactor));
       ei.HH_b2_rawpt = get_property(presel_bjets.at(1), Jet_pt) * (1-get_property(presel_bjets.at(1), Jet_rawFactor));
       ei.HH_b3_rawpt = get_property(presel_bjets.at(2), Jet_pt) * (1-get_property(presel_bjets.at(2), Jet_rawFactor));
       ei.HH_b4_rawpt = get_property(presel_bjets.at(3), Jet_pt) * (1-get_property(presel_bjets.at(3), Jet_rawFactor));
       ei.H1_b1_rawpt = get_property( ei.H1_b1.get(), Jet_pt) * (1 - get_property(ei.H1_b1.get(), Jet_rawFactor) );
       ei.H1_b2_rawpt = get_property( ei.H1_b2.get(), Jet_pt) * (1 - get_property(ei.H1_b2.get(), Jet_rawFactor) );
       ei.H2_b1_rawpt = get_property( ei.H2_b1.get(), Jet_pt) * (1 - get_property(ei.H2_b1.get(), Jet_rawFactor) );
       ei.H2_b2_rawpt = get_property( ei.H2_b2.get(), Jet_pt) * (1 - get_property(ei.H2_b2.get(), Jet_rawFactor) );
       //COSTHETA VARIABLES in HH CM FRAME
       TLorentzVector Sum = ei.H1->P4() + ei.H2->P4();
       auto boost_vctr = Sum.BoostVector();
       TLorentzVector H1_cm,H2_cm,HH_b1_cm,HH_b2_cm,HH_b3_cm,HH_b4_cm;
       H1_cm = ei.H1->P4(); H1_cm.Boost(-boost_vctr);
       H2_cm = ei.H2->P4(); H2_cm.Boost(-boost_vctr);
       if( breg ){
           HH_b1_cm = ei.HH_b1->P4Regressed(); HH_b1_cm.Boost(-boost_vctr);
           HH_b2_cm = ei.HH_b2->P4Regressed(); HH_b2_cm.Boost(-boost_vctr);
           HH_b3_cm = ei.HH_b3->P4Regressed(); HH_b3_cm.Boost(-boost_vctr);
           HH_b4_cm = ei.HH_b4->P4Regressed(); HH_b4_cm.Boost(-boost_vctr);
       }
       else{
           HH_b1_cm = ei.HH_b1->P4(); HH_b1_cm.Boost(-boost_vctr);
           HH_b2_cm = ei.HH_b2->P4(); HH_b2_cm.Boost(-boost_vctr);
           HH_b3_cm = ei.HH_b3->P4(); HH_b3_cm.Boost(-boost_vctr);
           HH_b4_cm = ei.HH_b4->P4(); HH_b4_cm.Boost(-boost_vctr);
       }
       ei.abs_costh_H1_ggfcm    = abs(H1_cm.CosTheta());
       ei.abs_costh_H2_ggfcm    = abs(H2_cm.CosTheta());
       ei.abs_costh_HH_b1_ggfcm = abs(HH_b1_cm.CosTheta());
       ei.abs_costh_HH_b2_ggfcm = abs(HH_b2_cm.CosTheta());
       ei.abs_costh_HH_b3_ggfcm = abs(HH_b3_cm.CosTheta());
       ei.abs_costh_HH_b4_ggfcm = abs(HH_b4_cm.CosTheta());
       //COSTHETA VARIABLES in H1 CM FRAME
       TLorentzVector Sumh1 = ei.H1->P4();
       auto boost_vctr_h1 = Sumh1.BoostVector();
       TLorentzVector H1_b1_h1cm,H1_b2_h1cm;
       if( breg ){
           H1_b1_h1cm = ei.H1_b1->P4Regressed(); H1_b1_h1cm.Boost(-boost_vctr_h1);
           H1_b2_h1cm = ei.H1_b2->P4Regressed(); H1_b2_h1cm.Boost(-boost_vctr_h1);
       }
       else{
           H1_b1_h1cm = ei.H1_b1->P4(); H1_b1_h1cm.Boost(-boost_vctr_h1);
           H1_b2_h1cm = ei.H1_b2->P4(); H1_b2_h1cm.Boost(-boost_vctr_h1);
       }
       ei.abs_costh_H1_b1_h1cm = abs(H1_b1_h1cm.CosTheta());
       ei.abs_costh_H1_b2_h1cm = abs(H1_b2_h1cm.CosTheta());
       //COSTHETA VARIABLES in H2 CM FRAME
       TLorentzVector Sumh2 = ei.H2->P4();
       auto boost_vctr_h2 = Sumh2.BoostVector();
       TLorentzVector H2_b1_h2cm,H2_b2_h2cm;
       if( breg ){
           H2_b1_h2cm = ei.H2_b1->P4Regressed(); H2_b1_h2cm.Boost(-boost_vctr_h2);
           H2_b2_h2cm = ei.H2_b2->P4Regressed(); H2_b2_h2cm.Boost(-boost_vctr_h2);
       }
       else{
           H2_b1_h2cm = ei.H2_b1->P4(); H2_b1_h2cm.Boost(-boost_vctr_h2);
           H2_b2_h2cm = ei.H2_b2->P4(); H2_b2_h2cm.Boost(-boost_vctr_h2);
       }
       ei.abs_costh_H2_b1_h2cm = abs(H2_b1_h2cm.CosTheta());
       ei.abs_costh_H2_b2_h2cm = abs(H2_b2_h2cm.CosTheta());
       //GGF-QCDKiller
       ei.BDT3     = GetBDT3Score(ei);
       ei.BDT3cat1 = GetBDT3cat1Score(ei);
       ei.BDT3cat2 = GetBDT3cat2Score(ei);
       return;
}

void OfflineProducerHelper::AddVBFCategoryVariables(NanoAODTree& nat, EventInfo& ei,std::vector<Jet> presel_jets){
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
       ei.b1j1_deltaPhi = abs(ei.HH_b1->P4().DeltaPhi(ei.JJ_j1->P4()));
       ei.b1j2_deltaPhi = abs(ei.HH_b1->P4().DeltaPhi(ei.JJ_j2->P4()));
       ei.b2j1_deltaPhi = abs(ei.HH_b2->P4().DeltaPhi(ei.JJ_j1->P4()));
       ei.b2j2_deltaPhi = abs(ei.HH_b2->P4().DeltaPhi(ei.JJ_j2->P4()));
       ei.b3j1_deltaPhi = abs(ei.HH_b3->P4().DeltaPhi(ei.JJ_j1->P4()));
       ei.b3j2_deltaPhi = abs(ei.HH_b3->P4().DeltaPhi(ei.JJ_j2->P4()));
       ei.b4j1_deltaPhi = abs(ei.HH_b4->P4().DeltaPhi(ei.JJ_j1->P4()));
       ei.b4j2_deltaPhi = abs(ei.HH_b4->P4().DeltaPhi(ei.JJ_j2->P4()));
       ei.j1j2_deltaPhi = abs(ei.JJ_j1->P4().DeltaPhi(ei.JJ_j2->P4()));
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
       ei.h1j1_deltaPhi = abs(ei.H1->P4().DeltaPhi(ei.JJ_j1->P4()));
       ei.h1j2_deltaPhi = abs(ei.H1->P4().DeltaPhi(ei.JJ_j2->P4()));
       ei.h2j1_deltaPhi = abs(ei.H2->P4().DeltaPhi(ei.JJ_j1->P4()));
       ei.h2j2_deltaPhi = abs(ei.H2->P4().DeltaPhi(ei.JJ_j2->P4()));
       ei.h1jj_deltaPhi = abs(ei.H1->P4().DeltaPhi(ei.JJ->P4()));
       ei.h2jj_deltaPhi = abs(ei.H2->P4().DeltaPhi(ei.JJ->P4()));
       ei.hhj1_deltaPhi = abs(ei.HH->P4().DeltaPhi(ei.JJ_j1->P4()));
       ei.hhj2_deltaPhi = abs(ei.HH->P4().DeltaPhi(ei.JJ_j2->P4()));
       ei.hhjj_deltaPhi = abs(ei.HH->P4().DeltaPhi(ei.JJ->P4()));
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

       ei.hhjj_pt       = (ei.HH->P4() + ei.JJ->P4()).Pt();

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
       //COSTHETA VARIABLES in HH+JJ CM FRAME
       TLorentzVector Sum = ei.H1->P4() + ei.H2->P4() + ei.JJ_j1->P4() + ei.JJ_j2->P4();
       auto boost_vctr = Sum.BoostVector();
       TLorentzVector H1_cm = ei.H1->P4(); H1_cm.Boost(-boost_vctr);
       TLorentzVector H2_cm = ei.H2->P4(); H2_cm.Boost(-boost_vctr);
       TLorentzVector HH_cm = ei.HH->P4(); HH_cm.Boost(-boost_vctr);
       TLorentzVector JJ_cm = ei.JJ->P4(); JJ_cm.Boost(-boost_vctr);
       TLorentzVector HH_b1_cm,HH_b2_cm,HH_b3_cm,HH_b4_cm;
       if( any_cast<bool>(parameterList_->at("BjetRegression")) ){
           HH_b1_cm = ei.HH_b1->P4Regressed(); HH_b1_cm.Boost(-boost_vctr);
           HH_b2_cm = ei.HH_b2->P4Regressed(); HH_b2_cm.Boost(-boost_vctr);
           HH_b3_cm = ei.HH_b3->P4Regressed(); HH_b3_cm.Boost(-boost_vctr);
           HH_b4_cm = ei.HH_b4->P4Regressed(); HH_b4_cm.Boost(-boost_vctr);
       }
       else{
           HH_b1_cm = ei.HH_b1->P4(); HH_b1_cm.Boost(-boost_vctr);
           HH_b2_cm = ei.HH_b2->P4(); HH_b2_cm.Boost(-boost_vctr);
           HH_b3_cm = ei.HH_b3->P4(); HH_b3_cm.Boost(-boost_vctr);
           HH_b4_cm = ei.HH_b4->P4(); HH_b4_cm.Boost(-boost_vctr);
       }       
       TLorentzVector JJ_j1_cm = ei.JJ_j1->P4(); JJ_j1_cm.Boost(-boost_vctr);
       TLorentzVector JJ_j2_cm = ei.JJ_j2->P4(); JJ_j2_cm.Boost(-boost_vctr);
       ei.abs_costh_H1_vbfcm    = abs(H1_cm.CosTheta());
       ei.abs_costh_H2_vbfcm    = abs(H2_cm.CosTheta());
       ei.abs_costh_HH_vbfcm    = abs(HH_cm.CosTheta());
       ei.abs_costh_JJ_vbfcm    = abs(JJ_cm.CosTheta());
       ei.abs_costh_HH_b1_vbfcm = abs(HH_b1_cm.CosTheta());
       ei.abs_costh_HH_b2_vbfcm = abs(HH_b2_cm.CosTheta());
       ei.abs_costh_HH_b3_vbfcm = abs(HH_b3_cm.CosTheta());
       ei.abs_costh_HH_b4_vbfcm = abs(HH_b4_cm.CosTheta());
       ei.abs_costh_JJ_j1_vbfcm = abs(JJ_j1_cm.CosTheta());
       ei.abs_costh_JJ_j2_vbfcm = abs(JJ_j2_cm.CosTheta());  
       //COSTHETA VARIABLES in JJ CM FRAME
       TLorentzVector Sumjj = ei.JJ->P4();
       auto boost_vctr_jj = Sumjj.BoostVector();
       TLorentzVector JJ_j1_jjcm,JJ_j2_jjcm;
       JJ_j1_jjcm = ei.JJ_j1->P4(); JJ_j1_jjcm.Boost(-boost_vctr_jj);
       JJ_j2_jjcm = ei.JJ_j2->P4(); JJ_j2_jjcm.Boost(-boost_vctr_jj);
       ei.abs_costh_JJ_j1_jjcm = abs(JJ_j1_jjcm.CosTheta());
       ei.abs_costh_JJ_j2_jjcm = abs(JJ_j2_jjcm.CosTheta());
       // //The BDT always at the end, so the variables are defined for the calculation
       ei.BDT1 = GetBDT1Score(ei);
       //VBF-QCDKiller(We changed our mind, maybe for Run3 :-/ )
       ei.BDT2 = 1;// GetBDT2Score(ei);

       return;
}

float OfflineProducerHelper::Get_bRegRes(Jet jet){
    float corr = get_property(jet,Jet_bRegCorr);
    float res  = get_property(jet,Jet_bRegRes);
    float pt   = get_property(jet,Jet_pt);
    //The condition below needs to be checked in every NANOAOD release. It is not implemmented in NANOAODv5.
    if (  !( (corr>0.1) && (corr<2) && (res>0.005) && (res<0.9) ) ) {
        res = 0.2;
    }
    else if( pt < 20 ){
        res = 0.2;
    }
    else
    {
        //do nothing
    }    
    return res;
}

float OfflineProducerHelper::Get_bRegCorr(Jet jet){
    float corr = get_property(jet,Jet_bRegCorr);
    float res  = get_property(jet,Jet_bRegRes);
    float pt   = get_property(jet,Jet_pt);
    //The condition below needs to be checked in every NANOAOD release. It is not implemmented in NANOAODv5.
    if (  !( (corr>0.1) && (corr<2) && (res>0.005) && (res<0.9) ) ) {
        corr = 1.0;
    }
    else if( pt < 20 ){
        corr = 1.0;
    }
    else
    {
        //do nothing
    }    
    return corr;
}

void OfflineProducerHelper::init_BDT_evals()
{

   string strategy = any_cast<string>(parameterList_->at("bbbbChoice"));
   string dataset  = any_cast<string>(parameterList_->at("datasetname"));

   string method = "BDT method";

   string weights_BDT1 = "weights/TMVAClassification2016BSM_BDT1_breg.weights.xml"; // FIXME: OK as default?
   string weights_BDT2 = "weights/TMVAClassification_BDT2_placeholder.weights.xml"; // FIXME: OK as default?
   string weights_BDT3 = "weights/TMVAClassification2016BSM_BDT3_breg.weights.xml"; // FIXME: OK as default?
   string weights_BDT3cat1 = "weights/TMVAClassification2016BSMcat1_BDT3_breg.weights.xml"; // FIXME: OK as default?
   string weights_BDT3cat2 = "weights/TMVAClassification2016BSMcat2_BDT3_breg.weights.xml"; // FIXME: OK as default?   
    // GGF-HHKiller weights
    if(strategy=="BothClosestToMh"){
        //Define weights here if needed
    }
    else if(strategy=="BothClosestToDiagonal"){
        if (dataset=="2016")
        {
           weights_BDT1 = "weights/TMVAClassification2016BSM_BDT1_breg.weights.xml"; 
           weights_BDT3 = "weights/TMVAClassification2016BSM_BDT3_breg.weights.xml";
           weights_BDT3cat1 = "weights/TMVAClassification2016BSMcat1_BDT3_breg.weights.xml";
           weights_BDT3cat2 = "weights/TMVAClassification2016BSMcat2_BDT3_breg.weights.xml";
        }
        else if (dataset=="2017")
        {
           weights_BDT1 = "weights/TMVAClassification2017BSM_BDT1_breg.weights.xml";
           weights_BDT3 = "weights/TMVAClassification2017BSM_BDT3_breg.weights.xml";
           weights_BDT3cat1 = "weights/TMVAClassification2017BSMcat1_BDT3_breg.weights.xml";
           weights_BDT3cat2 = "weights/TMVAClassification2017BSMcat2_BDT3_breg.weights.xml";
        }
        else{
           weights_BDT1 = "weights/TMVAClassification2018BSM_BDT1_breg.weights.xml";
           weights_BDT3 = "weights/TMVAClassification2018BSM_BDT3_breg.weights.xml";
           weights_BDT3cat1 = "weights/TMVAClassification2018BSMcat1_BDT3_breg.weights.xml";
           weights_BDT3cat2 = "weights/TMVAClassification2018BSMcat2_BDT3_breg.weights.xml";
        }
    }
    else if(strategy=="MinMassDifference"){
        //Define weights here if needed
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
    eval_BDT3cat1_->init(method, weights_BDT3cat1);
    eval_BDT3cat2_->init(method, weights_BDT3cat2);
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

        float H1_pt_bdt                       = ei.H1->P4().Pt();
        float H2_pt_bdt                       = ei.H2->P4().Pt();
        float JJ_j1_pt_bdt                    = ei.JJ_j1->P4().Pt();
        float JJ_j2_pt_bdt                    = ei.JJ_j2->P4().Pt();
        float abs_JJ_eta_bdt                  = abs(ei.JJ->P4().Eta());
        float h1h2_deltaR_bdt                 = ei.h1h2_deltaR.get();
        float h1j1_deltaR_bdt                 = ei.h1j1_deltaR.get();
        float h1j2_deltaR_bdt                 = ei.h1j2_deltaR.get();
        float h2j1_deltaR_bdt                 = ei.h2j1_deltaR.get();
        float h2j2_deltaR_bdt                 = ei.h2j2_deltaR.get();
        float abs_costh_JJ_j1_vbfcm_bdt          = abs(ei.abs_costh_JJ_j1_vbfcm.get());
        float abs_costh_JJ_j2_vbfcm_bdt          = abs(ei.abs_costh_JJ_j2_vbfcm.get());
        float JJ_m_bdt                           = ei.JJ->P4().M();

        eval_BDT1_ -> floatVarsMap["H1_pt"]                                          = H1_pt_bdt;
        eval_BDT1_ -> floatVarsMap["H2_pt"]                                          = H2_pt_bdt;
        eval_BDT1_ -> floatVarsMap["JJ_j1_pt"]                                       = JJ_j1_pt_bdt;
        eval_BDT1_ -> floatVarsMap["JJ_j2_pt"]                                       = JJ_j2_pt_bdt;
        eval_BDT1_ -> floatVarsMap["abs_JJ_eta:=abs(JJ_eta)"]                        = abs_JJ_eta_bdt;
        eval_BDT1_ -> floatVarsMap["h1h2_deltaR"]                                    = h1h2_deltaR_bdt;
        eval_BDT1_ -> floatVarsMap["h1j1_deltaR"]                                    = h1j1_deltaR_bdt;
        eval_BDT1_ -> floatVarsMap["h1j2_deltaR"]                                    = h1j2_deltaR_bdt;
        eval_BDT1_ -> floatVarsMap["h2j1_deltaR"]                                    = h2j1_deltaR_bdt;
        eval_BDT1_ -> floatVarsMap["h2j2_deltaR"]                                    = h2j2_deltaR_bdt;
        eval_BDT1_ -> floatVarsMap["abs_costh_JJ_j1_vbfcm:=abs(costh_JJ_j1_vbfcm)"]  = abs_costh_JJ_j1_vbfcm_bdt;
        eval_BDT1_ -> floatVarsMap["abs_costh_JJ_j2_vbfcm:=abs(costh_JJ_j2_vbfcm)"]  = abs_costh_JJ_j2_vbfcm_bdt;
        eval_BDT1_ -> floatVarsMap["JJ_m"]                                           = JJ_m_bdt;

        return eval_BDT1_ -> eval();
}

float OfflineProducerHelper::GetBDT2Score(EventInfo& ei){
    
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
       float abs_costh_JJ_j1_cm_bdt     = abs(ei.abs_costh_JJ_j1_vbfcm.get());

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

        string dataset  = any_cast<string>(parameterList_->at("datasetname"));

        float H1_pt_bdt              =  ei.H1->P4().Pt();
        float H2_pt_bdt              =  ei.H2->P4().Pt();
        float H1_m_bdt               =  ei.H1->P4().M();
        float H2_m_bdt               =  ei.H2->P4().M();
        float HH_m_bdt               =  ei.HH->P4().M();        
        float h1h2_deltaEta_bdt      =  ei.h1h2_deltaEta.get();
        float H1_bb_deltaR_bdt       =  ei.H1_bb_deltaR.get();
        float H2_bb_deltaR_bdt       =  ei.H2_bb_deltaR.get();        
        float abs_costh_HH_b1_cm_bdt =  abs(ei.abs_costh_HH_b1_ggfcm.get());
        float HH_btag_b3_bscore_bdt  =  ei.HH_btag_b3_bscore.get();
        float HH_btag_b3_bres_bdt    =  ei.HH_btag_b3_bres.get();
        eval_BDT3_ -> floatVarsMap["H1_pt"]                                       = H1_pt_bdt;
        eval_BDT3_ -> floatVarsMap["H2_pt"]                                       = H2_pt_bdt;
        eval_BDT3_ -> floatVarsMap["H1_m"]                                        = H1_m_bdt;
        eval_BDT3_ -> floatVarsMap["H2_m"]                                        = H2_m_bdt;
        eval_BDT3_ -> floatVarsMap["HH_m"]                                        = HH_m_bdt;
        eval_BDT3_ -> floatVarsMap["h1h2_deltaEta"]                               = h1h2_deltaEta_bdt;
        eval_BDT3_ -> floatVarsMap["H1_bb_deltaR"]                                = H1_bb_deltaR_bdt;
        eval_BDT3_ -> floatVarsMap["H2_bb_deltaR"]                                = H2_bb_deltaR_bdt;
        eval_BDT3_ -> floatVarsMap["abs_costh_HH_b1_cm:=abs(costh_HH_b1_ggfcm)"]  = abs_costh_HH_b1_cm_bdt; 
        eval_BDT3_ -> floatVarsMap["HH_btag_b3_bscore"]                           = HH_btag_b3_bscore_bdt;
        eval_BDT3_ -> floatVarsMap["HH_btag_b3_bres"]                             = HH_btag_b3_bres_bdt;


        return eval_BDT3_ -> eval();
}


float OfflineProducerHelper::GetBDT3cat1Score(EventInfo& ei){
        string dataset  = any_cast<string>(parameterList_->at("datasetname"));
        float H1_pt_bdt              =  ei.H1->P4().Pt();
        float H2_pt_bdt              =  ei.H2->P4().Pt();
        float H1_m_bdt               =  ei.H1->P4().M();
        float H2_m_bdt               =  ei.H2->P4().M();      
        float h1h2_deltaEta_bdt      =  ei.h1h2_deltaEta.get();
        float H1_bb_deltaR_bdt       =  ei.H1_bb_deltaR.get();
        float H2_bb_deltaR_bdt       =  ei.H2_bb_deltaR.get();        
        float abs_costh_HH_b1_cm_bdt =  abs(ei.abs_costh_HH_b1_ggfcm.get());
        float HH_btag_b3_bscore_bdt  =  ei.HH_btag_b3_bscore.get();
        float HH_btag_b3_bres_bdt    =  ei.HH_btag_b3_bres.get();
        eval_BDT3cat1_ -> floatVarsMap["H1_pt"]                                       = H1_pt_bdt;
        eval_BDT3cat1_ -> floatVarsMap["H2_pt"]                                       = H2_pt_bdt;
        eval_BDT3cat1_ -> floatVarsMap["H1_m"]                                        = H1_m_bdt;
        eval_BDT3cat1_ -> floatVarsMap["H2_m"]                                        = H2_m_bdt;
        eval_BDT3cat1_ -> floatVarsMap["h1h2_deltaEta"]                               = h1h2_deltaEta_bdt;
        eval_BDT3cat1_ -> floatVarsMap["H1_bb_deltaR"]                                = H1_bb_deltaR_bdt;
        eval_BDT3cat1_ -> floatVarsMap["H2_bb_deltaR"]                                = H2_bb_deltaR_bdt;
        eval_BDT3cat1_ -> floatVarsMap["abs_costh_HH_b1_cm:=abs(costh_HH_b1_ggfcm)"]  = abs_costh_HH_b1_cm_bdt; 
        eval_BDT3cat1_ -> floatVarsMap["HH_btag_b3_bscore"]                           = HH_btag_b3_bscore_bdt;
        eval_BDT3cat1_ -> floatVarsMap["HH_btag_b3_bres"]                             = HH_btag_b3_bres_bdt;
        return eval_BDT3cat1_ -> eval();
}

float OfflineProducerHelper::GetBDT3cat2Score(EventInfo& ei){
        string dataset  = any_cast<string>(parameterList_->at("datasetname"));
        float H1_pt_bdt              =  ei.H1->P4().Pt();
        float H2_pt_bdt              =  ei.H2->P4().Pt();
        float H1_m_bdt               =  ei.H1->P4().M();
        float H2_m_bdt               =  ei.H2->P4().M();      
        float h1h2_deltaEta_bdt      =  ei.h1h2_deltaEta.get();
        float H1_bb_deltaR_bdt       =  ei.H1_bb_deltaR.get();
        float H2_bb_deltaR_bdt       =  ei.H2_bb_deltaR.get();        
        float abs_costh_HH_b1_cm_bdt =  abs(ei.abs_costh_HH_b1_ggfcm.get());
        float HH_btag_b3_bscore_bdt  =  ei.HH_btag_b3_bscore.get();
        float HH_btag_b3_bres_bdt    =  ei.HH_btag_b3_bres.get();
        eval_BDT3cat2_ -> floatVarsMap["H1_pt"]                                       = H1_pt_bdt;
        eval_BDT3cat2_ -> floatVarsMap["H2_pt"]                                       = H2_pt_bdt;
        eval_BDT3cat2_ -> floatVarsMap["H1_m"]                                        = H1_m_bdt;
        eval_BDT3cat2_ -> floatVarsMap["H2_m"]                                        = H2_m_bdt;
        eval_BDT3cat2_ -> floatVarsMap["h1h2_deltaEta"]                               = h1h2_deltaEta_bdt;
        eval_BDT3cat2_ -> floatVarsMap["H1_bb_deltaR"]                                = H1_bb_deltaR_bdt;
        eval_BDT3cat2_ -> floatVarsMap["H2_bb_deltaR"]                                = H2_bb_deltaR_bdt;
        eval_BDT3cat2_ -> floatVarsMap["abs_costh_HH_b1_cm:=abs(costh_HH_b1_ggfcm)"]  = abs_costh_HH_b1_cm_bdt; 
        eval_BDT3cat2_ -> floatVarsMap["HH_btag_b3_bscore"]                           = HH_btag_b3_bscore_bdt;
        eval_BDT3cat2_ -> floatVarsMap["HH_btag_b3_bres"]                             = HH_btag_b3_bres_bdt;
        return eval_BDT3cat2_ -> eval();
}


bool checkBit(int number, int bitpos)
{
    return (number & (1 << bitpos));
}

std::vector<Jet> OfflineProducerHelper::bjJets_PreselectionCut(NanoAODTree& nat, EventInfo& ei,OutputTree &ot,std::vector<Jet> jets)

{
    float bminimumDeepCSVAccepted           = any_cast<float>(parameterList_->at("bMinDeepCSV"  ));
    float bminimumDeepJetAccepted           = any_cast<float>(parameterList_->at("bMinDeepJet"  ));
    float bminimumPtAccepted                = any_cast<float>(parameterList_->at("bMinPt"       ));
    float bmaximumAbsEtaAccepted            = any_cast<float>(parameterList_->at("bMaxAbsEta"   ));
    int   bJetIdAccepted                    = any_cast<int>(parameterList_->at("bJetId"         ));
    int   bPUIdAccepted                     = any_cast<int>(parameterList_->at("bPUId"          ));
    float jminimumPtAccepted                = any_cast<float>(parameterList_->at("jMinPt"       ));
    float jmaximumAbsEtaAccepted            = any_cast<float>(parameterList_->at("jMaxAbsEta"   ));
    int   jJetIdAccepted                    = any_cast<int>(parameterList_->at("jJetId"         ));
    int   jPUIdAccepted                     = any_cast<int>(parameterList_->at("jPUId"          ));
    bool  jEENoiseVeto                      = any_cast<bool>(parameterList_->at("jEENoiseVeto"  ));
    bool  jEENoiseVetoHmumu                 = any_cast<bool>(parameterList_->at("jEENoiseVetoHmumu"  ));
    std::vector<Jet> nobjets,outputJets,btaggedjets;

    /////////////--------Select four b-jet candidates
    ///////////// Apply PT, ETA, jetid, puid requirements
    auto it = jets.begin();
    while (it != jets.end()){
        if(bminimumPtAccepted>0.){
            if( (*it).P4().Pt() < bminimumPtAccepted){
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
        if(bJetIdAccepted>=0.){
            if( ! checkBit(get_property( (*it),Jet_jetId), bJetIdAccepted) ){
                nobjets.emplace_back(*it);
                it=jets.erase(it);
                continue;
            }
        }
        if(bPUIdAccepted>=0.){
            if( !checkBit(get_property( (*it),Jet_puId), bPUIdAccepted) && get_property( (*it),Jet_pt) < 50 ){
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
    if(any_cast<bool>(parameterList_->at("FourthAntiBTagInfo"))){MinimumNumberOfBTags= 3;}
    else{MinimumNumberOfBTags= 4;}
    //Count number of b-tagged jets. If btags < 3, then reject event (not important for us) and select the four most b-tagged jets
    int btags=0;
    if(any_cast<bool>(parameterList_->at("NewestBtaggingAlgorithm")))
    {
          for (uint i=0;i<jets.size();i++ ){ if( get_property( jets.at(i) ,Jet_btagDeepFlavB) >= bminimumDeepJetAccepted) btags++;}
          stable_sort(jets.begin(), jets.end(), [](const Jet & a, const Jet & b) -> bool
          { return ( get_property(a, Jet_btagDeepFlavB) > get_property(b, Jet_btagDeepFlavB) );}); 
    }
    else
    {
          for (uint i=0;i<jets.size();i++ ){ if( get_property( jets.at(i) ,Jet_btagDeepB) >= bminimumDeepCSVAccepted) btags++;}
          stable_sort(jets.begin(), jets.end(), [](const Jet & a, const Jet & b) -> bool
          { return ( get_property(a, Jet_btagDeepB) > get_property(b, Jet_btagDeepB) );}); 
    }
    if(btags<MinimumNumberOfBTags) return outputJets;
    outputJets = {{*(jets.begin()+0),*(jets.begin()+1),*(jets.begin()+2),*(jets.begin()+3)}};
    int c=0; while (c<4){jets.erase(jets.begin());c++;}
    /////////////--------Done selecting b-jets------------------------------------

    /////////////--------Select two VBF-jet candidates based on PT and ETA1*ETA2<0
    ///////////// Apply PT and ETA requirements
    jets.insert(jets.end(), nobjets.begin(), nobjets.end());
    auto jt = jets.begin();
    while (jt != jets.end()){
        if(jminimumPtAccepted>0.){
            if( (*jt).P4().Pt() < jminimumPtAccepted){
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
        if(jJetIdAccepted>=0.){
            if( ! checkBit(get_property( (*jt),Jet_jetId), jJetIdAccepted) ){
                jt=jets.erase(jt);
                continue;
            }
        }
        if(jPUIdAccepted>=0.){
            if( !checkBit(get_property( (*jt),Jet_puId), jPUIdAccepted) && get_property( (*jt), Jet_pt) < 50 ){
                jt=jets.erase(jt);
                continue;
            }
        }
        if(jEENoiseVeto){
            if( (abs(get_property( (*jt), Jet_eta)) > 2.650) &&
                (abs(get_property( (*jt), Jet_eta)) < 3.139) &&
                ( (get_property( (*jt), Jet_pt)) * (1-get_property( (*jt), Jet_rawFactor)) < 50) ){
                jt=jets.erase(jt);
                continue;
            }
        }
        if(jEENoiseVetoHmumu){
            if( (abs(get_property( (*jt), Jet_eta)) > 2.6) &&
                (abs(get_property( (*jt), Jet_eta)) < 3.1) &&
                ( !checkBit(get_property( (*jt),Jet_puId), 2) ) ){
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
{ return ( a.P4().Pt()  >  b.P4().Pt()  );});
outputJets = {{*(jets.begin()+0),*(jets.begin()+1)}}; return outputJets;
}

std::vector<Jet> OfflineProducerHelper::OppositeEtaJetPair(std::vector<Jet> jets){
//Initialize DEta
int id;
float Eta1Eta2= 0; bool foundpair=false;
std::vector<Jet> outputJets;
//Order the jets py pt
stable_sort(jets.begin(), jets.end(), [](const Jet & a, const Jet & b) -> bool
{ return ( a.P4().Pt() >  b.P4().Pt()  );});
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
{ return ( a.P4().Pt() >  b.P4().Pt() );});
//Find the j1 (highest pt jet) and j2 (the jet with highest pt and opposite eta sign)
outputJets = {{*(jets.begin()+0),*(jets.begin()+1)}}; return outputJets;
}

// Study the minimizing distance to the diagonal (alla ATLAS)
void OfflineProducerHelper::study_diagonal_selectedjets(NanoAODTree& nat, EventInfo& ei, OutputTree &ot)
{
    //Get the preselected b-jets
    std::vector<Jet> presel_jets = {{*(ei.H1_b1),*(ei.H1_b2),*(ei.H2_b1),*(ei.H2_b2)}};

    //Do you add b-jet regression into the pairing?
    bool breg = any_cast<bool>(parameterList_->at("BjetRegression"));

    //What is the central mass you are interested in?
    float targetHiggsMass1Mass2 = any_cast<float>(parameterList_->at("TargetHiggsMass1Mass2"));
    std::vector<TLorentzVector> p4s;

    //Loop over the jets and prepare the dijets
    for (uint i = 0; i < presel_jets.size(); ++i)
        for (uint j = i+1; j < presel_jets.size(); ++j)
        {
            TLorentzVector p4sum;
            if (breg) 
               p4sum= (presel_jets.at(i).P4Regressed() + presel_jets.at(j).P4Regressed());
            else
               p4sum= (presel_jets.at(i).P4() + presel_jets.at(j).P4());
            p4s.emplace_back(p4sum);
        }

    //Order dijets by pt and get their mass
    float m1,m2,m3,m4,m5,m6;
    //Ordering Option 1 (m1,m2)
    if(p4s.at(0).Pt() > p4s.at(5).Pt())
    {
      m1 = p4s.at(0).M(); m2 = p4s.at(5).M();
    }
    else
    {
      m1 = p4s.at(5).M(); m2 = p4s.at(0).M();        
    }
    //Ordering Option 2 (m3,m4)
    if(p4s.at(1).Pt() > p4s.at(4).Pt())
    {
      m3 = p4s.at(1).M(); m4 = p4s.at(4).M();
    }
    else
    {
      m3 = p4s.at(4).M(); m4 = p4s.at(1).M();       
    }
    //Ordering Option 3 (m5,m6)
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

    float d12_34 =  (1/ TMath::Sqrt(1 + pow(targetHiggsMass1Mass2,2) ) )*fabs( m_12_34.first - ((targetHiggsMass1Mass2)*m_12_34.second) );
    float d13_24 =  (1/ TMath::Sqrt(1 + pow(targetHiggsMass1Mass2,2) ) )*fabs( m_13_24.first - ((targetHiggsMass1Mass2)*m_13_24.second) );
    float d14_23 =  (1/ TMath::Sqrt(1 + pow(targetHiggsMass1Mass2,2) ) )*fabs( m_14_23.first - ((targetHiggsMass1Mass2)*m_14_23.second) );

    //Order distances
    vector<float> distances = {d12_34, d13_24, d14_23};
    std::sort(distances.begin(), distances.end());
    float the_fst = distances.at(0);float the_snd = distances.at(1);float the_trd = distances.at(2);
    ot.userFloat("dhh_delta12") = abs(the_fst-the_snd);
    ot.userFloat("dhh_delta13") = abs(the_fst-the_trd);
    ot.userFloat("dhh_delta23") = abs(the_snd-the_trd);

    //Fill the three pairings by shortest distance
    std::vector<Jet> outputJets_thefst = presel_jets;
    std::vector<Jet> outputJets_thesnd = presel_jets;
    std::vector<Jet> outputJets_thetrd = presel_jets;
    std::vector<Jet> outputJets_theop1 = presel_jets;
    std::vector<Jet> outputJets_theop2 = presel_jets;
    //Closest
    if (the_fst == d12_34){
        outputJets_thefst.at(0) = presel_jets.at(1 - 1);outputJets_thefst.at(1) = presel_jets.at(2 - 1);
        outputJets_thefst.at(2) = presel_jets.at(3 - 1);outputJets_thefst.at(3) = presel_jets.at(4 - 1);
    }
    else if (the_fst == d13_24){
        outputJets_thefst.at(0) = presel_jets.at(1 - 1);outputJets_thefst.at(1) = presel_jets.at(3 - 1);
        outputJets_thefst.at(2) = presel_jets.at(2 - 1);outputJets_thefst.at(3) = presel_jets.at(4 - 1);
    }
    else if (the_fst == d14_23){
        outputJets_thefst.at(0) = presel_jets.at(1 - 1);outputJets_thefst.at(1) = presel_jets.at(4 - 1);
        outputJets_thefst.at(2) = presel_jets.at(2 - 1);outputJets_thefst.at(3) = presel_jets.at(3 - 1);
    }
    else
    {
        //Do nothing
    }
    //Second closest
    if (the_snd == d12_34){
        outputJets_thesnd.at(0) = presel_jets.at(1 - 1);outputJets_thesnd.at(1) = presel_jets.at(2 - 1);
        outputJets_thesnd.at(2) = presel_jets.at(3 - 1);outputJets_thesnd.at(3) = presel_jets.at(4 - 1);
    }
    else if (the_snd == d13_24){
        outputJets_thesnd.at(0) = presel_jets.at(1 - 1);outputJets_thesnd.at(1) = presel_jets.at(3 - 1);
        outputJets_thesnd.at(2) = presel_jets.at(2 - 1);outputJets_thesnd.at(3) = presel_jets.at(4 - 1);
    }
    else if (the_snd == d14_23){
        outputJets_thesnd.at(0) = presel_jets.at(1 - 1);outputJets_thesnd.at(1) = presel_jets.at(4 - 1);
        outputJets_thesnd.at(2) = presel_jets.at(2 - 1);outputJets_thesnd.at(3) = presel_jets.at(3 - 1);
    }
    else
    {
        //Do nothing
    }
    //Third closest
    if (the_trd == d12_34){
        outputJets_thetrd.at(0) = presel_jets.at(1 - 1);outputJets_thetrd.at(1) = presel_jets.at(2 - 1);
        outputJets_thetrd.at(2) = presel_jets.at(3 - 1);outputJets_thetrd.at(3) = presel_jets.at(4 - 1);
    }
    else if (the_trd == d13_24){
        outputJets_thetrd.at(0) = presel_jets.at(1 - 1);outputJets_thetrd.at(1) = presel_jets.at(3 - 1);
        outputJets_thetrd.at(2) = presel_jets.at(2 - 1);outputJets_thetrd.at(3) = presel_jets.at(4 - 1);
    }
    else if (the_trd == d14_23){
        outputJets_thetrd.at(0) = presel_jets.at(1 - 1);outputJets_thetrd.at(1) = presel_jets.at(4 - 1);
        outputJets_thetrd.at(2) = presel_jets.at(2 - 1);outputJets_thetrd.at(3) = presel_jets.at(3 - 1);
    }
    else
    {
        //Do nothing
    }

    //Just study the different orders
    compute_pairingvariables(ei,ot,outputJets_thefst,the_fst,0,"thefst");
    compute_pairingvariables(ei,ot,outputJets_thesnd,the_snd,1,"thesnd");
    compute_pairingvariables(ei,ot,outputJets_thetrd,the_trd,2,"thetrd");

    //Option 0: Closest to the diagonal
    compute_pairingvariables(ei,ot,outputJets_thefst,the_fst,-1,"theop0");
    float thefst_h1pt_cm = compute_higgsptcm(outputJets_thefst);
    float thesnd_h1pt_cm = compute_higgsptcm(outputJets_thesnd);
    //Option 1  
    //if   (dhh_delta12> 30), takes the Closest to the diagonal
    //else (dhh_delta12< 30), then switch to the one with Highest HpT at CM
    if  (abs(the_fst-the_snd)> 30){
        compute_pairingvariables(ei,ot,outputJets_thefst,the_fst,-1,"theop1");
    }
    else{
        if(thefst_h1pt_cm > thesnd_h1pt_cm)
          compute_pairingvariables(ei,ot,outputJets_thefst,the_fst,-1,"theop1");
        else
          compute_pairingvariables(ei,ot,outputJets_thesnd,the_snd,-1,"theop1");          
    }
    //Option 2:
    //Take the one with the one with Highest HpT at CM.  
    if(thefst_h1pt_cm > thesnd_h1pt_cm)
      compute_pairingvariables(ei,ot,outputJets_thefst,the_fst,-1,"theop2");
    else
      compute_pairingvariables(ei,ot,outputJets_thesnd,the_snd,-1,"theop2");  
}

float OfflineProducerHelper::compute_higgsptcm(std::vector<Jet> jets)
{
    bool breg      = any_cast<bool>(parameterList_->at("BjetRegression"));
    TLorentzVector p4sum_h1,p4sum_h2,p4sum_h1_tmp,p4sum_h2_tmp;

    if (breg) {
        p4sum_h1_tmp = (jets.at(0).P4Regressed() + jets.at(1).P4Regressed());
        p4sum_h2_tmp = (jets.at(2).P4Regressed() + jets.at(3).P4Regressed());
    }
    else{
        p4sum_h1_tmp = (jets.at(0).P4() + jets.at(1).P4());
        p4sum_h2_tmp = (jets.at(2).P4() + jets.at(3).P4());
    }
    //Order the dijets by pt
    if(p4sum_h1_tmp.Pt() > p4sum_h2_tmp.Pt())
    {
        p4sum_h1=p4sum_h1_tmp; 
        p4sum_h2=p4sum_h2_tmp; 
    }
    else{
        p4sum_h1=p4sum_h2_tmp; 
        p4sum_h2=p4sum_h1_tmp;             
    }
    //COSTHETA variables in HH CM FRAME
    TLorentzVector Sum = p4sum_h1 + p4sum_h2;
    auto boost_vctr = Sum.BoostVector();
    TLorentzVector H1_cm,H2_cm;
    H1_cm = p4sum_h1; H1_cm.Boost(-boost_vctr);
    H2_cm = p4sum_h2; H2_cm.Boost(-boost_vctr);
    float H_cmpt      = H1_cm.Pt();    

return H_cmpt;

}

float OfflineProducerHelper::MindR(std::vector<Jet> jets, bool min){

    std::vector<float> drs;
    //Loop over the jets and prepare the dijets
    for (uint i = 0; i < jets.size(); ++i)
        for (uint j = i+1; j < jets.size(); ++j)
        {
            drs.emplace_back(   (jets.at(i).P4()).DeltaR(jets.at(j).P4()) );
        }
     
    float output; 
    if (min) 
        output = *std::min_element(drs.begin(), drs.end());
    else 
        output = *std::max_element(drs.begin(), drs.end());

    return output;  
}

void OfflineProducerHelper::compute_pairingvariables(EventInfo& ei, OutputTree &ot,std::vector<Jet> jets, float distance, int sort, string tag)
{
    bool breg      = any_cast<bool>(parameterList_->at("BjetRegression"));
    bool is_signal = any_cast<bool>(parameterList_->at("is_signal"));
    TLorentzVector p4sum_h1,p4sum_h2,p4sum_h1_tmp,p4sum_h2_tmp;
    float dRbb1,dRbb2,dRbb1_tmp,dRbb2_tmp;  
    float bsum1,bsum2,bsum1_tmp,bsum2_tmp;
    float mindRbb = MindR(jets,true);
    float maxdRbb = MindR(jets,false);
    dRbb1_tmp     = jets.at(0).P4().DeltaR(jets.at(1).P4());
    dRbb2_tmp     = jets.at(2).P4().DeltaR(jets.at(3).P4());
    bsum1_tmp     = get_property(jets.at(0),Jet_btagDeepFlavB) + get_property(jets.at(1),Jet_btagDeepFlavB);
    bsum2_tmp     = get_property(jets.at(2),Jet_btagDeepFlavB) + get_property(jets.at(3),Jet_btagDeepFlavB);

    if (breg) {
        p4sum_h1_tmp = (jets.at(0).P4Regressed() + jets.at(1).P4Regressed());
        p4sum_h2_tmp = (jets.at(2).P4Regressed() + jets.at(3).P4Regressed());
    }
    else{
        p4sum_h1_tmp = (jets.at(0).P4() + jets.at(1).P4());
        p4sum_h2_tmp = (jets.at(2).P4() + jets.at(3).P4());
    }
    //Order the dijets by pt
    if(p4sum_h1_tmp.Pt() > p4sum_h2_tmp.Pt())
    {
        p4sum_h1=p4sum_h1_tmp; 
        p4sum_h2=p4sum_h2_tmp; 
        dRbb1=dRbb1_tmp;
        dRbb2=dRbb2_tmp;
        bsum1=bsum1_tmp;
        bsum2=bsum2_tmp;
    }
    else{
        p4sum_h1=p4sum_h2_tmp; 
        p4sum_h2=p4sum_h1_tmp;             
        dRbb1=dRbb2_tmp;
        dRbb2=dRbb1_tmp;
        bsum1=bsum2_tmp;
        bsum2=bsum1_tmp;
    }
    //COSTHETA variables in HH CM FRAME
    TLorentzVector Sum = p4sum_h1 + p4sum_h2;
    auto boost_vctr = Sum.BoostVector();
    TLorentzVector H1_cm,H2_cm;
    H1_cm = p4sum_h1; H1_cm.Boost(-boost_vctr);
    H2_cm = p4sum_h2; H2_cm.Boost(-boost_vctr);
    float abscostheta  = abs(H1_cm.CosTheta());
    float H1_cmboost   = H1_cm.Beta();    
    float H2_cmboost   = H2_cm.Beta(); 
    float H1_cmpt      = H1_cm.Pt();    
    float H2_cmpt      = H2_cm.Pt(); 
    float H1H2_cmboost_ratio = H1_cmboost/H2_cmboost;
    float H1H2_cmpt_ratio    = H1_cmpt/H2_cmpt;
    //Fill the information
    ot.userFloat(Form("%s_dhh",tag.c_str()))        = distance;
    ot.userFloat(Form("%s_cm_abscostheta",tag.c_str()))= abscostheta;    
    ot.userFloat(Form("%s_H1_cm_boost",tag.c_str()))         = H1_cmboost;  
    ot.userFloat(Form("%s_H2_cm_boost",tag.c_str()))         = H2_cmboost; 
    ot.userFloat(Form("%s_H1_cm_pt",tag.c_str()))         = H1_cmpt;  
    ot.userFloat(Form("%s_H2_cm_pt",tag.c_str()))         = H2_cmpt; 
    ot.userFloat(Form("%s_cm_boost_h1h2ratio",tag.c_str())) = H1H2_cmboost_ratio;
    ot.userFloat(Form("%s_cm_pt_h1h2ratio",tag.c_str()))    = H1H2_cmpt_ratio;
    ot.userFloat(Form("%s_mindR",tag.c_str()))      = mindRbb;
    ot.userFloat(Form("%s_maxdR",tag.c_str()))      = maxdRbb;
    ot.userFloat(Form("%s_h1h2deltaR",tag.c_str())) = p4sum_h1.DeltaR(p4sum_h2);
    ot.userFloat(Form("%s_H1_m",tag.c_str()))       = p4sum_h1.M();
    ot.userFloat(Form("%s_H1_pt",tag.c_str()))      = p4sum_h1.Pt();
    ot.userFloat(Form("%s_H1_dRbb",tag.c_str()))    = dRbb1;
    ot.userFloat(Form("%s_H1_bsum",tag.c_str()))    = bsum1;
    ot.userFloat(Form("%s_H2_m",tag.c_str()))       = p4sum_h2.M();
    ot.userFloat(Form("%s_H2_pt",tag.c_str() ))     = p4sum_h2.Pt();
    ot.userFloat(Form("%s_H2_dRbb",tag.c_str()))    = dRbb2;
    ot.userFloat(Form("%s_H2_bsum",tag.c_str()))    = bsum2;
    ot.userFloat(Form("%s_HH_m",tag.c_str()))       = (p4sum_h1 + p4sum_h2).M();
    ot.userFloat(Form("%s_HH_pt",tag.c_str()))      = (p4sum_h1 + p4sum_h2).Pt();
    //If the four selected b-jets are matched the Higgs b's, then check if the pairing is correct and save info
    if(is_signal)
    {
           int nh=0;
           if (ei.HH_qual.get()==4)
           {
              //Check if the first di-jet comes from a Higgs
              if( (   GetQuarkIdentifier(ei,jets.at(0))==0 &&  GetQuarkIdentifier(ei,jets.at(1))==1) 
               || (   GetQuarkIdentifier(ei,jets.at(0))==1 &&  GetQuarkIdentifier(ei,jets.at(1))==0) 
               || (   GetQuarkIdentifier(ei,jets.at(0))==2 &&  GetQuarkIdentifier(ei,jets.at(1))==3)   
               || (   GetQuarkIdentifier(ei,jets.at(0))==3 &&  GetQuarkIdentifier(ei,jets.at(1))==2)){nh++;}
              //Check if the second di-jet comes from a Higgs
              if( (   GetQuarkIdentifier(ei,jets.at(2))==0 &&  GetQuarkIdentifier(ei,jets.at(3))==1) 
               || (   GetQuarkIdentifier(ei,jets.at(2))==1 &&  GetQuarkIdentifier(ei,jets.at(3))==0) 
               || (   GetQuarkIdentifier(ei,jets.at(2))==2 &&  GetQuarkIdentifier(ei,jets.at(3))==3)   
               || (   GetQuarkIdentifier(ei,jets.at(2))==3 &&  GetQuarkIdentifier(ei,jets.at(3))==2)){nh++;}
           }
           if(nh==2)
           {
              if(sort != -1)
              {
                   ot.userFloat("thetrh_dhh")                = distance;
                   ot.userFloat("thetrh_cm_abscostheta")     = abscostheta; 
                   ot.userFloat("thetrh_H1_cm_boost")        = H1_cmboost;  
                   ot.userFloat("thetrh_H2_cm_boost")        = H2_cmboost; 
                   ot.userFloat("thetrh_H1_cm_pt")           = H1_cmpt;  
                   ot.userFloat("thetrh_H2_cm_pt")           = H2_cmpt; 
                   ot.userFloat("thetrh_cm_boost_h1h2ratio") = H1H2_cmboost_ratio;
                   ot.userFloat("thetrh_cm_pt_h1h2ratio")    = H1H2_cmpt_ratio;
                   ot.userFloat("thetrh_mindR")              = mindRbb;
                   ot.userFloat("thetrh_maxdR")              = maxdRbb;
                   ot.userFloat("thetrh_h1h2deltaR")         = p4sum_h1.DeltaR(p4sum_h2);
                   ot.userFloat("thetrh_H1_m")               = p4sum_h1.M();
                   ot.userFloat("thetrh_H1_pt")              = p4sum_h1.Pt();
                   ot.userFloat("thetrh_H1_dRbb")            = dRbb1;
                   ot.userFloat("thetrh_H1_bsum")            = bsum1;
                   ot.userFloat("thetrh_H2_m")               = p4sum_h2.M();
                   ot.userFloat("thetrh_H2_pt")              = p4sum_h2.Pt();
                   ot.userFloat("thetrh_H2_dRbb")            = dRbb2;
                   ot.userFloat("thetrh_H2_bsum")            = bsum2;
                   ot.userFloat("thetrh_HH_m")               = (p4sum_h1 + p4sum_h2).M();
                   ot.userFloat("thetrh_HH_pt")              = (p4sum_h1 + p4sum_h2).Pt();
                   ot.userInt("thetrh_sort")                 = sort;
              }
              ot.userInt(Form("%s_qual",tag.c_str()))   = 1;
           }
           else{
               ot.userInt(Form("%s_qual",tag.c_str()))   = 0;    
           }
    }


}

int OfflineProducerHelper::GetQuarkIdentifier(EventInfo& ei, Jet jet)
{
   //Get the matched jet idx to the quarks, and the selected jets idx
   std::vector<int> q_idx;
   int qid=-1;
   q_idx = {ei.gen_H1_b1_jetidx.get(), ei.gen_H1_b2_jetidx.get(),
            ei.gen_H2_b1_jetidx.get(), ei.gen_H2_b2_jetidx.get(), 
            ei.gen_q1_out_jetidx.get(),ei.gen_q2_out_jetidx.get()};
   //Check the coincidence
   for(uint j=0; j<q_idx.size();j++) if( jet.getIdx() == q_idx.at(j)) {qid=j;}    

   return qid;
}


void OfflineProducerHelper::study_gen_selectedjets(NanoAODTree& nat, EventInfo& ei)
{
///Add some genjetidx,partonflavour,hadronflavour
ei.H1_b1_genJetIdx = get_property(ei.H1_b1.get(),Jet_genJetIdx);
ei.H1_b2_genJetIdx = get_property(ei.H1_b2.get(),Jet_genJetIdx);
ei.H2_b1_genJetIdx = get_property(ei.H2_b1.get(),Jet_genJetIdx);
ei.H2_b2_genJetIdx = get_property(ei.H2_b2.get(),Jet_genJetIdx);
if(ei.VBFEvent.get() == 1) ei.JJ_j1_genJetIdx = get_property(ei.JJ_j1.get(),Jet_genJetIdx);
if(ei.VBFEvent.get() == 1) ei.JJ_j2_genJetIdx = get_property(ei.JJ_j2.get(),Jet_genJetIdx);
ei.H1_b1_partonFlavour = get_property(ei.H1_b1.get(),Jet_partonFlavour);
ei.H1_b2_partonFlavour = get_property(ei.H1_b2.get(),Jet_partonFlavour);
ei.H2_b1_partonFlavour = get_property(ei.H2_b1.get(),Jet_partonFlavour);
ei.H2_b2_partonFlavour = get_property(ei.H2_b2.get(),Jet_partonFlavour);
if(ei.VBFEvent.get() == 1) ei.JJ_j1_partonFlavour = get_property(ei.JJ_j1.get(),Jet_partonFlavour);
if(ei.VBFEvent.get() == 1) ei.JJ_j2_partonFlavour = get_property(ei.JJ_j2.get(),Jet_partonFlavour);
ei.H1_b1_hadronFlavour = get_property(ei.H1_b1.get(),Jet_hadronFlavour);
ei.H1_b2_hadronFlavour = get_property(ei.H1_b2.get(),Jet_hadronFlavour);
ei.H2_b1_hadronFlavour = get_property(ei.H2_b1.get(),Jet_hadronFlavour);
ei.H2_b2_hadronFlavour = get_property(ei.H2_b2.get(),Jet_hadronFlavour);
if(ei.VBFEvent.get() == 1) ei.JJ_j1_hadronFlavour = get_property(ei.JJ_j1.get(),Jet_hadronFlavour);
if(ei.VBFEvent.get() == 1) ei.JJ_j2_hadronFlavour = get_property(ei.JJ_j2.get(),Jet_hadronFlavour);
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

//Fill qid
ei.H1_b1_qid = GetQuarkIdentifier(ei,ei.H1_b1.get());
ei.H1_b2_qid = GetQuarkIdentifier(ei,ei.H1_b2.get());
ei.H2_b1_qid = GetQuarkIdentifier(ei,ei.H2_b1.get());
ei.H2_b2_qid = GetQuarkIdentifier(ei,ei.H2_b2.get());
if(ei.VBFEvent.get() == 1)
{
    ei.JJ_j1_qid = GetQuarkIdentifier(ei,ei.JJ_j1.get());
    ei.JJ_j2_qid = GetQuarkIdentifier(ei,ei.JJ_j2.get());
}
else{
    ei.JJ_j1_qid = -1;
    ei.JJ_j2_qid = -1;
}

//Put all ids in one array
std::vector<int> jsel_qid = {ei.H1_b1_qid.get(),ei.H1_b2_qid.get(),ei.H2_b1_qid.get(),ei.H2_b2_qid.get(),ei.JJ_j1_qid.get(),ei.JJ_j2_qid.get()};

//---Fill the quality of the selection
//Fill H1 quality
int h1qual;
if(      (jsel_qid.at(0)==0 && jsel_qid.at(1)==1) || (jsel_qid.at(0)==1 && jsel_qid.at(1)==0) ){h1qual=1;}
else if( (jsel_qid.at(0)==2 && jsel_qid.at(1)==3) || (jsel_qid.at(0)==3 && jsel_qid.at(1)==2) ){h1qual=2;}
else{
    h1qual=0;
}
//Fill H2 quality
int h2qual;
if(      (jsel_qid.at(2)==0 && jsel_qid.at(3)==1) || (jsel_qid.at(2)==1 && jsel_qid.at(3)==0) ){h2qual=1;}
else if( (jsel_qid.at(2)==2 && jsel_qid.at(3)==3) || (jsel_qid.at(2)==3 && jsel_qid.at(3)==2) ){h2qual=2;}
else{
    h2qual=0;
}
//Fill H1_b1 quality
int h1b1qual,hhqual=0;
if(      (jsel_qid.at(0)==0 || jsel_qid.at(0)==1) ){h1b1qual=1;hhqual++;}
else if( (jsel_qid.at(0)==2 || jsel_qid.at(0)==3) ){h1b1qual=2;hhqual++;}
else{
    h1b1qual=0;
}
//Fill H1_b2_ quality
int h1b2qual;
if(      (jsel_qid.at(1)==0 || jsel_qid.at(1)==1) ){h1b2qual=1;hhqual++;}
else if( (jsel_qid.at(1)==2 || jsel_qid.at(1)==3) ){h1b2qual=2;hhqual++;}
else{
    h1b2qual=0;
}
//Fill H2_b1 quality
int h2b1qual;
if(      (jsel_qid.at(2)==0 || jsel_qid.at(2)==1) ){h2b1qual=1;hhqual++;}
else if( (jsel_qid.at(2)==2 || jsel_qid.at(2)==3) ){h2b1qual=2;hhqual++;}
else{
    h2b1qual=0;
}
//Fill H2_b2_ quality
int h2b2qual;
if(      (jsel_qid.at(3)==0 || jsel_qid.at(3)==1) ){h2b2qual=1;hhqual++;}
else if( (jsel_qid.at(3)==2 || jsel_qid.at(3)==3) ){h2b2qual=2;hhqual++;}
else{
    h2b2qual=0;
}
//Fill JJ_j1 quality
int j1qual,jjqual=0;
if(      jsel_qid.at(4)==4 ){j1qual=1;jjqual++;}
else if( jsel_qid.at(4)==5 ){j1qual=2;jjqual++;}
else{
    j1qual=0;
}
//Fill JJ_j2 quality
int j2qual;
if(      jsel_qid.at(5)==4 ){j2qual=1;jjqual++;}
else if( jsel_qid.at(5)==5 ){j2qual=2;jjqual++;}
else{
    j2qual=0;
}
//Fill qualities
ei.H1_b1_qual = h1b1qual;
ei.H1_b2_qual = h1b2qual;
ei.H2_b1_qual = h2b1qual;
ei.H2_b2_qual = h2b2qual;
ei.JJ_j1_qual = j1qual;
ei.JJ_j2_qual = j2qual;
ei.H1_qual    = h1qual;
ei.H2_qual    = h2qual;
ei.HH_qual    = hhqual;
ei.JJ_qual    = jjqual;

}


void OfflineProducerHelper::match_gen_recojets(NanoAODTree& nat, EventInfo& ei, bool vbfmode)
{
std::vector<Jet> jets;
std::vector<int> quark_jetidxs, quark_jetmatched;
jets.reserve(*(nat.nJet));

//Get the jet collection
for (uint ij = 0; ij < *(nat.nJet); ++ij)
{
    jets.emplace_back(Jet(ij, &nat));
}
//Get the jet idx of the matched jet to the quark
quark_jetidxs  = GetMatchedJetIdx(nat,ei,jets,vbfmode);
//Get flag if matched or jot to a jet
for(uint k=0;  k<quark_jetidxs.size();k++)
{ 
    if(quark_jetidxs.at(k) != -1)
    {
       quark_jetmatched.push_back(1); 
    }
    else{
       quark_jetmatched.push_back(0);        
    }  
}
//Fill the branch with matched-jet index
ei.gen_H1_b1_jetidx      = quark_jetidxs.at(0);
ei.gen_H1_b2_jetidx      = quark_jetidxs.at(1);
ei.gen_H2_b1_jetidx      = quark_jetidxs.at(2);
ei.gen_H2_b2_jetidx      = quark_jetidxs.at(3);
ei.gen_q1_out_jetidx     = quark_jetidxs.at(4);
ei.gen_q2_out_jetidx     = quark_jetidxs.at(5);
//Fill the branches with jet matched flags
ei.gen_H1_b1_jetmatched  = quark_jetmatched.at(0);
ei.gen_H1_b2_jetmatched  = quark_jetmatched.at(1);
ei.gen_H2_b1_jetmatched  = quark_jetmatched.at(2);
ei.gen_H2_b2_jetmatched  = quark_jetmatched.at(3);
ei.gen_q1_out_jetmatched = quark_jetmatched.at(4);
ei.gen_q2_out_jetmatched = quark_jetmatched.at(5);

int HH_qual=0;
for(uint k=0;  k<4;k++)
{ 
    if(quark_jetidxs.at(k) != -1) HH_qual++;
}
int qq_qual=0;
for(uint k=4;  k<quark_jetidxs.size();k++)
{ 
    if(quark_jetidxs.at(k) != -1) qq_qual++;
}
ei.gen_HH_qual = HH_qual;
ei.gen_qq_qual = qq_qual;

}

std::vector<int> OfflineProducerHelper::GetMatchedJetIdx(NanoAODTree& nat, EventInfo& ei, const std::vector<Jet> jets, bool vbfmode)
{
    //Order the generator level information by specific PT
    order_by_pT(ei.gen_H1_b1.get(), ei.gen_H1_b2.get());
    order_by_pT(ei.gen_H2_b1.get(), ei.gen_H2_b2.get());
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
    std::vector<GenPart> quarks;
    if (vbfmode){
        //Define a vector with the six ordered quarks
        order_by_pT(ei.gen_q1_out.get(), ei.gen_q2_out.get());    
        quarks = {*(ei.gen_H1_b1),*(ei.gen_H1_b2),*(ei.gen_H2_b1),*(ei.gen_H2_b2), *(ei.gen_q1_out),*(ei.gen_q2_out)};
    }
    else{
        //Define a vector with the four ordered quarks
        quarks = {*(ei.gen_H1_b1),*(ei.gen_H1_b2),*(ei.gen_H2_b1),*(ei.gen_H2_b2)};
    } 

    std::vector<int> quark_jetidxs = JetToQuarkMatcher(quarks,jets,vbfmode);
    return quark_jetidxs;
 }


std::vector<int> OfflineProducerHelper::JetToQuarkMatcher(const std::vector<GenPart> quarks, std::vector<Jet> jets, bool vbfmode){
//Define and initialize variables
std::vector<int> quark_jetidx;
int id;
float maxDeltaR, DeltaR; bool matched;
//Create all possible combinations and find the jet set that minimize the deltaR
for (uint x=0;x < quarks.size();x++ )
{
   maxDeltaR = 0.4;
   matched   = false;
   id=-1;
   for (uint y=0;y < jets.size(); y++ ){
        if ( get_property(jets[y],Jet_genJetIdx) == -1) continue; 
        DeltaR = quarks[x].P4().DeltaR(jets[y].P4());
        if(DeltaR < maxDeltaR){maxDeltaR=DeltaR; matched=true; id = y ;}
   }
   if(matched) 
   {
    quark_jetidx.emplace_back(jets[id].getIdx()); 
    jets.erase(jets.begin()+id);
   }
   else
   {
    quark_jetidx.emplace_back(-1);
   }
}
if(!vbfmode){quark_jetidx.emplace_back(-1);quark_jetidx.emplace_back(-1);}

return quark_jetidx;
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
                cout << "** [WARNING] : select_gen_HH : more than two Higgs found" << endl;
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
                cout << "** [WARNING] : select_gen_HH : more than two Higgs found (last copy)" << endl;
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
        cout << "** [WARNING] : select_gen_bb_bb : you need to search for H1 and H2 before" << endl;
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
                cout << "** [WARNING] : select_gen_bb_bb : more than two b from H1 found" << endl;
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
                cout << "** [WARNING] : select_gen_bb_bb : more than two b from H2 found" << endl;
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
