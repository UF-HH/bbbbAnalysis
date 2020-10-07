#ifndef OFFLINEPRODUCERHELPER_H
#define OFFLINEPRODUCERHELPER_H

/**
** class  : OfflineProducerHelper
** author : L. Cadamuro (UF)
** date   : 10/01/2018
** brief  : A namespace containing a set of helper functions to analyze the events and process the skims
** note   : the prototype of the general function accepts as first input the NanoAODTree 
**          and as the second a reference to EventInto.
**          It's a flexible way to let the function know what it should know on the event, and possibly set extra information
**/

#include <TH1F.h>
#include <TLorentzVector.h>
#include "CfgParser.h"
#include "NanoAODTree.h"
#include "EventInfo.h"
#include "CompositeCandidate.h"
#include "Candidate.h"
#include "OutputTree.h"
#include "BTagCalibrationStandalone.h"
#include "JetMETCorrections/Modules/interface/JetResolution.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "Jet.h"
#include "GenJet.h"
#include "SkimEffCounter.h"
#include "BDTEval.h"
#include "HHReweight5D.h"
#include "TriggerEfficiencyCalculator.h"
#include "TriggerEfficiencyCalculator_TriggerMatched.h"

#include <array>
#include <utility>
#include <functional>
#include <initializer_list>
#include <boost/optional.hpp>
#include <any>

// namespace OfflineProducerHelper {
class OfflineProducerHelper{
    
    public:

        OfflineProducerHelper ()
        {
            // BDT evals
            eval_BDT1_ = std::unique_ptr<BDTEval> (new BDTEval (float_varlist_BDT1, std::vector<std::string>(0)) );
            eval_BDT2_ = std::unique_ptr<BDTEval> (new BDTEval (float_varlist_BDT2, std::vector<std::string>(0)) );
            eval_BDT3_ = std::unique_ptr<BDTEval> (new BDTEval (float_varlist_BDT3, std::vector<std::string>(0)) );
            eval_BDT3cat1_ = std::unique_ptr<BDTEval> (new BDTEval (float_varlist_BDT3cat1, std::vector<std::string>(0)) );
            eval_BDT3cat2_ = std::unique_ptr<BDTEval> (new BDTEval (float_varlist_BDT3cat2, std::vector<std::string>(0)) );
            // HH reweighters
            hhreweighter_    = std::unique_ptr<HHReweight5D>(nullptr);
            hhreweighter_kl_ = 1.0;
        };

        ~OfflineProducerHelper() {};

        std::unique_ptr<BDTEval> eval_BDT1_;
        std::unique_ptr<BDTEval> eval_BDT2_;
        std::unique_ptr<BDTEval> eval_BDT3_;
        std::unique_ptr<BDTEval> eval_BDT3cat1_;
        std::unique_ptr<BDTEval> eval_BDT3cat2_;
        std::unique_ptr<HHReweight5D> hhreweighter_;
        float hhreweighter_kl_;

        TriggerEfficiencyCalculator *theTriggerEfficiencyCalculator_ {nullptr};
        TriggerEfficiencyCalculator_TriggerMatched *theTriggerEfficiencyCalculator_TriggerMatched_ {nullptr};
        bool debug = true;
        // Load configurations to match the b jets
        // bool loadConfiguration(CfgParser config);
        ///static bacause if not I got a glibc detected when the execution is completed

        const std::map<std::string, std::any> *parameterList_;
        //  weights are stored together with their corrections:
        //  map < weightName,     pair < nominal ,  map < corrName  , corrValue > > >  
        std::map<std::string, std::pair< float, std::map<std::string, float> > > weightMap_;
        // std::map<std::string, TH1D*> PUWeightHistogramMap_;
        std::map<std::string, std::map<std::pair<float,float>,float> > PUWeightMap_;
        std::map<std::string, JetCorrectionUncertainty*> mapForJECuncertanties_;
        std::map<std::string, Variation> mapJERNamesAndVariation_;
        std::map<std::string, bool> mapJECNamesAndVariation_;
        // < particleId, <filterId> > 
        std::map<int, std::vector<int> >  mapTriggerObjectIdAndFilter_;
        // < <particleId, filterId> numberOfObjects> 
        std::map<std::pair<int,int>, int> mapTriggerMatching_;

