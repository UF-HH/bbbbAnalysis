/*
** class  : skim_for_das.cpp
** author : L. Cadamuro (UF)
** date   : 12/11/2019
** brief  : transforms a NanoAOD into a bbbb ntuple for the subsequent plots/analysis
*/

#include <iostream>
#include <string>
#include <iomanip>
#include <any>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "CfgParser.h"
#include "NanoAODTree.h"
#include "EventInfo.h"

#include "SkimUtils.h"
namespace su = SkimUtils;

#include "OfflineProducerHelper.h"
// namespace oph = OfflineProducerHelper;

// #include "OutputTree.h"
#include "SkimEffCounter.h"
#include "jsonLumiFilter.h"

#include "TFile.h"

using namespace std;

#define B_TAG_ALGO Jet_btagDeepB

// int match_genJets (Jet jet, std::vector<GenJet>& genjets)
// {
//     double maxDr = 0.3;
//     std::vector<std::pair<double, int>> genjets_matched;
//     for (uint igj = 0; igj < genjets.size(); ++igj)
//     {
//         double dR = genjets.at(igj).P4().DeltaR(jet.P4());
//         if (dR < maxDr)
//             genjets_matched.push_back(make_pair(dR, igj));
//     }

//     if (genjets_matched.size() == 0)
//         return -1;

//     sort(genjets_matched.begin(), genjets_matched.end());
//     return genjets_matched.at(0).second;
// }

bool checkBit(int number, int bitpos)
{
    return (number & (1 << bitpos));
}

float computePUweight(TH1* histo_pileup, double npu)
{
    int nbin = histo_pileup->FindBin(npu);
    return histo_pileup->GetBinContent(nbin);
}

// returns SF organised as
// central
// (bup, bdown)
// (cup, cdown)
// (lup, ldown)
std::tuple<float, std::pair<float,float>, std::pair<float,float>, std::pair<float,float>>
computeBtagSF(std::vector<Jet> jets, BTagCalibrationReader* btcr)
{
    float tmpScaleFactor_bJets_central     = 1.;
    float tmpScaleFactor_bJets_up          = 1.;
    float tmpScaleFactor_bJets_down        = 1.;
    float tmpScaleFactor_cJets_central     = 1.;
    float tmpScaleFactor_cJets_up          = 1.;
    float tmpScaleFactor_cJets_down        = 1.;
    float tmpScaleFactor_lightJets_central = 1.;
    float tmpScaleFactor_lightJets_up      = 1.;
    float tmpScaleFactor_lightJets_down    = 1.;

    for (auto& jet : jets)
    {
        int jetFlavour = abs(get_property(jet,Jet_hadronFlavour));
        
        if(jetFlavour == 5){
            tmpScaleFactor_bJets_central     *= btcr->eval_auto_bounds("central", BTagEntry::FLAV_B   , jet.P4().Eta(), jet.P4().Pt());
            tmpScaleFactor_bJets_up          *= btcr->eval_auto_bounds("up"     , BTagEntry::FLAV_B   , jet.P4().Eta(), jet.P4().Pt());
            tmpScaleFactor_bJets_down        *= btcr->eval_auto_bounds("down"   , BTagEntry::FLAV_B   , jet.P4().Eta(), jet.P4().Pt());
        }
        else if(jetFlavour == 4){
            tmpScaleFactor_cJets_central     *= btcr->eval_auto_bounds("central", BTagEntry::FLAV_C   , jet.P4().Eta(), jet.P4().Pt());
            tmpScaleFactor_cJets_up          *= btcr->eval_auto_bounds("up"     , BTagEntry::FLAV_C   , jet.P4().Eta(), jet.P4().Pt());
            tmpScaleFactor_cJets_down        *= btcr->eval_auto_bounds("down"   , BTagEntry::FLAV_C   , jet.P4().Eta(), jet.P4().Pt());
        }
        else{
            tmpScaleFactor_lightJets_central *= btcr->eval_auto_bounds("central", BTagEntry::FLAV_UDSG, jet.P4().Eta(), jet.P4().Pt());
            tmpScaleFactor_lightJets_up      *= btcr->eval_auto_bounds("up"     , BTagEntry::FLAV_UDSG, jet.P4().Eta(), jet.P4().Pt());
            tmpScaleFactor_lightJets_down    *= btcr->eval_auto_bounds("down"   , BTagEntry::FLAV_UDSG, jet.P4().Eta(), jet.P4().Pt());
        }        
    }

    float bTagScaleFactor_central         = tmpScaleFactor_bJets_central * tmpScaleFactor_cJets_central * tmpScaleFactor_lightJets_central ;
    float bTagScaleFactor_bJets_up        = tmpScaleFactor_bJets_up      * tmpScaleFactor_cJets_central * tmpScaleFactor_lightJets_central ;
    float bTagScaleFactor_bJets_down      = tmpScaleFactor_bJets_down    * tmpScaleFactor_cJets_central * tmpScaleFactor_lightJets_central ;
    float bTagScaleFactor_cJets_up        = tmpScaleFactor_bJets_central * tmpScaleFactor_cJets_up      * tmpScaleFactor_lightJets_central ;
    float bTagScaleFactor_cJets_down      = tmpScaleFactor_bJets_central * tmpScaleFactor_cJets_down    * tmpScaleFactor_lightJets_central ;
    float bTagScaleFactor_lightJets_up    = tmpScaleFactor_bJets_central * tmpScaleFactor_cJets_central * tmpScaleFactor_lightJets_up      ;
    float bTagScaleFactor_lightJets_down  = tmpScaleFactor_bJets_central * tmpScaleFactor_cJets_central * tmpScaleFactor_lightJets_down    ;

    std::tuple <float, std::pair<float,float>, std::pair<float,float>, std::pair<float,float> > result = {
        bTagScaleFactor_central,
        make_pair(bTagScaleFactor_bJets_up, bTagScaleFactor_bJets_down),
        make_pair(bTagScaleFactor_cJets_up, bTagScaleFactor_cJets_down),
        make_pair(bTagScaleFactor_lightJets_up, bTagScaleFactor_lightJets_down),
    };
    return result;

}

