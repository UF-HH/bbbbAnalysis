/*
** class  : check_XYH_sample.cpp
** author : L. Cadamuro (UF)
** date   : 19/09/2019
** brief  : run on a NMSSM X->YH sample and extracts the gen info from all events (no preselections) from a NanoAOD.
**        : Samples are produced for a common X value, so this allows for checking which events are included to validate a sample
*/

#include <iostream>
#include <string>
#include <iomanip>
#include <algorithm>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "CfgParser.h"
#include "NanoAODTree.h"
#include "EventInfo.h"

#include "SkimUtils.h"
namespace su = SkimUtils;

#include "OfflineProducerHelper.h"
// namespace oph = OfflineProducerHelper;

#include "OutputTree.h"
#include "SkimEffCounter.h"
#include "jsonLumiFilter.h"
#include "HHReweight5D.h"

#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"

using namespace std;

#include <any>

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
        ("input" , po::value<string>()->required(), "input file list")
        ("output", po::value<string>()->required(), "output file LFN")
        // optional
        ("maxEvts"   , po::value<int>()->default_value(-1),     "max number of events to process")
        // flags
        ("save-p4"   , po::value<bool>()->zero_tokens()->implicit_value(true)->default_value(false), "save the tlorentzvectors in the output")
        ("count-only", po::value<bool>()->zero_tokens()->implicit_value(true)->default_value(false), "just count the events per point")
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

    // task flags
    bool save_p4    = opts["save-p4"].as<bool>();
    bool count_only = opts["count-only"].as<bool>();

    OfflineProducerHelper oph;

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

    ////////////////////////////////////////////////////////////////////////
    // Prepare the output
    ////////////////////////////////////////////////////////////////////////
    
    string outputFileName = opts["output"].as<string>();
    cout << "[INFO] ... saving output to file : " << outputFileName << endl;
    TFile outputFile(outputFileName.c_str(), "recreate");
    
    // TTree with a custom format
    TTree* tOut = new TTree("HH_gen_info", "HH_gen_info");

    // vars
    float gen_X_m;
    float gen_X_pt;
    float gen_X_eta;
    float gen_X_phi;
    TLorentzVector gen_X_p4;

    float gen_Y_m;
    float gen_Y_pt;
    float gen_Y_eta;
    float gen_Y_phi;
    TLorentzVector gen_Y_p4;

    float gen_H_m;
    float gen_H_pt;
    float gen_H_eta;
    float gen_H_phi;
    TLorentzVector gen_H_p4;

    // branches
    tOut->Branch("gen_X_m",   &gen_X_m);
    tOut->Branch("gen_X_pt",  &gen_X_pt);
    tOut->Branch("gen_X_eta", &gen_X_eta);
    tOut->Branch("gen_X_phi", &gen_X_phi);
    if (save_p4) tOut->Branch("gen_X_p4",  &gen_X_p4);

    tOut->Branch("gen_Y_m",   &gen_Y_m);
    tOut->Branch("gen_Y_pt",  &gen_Y_pt);
    tOut->Branch("gen_Y_eta", &gen_Y_eta);
    tOut->Branch("gen_Y_phi", &gen_Y_phi);
    if (save_p4) tOut->Branch("gen_Y_p4",  &gen_Y_p4);

    tOut->Branch("gen_H_m",   &gen_H_m);
    tOut->Branch("gen_H_pt",  &gen_H_pt);
    tOut->Branch("gen_H_eta", &gen_H_eta);
    tOut->Branch("gen_H_phi", &gen_H_phi);
    if (save_p4) tOut->Branch("gen_H_p4",  &gen_H_p4);

    // a map to count the number of events per mass point
    // assume that all mass values have been generated with integers (and it'd better be the case!)
    // key is mX, mY
    std::map <std::pair<int, int>, long long unsigned int> events_per_point; 
    unsigned long long int nEvents = 0;

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
        
        ++nEvents;      
        EventInfo ei;

        oph.select_gen_XYH(nat, ei);

        // copy selected values to output
        // NOTE: when accessing the first copy, sometimes X has pt = 0 -> the TLV returns a -nan mass
        // so better to access the mass of X directly from the miniAOD
        gen_X_m   = get_property((*ei.gen_X), GenPart_mass); // ei.gen_X->P4().M();
        gen_X_pt  = get_property((*ei.gen_X), GenPart_pt);   // ei.gen_X->P4().Pt();
        gen_X_eta = get_property((*ei.gen_X), GenPart_eta);  // ei.gen_X->P4().Eta();
        gen_X_phi = get_property((*ei.gen_X), GenPart_phi);  // ei.gen_X->P4().Phi();
        gen_X_p4  = ei.gen_X->P4();

        gen_Y_m   = get_property((*ei.gen_H1), GenPart_mass); // ei.gen_H1->P4().M();
        gen_Y_pt  = get_property((*ei.gen_H1), GenPart_pt);   // ei.gen_H1->P4().Pt();
        gen_Y_eta = get_property((*ei.gen_H1), GenPart_eta);  // ei.gen_H1->P4().Eta();
        gen_Y_phi = get_property((*ei.gen_H1), GenPart_phi);  // ei.gen_H1->P4().Phi();
        gen_Y_p4  = ei.gen_H1->P4();
        
        gen_H_m   = get_property((*ei.gen_H2), GenPart_mass); // ei.gen_H2->P4().M();
        gen_H_pt  = get_property((*ei.gen_H2), GenPart_pt);   // ei.gen_H2->P4().Pt();
        gen_H_eta = get_property((*ei.gen_H2), GenPart_eta);  // ei.gen_H2->P4().Eta();
        gen_H_phi = get_property((*ei.gen_H2), GenPart_phi);  // ei.gen_H2->P4().Phi();
        gen_H_p4  = ei.gen_H2->P4();

        if (!count_only) tOut->Fill();

        // now counts
        int mX = (int) gen_X_m;
        int mY = (int) gen_Y_m;
        int mH = (int) gen_H_m;

        // if (mX < 0)
        //     cout << mX << " " << gen_X_m << endl;

        if (mH != 125)
            cout << "[ERROR] : the Higgs boson has a mass of " << mH << endl;

        std::pair<int,int> key = make_pair (mX, mY);
        // new element found
        if (events_per_point.find(key) == events_per_point.end())
            events_per_point[key] = 0;
        // update count
            events_per_point[key] += 1;
    }

    cout << "[INFO] ... done, saving output file" << endl;
    outputFile.cd();
    if (!count_only) tOut->Write();

    // print the results
    std::vector<std::pair<int,int>> all_points;
    for(auto it = events_per_point.begin(); it != events_per_point.end(); ++it) {
        all_points.push_back(it->first);
    }

    sort(all_points.begin(), all_points.end()); // for a nice display
    cout << "... I found " << all_points.size() << " different mass points" << endl;
    for (auto point : all_points)
    {
        auto Nthis = events_per_point[point];
        cout << "mX = " << point.first << " mY = " << point.second << " N = " << Nthis << " [" << 100.*Nthis/nEvents << "%]" << endl;
    }
}