        std::map< std::string, std::vector<std::map<std::pair<int,int>, bool>>> triggerObjectPerJetCount_;
        std::map< std::string, std::map<std::pair<int,int>, int>> triggerObjectTotalCount_;
        void initializeTriggerMatching(OutputTree &ot, int numberOfCandidates);
        void calculateTriggerMatching(const std::vector< std::unique_ptr<const Candidate> > &candidateList, NanoAODTree& nat);
        bool checkTriggerObjectMatching(std::vector<std::string>, OutputTree &ot);
        void CalculateTriggerScaleFactor(NanoAODTree& nat,OutputTree &ot, EventInfo& ei, std::vector<Jet> presel_jets);
        void TriggerMatchingModule(NanoAODTree& nat,OutputTree &ot, EventInfo& ei,std::vector<std::string> listOfPassedTriggers);
        // branch Name, default value
        std::map<std::string, float> branchesAffectedByJetEnergyVariations_;
        float sampleCrossSection_;

        // to apply a single jet energy correction to all the incoming jets
        const static std::string nominal_jes_syst_shift_name;
        std::string jes_syst_shift_name_ = nominal_jes_syst_shift_name;
        bool        jes_syst_shift_dir_is_up_;
        std::unique_ptr<JetCorrectorParameters>   jcp_;
        std::unique_ptr<JetCorrectionUncertainty> jcu_;

        // to define the smearing parameters of all the incoming jets
        Variation jer_dir_;      // NOMINAL, UP, DOWN
        Variation breg_jer_dir_; // b regression smearing does not use the same class for the smearing, but I encode nominal, up, down with the same enum

        // -------------------------------------------------------------------------------
        // helpers for the BDT evaluation
        std::vector<std::string> float_varlist_BDT1 = {
            "H1_pt",
            "H2_pt",
            "JJ_j1_pt",
            "JJ_j2_pt",
            "abs_JJ_eta:=abs(JJ_eta)",
            "h1h2_deltaR",
            "h1j1_deltaR",
            "h1j2_deltaR",
            "h2j1_deltaR",
            "h2j2_deltaR",
            "abs_costh_JJ_j1_vbfcm:=abs(costh_JJ_j1_vbfcm)",
            "abs_costh_JJ_j2_vbfcm:=abs(costh_JJ_j2_vbfcm)",
            "JJ_m",
        };

        std::vector<std::string> float_varlist_BDT2 = {
            "HH_b3_pt",
            "HH_b4_pt",
            "JJ_j1_qgl",
            "JJ_j2_qgl",
            "H1_m",
            "H2_m",
            "H1_bb_deltaR",
            "H2_bb_deltaR",
            "JJ_m",
            "j1j2_deltaEta",
            "abs_costh_JJ_j1_cm:=abs(costh_JJ_j1_cm)",
        };

        std::vector<std::string> float_varlist_BDT3= {
            "H1_pt",
            "H2_pt",
            "H1_m",
            "H2_m",
            "HH_m",
            "h1h2_deltaEta",
            "H1_bb_deltaR",
            "H2_bb_deltaR",
            "abs_costh_HH_b1_cm:=abs(costh_HH_b1_ggfcm)",
            "HH_btag_b3_bscore", 
            "HH_btag_b3_bres",           
        };            

        std::vector<std::string> float_varlist_BDT3cat1= {
            "H1_pt",
            "H2_pt",
            "H1_m",
            "H2_m",
            "h1h2_deltaEta",
            "H1_bb_deltaR",
            "H2_bb_deltaR",
            "abs_costh_HH_b1_cm:=abs(costh_HH_b1_ggfcm)",
            "HH_btag_b3_bscore", 
            "HH_btag_b3_bres",           
        };  

        std::vector<std::string> float_varlist_BDT3cat2= {
            "H1_pt",
            "H2_pt",
            "H1_m",
            "H2_m",
            "h1h2_deltaEta",
            "H1_bb_deltaR",
            "H2_bb_deltaR",
            "abs_costh_HH_b1_cm:=abs(costh_HH_b1_ggfcm)",
            "HH_btag_b3_bscore", 
            "HH_btag_b3_bres",           
        }; 

        void init_BDT_evals();
        void init_HH_reweighter(OutputTree& ot, std::string coeffFile, std::string hhreweighterInputMap, std::string histoName);

        // -------------------------------------------------------------------------------

        // All maps need to be cleared otherwise we have a glibc detected
        // segfault issue fixed by making oph a class -> removing function
        // void clean() {
        //     weightMap_.clear();
        //     PUWeightMap_.clear();
        //     mapForJECuncertanties_.clear();
        //     mapJERNamesAndVariation_.clear();
        //     mapJECNamesAndVariation_.clear();
        //     branchesAffectedByJetEnergyVariations_.clear();
        //     mapTriggerMatching_.clear();
        //     mapTriggerObjectIdAndFilter_.clear();