int main(int argc, char** argv)
{
    cout << "[INFO] ... starting program" << endl;

    ////////////////////////////////////////////////////////////////////////
    // Decalre command line options
    ////////////////////////////////////////////////////////////////////////
    
    po::options_description desc("Skim options");
    desc.add_options()
        ("help", "produce help message")
        // required
        ("cfg"   , po::value<string>()->required(), "skim config")
        ("input" , po::value<string>()->required(), "input file list")
        ("output", po::value<string>()->required(), "output file LFN")
        // optional
        ("xs"       , po::value<float>(), "cross section [pb]")
        ("maxEvts"  , po::value<int>()->default_value(-1), "max number of events to process")
        ("puWeight" , po::value<string>()->default_value(""), "PU weight file name")
        // flags
        ("is-data",    po::value<bool>()->zero_tokens()->implicit_value(true)->default_value(false), "mark as a data sample (default is false)")
        ("is-signal",  po::value<bool>()->zero_tokens()->implicit_value(true)->default_value(false), "mark as a HH signal sample (default is false)")
        ("is-VBF-sig", po::value<bool>()->zero_tokens()->implicit_value(true)->default_value(false), "mark as a HH VBF signal sample (default is false)")
        ("save-p4",    po::value<bool>()->zero_tokens()->implicit_value(true)->default_value(false), "save the tlorentzvectors in the output")
    ;

    po::variables_map opts;
    try {
        po::store(parse_command_line(argc, argv, desc, po::command_line_style::unix_style ^ po::command_line_style::allow_short), opts);
        if (opts.count("help")) {
            cout << desc << "\n";
            return 1;
        }
        po::notify(opts);
    }    
    catch (po::error& e) {
        cerr << "** [ERROR] " << e.what() << endl;
        return 1;
    }

    ////////////////////////////////////////////////////////////////////////
    // Read config and other cmd line options for skims
    ////////////////////////////////////////////////////////////////////////

    const bool is_data = opts["is-data"].as<bool>();
    if (!is_data && !opts.count("xs")){
        cerr << "** [ERROR] please provide xs for this sample that is marked as MC" << endl;
        return 1;
    }
    cout << "[INFO] ... is a data sample? " << std::boolalpha << is_data << std::noboolalpha << endl;

    const bool is_signal = (is_data ? false : opts["is-signal"].as<bool>());
    cout << "[INFO] ... is a HH signal sample? " << std::boolalpha << is_signal << std::noboolalpha << endl;

    const bool is_VBF_sig = (is_data ? false : opts["is-VBF-sig"].as<bool>());
    cout << "[INFO] ... is a HH VBF sample? " << std::boolalpha << is_VBF_sig << std::noboolalpha << endl;

    const float xs = (is_data ? 1.0 : opts["xs"].as<float>());
    cout << "[INFO] ... cross section is : " << xs << " pb" << endl;

    CfgParser config;
    if (!config.init(opts["cfg"].as<string>())) return 1;
    cout << "[INFO] ... using config file " << opts["cfg"].as<string>() << endl;

    OfflineProducerHelper oph;
    SkimEffCounter ec;

    ////////////////////////////////////////////////////////////////////////
    // Prepare event loop
    ////////////////////////////////////////////////////////////////////////

    cout << "[INFO] ... opening file list : " << opts["input"].as<string>().c_str() << endl;
    if ( access( opts["input"].as<string>().c_str(), F_OK ) == -1 ){
        cerr << "** [ERROR] The input file list does not exist, aborting" << endl;
        return 1;        
    }

    // Joining all the NANOAD input file in a TChain in order to be used like an unique three
    TChain ch("Events");
    int nfiles = su::appendFromFileList(&ch, opts["input"].as<string>());
    
    if (nfiles == 0){
        cerr << "** [ERROR] The input file list contains no files, aborting" << endl;
        return 1;
    }
    cout << "[INFO] ... file list contains " << nfiles << " files" << endl;

    cout << "[INFO] ... creating tree reader" << endl;

    // The TChain is passed to the NanoAODTree_SetBranchImpl to parse all the brances
    NanoAODTree nat (&ch);

    cout << "[INFO] ... loading the following triggers" << endl;
    for (auto trg : config.readStringListOpt("triggers::makeORof"))
        cout << "   - " << trg << endl;
    nat.triggerReader().setTriggers(config.readStringListOpt("triggers::makeORof"));

    const float btagscore_WPmedium = config.readFloatOpt("btag::WPmedium");

    jsonLumiFilter jlf;
    if (is_data)
        jlf.loadJSON(config.readStringOpt("data::lumimask")); // just read the info for data, so if I just skim MC I'm not forced to parse a JSON

    // pu reweight
    TFile* fPileUp      = nullptr;
    TH1*   histo_pileup = nullptr;
    if (!is_data)
    {
        fPileUp      = TFile::Open(opts["puWeight"].as<string>().c_str());
        histo_pileup = (TH1*) fPileUp->Get("PUweights"); // _up, _down
    }

    // btag reweight

    std::unique_ptr<BTagCalibration> btagCalibration;
    std::unique_ptr<BTagCalibrationReader> btcr;

    if (!is_data)
    {
        cout << "[INFO] : b tag SF file : " << config.readStringOpt("parameters::BJetScaleFactorsFile") << endl;
        btagCalibration = std::unique_ptr<BTagCalibration> (new BTagCalibration ("DeepCSV",any_cast<string>(config.readStringOpt("parameters::BJetScaleFactorsFile"))));
        btcr            = std::unique_ptr<BTagCalibrationReader> (new BTagCalibrationReader(BTagEntry::OP_MEDIUM,"central",{"up", "down"}));
        btcr->load(*btagCalibration, BTagEntry::FLAV_UDSG, "incl"  );
        btcr->load(*btagCalibration, BTagEntry::FLAV_C   , "mujets");
        btcr->load(*btagCalibration, BTagEntry::FLAV_B   , "mujets");
    }

    ////////////////////////////////////////////////////////////////////////
    // Prepare the output
    ////////////////////////////////////////////////////////////////////////
    
    string outputFileName = opts["output"].as<string>();
    cout << "[INFO] ... saving output to file : " << outputFileName << endl;
    TFile outputFile(outputFileName.c_str(), "recreate");
    
    // TTree with a custom format
    TTree* tOut = new TTree("bbbbTree", "bbbbTree");

    // common
    unsigned int run_;
    unsigned int luminosityBlock_;
    long long    event_;

    float xs_;

    float btag_SF_;
    float btag_SF_bup_;
    float btag_SF_bdown_;
    float btag_SF_cup_;
    float btag_SF_cdown_;    
    float btag_SF_lightup_;
    float btag_SF_lightdown_;

    float weight_;

    // jets
    float jet1_pt_;
    float jet1_eta_;
    float jet1_phi_;
    float jet1_m_;
    // float jet1_CMVA_;
    float jet1_btagscore_;
    float jet1_bRegCorr_;
    float jet1_bRegRes_;
    int   jet1_jetId_;
    int   jet1_puId_;
    int   jet1_hadronFlavour_;

    float jet2_pt_;
    float jet2_eta_;
    float jet2_phi_;
    float jet2_m_;
    // float jet2_CMVA_;
    float jet2_btagscore_;
    float jet2_bRegCorr_;
    float jet2_bRegRes_;
    int   jet2_jetId_;
    int   jet2_puId_;
    int   jet2_hadronFlavour_;


    float jet3_pt_;
    float jet3_eta_;
    float jet3_phi_;
    float jet3_m_;
    // float jet3_CMVA_;
    float jet3_btagscore_;
    float jet3_bRegCorr_;
    float jet3_bRegRes_;
    int   jet3_jetId_;
    int   jet3_puId_;
    int   jet3_hadronFlavour_;

    float jet4_pt_;
    float jet4_eta_;
    float jet4_phi_;
    float jet4_m_;
    // float jet4_CMVA_;
    float jet4_btagscore_;
    float jet4_bRegCorr_;
    float jet4_bRegRes_;
    int   jet4_jetId_;
    int   jet4_puId_;
    int   jet4_hadronFlavour_;

    // genjets associated to the 4 jets
    float genjet1_pt_;
    float genjet1_eta_;
    float genjet1_phi_;
    float genjet1_m_;
    // int   genjet1_partonFlavour_;
    int   genjet1_hadronFlavour_;

    float genjet2_pt_;
    float genjet2_eta_;
    float genjet2_phi_;
    float genjet2_m_;
    // int   genjet2_partonFlavour_;
    int   genjet2_hadronFlavour_;

    float genjet3_pt_;
    float genjet3_eta_;
    float genjet3_phi_;
    float genjet3_m_;
    // int   genjet3_partonFlavour_;
    int   genjet3_hadronFlavour_;

    float genjet4_pt_;
    float genjet4_eta_;
    float genjet4_phi_;
    float genjet4_m_;
    // int   genjet4_partonFlavour_;
    int   genjet4_hadronFlavour_;

    // // genjets associated to the 4 jets - my matching for cross check
    // float genjet1_xc_pt_;
    // float genjet1_xc_eta_;
    // float genjet1_xc_phi_;
    // float genjet1_xc_m_;
    // int   genjet1_xc_partonFlavour_;
    // int   genjet1_xc_hadronFlavour_;

    // float genjet2_xc_pt_;
    // float genjet2_xc_eta_;
    // float genjet2_xc_phi_;
    // float genjet2_xc_m_;
    // int   genjet2_xc_partonFlavour_;
    // int   genjet2_xc_hadronFlavour_;

    // float genjet3_xc_pt_;
    // float genjet3_xc_eta_;
    // float genjet3_xc_phi_;
    // float genjet3_xc_m_;
    // int   genjet3_xc_partonFlavour_;
    // int   genjet3_xc_hadronFlavour_;

    // float genjet4_xc_pt_;
    // float genjet4_xc_eta_;
    // float genjet4_xc_phi_;
    // float genjet4_xc_m_;
    // int   genjet4_xc_partonFlavour_;
    // int   genjet4_xc_hadronFlavour_;

 
    // gen vars - HH
    float gen_H1_pt_;
    float gen_H1_eta_;
    float gen_H1_phi_;
    float gen_H1_m_;
    
    float gen_H2_pt_;
    float gen_H2_eta_;
    float gen_H2_phi_;
    float gen_H2_m_;

    float gen_mHH_;
    float gen_costh_H1_cm_;
    float gen_costh_H2_cm_;

    // gen vars - b quarks from the decay
    float gen_H1_b1_pt_;
    float gen_H1_b1_eta_;
    float gen_H1_b1_phi_;
    float gen_H1_b1_m_;

    float gen_H1_b2_pt_;
    float gen_H1_b2_eta_;
    float gen_H1_b2_phi_;
    float gen_H1_b2_m_;
    
    float gen_H2_b1_pt_;
    float gen_H2_b1_eta_;
    float gen_H2_b1_phi_;
    float gen_H2_b1_m_;

    float gen_H2_b2_pt_;
    float gen_H2_b2_eta_;
    float gen_H2_b2_phi_;
    float gen_H2_b2_m_;

    // random numbers, that can be useful for selection of events etc
    float rndm_1_;
    float rndm_2_;
    float rndm_3_;

    tOut->Branch("run",              &run_);
    tOut->Branch("luminosityBlock",  &luminosityBlock_);
    tOut->Branch("event",            &event_);

    tOut->Branch("xs",            &xs_);

    tOut->Branch("btag_SF",           &btag_SF_);
    tOut->Branch("btag_SF_bup",       &btag_SF_bup_);
    tOut->Branch("btag_SF_bdown",     &btag_SF_bdown_);
    tOut->Branch("btag_SF_cup",       &btag_SF_cup_);
    tOut->Branch("btag_SF_cdown",     &btag_SF_cdown_);
    tOut->Branch("btag_SF_lightup",   &btag_SF_lightup_);
    tOut->Branch("btag_SF_lightdown", &btag_SF_lightdown_);

    tOut->Branch("weight",        &weight_);

    tOut->Branch("jet1_pt",            &jet1_pt_);
    tOut->Branch("jet1_eta",           &jet1_eta_);
    tOut->Branch("jet1_phi",           &jet1_phi_);
    tOut->Branch("jet1_m",             &jet1_m_);
    // tOut->Branch("jet1_CMVA",       &jet1_CMVA_);
    tOut->Branch("jet1_btagscore",     &jet1_btagscore_);
    tOut->Branch("jet1_bRegCorr",      &jet1_bRegCorr_);
    tOut->Branch("jet1_bRegRes",       &jet1_bRegRes_);
    tOut->Branch("jet1_jetId",         &jet1_jetId_);
    tOut->Branch("jet1_puId",          &jet1_puId_);
    tOut->Branch("jet1_hadronFlavour", &jet1_hadronFlavour_);

    tOut->Branch("jet2_pt",            &jet2_pt_);
    tOut->Branch("jet2_eta",           &jet2_eta_);
    tOut->Branch("jet2_phi",           &jet2_phi_);
    tOut->Branch("jet2_m",             &jet2_m_);
    // tOut->Branch("jet2_CMVA",       &jet2_CMVA_);
    tOut->Branch("jet2_btagscore",     &jet2_btagscore_);
    tOut->Branch("jet2_bRegCorr",      &jet2_bRegCorr_);
    tOut->Branch("jet2_bRegRes",       &jet2_bRegRes_);
    tOut->Branch("jet2_jetId",         &jet2_jetId_);
    tOut->Branch("jet2_puId",          &jet2_puId_);
    tOut->Branch("jet2_hadronFlavour", &jet2_hadronFlavour_);

    tOut->Branch("jet3_pt",            &jet3_pt_);
    tOut->Branch("jet3_eta",           &jet3_eta_);
    tOut->Branch("jet3_phi",           &jet3_phi_);
    tOut->Branch("jet3_m",             &jet3_m_);
    // tOut->Branch("jet3_CMVA",       &jet3_CMVA_);
    tOut->Branch("jet3_btagscore",     &jet3_btagscore_);
    tOut->Branch("jet3_bRegCorr",      &jet3_bRegCorr_);
    tOut->Branch("jet3_bRegRes",       &jet3_bRegRes_);
    tOut->Branch("jet3_jetId",         &jet3_jetId_);
    tOut->Branch("jet3_puId",          &jet3_puId_);
    tOut->Branch("jet3_hadronFlavour", &jet3_hadronFlavour_);

    tOut->Branch("jet4_pt",            &jet4_pt_);
    tOut->Branch("jet4_eta",           &jet4_eta_);
    tOut->Branch("jet4_phi",           &jet4_phi_);
    tOut->Branch("jet4_m",             &jet4_m_);
    // tOut->Branch("jet4_CMVA",       &jet4_CMVA_);
    tOut->Branch("jet4_btagscore",     &jet4_btagscore_);
    tOut->Branch("jet4_bRegCorr",      &jet4_bRegCorr_);
    tOut->Branch("jet4_bRegRes",       &jet4_bRegRes_);
    tOut->Branch("jet4_jetId",         &jet4_jetId_);
    tOut->Branch("jet4_puId",          &jet4_puId_);
    tOut->Branch("jet4_hadronFlavour", &jet4_hadronFlavour_);

    if (!is_data)
    {
        tOut->Branch("genjet1_pt",             &genjet1_pt_);
        tOut->Branch("genjet1_eta",            &genjet1_eta_);
        tOut->Branch("genjet1_phi",            &genjet1_phi_);
        tOut->Branch("genjet1_m",              &genjet1_m_);
        // tOut->Branch("genjet1_partonFlavour",  &genjet1_partonFlavour_);
        tOut->Branch("genjet1_hadronFlavour",  &genjet1_hadronFlavour_);
        
        tOut->Branch("genjet2_pt",             &genjet2_pt_);
        tOut->Branch("genjet2_eta",            &genjet2_eta_);
        tOut->Branch("genjet2_phi",            &genjet2_phi_);
        tOut->Branch("genjet2_m",              &genjet2_m_);
        // tOut->Branch("genjet2_partonFlavour",  &genjet2_partonFlavour_);
        tOut->Branch("genjet2_hadronFlavour",  &genjet2_hadronFlavour_);
        
        tOut->Branch("genjet3_pt",             &genjet3_pt_);
        tOut->Branch("genjet3_eta",            &genjet3_eta_);
        tOut->Branch("genjet3_phi",            &genjet3_phi_);
        tOut->Branch("genjet3_m",              &genjet3_m_);
        // tOut->Branch("genjet3_partonFlavour",  &genjet3_partonFlavour_);
        tOut->Branch("genjet3_hadronFlavour",  &genjet3_hadronFlavour_);
        
        tOut->Branch("genjet4_pt",             &genjet4_pt_);
        tOut->Branch("genjet4_eta",            &genjet4_eta_);
        tOut->Branch("genjet4_phi",            &genjet4_phi_);
        tOut->Branch("genjet4_m",              &genjet4_m_);
        // tOut->Branch("genjet4_partonFlavour",  &genjet4_partonFlavour_);
        tOut->Branch("genjet4_hadronFlavour",  &genjet4_hadronFlavour_);

    //     // the cross check
    //     tOut->Branch("genjet1_xc_pt",             &genjet1_xc_pt_);
    //     tOut->Branch("genjet1_xc_eta",            &genjet1_xc_eta_);
    //     tOut->Branch("genjet1_xc_phi",            &genjet1_xc_phi_);
    //     tOut->Branch("genjet1_xc_m",              &genjet1_xc_m_);
    //     tOut->Branch("genjet1_xc_partonFlavour",  &genjet1_xc_partonFlavour_);
    //     tOut->Branch("genjet1_xc_hadronFlavour",  &genjet1_xc_hadronFlavour_);
        
    //     tOut->Branch("genjet2_xc_pt",             &genjet2_xc_pt_);
    //     tOut->Branch("genjet2_xc_eta",            &genjet2_xc_eta_);
    //     tOut->Branch("genjet2_xc_phi",            &genjet2_xc_phi_);
    //     tOut->Branch("genjet2_xc_m",              &genjet2_xc_m_);
    //     tOut->Branch("genjet2_xc_partonFlavour",  &genjet2_xc_partonFlavour_);
    //     tOut->Branch("genjet2_xc_hadronFlavour",  &genjet2_xc_hadronFlavour_);
        
    //     tOut->Branch("genjet3_xc_pt",             &genjet3_xc_pt_);
    //     tOut->Branch("genjet3_xc_eta",            &genjet3_xc_eta_);
    //     tOut->Branch("genjet3_xc_phi",            &genjet3_xc_phi_);
    //     tOut->Branch("genjet3_xc_m",              &genjet3_xc_m_);
    //     tOut->Branch("genjet3_xc_partonFlavour",  &genjet3_xc_partonFlavour_);
    //     tOut->Branch("genjet3_xc_hadronFlavour",  &genjet3_xc_hadronFlavour_);
        
    //     tOut->Branch("genjet4_xc_pt",             &genjet4_xc_pt_);
    //     tOut->Branch("genjet4_xc_eta",            &genjet4_xc_eta_);
    //     tOut->Branch("genjet4_xc_phi",            &genjet4_xc_phi_);
    //     tOut->Branch("genjet4_xc_m",              &genjet4_xc_m_);
    //     tOut->Branch("genjet4_xc_partonFlavour",  &genjet4_xc_partonFlavour_);
    //     tOut->Branch("genjet4_xc_hadronFlavour",  &genjet4_xc_hadronFlavour_);        
    }

    if (is_signal)
    {
        tOut->Branch("gen_H1_m",   &gen_H1_m_);
        tOut->Branch("gen_H1_pt",  &gen_H1_pt_);
        tOut->Branch("gen_H1_eta", &gen_H1_eta_);
        tOut->Branch("gen_H1_phi", &gen_H1_phi_);

        tOut->Branch("gen_H2_m",   &gen_H2_m_);
        tOut->Branch("gen_H2_pt",  &gen_H2_pt_);
        tOut->Branch("gen_H2_eta", &gen_H2_eta_);
        tOut->Branch("gen_H2_phi", &gen_H2_phi_);

        tOut->Branch("gen_mHH",         &gen_mHH_);
        tOut->Branch("gen_costh_H1_cm", &gen_costh_H1_cm_);
        tOut->Branch("gen_costh_H2_cm", &gen_costh_H2_cm_);

        tOut->Branch("gen_H1_b1_m",   &gen_H1_b1_m_);
        tOut->Branch("gen_H1_b1_pt",  &gen_H1_b1_pt_);
        tOut->Branch("gen_H1_b1_eta", &gen_H1_b1_eta_);
        tOut->Branch("gen_H1_b1_phi", &gen_H1_b1_phi_);

        tOut->Branch("gen_H1_b2_m",   &gen_H1_b2_m_);
        tOut->Branch("gen_H1_b2_pt",  &gen_H1_b2_pt_);
        tOut->Branch("gen_H1_b2_eta", &gen_H1_b2_eta_);
        tOut->Branch("gen_H1_b2_phi", &gen_H1_b2_phi_);

        tOut->Branch("gen_H2_b1_m",   &gen_H2_b1_m_);
        tOut->Branch("gen_H2_b1_pt",  &gen_H2_b1_pt_);
        tOut->Branch("gen_H2_b1_eta", &gen_H2_b1_eta_);
        tOut->Branch("gen_H2_b1_phi", &gen_H2_b1_phi_);

        tOut->Branch("gen_H2_b2_m",   &gen_H2_b2_m_);
        tOut->Branch("gen_H2_b2_pt",  &gen_H2_b2_pt_);
        tOut->Branch("gen_H2_b2_eta", &gen_H2_b2_eta_);
        tOut->Branch("gen_H2_b2_phi", &gen_H2_b2_phi_);
    }

    tOut->Branch("rndm_1", &rndm_1_);
    tOut->Branch("rndm_2", &rndm_2_);
    tOut->Branch("rndm_3", &rndm_3_);

    xs_ = (is_data ? -999 : opts["xs"].as<float>() );

    ////////////////////////////////////////////////////////////////////////
    // Execute event loop
    ////////////////////////////////////////////////////////////////////////

    int maxEvts = opts["maxEvts"].as<int>();
    if (maxEvts >= 0)
        cout << "[INFO] ... running on : " << maxEvts << " events" << endl;

    TRandom3 rndm_engine(0);

    for (int iEv = 0; true; ++iEv)
    {
        if (maxEvts >= 0 && iEv >= maxEvts)
            break;

        if (!nat.Next()) break;
        if (iEv % 10000 == 0) cout << "... processing event " << iEv << endl;

        // set the seed to the first event found, for reproducibility
        rndm_engine.SetSeed(*(nat.event));

        // aply json filter to data
        if (is_data && !jlf.isValid(*nat.run, *nat.luminosityBlock)){
            continue; // not a valid lumi
        }
              
        EventInfo ei;

        run_             = *(nat.run);
        luminosityBlock_ = *(nat.luminosityBlock);
        event_           = *(nat.event);

        if (is_signal)
        {

            oph.select_gen_HH(nat, ei);
            oph.select_gen_bb_bb(nat, ei);

            // gen info
            gen_H1_pt_  = ei.gen_H1->P4().Pt();
            gen_H1_eta_ = ei.gen_H1->P4().Eta();
            gen_H1_phi_ = ei.gen_H1->P4().Phi();
            gen_H1_m_   = ei.gen_H1->P4().M();
            TLorentzVector gen_H1_p4  = ei.gen_H1->P4();
            
            gen_H2_pt_  = ei.gen_H2->P4().Pt();
            gen_H2_eta_ = ei.gen_H2->P4().Eta();
            gen_H2_phi_ = ei.gen_H2->P4().Phi();
            gen_H2_m_   = ei.gen_H2->P4().M();
            TLorentzVector gen_H2_p4  = ei.gen_H2->P4();
            
            TLorentzVector vSum = gen_H1_p4 + gen_H2_p4;
            
            // boost to CM
            TLorentzVector vH1_cm = gen_H1_p4;
            TLorentzVector vH2_cm = gen_H2_p4;
            vH1_cm.Boost(-vSum.BoostVector());
            vH2_cm.Boost(-vSum.BoostVector());

            gen_mHH_          = vSum.M();
            gen_costh_H1_cm_  = vH1_cm.CosTheta();
            gen_costh_H2_cm_  = vH2_cm.CosTheta();


            // bbbb properties
            gen_H1_b1_pt_  = ei.gen_H1_b1->P4().Pt();
            gen_H1_b1_eta_ = ei.gen_H1_b1->P4().Eta();
            gen_H1_b1_phi_ = ei.gen_H1_b1->P4().Phi();
            gen_H1_b1_m_   = ei.gen_H1_b1->P4().M();
           
            gen_H1_b2_pt_  = ei.gen_H1_b2->P4().Pt();
            gen_H1_b2_eta_ = ei.gen_H1_b2->P4().Eta();
            gen_H1_b2_phi_ = ei.gen_H1_b2->P4().Phi();
            gen_H1_b2_m_   = ei.gen_H1_b2->P4().M();

            gen_H2_b1_pt_  = ei.gen_H2_b1->P4().Pt();
            gen_H2_b1_eta_ = ei.gen_H2_b1->P4().Eta();
            gen_H2_b1_phi_ = ei.gen_H2_b1->P4().Phi();
            gen_H2_b1_m_   = ei.gen_H2_b1->P4().M();

            gen_H2_b2_pt_  = ei.gen_H2_b2->P4().Pt();
            gen_H2_b2_eta_ = ei.gen_H2_b2->P4().Eta();
            gen_H2_b2_phi_ = ei.gen_H2_b2->P4().Phi();
            gen_H2_b2_m_   = ei.gen_H2_b2->P4().M();

        }

        // calculate here weights used for the analysis
        float w_PU   = (is_data ? 1    : computePUweight(histo_pileup, *(nat.Pileup_nTrueInt)));
        float genWeight = (is_data ? 1 : *(nat.genWeight));
        weight_ = w_PU * genWeight;

        // if(!is_data) weight = oph.calculateEventWeight(nat, ei, ot, ec); // FIXME!
        
        ec.updateProcessed(weight_);

        // check the trigger
        if( !nat.getTrgOr() ) continue;
        ec.updateTriggered(weight_);

        // find the four most b tagged jets and save them in the output

        std::vector<Jet> all_jets;
        all_jets.reserve(*(nat.nJet));

        for (uint ij = 0; ij < *(nat.nJet); ++ij)
        {
            // here preselect jets
            Jet jet (ij, &nat);

            // Jet ID flags bit1 is loose (always false in 2017 since it does not exist), bit2 is tight, bit3 is tightLepVeto
            // but note that bit1 means idx 0 and so on
            int jetid = get_property(jet, Jet_jetId); 
            
            // PU ID : bit idx 0 -> tight, bit idx 1 -> medium, bit idx 2 -> loose
            int puid  = get_property(jet, Jet_puId);

            if (jet.P4().Pt() <= 25)
                continue;

            if (std::abs(jet.P4().Eta()) > 2.4)
                continue;

            if (!checkBit(jetid, 1)) // tight jet Id
                continue;

            if (!checkBit(puid, 1) && jet.P4().Pt() <= 50) // medium PU Id - NOTE : not to be applied beyond 50 GeV: https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupJetID
                continue;

            all_jets.emplace_back(Jet(ij, &nat));
        }

        if (all_jets.size() < 4) // I don't have 4 preselected jets
            continue;

        // NOTE that this sorts from small to large, with A < B implemented as btagA > btagB, so the first element in the vector has the largest btag score
        stable_sort(all_jets.begin(), all_jets.end(), [](const Jet & a, const Jet & b) -> bool
        {
            return ( get_property(a, B_TAG_ALGO) > get_property(b, B_TAG_ALGO) );
        });

        if (get_property(all_jets.at(2), B_TAG_ALGO) <= btagscore_WPmedium) // skip if the first 3 jets are not b tagged
            continue;

        ec.updateSelected(weight_);

        jet1_pt_            = all_jets.at(0).P4().Pt();
        jet1_eta_           = all_jets.at(0).P4().Eta();
        jet1_phi_           = all_jets.at(0).P4().Phi();
        jet1_m_             = all_jets.at(0).P4().M();
        // jet1_CMVA_     = get_property(all_jets.at(0), Jet_btagCMVA);
        jet1_btagscore_     = get_property(all_jets.at(0), B_TAG_ALGO);
        jet1_bRegCorr_      = get_property(all_jets.at(0), Jet_bRegCorr);
        jet1_bRegRes_       = get_property(all_jets.at(0), Jet_bRegRes);
        jet1_jetId_         = get_property(all_jets.at(0), Jet_jetId);
        jet1_puId_          = get_property(all_jets.at(0), Jet_puId);
        jet1_hadronFlavour_ = (is_data ? -999 : get_property(all_jets.at(0), Jet_hadronFlavour));

        jet2_pt_            = all_jets.at(1).P4().Pt();
        jet2_eta_           = all_jets.at(1).P4().Eta();
        jet2_phi_           = all_jets.at(1).P4().Phi();
        jet2_m_             = all_jets.at(1).P4().M();
        // jet2_CMVA_     = get_property(all_jets.at(1), Jet_btagCMVA);
        jet2_btagscore_     = get_property(all_jets.at(1), B_TAG_ALGO);
        jet2_bRegCorr_      = get_property(all_jets.at(1), Jet_bRegCorr);
        jet2_bRegRes_       = get_property(all_jets.at(1), Jet_bRegRes);
        jet2_jetId_         = get_property(all_jets.at(1), Jet_jetId);
        jet2_puId_          = get_property(all_jets.at(1), Jet_puId);
        jet2_hadronFlavour_ = (is_data ? -999 : get_property(all_jets.at(1), Jet_hadronFlavour));

        jet3_pt_            = all_jets.at(2).P4().Pt();
        jet3_eta_           = all_jets.at(2).P4().Eta();
        jet3_phi_           = all_jets.at(2).P4().Phi();
        jet3_m_             = all_jets.at(2).P4().M();
        // jet3_CMVA_     = get_property(all_jets.at(2), Jet_btagCMVA);
        jet3_btagscore_     = get_property(all_jets.at(2), B_TAG_ALGO);
        jet3_bRegCorr_      = get_property(all_jets.at(2), Jet_bRegCorr);
        jet3_bRegRes_       = get_property(all_jets.at(2), Jet_bRegRes);
        jet3_jetId_         = get_property(all_jets.at(2), Jet_jetId);
        jet3_puId_          = get_property(all_jets.at(2), Jet_puId);
        jet3_hadronFlavour_ = (is_data ? -999 : get_property(all_jets.at(2), Jet_hadronFlavour));

        jet4_pt_            = all_jets.at(3).P4().Pt();
        jet4_eta_           = all_jets.at(3).P4().Eta();
        jet4_phi_           = all_jets.at(3).P4().Phi();
        jet4_m_             = all_jets.at(3).P4().M();
        // jet4_CMVA_     = get_property(all_jets.at(3), Jet_btagCMVA);
        jet4_btagscore_     = get_property(all_jets.at(3), B_TAG_ALGO);
        jet4_bRegCorr_      = get_property(all_jets.at(3), Jet_bRegCorr);
        jet4_bRegRes_       = get_property(all_jets.at(3), Jet_bRegRes);
        jet4_jetId_         = get_property(all_jets.at(3), Jet_jetId);
        jet4_puId_          = get_property(all_jets.at(3), Jet_puId);
        jet4_hadronFlavour_ = (is_data ? -999 : get_property(all_jets.at(3), Jet_hadronFlavour));


        std::vector<Jet> preselected_jets = {
            all_jets.at(0),
            all_jets.at(1),
            all_jets.at(2),
            all_jets.at(3)
        };

        if (!is_data)
        {
            auto all_btag_SF = computeBtagSF(preselected_jets, btcr.get());
            btag_SF_ = std::get<0>(all_btag_SF);

            btag_SF_bup_   = std::get<1>(all_btag_SF).first;
            btag_SF_bdown_ = std::get<1>(all_btag_SF).second;

            btag_SF_cup_   = std::get<2>(all_btag_SF).first;
            btag_SF_cdown_ = std::get<2>(all_btag_SF).second;

            btag_SF_lightup_   = std::get<3>(all_btag_SF).first;
            btag_SF_lightdown_ = std::get<3>(all_btag_SF).second;
        }
        else
        {
            btag_SF_            = 1.0;
            btag_SF_bup_        = 1.0;
            btag_SF_bdown_      = 1.0;
            btag_SF_cup_        = 1.0;
            btag_SF_cdown_      = 1.0;
            btag_SF_lightup_    = 1.0;
            btag_SF_lightdown_  = 1.0;
        }

        if (!is_data)
        {
            int  ngj  = *nat.nGenJet;

            int  igj1 = get_property(all_jets.at(0), Jet_genJetIdx);
            bool has_gj1 = (igj1 >= 0 && igj1 < ngj);

            genjet1_pt_             = (has_gj1 ? nat.GenJet_pt            .At(igj1) : -999);
            genjet1_eta_            = (has_gj1 ? nat.GenJet_eta           .At(igj1) : -999);
            genjet1_phi_            = (has_gj1 ? nat.GenJet_phi           .At(igj1) : -999);
            genjet1_m_              = (has_gj1 ? nat.GenJet_mass          .At(igj1) : -999);
            // genjet1_partonFlavour_  = (has_gj1 ? nat.GenJet_partonFlavour .At(igj1) : -999);
            genjet1_hadronFlavour_  = (has_gj1 ? nat.GenJet_hadronFlavour .At(igj1) : -999);


            int  igj2 = get_property(all_jets.at(1), Jet_genJetIdx);
            bool has_gj2 = (igj2 >= 0 && igj2 < ngj);

            genjet2_pt_             = (has_gj2 ? nat.GenJet_pt            .At(igj2) : -999);
            genjet2_eta_            = (has_gj2 ? nat.GenJet_eta           .At(igj2) : -999);
            genjet2_phi_            = (has_gj2 ? nat.GenJet_phi           .At(igj2) : -999);
            genjet2_m_              = (has_gj2 ? nat.GenJet_mass          .At(igj2) : -999);
            // genjet2_partonFlavour_  = (has_gj2 ? nat.GenJet_partonFlavour .At(igj2) : -999);
            genjet2_hadronFlavour_  = (has_gj2 ? nat.GenJet_hadronFlavour .At(igj2) : -999);


            int  igj3 = get_property(all_jets.at(2), Jet_genJetIdx);
            bool has_gj3 = (igj3 >= 0 && igj3 < ngj);

            genjet3_pt_             = (has_gj3 ? nat.GenJet_pt            .At(igj3) : -999);
            genjet3_eta_            = (has_gj3 ? nat.GenJet_eta           .At(igj3) : -999);
            genjet3_phi_            = (has_gj3 ? nat.GenJet_phi           .At(igj3) : -999);
            genjet3_m_              = (has_gj3 ? nat.GenJet_mass          .At(igj3) : -999);
            // genjet3_partonFlavour_  = (has_gj3 ? nat.GenJet_partonFlavour .At(igj3) : -999);
            genjet3_hadronFlavour_  = (has_gj3 ? nat.GenJet_hadronFlavour .At(igj3) : -999);


            int  igj4 = get_property(all_jets.at(3), Jet_genJetIdx);
            bool has_gj4 = (igj4 >= 0 && igj4 < ngj);

            genjet4_pt_             = (has_gj4 ? nat.GenJet_pt            .At(igj4) : -999);
            genjet4_eta_            = (has_gj4 ? nat.GenJet_eta           .At(igj4) : -999);
            genjet4_phi_            = (has_gj4 ? nat.GenJet_phi           .At(igj4) : -999);
            genjet4_m_              = (has_gj4 ? nat.GenJet_mass          .At(igj4) : -999);
            // genjet4_partonFlavour_  = (has_gj4 ? nat.GenJet_partonFlavour .At(igj4) : -999);
            genjet4_hadronFlavour_  = (has_gj4 ? nat.GenJet_hadronFlavour .At(igj4) : -999);

            // /////// cross check association
            // //// result: cases where my hand done match finds something and the NanoAOD does not, or viceversa, are below 0.5%
            // std::vector<GenJet> all_gen_jets;
            // all_gen_jets.reserve(*(nat.nGenJet));
            // for (uint igj = 0; igj < *(nat.nGenJet); ++igj)
            // {
            //     all_gen_jets.emplace_back(GenJet(igj, &nat));
            // }

            // int imgj1 = match_genJets(all_jets.at(0), all_gen_jets);
            // int imgj2 = match_genJets(all_jets.at(1), all_gen_jets);
            // int imgj3 = match_genJets(all_jets.at(2), all_gen_jets);
            // int imgj4 = match_genJets(all_jets.at(3), all_gen_jets);

            // bool has_mgj1 = (imgj1 >= 0 && imgj1 < ngj);

            // genjet1_xc_pt_             = (has_mgj1 ? nat.GenJet_pt            .At(imgj1) : -999);
            // genjet1_xc_eta_            = (has_mgj1 ? nat.GenJet_eta           .At(imgj1) : -999);
            // genjet1_xc_phi_            = (has_mgj1 ? nat.GenJet_phi           .At(imgj1) : -999);
            // genjet1_xc_m_              = (has_mgj1 ? nat.GenJet_mass          .At(imgj1) : -999);
            // genjet1_xc_partonFlavour_  = (has_mgj1 ? nat.GenJet_partonFlavour .At(imgj1) : -999);
            // genjet1_xc_hadronFlavour_  = (has_mgj1 ? nat.GenJet_hadronFlavour .At(imgj1) : -999);


            // bool has_mgj2 = (imgj2 >= 0 && imgj2 < ngj);

            // genjet2_xc_pt_             = (has_mgj2 ? nat.GenJet_pt            .At(imgj2) : -999);
            // genjet2_xc_eta_            = (has_mgj2 ? nat.GenJet_eta           .At(imgj2) : -999);
            // genjet2_xc_phi_            = (has_mgj2 ? nat.GenJet_phi           .At(imgj2) : -999);
            // genjet2_xc_m_              = (has_mgj2 ? nat.GenJet_mass          .At(imgj2) : -999);
            // genjet2_xc_partonFlavour_  = (has_mgj2 ? nat.GenJet_partonFlavour .At(imgj2) : -999);
            // genjet2_xc_hadronFlavour_  = (has_mgj2 ? nat.GenJet_hadronFlavour .At(imgj2) : -999);


            // bool has_mgj3 = (imgj3 >= 0 && imgj3 < ngj);

            // genjet3_xc_pt_             = (has_mgj3 ? nat.GenJet_pt            .At(imgj3) : -999);
            // genjet3_xc_eta_            = (has_mgj3 ? nat.GenJet_eta           .At(imgj3) : -999);
            // genjet3_xc_phi_            = (has_mgj3 ? nat.GenJet_phi           .At(imgj3) : -999);
            // genjet3_xc_m_              = (has_mgj3 ? nat.GenJet_mass          .At(imgj3) : -999);
            // genjet3_xc_partonFlavour_  = (has_mgj3 ? nat.GenJet_partonFlavour .At(imgj3) : -999);
            // genjet3_xc_hadronFlavour_  = (has_mgj3 ? nat.GenJet_hadronFlavour .At(imgj3) : -999);


            // bool has_mgj4 = (imgj4 >= 0 && imgj4 < ngj);

            // genjet4_xc_pt_             = (has_mgj4 ? nat.GenJet_pt            .At(imgj4) : -999);
            // genjet4_xc_eta_            = (has_mgj4 ? nat.GenJet_eta           .At(imgj4) : -999);
            // genjet4_xc_phi_            = (has_mgj4 ? nat.GenJet_phi           .At(imgj4) : -999);
            // genjet4_xc_m_              = (has_mgj4 ? nat.GenJet_mass          .At(imgj4) : -999);
            // genjet4_xc_partonFlavour_  = (has_mgj4 ? nat.GenJet_partonFlavour .At(imgj4) : -999);
            // genjet4_xc_hadronFlavour_  = (has_mgj4 ? nat.GenJet_hadronFlavour .At(imgj4) : -999);


        }

        rndm_1_ = rndm_engine.Rndm();
        rndm_2_ = rndm_engine.Rndm();
        rndm_3_ = rndm_engine.Rndm();

        tOut->Fill();
    }

    cout << "[INFO] ... done, saving output file" << endl;
    outputFile.cd();
    tOut->Write();
    ec.write();

}