/*
** class  : skim_for_das.cpp
** author : L. Cadamuro (UF)
** date   : 12/11/2019
** brief  : transforms a NanoAOD into a bbbb ntuple for the subsequent plots/analysis
*/

// skim_trigger_for_das.exe --cfg config/skim_CMSDAS_2016Trigger.cfg --input inputFiles/CMSDAS/Samples2016/SingleMuon_Data_forTrigger.txt --output SingleMuon_forTrigger.root --is-data --maxEvts 100000
// skim_trigger_for_das.exe --cfg config/skim_CMSDAS_2016Trigger.cfg --input inputFiles/CMSDAS/Samples2016/MC_TT_TuneCUETP8M2T4_13TeV_forTrigger.txt --output TTBar_forTrigger.root --puWeight weights/2016/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8_PUweights.root --maxEvts 100000

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
        ("maxEvts"  , po::value<int>()->default_value(-1), "max number of events to process")
        ("puWeight" , po::value<string>()->default_value(""), "PU weight file name")
        // flags
        ("is-data",    po::value<bool>()->zero_tokens()->implicit_value(true)->default_value(false), "mark as a data sample (default is false)")
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
    cout << "[INFO] ... is a data sample? " << std::boolalpha << is_data << std::noboolalpha << endl;

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

    float btag_SF_;

    float weight_;

    // jets
    float jetFirstHighestPt_pt_;
    float jetSecondHighestPt_pt_;
    float jetThirdHighestPt_pt_;
    float jetForthHighestPt_pt_;
    float fourHighestJetPt_sum_;
    float jetFirstHighestDeepCSV_deepCSV_;
    float jetThirdHighestDeepCSV_deepCSV_;

    tOut->Branch("run",              &run_);
    tOut->Branch("luminosityBlock",  &luminosityBlock_);
    tOut->Branch("event",            &event_);

    tOut->Branch("btag_SF",           &btag_SF_);
    
    tOut->Branch("weight",        &weight_);

    tOut->Branch("jetFirstHighestPt_pt" , &jetFirstHighestPt_pt_ );
    tOut->Branch("jetSecondHighestPt_pt", &jetSecondHighestPt_pt_);
    tOut->Branch("jetThirdHighestPt_pt" , &jetThirdHighestPt_pt_ );
    tOut->Branch("jetForthHighestPt_pt" , &jetForthHighestPt_pt_ );
    tOut->Branch("fourHighestJetPt_sum" , &fourHighestJetPt_sum_ );
    tOut->Branch("jetFirstHighestDeepCSV_deepCSV" , &jetFirstHighestDeepCSV_deepCSV_ );
    tOut->Branch("jetThirdHighestDeepCSV_deepCSV" , &jetThirdHighestDeepCSV_deepCSV_ );

    //enagle trigger filters
    std::map<std::pair<int,int>, std::string > triggerObjectsForStudies;   
    std::map<std::pair<int,int>, int > triggerObjectsForStudiesCount;   
    const string objectsForCut = config.readStringOpt("parameters::ObjectsForCut");
    if(objectsForCut == "TriggerObjects"){


        std::vector<std::string> triggerObjectMatchingVector = config.readStringListOpt("parameters::ListOfTriggerObjectsAndBit");
        
        std::string delimiter = ":";
        size_t pos = 0;
        
        for (auto & triggerObject : triggerObjectMatchingVector)
        {

            std::vector<std::string> triggerObjectTokens;
            while ((pos = triggerObject.find(delimiter)) != std::string::npos)
            {
                triggerObjectTokens.push_back(triggerObject.substr(0, pos));
                triggerObject.erase(0, pos + delimiter.length());
            }
            triggerObjectTokens.push_back(triggerObject); // last part splitted
            if (triggerObjectTokens.size() != 3)
            {
                throw std::runtime_error("** skim_ntuple : could not parse triggerObject for Cuts entry " + triggerObject + " , aborting");
            }

            std::pair<int,int> objectAndFilter = std::make_pair(atoi(triggerObjectTokens[0].data()),atoi(triggerObjectTokens[1].data()));
            triggerObjectsForStudies[objectAndFilter] = triggerObjectTokens[2];
            triggerObjectsForStudiesCount[objectAndFilter] = 0;
            tOut->Branch(triggerObjectsForStudies[objectAndFilter].data(), &triggerObjectsForStudiesCount[objectAndFilter]); 
        }
    }


    ////////////////////////////////////////////////////////////////////////
    // Execute event loop
    ////////////////////////////////////////////////////////////////////////

    int maxEvts = opts["maxEvts"].as<int>();
    if (maxEvts >= 0)
        cout << "[INFO] ... running on : " << maxEvts << " events" << endl;

    for (int iEv = 0; true; ++iEv)
    {
        if (maxEvts >= 0 && iEv >= maxEvts)
            break;

        if (!nat.Next()) break;
        if (iEv % 10000 == 0) cout << "... processing event " << iEv << endl;

        // aply json filter to data
        if (is_data && !jlf.isValid(*nat.run, *nat.luminosityBlock)){
            continue; // not a valid lumi
        }
              
        EventInfo ei;

        run_             = *(nat.run);
        luminosityBlock_ = *(nat.luminosityBlock);
        event_           = *(nat.event);

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
            // int puid  = get_property(jet, Jet_puId);

            if (jet.P4().Pt() <= 25)
                continue;

            if (std::abs(jet.P4().Eta()) > 2.4)
                continue;

            if (!checkBit(jetid, 1)) // tight jet Id
                continue;

            // if (!checkBit(puid, 1) && jet.P4().Pt() <= 50) // medium PU Id - NOTE : not to be applied beyond 50 GeV: https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupJetID
            //     continue;

            all_jets.emplace_back(jet);
        }

        if (all_jets.size() < 4) // I don't have 4 preselected jets
            continue;
        
        ec.updateSelected(weight_);

        stable_sort(all_jets.begin(), all_jets.end(), [](const Jet & a, const Jet & b) -> bool
        {
            return ( a.P4().Pt() > b.P4().Pt() );
        });

        //Check if there is and iso muon with Pt>30 GeV
        bool isoMuonFound = false;
        for (uint ij = 0; ij < *(nat.nMuon); ++ij)
        {
            Muon theMuon (ij, &nat);
            if(get_property(theMuon, Muon_pfRelIso04_all)<0.1 && get_property(theMuon, Muon_mediumId) && theMuon.P4().Pt() >= 40.)
            {
                int muonJetId = get_property(theMuon, Muon_jetIdx);
                if(muonJetId >= 0)
                {
                    bool isUnmatched = true;
                    for(auto jet : all_jets)
                    {
                        if(jet.getIdx() == muonJetId)
                        {
                            isUnmatched = false;
                            break;  
                        }
                    }
                    isoMuonFound = isUnmatched;
                }
                else isoMuonFound = true;
            }
            if(isoMuonFound) break;

        }
        if(!isoMuonFound) continue;

        jetFirstHighestPt_pt_  = all_jets.at(0).P4().Pt();
        jetSecondHighestPt_pt_ = all_jets.at(1).P4().Pt();
        jetThirdHighestPt_pt_  = all_jets.at(2).P4().Pt();
        jetForthHighestPt_pt_  = all_jets.at(3).P4().Pt();
        fourHighestJetPt_sum_  = jetFirstHighestPt_pt_ + jetSecondHighestPt_pt_ + jetThirdHighestPt_pt_ + jetForthHighestPt_pt_;

        // NOTE that this sorts from small to large, with A < B implemented as btagA > btagB, so the first element in the vector has the largest btag score
        stable_sort(all_jets.begin(), all_jets.end(), [](const Jet & a, const Jet & b) -> bool
        {
            return ( get_property(a, B_TAG_ALGO) > get_property(b, B_TAG_ALGO) );
        });

        jetFirstHighestDeepCSV_deepCSV_ = get_property(all_jets.at(0), B_TAG_ALGO);
        jetThirdHighestDeepCSV_deepCSV_ = get_property(all_jets.at(2), B_TAG_ALGO);

        // get number of trigger filters

        // reset map
        for(auto &triggerFilter : triggerObjectsForStudiesCount) triggerFilter.second = 0;
    
        // loop over all trigger objects
        for (uint trigObjIt = 0; trigObjIt < *(nat.nTrigObj); ++trigObjIt) //for over all trigger objects
        {
            int triggerObjectId = nat.TrigObj_id.At(trigObjIt);
            int triggerFilterBitSum = nat.TrigObj_filterBits.At(trigObjIt);

            for(auto &triggerFilter : triggerObjectsForStudiesCount)
            {
                if(triggerObjectId != triggerFilter.first.first) continue;
                if( (triggerFilterBitSum >> triggerFilter.first.second) & 0x1 ) //check object passes the filter
                    ++triggerFilter.second;
            }
        }
        
        if (!is_data)
        {
            std::vector<Jet> preselected_jets = {
                all_jets.at(0),
                all_jets.at(1),
                all_jets.at(2),
                all_jets.at(3)
            };
            auto all_btag_SF = computeBtagSF(preselected_jets, btcr.get());
            btag_SF_ = std::get<0>(all_btag_SF);
        }
        else
        {
            btag_SF_            = 1.0;
        }

        tOut->Fill();
    }

    cout << "[INFO] ... done, saving output file" << endl;
    outputFile.cd();
    tOut->Write();
    ec.write();

}