        //     eval_BDT1_->floatVarsMap.clear();
        //     eval_BDT2_->floatVarsMap.clear();
        //     eval_BDT3_->floatVarsMap.clear();
        //     eval_BDT1_->intVarsMap.clear();
        //     eval_BDT2_->intVarsMap.clear();
        //     eval_BDT3_->intVarsMap.clear();
        // }

        void initializeOfflineProducerHelper(const std::map<std::string, std::any> *parameterList) {
            parameterList_ = parameterList;
            //standard branches present in the EventInfo, other branches should de added when declaring the standard ones (see bTagScaleFactor_central)
            branchesAffectedByJetEnergyVariations_["H1_b1_pt"] = -1.;
            branchesAffectedByJetEnergyVariations_["H1_b2_pt"] = -1.;
            branchesAffectedByJetEnergyVariations_["H2_b1_pt"] = -1.;
            branchesAffectedByJetEnergyVariations_["H2_b2_pt"] = -1.;
            // branchesAffectedByJetEnergyVariations_["H1_b1_m"] = -1.;
            // branchesAffectedByJetEnergyVariations_["H1_b2_m"] = -1.;
            // branchesAffectedByJetEnergyVariations_["H2_b1_m"] = -1.;
            // branchesAffectedByJetEnergyVariations_["H2_b2_m"] = -1.;
            branchesAffectedByJetEnergyVariations_["H1_b1_ptRegressed"] = -1.;
            branchesAffectedByJetEnergyVariations_["H1_b2_ptRegressed"] = -1.;
            branchesAffectedByJetEnergyVariations_["H2_b1_ptRegressed"] = -1.;
            branchesAffectedByJetEnergyVariations_["H2_b2_ptRegressed"] = -1.;
            branchesAffectedByJetEnergyVariations_["H1_m"] = -1.;
            branchesAffectedByJetEnergyVariations_["H2_m"] = -1.;
            // branchesAffectedByJetEnergyVariations_["H1_pt"] = -1.;
            // branchesAffectedByJetEnergyVariations_["H2_pt"] = -1.;
            branchesAffectedByJetEnergyVariations_["HH_m"] = -1.;
            // branchesAffectedByJetEnergyVariations_["HH_pt"] = -1.;
            branchesAffectedByJetEnergyVariations_["HH_m_kinFit"] = -1.;
            branchesAffectedByJetEnergyVariations_["HH_2DdeltaM"] = 999.;

            init_BDT_evals();
            // if (parameterList_->count("do_kl_rew") > 0 && std::any_cast<bool>(parameterList_->at("do_kl_rew")))
            //     init_HH_reweighter(
            //         std::any_cast<std::string>(parameterList_->at("kl_coeffs")),
            //         std::any_cast<std::string>(parameterList_->at("kl_map")),
            //         std::any_cast<std::string>(parameterList_->at("kl_histo")));
        }
        //for TriggerSFs without matching
        void initializeTriggerScaleFactors(NanoAODTree& nat, OutputTree& ot)
        {
            if( std::any_cast<bool>(parameterList_->at("UseTriggerScaleFactor")) )
            {
                int year = std::any_cast<int>(parameterList_->at("DatasetYear"));
                std::string triggerEfficiencyFileName = std::any_cast<std::string>(parameterList_->at("TriggerEfficiencyFileName"));
                //bool matchWithTriggerObjects = std::any_cast<bool>(parameterList_->at("MatchWithSelectedObjects"));
                bool matchWithTriggerObjects = false;

                if(year == 2016) 
                {
                    theTriggerEfficiencyCalculator_ = new TriggerEfficiencyCalculator_2016(triggerEfficiencyFileName, nat, matchWithTriggerObjects);
                }
                else if(year == 2017) 
                {
                    theTriggerEfficiencyCalculator_ = new TriggerEfficiencyCalculator_2017(triggerEfficiencyFileName, nat, matchWithTriggerObjects);
                }
                else if(year == 2018)
                {
                    theTriggerEfficiencyCalculator_ = new TriggerEfficiencyCalculator_2018(triggerEfficiencyFileName, nat, matchWithTriggerObjects);
                }
                else
                {
                    std::cout<<"Trigger scale factor year can be 2016, 2017 or 2018. Aborting..." << std::endl;
                    abort();
                }
                bool simulateTrigger = std::any_cast<bool>(parameterList_->at("SimulateTrigger"));
                if(simulateTrigger) theTriggerEfficiencyCalculator_->simulateTrigger(&ot);
                //theTriggerEfficiencyCalculator_->applyTurnOnCut(std::any_cast<bool>(parameterList_->at("ApplyTurnOnCuts")));
            }
        }

