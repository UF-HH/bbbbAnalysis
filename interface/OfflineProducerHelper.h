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
#include "CfgParser.h"
#include "NanoAODTree.h"
#include "EventInfo.h"
#include "CompositeCandidate.h"
#include "Candidate.h"
#include "OutputTree.h"
#include "BTagCalibrationStandalone.h"
#include "JetMETCorrections/Modules/interface/JetResolution.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "Jet.h"
#include "SkimEffCounter.h"

#include <array>
#include <utility>
#include <functional>
#include <initializer_list>
#include <boost/optional.hpp>
#include <experimental/any>
 
using namespace std::experimental;


namespace OfflineProducerHelper {

    bool debug = false;
    // Load configurations to match the b jets
    // bool loadConfiguration(CfgParser config);
    ///static bacause if not I got a glibc detected when the execution is completed

    const std::map<std::string,any> *parameterList_;
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

    // branch Name, default value
    std::map<std::string, float> branchesAffectedByJetEnergyVariations_;
    float sampleCrossSection_;

    // All maps need to be cleared otherwise we have a glibc detected
    void clean() {
        weightMap_.clear();
        PUWeightMap_.clear();
        mapForJECuncertanties_.clear();
        mapJERNamesAndVariation_.clear();
        mapJECNamesAndVariation_.clear();
        branchesAffectedByJetEnergyVariations_.clear();
        mapTriggerMatching_.clear();
        mapTriggerObjectIdAndFilter_.clear();
    }

    void initializeOfflineProducerHelper(const std::map<std::string,any> *parameterList) {
        parameterList_=parameterList;
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
    }

    void initializeObjectsForCuts(OutputTree &ot);
    // functions to select events based on non-jet particles:
    void (*save_objects_for_cut)(NanoAODTree&, OutputTree&, EventInfo& ei);
    // Object to reject events with leptons that may come from W and Z decays
    void save_WAndZLeptonDecays (NanoAODTree& nat, OutputTree &ot, EventInfo& ei);
    // save trigger Objects for trigger studies
    void save_TriggerObjects (NanoAODTree& nat, OutputTree &ot, EventInfo& ei);
    // Calculate trigger map
    void calculateTriggerMatching(const std::vector<Jet> candidateList, NanoAODTree& nat);

    //Initialize trigger Matching variables
    void initializeTriggerMatching(OutputTree &ot);
    //Function to check that the selected objects are the one that fired at list one of the triggers
    bool checkTriggerObjectMatching(std::vector<std::string>, OutputTree &ot);


    void initializeObjectsForEventWeight(OutputTree &ot, SkimEffCounter &ec, std::string PUWeightFileName, float crossSection);
    // functions to select events based on non-jet particles:
    float (*calculateEventWeight)(NanoAODTree&, OutputTree&, SkimEffCounter &ec);
    // reject events with leptons that may come from W and Z decays
    float calculateEventWeight_AllWeights(NanoAODTree& nat, OutputTree &ot, SkimEffCounter &ec);
    

    void initializeObjectsBJetForScaleFactors(OutputTree &ot);
    // compute events weight for four b
    void compute_scaleFactors_fourBtag_eventScaleFactor (const std::vector<Jet> &jets, NanoAODTree& nat, OutputTree &ot);


    JME::JetResolutionScaleFactor *jetResolutionScaleFactor_;
    JME::JetResolution *jetResolution_;
    void initializeJERsmearingAndVariations(OutputTree &ot);
    // function pointer to MC jet pt smearing
    std::vector<Jet> (*JERsmearing)(NanoAODTree& nat, std::vector<Jet> &jets);
    // function to smear the jet pt as indicated in https://twiki.cern.ch/twiki/bin/view/CMS/JetResolution#Smearing_procedures
    std::vector<Jet> standardJERsmearing(NanoAODTree& nat, std::vector<Jet> &jets);
    // function pointer for JER variations
    void (*JERvariations)(NanoAODTree& nat, std::vector<Jet> &jets, std::vector< std::pair<std::string, std::vector<Jet> > > &jetEnergyVariationsMap);
    // function to apply JER variation
    void standardJERVariations(NanoAODTree& nat, std::vector<Jet> &jets, std::vector< std::pair<std::string, std::vector<Jet> > > &jetEnergyVariationsMap);
    //function to apply JER
    std::vector<Jet> applyJERsmearing(NanoAODTree& nat, std::vector<Jet> jets, Variation variation = Variation::NOMINAL);


    void initializeJECVariations(OutputTree &ot);
    // function pointer for JEC variations
    void (*JECvariations)(NanoAODTree& nat, std::vector<Jet> &jets, std::vector< std::pair<std::string, std::vector<Jet> > > &jetEnergyVariationsMap);
    // function to apply all JEC variations
    void standardJECVariations(NanoAODTree& nat, std::vector<Jet> &jetsUnsmeared, std::vector< std::pair<std::string, std::vector<Jet> > > &jetEnergyVariationsMap);
    //function to apply JEC variation
    std::vector<Jet> applyJECVariation(NanoAODTree& nat, std::vector<Jet> jetsUnsmeared, std::string variationName, bool direction);

    //function to fill branches for JEC and JER variations
    void fillJetEnergyVariationBranch(OutputTree &ot, std::string branchName, std::string variation, float value);

    // compute events weight for four b
    void compute_scaleFactors_fourBtag_eventScaleFactor (const std::vector<Jet> &jets, NanoAODTree& nat, OutputTree &ot);


    BTagCalibrationReader *btagCalibrationReader_lightJets_;
    BTagCalibrationReader *btagCalibrationReader_cJets_;
    BTagCalibrationReader *btagCalibrationReader_bJets_;
    //functions fo apply preselection cuts:
    void bJets_PreselectionCut(std::vector<Jet> &jets);
    std::vector<std::tuple<Jet,int,int>> bjJets_PreselectionCut(std::vector<std::tuple<Jet,int,int>> jetsinfo);
    std::vector<std::tuple<Jet,int,int>> bbbbBothClosestToMh(const std::vector<std::tuple<Jet,int,int>> presel_jets);
    std::vector<std::tuple<Jet,int,int>> bbbbOneClosestToMh(std::vector<std::tuple<Jet,int,int>> presel_jets);
    std::vector<std::tuple<int,int,int>> QuarkToJetMatcher(const std::vector<GenPart> quarks, std::vector<Jet> jets);
    std::vector<std::tuple<Jet,int,int>> AddGenMatchingInfo(NanoAODTree& nat, EventInfo& ei, std::vector<Jet> jets);
    std::vector<std::tuple<Jet,int,int>> OppositeEtaJetPair(std::vector<std::tuple<Jet,int,int>> jjets);
    // functions that act on the EventInfo
    bool select_bbbb_jets (NanoAODTree& nat, EventInfo& ei, OutputTree &ot, std::vector<std::string> listOfPassedTriggers);
    bool select_bbbbjj_jets (NanoAODTree& nat, EventInfo& ei, OutputTree &ot);
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


    bool select_gen_HH    (NanoAODTree& nat, EventInfo& ei);
    bool select_gen_bb_bb (NanoAODTree& nat, EventInfo& ei);
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