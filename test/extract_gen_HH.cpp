/*
** class  : extract_gen_HH.cpp
** author : L. Cadamuro (UF)
** date   : 05/07/2019
** brief  : extracts the gen HH info from all the HH events (no preselections) from a NanoAOD
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
        ("kl-map"    , po::value<string>()->default_value(""), "klambda input map for reweighting")
        // flags
        ("save-p4"   , po::value<bool>()->zero_tokens()->implicit_value(true)->default_value(false), "save the tlorentzvectors in the output")
        ("histo-only", po::value<bool>()->zero_tokens()->implicit_value(true)->default_value(false), "just produce reweight histograms, do not store TTree (default is false)")    
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
    bool histo_only = opts["histo-only"].as<bool>();

    OfflineProducerHelper oph;

    // optional test for reweight
    std::unique_ptr<HHReweight5D> hhreweighter;
    std::vector<float> klambdas = {-2, -1, 0, 1, 2, 5, 10};
    if (opts["kl-map"].as<string>().size() > 0)
    {
            string kl_map    = opts["kl-map"].as<string>(); // sample map fname
            string kl_histo  = "hhGenLevelDistr";           // sample map histo
            string kl_coeffs = "weights/coefficientsByBin_extended_3M_costHHSim_19-4.txt"; // coefficient file

            TFile* fIn = TFile::Open(kl_map.c_str());
            TH2D*  hIn = (TH2D*) fIn->Get(kl_histo.c_str());
            hhreweighter = std::unique_ptr<HHReweight5D> (new HHReweight5D(kl_coeffs.c_str(), hIn));
            fIn->Close();
    }

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
    float gen_H1_m;
    float gen_H1_pt;
    float gen_H1_eta;
    float gen_H1_phi;
    TLorentzVector gen_H1_p4;

    float gen_H2_m;
    float gen_H2_pt;
    float gen_H2_eta;
    float gen_H2_phi;
    TLorentzVector gen_H2_p4;

    float gen_mHH;
    float gen_costh_H1_cm;
    float gen_costh_H2_cm;

    // branches
    tOut->Branch("gen_H1_m",   &gen_H1_m);
    tOut->Branch("gen_H1_pt",  &gen_H1_pt);
    tOut->Branch("gen_H1_eta", &gen_H1_eta);
    tOut->Branch("gen_H1_phi", &gen_H1_phi);
    if (save_p4) tOut->Branch("gen_H1_p4",  &gen_H1_p4);

    tOut->Branch("gen_H2_m",   &gen_H2_m);
    tOut->Branch("gen_H2_pt",  &gen_H2_pt);
    tOut->Branch("gen_H2_eta", &gen_H2_eta);
    tOut->Branch("gen_H2_phi", &gen_H2_phi);
    if (save_p4) tOut->Branch("gen_H2_p4",  &gen_H2_p4);

    tOut->Branch("gen_mHH",         &gen_mHH);
    tOut->Branch("gen_costh_H1_cm", &gen_costh_H1_cm);
    tOut->Branch("gen_costh_H2_cm", &gen_costh_H2_cm);

    std::vector<float>  klambdas_branch(klambdas.size());
    std::vector<string> klambdas_names(klambdas.size());
    if (hhreweighter)
    {
        for (uint ikl = 0; ikl < klambdas.size(); ++ikl)
        {
            float kl = klambdas.at(ikl);
            // string brname = std::to_string(kl);
            std::stringstream stream;
            stream << std::fixed << std::setprecision(1) << kl;
            std::string brname = stream.str();
            std::replace( brname.begin(), brname.end(), '-', 'm');
            std::replace( brname.begin(), brname.end(), '.', 'd');
            brname = string("HH_rew_") + brname;
            cout << ".... making reweight for " << kl << " in branch " << brname << endl;
            tOut->Branch(brname.c_str(), &klambdas_branch.at(ikl));
        }
    }

    // output histograms used for reweighting
    // NOTE: binning must match the one of the reweighting coefficients
    double binning_mHH [56] = { 250,260,270,280,290,300,310,320,330,340,
                                350,360,370,380,390,400,410,420,430,440, 
                                450,460,470,480,490,
                                500,510,520,530,540,550,600,610,620,630,
                                640,650,660,670,680,690,700,750,800,850,
                                900,950,1000,1100,1200,1300,1400,1500.,1750,2000,50000};
    double binning_cth [5]  = {0.0, 0.4, 0.6, 0.8, 1.0} ;
    int nbins_mHH = 55; // size of arrays - 1
    int nbins_cth = 4;  // size of arrays - 1
    TH2D* hMap       = new TH2D ("hhGenLevelDistr", "hhGenLevelDistr", nbins_mHH, binning_mHH, nbins_cth, binning_cth );
    // TH2F* hMapFolded = new TH2F ("hhGenLevelDistrFolded", "hhGenLevelDistrFolded", 90, 0, 1800, 5, 0, 1); // won't be used for reweight
    TH1D* hMap1D     = new TH1D ("hhGenLevelDistr1D", "hhGenLevelDistr1D", nbins_mHH, binning_mHH);

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
              
        EventInfo ei;

        oph.select_gen_HH(nat, ei);
        // oph.select_gen_bb_bb(nat, ei);            

        // copy selected values to output
        gen_H1_m   = ei.gen_H1->P4().M();
        gen_H1_pt  = ei.gen_H1->P4().Pt();
        gen_H1_eta = ei.gen_H1->P4().Eta();
        gen_H1_phi = ei.gen_H1->P4().Phi();
        gen_H1_p4  = ei.gen_H1->P4();
        
        gen_H2_m   = ei.gen_H2->P4().M();
        gen_H2_pt  = ei.gen_H2->P4().Pt();
        gen_H2_eta = ei.gen_H2->P4().Eta();
        gen_H2_phi = ei.gen_H2->P4().Phi();
        gen_H2_p4  = ei.gen_H2->P4();
        
        TLorentzVector vSum = gen_H1_p4 + gen_H2_p4;
        
        // boost to CM
        TLorentzVector vH1_cm = gen_H1_p4;
        TLorentzVector vH2_cm = gen_H2_p4;
        vH1_cm.Boost(-vSum.BoostVector());
        vH2_cm.Boost(-vSum.BoostVector());

        gen_mHH          = vSum.M();
        gen_costh_H1_cm  = vH1_cm.CosTheta();
        gen_costh_H2_cm  = vH2_cm.CosTheta();

        if (hhreweighter)
        {
            for (uint ikl = 0; ikl < klambdas.size(); ++ikl)
            {
                float kl = klambdas.at(ikl);
                float w = hhreweighter->getWeight(kl, 1.0, gen_mHH, gen_costh_H1_cm);
                klambdas_branch.at(ikl) = w;
            }
        }

        if (!histo_only) tOut->Fill();

        // fill histograms
        hMap   -> Fill(gen_mHH, TMath::Abs(gen_costh_H1_cm));
        hMap1D -> Fill(gen_mHH);

    }

    cout << "[INFO] ... done, saving output file" << endl;
    outputFile.cd();
    if (!histo_only) tOut->Write();
    hMap   -> Write();
    hMap1D -> Write();

}