        void initializeTriggerScaleFactors_TriggerMatched(NanoAODTree& nat, OutputTree& ot)
        {
            if( std::any_cast<bool>(parameterList_->at("UseTriggerScaleFactor")) )
            {
                int year = std::any_cast<int>(parameterList_->at("DatasetYear"));
                std::string triggerEfficiencyFileName = std::any_cast<std::string>(parameterList_->at("TriggerEfficiencyFileName"));
                // bool matchWithTriggerObjects = std::any_cast<bool>(parameterList_->at("MatchWithSelectedObjects"));
                if(year == 2016) 
                {
                    theTriggerEfficiencyCalculator_TriggerMatched_ = new TriggerEfficiencyCalculator_TriggerMatched_2016(triggerEfficiencyFileName, nat);
                }
                else if(year == 2017) 
                {
                    theTriggerEfficiencyCalculator_TriggerMatched_ = new TriggerEfficiencyCalculator_TriggerMatched_2017(triggerEfficiencyFileName, nat);
                }
                else if(year == 2018)
                {
                    theTriggerEfficiencyCalculator_TriggerMatched_ = new TriggerEfficiencyCalculator_TriggerMatched_2018(triggerEfficiencyFileName, nat);
                }
                else
                {
                    std::cout<<"Trigger scale factor year can be 2016, 2017 or 2018. Aborting..." << std::endl;
                    abort();
                }

                bool simulateTrigger = std::any_cast<bool>(parameterList_->at("SimulateTrigger"));
                if(simulateTrigger) theTriggerEfficiencyCalculator_TriggerMatched_->simulateTrigger(&ot);
                //theTriggerEfficiencyCalculator_->applyTurnOnCut(std::any_cast<bool>(parameterList_->at("ApplyTurnOnCuts")));
            }
        }


        void initializeObjectsForCuts(OutputTree &ot);
        // functions to select events based on non-jet particles:
        // void (OfflineProducerHelper::* save_objects_for_cut)(NanoAODTree&, OutputTree&, EventInfo& ei);
        std::function<void (NanoAODTree&, OutputTree&, EventInfo& ei)>  save_objects_for_cut;

        // Object to reject events with leptons that may come from W and Z decays
        void save_WAndZLeptonDecays (NanoAODTree& nat, OutputTree &ot, EventInfo& ei);
        // save trigger Objects for trigger studies
        void save_TriggerObjects (NanoAODTree& nat, OutputTree &ot, EventInfo& ei);
        // Calculate trigger map
        //void calculateTriggerMatching(const std::vector< std::unique_ptr<Candidate> > &candidateList, NanoAODTree& nat);
        // save good isolated leptons
        void save_IsolatedLeptons (NanoAODTree& nat, OutputTree &ot, EventInfo& ei);
 
        //Initialize trigger Matching variables
        //void initializeTriggerMatching(OutputTree &ot);
        //Function to check that the selected objects are the one that fired at list one of the triggers
       // bool checkTriggerObjectMatching(std::vector<std::string>, OutputTree &ot);


        void initializeObjectsForEventWeight(OutputTree &ot, SkimEffCounter &ec, std::string PUWeightFileName, float crossSection);
        // functions to select events based on non-jet particles:
        std::function<float (NanoAODTree&,  EventInfo& ei, OutputTree&, SkimEffCounter &ec)> calculateEventWeight;
        // reject events with leptons that may come from W and Z decays
        float calculateEventWeight_AllWeights(NanoAODTree& nat, EventInfo& ei, OutputTree &ot, SkimEffCounter &ec);
        // reject nlo-hh events with large weights |w|>0.1
        bool checkEventWeight(NanoAODTree& nat, OutputTree &ot, SkimEffCounter &ec);
        

        void initializeObjectsBJetForScaleFactors(OutputTree &ot);
        void initializeObjectsL1PrefiringForScaleFactors(OutputTree &ot);
        std::function<void (NanoAODTree&, OutputTree&, EventInfo& ei)>  save_objects_for_l1prefiring;
        // compute events weight for four b
        void compute_scaleFactors_fourBtag_eventScaleFactor (const std::vector<Jet> &jets, NanoAODTree& nat, OutputTree &ot);
        void compute_scaleFactors_bTagReshaping (const std::vector<Jet> &jets, NanoAODTree& nat, OutputTree &ot, double event_weight);
        void CalculateBtagScaleFactor(const std::vector<Jet> presel_bjets,NanoAODTree& nat,OutputTree &ot);
        void CalculateL1prefiringScaleFactor(NanoAODTree& nat,OutputTree &ot, EventInfo& ei);
        void CalculateEventbyEventScaleFactors(NanoAODTree& nat,OutputTree &ot, EventInfo& ei, float xs);

        void writebTagReshapingHisto();

        std::unique_ptr<JME::JetResolutionScaleFactor> jetResolutionScaleFactor_;
        std::unique_ptr<JME::JetResolution>            jetResolution_;
        void initializeJERsmearingAndVariations(OutputTree &ot);
        // function pointer to MC jet pt smearing
        // std::vector<Jet> (*JERsmearing)(NanoAODTree& nat, std::vector<Jet> &jets);
        std::function<std::vector<Jet> (NanoAODTree& nat, std::vector<Jet> &jets)> JERsmearing;
        // function to smear the jet pt as indicated in https://twiki.cern.ch/twiki/bin/view/CMS/JetResolution#Smearing_procedures
        std::vector<Jet> standardJERsmearing(NanoAODTree& nat, std::vector<Jet> &jets);
        // function pointer for JER variations
        // void (OfflineProducerHelper::* JERvariations)(NanoAODTree& nat, std::vector<Jet> &jets, std::vector< std::pair<std::string, std::vector<Jet> > > &jetEnergyVariationsMap);
        std::function <void (NanoAODTree& nat, std::vector<Jet> &jets, std::vector< std::pair<std::string, std::vector<Jet> > > &jetEnergyVariationsMap)> JERvariations;
        // function to apply JER variation
        void standardJERVariations(NanoAODTree& nat, std::vector<Jet> &jets, std::vector< std::pair<std::string, std::vector<Jet> > > &jetEnergyVariationsMap);
        //function to apply JER
        std::vector<Jet> applyJERsmearing(NanoAODTree& nat, std::vector<Jet> jets, Variation variation = Variation::NOMINAL);

        // function that applies whatever smearing strategy is defined in the parameters, and returns the jet
        void initializeApplyJESshift(std::string syst_and_direction);
        std::vector<Jet> applyJESshift(NanoAODTree &nat, const std::vector<Jet> &jets, bool direction_is_up);

        // smear the jets (resolution values can be set for up/down with class member parameters for systematic studies)
        void initializeApplyJERAndBregSmearing(std::string syst_and_direction);
        std::vector<Jet> applyJERAndBregSmearing(NanoAODTree& nat, std::vector<Jet> jets);

        void initializeJECVariations(OutputTree &ot);
        // function pointer for JEC variations
        // void (OfflineProducerHelper::* JECvariations)(NanoAODTree& nat, std::vector<Jet> &jets, std::vector< std::pair<std::string, std::vector<Jet> > > &jetEnergyVariationsMap);
        std::function<void (NanoAODTree& nat, std::vector<Jet> &jets, std::vector< std::pair<std::string, std::vector<Jet> > > &jetEnergyVariationsMap)> JECvariations;
        // function to apply all JEC variations
        void standardJECVariations(NanoAODTree& nat, std::vector<Jet> &jetsUnsmeared, std::vector< std::pair<std::string, std::vector<Jet> > > &jetEnergyVariationsMap);
        //function to apply JEC variation
        std::vector<Jet> applyJECVariation(NanoAODTree& nat, std::vector<Jet> jetsUnsmeared, std::string variationName, bool direction);

        //function to fill branches for JEC and JER variations
        void fillJetEnergyVariationBranch(OutputTree &ot, std::string branchName, std::string variation, float value);

        // compute events weight for four b
        // void compute_scaleFactors_fourBtag_eventScaleFactor (const std::vector<Jet> &jets, NanoAODTree& nat, OutputTree &ot);


        BTagCalibrationReader *btagCalibrationReader_lightJets_;
        BTagCalibrationReader *btagCalibrationReader_cJets_;
        BTagCalibrationReader *btagCalibrationReader_bJets_;

        std::unique_ptr<BTagCalibrationReader> btagCalibrationReader_all_; // used for reshaping
        const std::vector<std::string> btag_sf_reshaping_unc_sources_ = {"jes", "hf", "lf", "lfstats1", "lfstats2", "hfstats1", "hfstats2", "cferr1", "cferr2"}; // x {up, down} in the code
        std::vector<std::string> btag_sf_reshaping_full_list_; // a buffer to store all the values to read from the csv. Will contain {central} + btag_sf_reshaping_unc_sources_ x {up, down}
        std::vector<double> btag_sf_reshaping_full_list_sumw_; // a buffer to store the sum weights for the b tag scale factor
        double btag_sf_reshaping_sumw_denom_; // sum of the weights to ensure the normalisation of the reshaping scale factors

        //Functions to be applied in the bjetregression scores
        float Get_bRegRes(Jet jet);
        float Get_bRegCorr(Jet jet);
        //functions fo apply preselection cuts:
        void bJets_PreselectionCut(std::vector<Jet> &jets); ///Fabio's function
        std::vector<Jet> bjJets_PreselectionCut(NanoAODTree& nat, EventInfo& ei, OutputTree &ot, std::vector<Jet> jets);
        std::vector<Jet> OppositeEtaJetPair(std::vector<Jet> jjets);
        std::vector<Jet> HighestPtJetPair(std::vector<Jet> jjets); 
        std::vector<Jet> ExternalEtaJetPair(std::vector<Jet> jjets, std::vector<Jet> bjets);  
        //functions for gen-level studies
        void match_gen_recojets(NanoAODTree& nat, EventInfo& ei, bool vbfmode);
        std::vector<int> GetMatchedJetIdx(NanoAODTree& nat, EventInfo& ei,const std::vector<Jet> jets, bool vbfmode);
        std::vector<int> JetToQuarkMatcher(const std::vector<GenPart> quarks,const std::vector<Jet> jets, bool vbfmode);
        int GetQuarkIdentifier(EventInfo& ei, Jet jet);
        void study_gen_selectedjets(NanoAODTree& nat, EventInfo& ei);
        void study_diagonal_selectedjets(NanoAODTree& nat, EventInfo& ei, OutputTree &ot);
        void compute_pairingvariables(EventInfo& ei, OutputTree &ot, std::vector<Jet> jets, float distance, int sort, std::string tag);
        float compute_higgsptcm(std::vector<Jet> jets);        
        float MindR(std::vector<Jet> jets, bool min);
        std::tuple<float,float> MinMax_Delta(std::vector<TLorentzVector> presel_jets_p4, int option);
        // functions that act on the EventInfo
        bool select_bbbb_jets (NanoAODTree& nat, EventInfo& ei, OutputTree &ot, std::vector<std::string> listOfPassedTriggers);
        bool select_bbbbjj_jets (NanoAODTree& nat, EventInfo& ei, OutputTree &ot);
        //functions for TTEMU studies
        bool select_bbemu(NanoAODTree& nat, EventInfo& ei, OutputTree &ot);
        std::vector<Jet> bbJets_PreselectionCut(std::vector<Jet> &jets);
        std::vector<std::tuple <Electron,Muon>> emu_PreselectionCut(NanoAODTree& nat, EventInfo& ei, OutputTree &ot);
        std::vector<std::tuple <Electron,Muon>> OppositeChargeEMUPair(std::vector<Electron> Electrons, std::vector<Muon> Muons, float mass); 
        // bbbbSelectionStrategy strategy_;
        // functions to pair a preselected set of four jets. They all shuffle the input set of jets and return them as (H1_b1, H1_b2, H2_b1, H2_b2)
        //
        // pick the pair that is closest to the Higgs mass, the other two remaining jets form the other pair
        std::vector<Jet> bbbb_jets_idxs_OneClosestToMh(const std::vector<Jet> *presel_jets);
        // minimize the distance of both pairs from the (targetmH, targetmH) point in a 2D plane
        std::vector<Jet> bbbb_jets_idxs_BothClosestToMh(const std::vector<Jet> *presel_jets);
        // by pair that is most back-to-back
        std::vector<Jet> bbbb_jets_idxs_MostBackToBack(const std::vector<Jet> *presel_jets);
        //pair by ordering the jets by CSV and then finding the compination closer to targetmH for both candidates
        std::vector<Jet> bbbb_jets_idxs_HighestCSVandClosestToMh(const std::vector<Jet> *presel_jets);
        //A la ATLAS
        std::vector<Jet> bbbb_jets_idxs_BothClosestToDiagonal(const std::vector<Jet> *presel_jets);
        //Luca's idea
        std::vector<Jet> bbbb_jets_idxs_BothClosestToSlope(const std::vector<Jet> *presel_jets);
        // A la 2016 paper
        std::vector<Jet> bbbb_jets_idxs_MinMassDifference(const std::vector<Jet> *presel_jets);

        //Additional kinematic variables
        void ApplyBjetRegression(std::vector<Jet> bjets);
        void AddVBFCategoryVariables(NanoAODTree& nat, EventInfo& ei,std::vector<Jet> ordered_jets);
        void AddInclusiveCategoryVariables(NanoAODTree& nat, EventInfo& ei,std::vector<Jet> ordered_jets, std::vector<Jet> fulljets);    
        // float GetBDT1Score(EventInfo& ei, std::string weights);//GGFHHKiller
        // float GetBDT2Score(EventInfo& ei, std::string weights);//VBFQCDKiller
        // float GetBDT3Score(EventInfo& ei, std::string weights);//GGFQCDHHKiller
        float GetBDT1Score(EventInfo& ei);//GGFHHKiller
        float GetBDT2Score(EventInfo& ei);//VBFQCDKiller
        float GetBDT3Score(EventInfo& ei);//GGFQCDHHKiller
        float GetBDT3cat1Score(EventInfo& ei);//GGFQCDHHKillercat1
        float GetBDT3cat2Score(EventInfo& ei);//GGFQCDHHKillercat2
        float GetDNNScore(EventInfo& ei);
        // combines a collection of type C of jets (either std::vector or std::array) into a collection of H H possible combinations
        // (i.e. all possible H1 = (jA, jB) and H2 = (jC, jD) choices)
        template <typename C>
        std::vector<std::pair<CompositeCandidate, CompositeCandidate>> make_jet_combinations (C jets);

        // given a collection of jets, returns the best two pairs.
        // the choice is made passing a sort_par_func that is evaluated on pairs of CompositeCandidates cc1 and cc2
        // e.g. if sort_par_func(cc1, cc2) is cc1.DeltaR(cc2), then the two CompositeCandidates that are less back-to-back is selected
        // using get_smallest = false returns the largest element instead
        // template <typename T>
        // std::pair<CompositeCandidate,CompositeCandidate> get_two_best_jet_pairs (
        //     std::vector<Jet> jets,
        //     std::function<T (std::pair<CompositeCandidate,CompositeCandidate>)> sort_par_func,
        //     bool get_smallest = true);

        template <typename T_sortpar, typename T_coll>
        std::pair<CompositeCandidate,CompositeCandidate> get_two_best_jet_pairs (
            T_coll jets,
            std::function<T_sortpar (std::pair<CompositeCandidate,CompositeCandidate>)> sort_par_func,
            bool get_smallest = true);


        bool select_gen_HH    (NanoAODTree& nat, EventInfo& ei); // for HH samples
        bool select_gen_XYH   (NanoAODTree& nat, EventInfo& ei); // for NMSSM X->YH samples. NB: ei.gen_H1 -> Y, ei.gen_H2 -> H (same order as X->YH)
        bool select_gen_bb_bb (NanoAODTree& nat, EventInfo& ei);
        bool select_gen_bb_bb_forXYH (NanoAODTree& nat, EventInfo& ei, const float deltaR_threshold);
        bool select_gen_YH           (NanoAODTree& nat, EventInfo& ei);
        bool select_gen_VBF_partons (NanoAODTree& nat, EventInfo& ei);


        // other utilities
        void dump_gen_part (NanoAODTree& nat, bool printFlags = true);
        // bool checkBit(int number, int bitpos);
        int  recursive_gen_mother_idx(const GenPart& gp, bool stop_at_moth_zero = true); // returns the uppermost ancestor in the gen particle chain
                                                                                        // if stop_at_moth_zero = true, it returns when a GenPart is found and his mother has idx 0
                                                                                        // instead of returning the GenPart # 0 itself
        
        // loops on targets, and assigns value to the first element of target that is found to be uninitialized
        // returns false if none could be assigned, else return true
        template <typename T>
        bool assign_to_uninit(T value, std::initializer_list<boost::optional<T>*> targets);

        // given to Candidates val1 and val2, order them by pt
        // the function returns true if the objects were swapped, or false if they were left unchanged
        // if max_first = true, pt (val1) > pt (val2), if max_first = false the opposite
        template <typename T>

        bool order_by_pT(T& val1, T& val2, bool max_first = true);

        static float deltaPhi(float phi1, float phi2)
        {
            float delphi = TMath::Abs(TMath::Abs(TMath::Abs(phi1 - phi2) - TMath::Pi())-TMath::Pi());
            return delphi;
        }

};

template <typename C>
std::vector<std::pair<CompositeCandidate, CompositeCandidate>> OfflineProducerHelper::make_jet_combinations (C jets)
{
    const static bool debug = false;
    if (debug)
    {
        std::cout << "DEBUG make_jet_combinations : my jets in input are (pT)" << std::endl;
        for (auto& j : jets)
            std::cout << j.P4().Pt() << std::endl;        
    }

    // make all possible jet pairs (CompositeCandidates)
    std::vector<CompositeCandidate> pairs;
    for (uint i = 0; i < jets.size(); ++i)
        for (uint j = i+1; j < jets.size(); ++j)
        {
            CompositeCandidate cc (jets.at(i), jets.at(j));
            pairs.push_back(cc);
        }

    if (debug)
    {
        std::cout << "DEBUG make_jet_combinations : my pairs are (pT)" << std::endl;
        for (auto& j : pairs)
            std::cout << "(" << j.getComponent1().P4().Pt() << ", " << j.getComponent2().P4().Pt() << ")" << std::endl;        
    }

    // make all possible pairs of CompositeCandiates, ensuring that each jet is taken only once
    std::vector<std::pair<CompositeCandidate, CompositeCandidate>> jet_combinations;
    for (uint i = 0; i < pairs.size(); ++i)
        for (uint j = i+1; j < pairs.size(); ++j)
        {
            auto& cc1 = pairs.at(i);
            auto& cc2 = pairs.at(j);
            if (cc1.sharesComponentWith(cc2)) continue;
            jet_combinations.push_back(std::make_pair(cc1, cc2));
        }

    if (debug)
    {
        std::cout << "DEBUG : my combinations are (pT, pT)" << std::endl;
        for (auto& p : jet_combinations)
            std::cout << "(" << p.first.getComponent1().P4().Pt()  << ", " << p.first.getComponent2().P4().Pt()  << ") . "
                      << "(" << p.second.getComponent1().P4().Pt() << ", " << p.second.getComponent2().P4().Pt() << ")"
                      << std::endl;        
    }


    return jet_combinations;
}


// the function is templated on the return type of the sort_par_func, which must be sortable.
// all jet pairs (== Higgs cand) are made, and they are grouped into exclusive pairs (H1 and H2, not made from the same jet)
// e.g. 5 jets : A B C D E
// --> build: AB AC AD AE BC BD BE CD CE DE
// --> build all possible H1 H2 cands:
// AB CD
// AB DE
// AC BD
// etc...
// --> compute the sort parameter for each one, and sort it keeping track of the original (H1, H2) index
// --> return the best (H1, H2) pair
template <typename T_sortpar, typename T_coll>
std::pair<CompositeCandidate,CompositeCandidate> OfflineProducerHelper::get_two_best_jet_pairs (T_coll jets, std::function<T_sortpar (std::pair<CompositeCandidate,CompositeCandidate>)> sort_par_func, bool get_smallest)
{
    const static bool debug = false;

    if (debug)
    {
        std::cout << "DEBUG get_two_best_jet_pairs : my jets in input are (pT)" << std::endl;
        for (auto& j : jets)
            std::cout << j.P4().Pt() << std::endl;        
    }

    // make all (H1, H2) candidates
    auto combs = make_jet_combinations (jets);

    if (debug)
    {
        std::cout << "DEBUG : my combinations are (pT, pT)" << std::endl;
        for (auto& p : combs)
            std::cout << "(" << p.first.getComponent1().P4().Pt()  << ", " << p.first.getComponent2().P4().Pt()  << ") . "
                      << "(" << p.second.getComponent1().P4().Pt() << ", " << p.second.getComponent2().P4().Pt() << ")"
                      << std::endl;        
    }

    // compute sort parameter
    std::vector<std::pair<T_sortpar,int>> sort_pars;
    for (unsigned int idx = 0; idx < combs.size(); ++idx)
    {
        auto& jet = combs.at(idx);
        T_sortpar sort_par = sort_par_func(jet);
        sort_pars.push_back(std::make_pair(sort_par, idx));
    }

    // make the sort and get best two results
    std::stable_sort(sort_pars.begin(), sort_pars.end());
    if (get_smallest)
        return combs.at(sort_pars.begin()->second);
    
    else
        return combs.at(sort_pars.rbegin()->second);
}

template <typename T>
bool OfflineProducerHelper::assign_to_uninit(T value, std::initializer_list<boost::optional<T>*> targets)
{
    for (boost::optional<T>* tar : targets)
    {
        if (!(*tar))
        {
            *tar = value;
            return true;
        }
    }
    return false;
}

template <typename T>
bool OfflineProducerHelper::order_by_pT(T& val1, T& val2, bool max_first)
{
    float pt1 = val1.P4().Pt();
    float pt2 = val2.P4().Pt();

    if (max_first && pt1 > pt2)
        return false;
    if (!max_first && pt1 <= pt2)
        return false;
    std::swap(val1, val2);
    return true;
}

float deltaPhi(float phi1, float phi2)
{
    float delphi = TMath::Abs(TMath::Abs(TMath::Abs(phi1 - phi2) - TMath::Pi())-TMath::Pi());
    return delphi;
}

#endif