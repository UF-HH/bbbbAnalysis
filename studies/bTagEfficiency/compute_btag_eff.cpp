// c++ -lm -o compute_btag_eff compute_btag_eff.cpp `root-config --glibs --cflags` -lTreePlayer

#include <iostream>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TH2D.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TTreeReaderValue.h"
#include "TLorentzVector.h"

using namespace std;

int appendFromFileList (TChain* chain, std::string filename)
{
    //cout << "=== inizio parser ===" << endl;
    std::ifstream infile(filename.c_str());
    std::string line;
    int nfiles = 0;
    while (std::getline(infile, line))
    {
        line = line.substr(0, line.find("#", 0)); // remove comments introduced by #
        while (line.find(" ") != std::string::npos) line = line.erase(line.find(" "), 1); // remove white spaces
        while (line.find("\n") != std::string::npos) line = line.erase(line.find("\n"), 1); // remove new line characters
        while (line.find("\r") != std::string::npos) line = line.erase(line.find("\r"), 1); // remove carriage return characters
        if (!line.empty()) // skip empty lines
        {
            chain->Add(line.c_str());
            ++nfiles;
        }
     }
    return nfiles;
}

struct jet_t {
    float pt;
    float eta;
    float phi;
    float mass;
    float ptregr;
    float massregr;
    float deepjet;
    int   flav;
    int   puid;
    int   jetid;
    TLorentzVector p4;
    TLorentzVector p4regr;
};

int main(int argc, char** argv)
{
    int year = 2018;

    if (argc > 1)
        year = std::stoi(argv[1]);

    cout << "... YEAR IS: " << year << endl;

    string flist;
    string foutName;
    std::vector<double> WPs;

    if (year == 2018){
        // flist = "../../inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH1_TuneCP5_PSWeights_13TeV-powheg-pythia8.txt";
        flist = "all_HH_NLO_2018.txt";
        foutName  = "btag_eff_maps_2018.root";
        WPs   = {0.0494, 0.2770, 0.7264};
    }

    else if (year == 2017){
        // flist = "../../inputFiles/2017NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH1_TuneCP5_PSWeights_13TeV-powheg-pythia8.txt";
        flist = "all_HH_NLO_2017.txt";
        foutName  = "btag_eff_maps_2017.root";
        WPs   = {0.0521, 0.3033, 0.7489};
    }

    else if (year == 2016){
        // flist = "../../inputFiles/2016NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_node_cHHH1_TuneCUETP8M1_PSWeights_13TeV-powheg-pythia8.txt";
        flist = "all_HH_NLO_2016.txt";
        foutName  = "btag_eff_maps_2016.root";
        WPs   = {0.0614, 0.3093, 0.7221};
    }

    // if (argc > 2){
    //     flist = argv[1];
    //     foutName  = argv[2];
    // }

    cout << "... running on file list " << flist << endl;
    cout << "... output file is " << foutName << endl;
    cout << "... working points " << endl;
    cout << "    - Loose  : " << WPs.at(0) << endl;
    cout << "    - Medium : " << WPs.at(1) << endl;
    cout << "    - Tight  : " << WPs.at(2) << endl;

    TChain ch ("Events");
    int nread = appendFromFileList (&ch, flist);
    cout << "... read " << nread << " files from input" << endl;

    cout << "[INFO] Creating branch readers" << endl;
    TTreeReader   fReader (&ch);
    // set branches

    TTreeReaderArray<Float_t>   Jet_pt            {fReader, "Jet_pt"};
    TTreeReaderArray<Float_t>   Jet_eta           {fReader, "Jet_eta"};
    TTreeReaderArray<Float_t>   Jet_phi           {fReader, "Jet_phi"};
    TTreeReaderArray<Float_t>   Jet_mass          {fReader, "Jet_mass"};
    TTreeReaderArray<Float_t>   Jet_btagDeepFlavB {fReader, "Jet_btagDeepFlavB"};
    TTreeReaderArray<Float_t>   Jet_btagDeepB     {fReader, "Jet_btagDeepB"};
    TTreeReaderArray<Float_t>   Jet_bRegCorr      {fReader, "Jet_bRegCorr"};
    TTreeReaderArray<Float_t>   Jet_bRegRes       {fReader, "Jet_bRegRes"};
    TTreeReaderArray<Int_t>     Jet_hadronFlavour {fReader, "Jet_hadronFlavour"};
    TTreeReaderArray<Int_t>     Jet_puId          {fReader, "Jet_puId"};
    TTreeReaderArray<Int_t>     Jet_jetId         {fReader, "Jet_jetId"};
    TTreeReaderValue<UInt_t>    nJet              {fReader, "nJet"};
    TTreeReaderValue<Float_t>   genWeight         {fReader, "genWeight"};

    // prepare output

    TFile* fOut = new TFile(foutName.c_str(), "recreate");

    // [jet flavour][WP][tot-pass]
    // std::vector<std::vector<std::std::vector<TH2D*>>> histos(3, std::vector(3, std::vector(2)));

    // index is WP
    std::vector<TH2*> h_b_pass(3);
    std::vector<TH2*> h_b_tot(3);

    std::vector<TH2*> h_c_pass(3);
    std::vector<TH2*> h_c_tot(3);

    std::vector<TH2*> h_udsg_pass(3);
    std::vector<TH2*> h_udsg_tot(3);

    std::vector<string> WPnames = {"loose", "medium", "tight"};

    // double bins_pt[] = {30, 50, 80, 120, 180, 300, 1000};
    // int nbins_pt = sizeof(bins_pt)/sizeof(double);

    double bins_eta[] = {0, 0.7, 1.5, 2.5};
    int nbins_eta = sizeof(bins_eta)/sizeof(double);

    double bins_pt[] = {30, 40, 50, 60, 80, 100, 120, 140, 180, 220, 300, 1000};
    int nbins_pt = sizeof(bins_pt)/sizeof(double);

    // double bins_eta[] = {0, 0.5, 1, 1.5, 2, 2.5};
    // int nbins_eta = sizeof(bins_eta)/sizeof(double);

    cout << "... binning pt" << endl;
    for (uint i = 0; i < nbins_pt; ++i)
    cout << bins_pt[i] << " ";
        cout << endl;

    cout << "... binning eta" << endl;
    for (uint i = 0; i < nbins_eta; ++i)
    cout << bins_eta[i] << " ";
        cout << endl;

    for (uint iWP = 0; iWP < 3; ++iWP) {

        // h_b_pass.at(iWP)    = new TH2D((string("h_b_pass_")    + WPnames.at(iWP)).c_str(), ";pt;aeta", 100, 0, 1000, 10, 0, 2.5);
        // h_b_tot.at(iWP)     = new TH2D((string("h_b_tot_")     + WPnames.at(iWP)).c_str(), ";pt;aeta", 100, 0, 1000, 10, 0, 2.5);
        
        // h_c_pass.at(iWP)    = new TH2D((string("h_c_pass_")    + WPnames.at(iWP)).c_str(), ";pt;aeta", 100, 0, 1000, 10, 0, 2.5);
        // h_c_tot.at(iWP)     = new TH2D((string("h_c_tot_")     + WPnames.at(iWP)).c_str(), ";pt;aeta", 100, 0, 1000, 10, 0, 2.5);
        
        // h_udsg_pass.at(iWP) = new TH2D((string("h_udsg_pass_") + WPnames.at(iWP)).c_str(), ";pt;aeta", 100, 0, 1000, 10, 0, 2.5);
        // h_udsg_tot.at(iWP)  = new TH2D((string("h_udsg_tot_")  + WPnames.at(iWP)).c_str(), ";pt;aeta", 100, 0, 1000, 10, 0, 2.5);

        h_b_pass.at(iWP)    = new TH2D((string("h_b_pass_")    + WPnames.at(iWP)).c_str(), ";pt;aeta", nbins_pt-1, bins_pt, nbins_eta-1, bins_eta);
        h_b_tot.at(iWP)     = new TH2D((string("h_b_tot_")     + WPnames.at(iWP)).c_str(), ";pt;aeta", nbins_pt-1, bins_pt, nbins_eta-1, bins_eta);
        
        h_c_pass.at(iWP)    = new TH2D((string("h_c_pass_")    + WPnames.at(iWP)).c_str(), ";pt;aeta", nbins_pt-1, bins_pt, nbins_eta-1, bins_eta);
        h_c_tot.at(iWP)     = new TH2D((string("h_c_tot_")     + WPnames.at(iWP)).c_str(), ";pt;aeta", nbins_pt-1, bins_pt, nbins_eta-1, bins_eta);
        
        h_udsg_pass.at(iWP) = new TH2D((string("h_udsg_pass_") + WPnames.at(iWP)).c_str(), ";pt;aeta", nbins_pt-1, bins_pt, nbins_eta-1, bins_eta);
        h_udsg_tot.at(iWP)  = new TH2D((string("h_udsg_tot_")  + WPnames.at(iWP)).c_str(), ";pt;aeta", nbins_pt-1, bins_pt, nbins_eta-1, bins_eta);
    }


    int maxEvts = -1;
    for (uint iEv = 0; true; ++iEv)
    {
    
        bool got = fReader.Next();
        if (!got)
            break;

        if (maxEvts > -1 && iEv > maxEvts)
            break;

        if (iEv % 10000 == 0)
            cout << "... doing event " << iEv << endl;

        // create objects
        std::vector<jet_t> jets(*nJet);
        for (int ijet = 0; ijet < *nJet; ++ijet)
        {
            jets.at(ijet).pt      = Jet_pt.At(ijet);
            jets.at(ijet).eta     = Jet_eta.At(ijet);
            jets.at(ijet).phi     = Jet_phi.At(ijet);
            jets.at(ijet).mass    = Jet_mass.At(ijet);
            // jets.at(ijet).deepjet = (use_deepjet ? Jet_btagDeepFlavB.At(ijet) : Jet_btagDeepB.At(ijet) );
            jets.at(ijet).deepjet = Jet_btagDeepFlavB.At(ijet);
            jets.at(ijet).flav    = Jet_hadronFlavour.At(ijet);
            jets.at(ijet).puid    = Jet_puId.At(ijet);
            jets.at(ijet).jetid   = Jet_jetId.At(ijet);
            jets.at(ijet).p4.SetPtEtaPhiM(jets.at(ijet).pt, jets.at(ijet).eta, jets.at(ijet).phi, jets.at(ijet).mass);

            float corr = Jet_bRegCorr.At(ijet);
            float res  = Jet_bRegRes.At(ijet);

            if (  !( (corr>0.1) && (corr<2) && (res>0.005) && (res<0.9) ) ) {
                 corr = 1.;
            }
            else if( Jet_pt.At(ijet) < 20 ){
                corr = 1.;
            }
            jets.at(ijet).ptregr   = Jet_pt.At(ijet)   * corr;
            jets.at(ijet).massregr = Jet_mass.At(ijet) * corr;
            jets.at(ijet).p4regr   = jets.at(ijet).p4  * corr;
        }

        // here preselections
        // >= 4 jets with pt > 40 and |eta| < 2.5

        double jet_min_pt = 40.;
        double jet_max_eta = 2.5;

        if (year == 2016){
            jet_min_pt = 30.;
            jet_max_eta = 2.4;
        }

        std::vector<jet_t> presel_jets;
        for (int ijet = 0; ijet < jets.size(); ++ijet)
        {
            jet_t jet = jets.at(ijet);
            bool pass_id = (jet.jetid & (1 << 1)); // tight WP
            bool pass_pu = (jet.puid & (1 << 1)); // medium WP
            // bool pass_pu = (jet.puid & (1 << 2)); // loose WP
            if (jet.pt > jet_min_pt && std::abs(jet.eta) < jet_max_eta && pass_id && pass_pu){
                presel_jets.push_back(jet);
            }
        }

        if (presel_jets.size() < 4)
            continue;

        // in the 4b phase space, compute the efficiency
        for (auto& jet : presel_jets){
            
            int aflav   = abs(jet.flav);
            double pt   = jet.pt;
            double aeta = std::abs(jet.eta);

            for (uint iWP = 0; iWP < 3; ++iWP){
                bool pass = (jet.deepjet > WPs.at(iWP) );

                if (aflav == 5){
                    h_b_tot.at(iWP)->Fill(pt, aeta);
                    if (pass) h_b_pass.at(iWP)->Fill(pt, aeta);
                }
                else if (aflav == 4){
                    h_c_tot.at(iWP)->Fill(pt, aeta);
                    if (pass) h_c_pass.at(iWP)->Fill(pt, aeta);
                }
                else{
                    h_udsg_tot.at(iWP)->Fill(pt, aeta);
                    if (pass) h_udsg_pass.at(iWP)->Fill(pt, aeta);
                }
            }
        }

    }

    // now compute the efficiencies
    std::vector<TH2D*> h_eff_b(3);
    std::vector<TH2D*> h_eff_c(3);
    std::vector<TH2D*> h_eff_udsg(3);

    for (uint iWP = 0; iWP < 3; ++iWP){
        h_eff_b.at(iWP)     = (TH2D*) h_b_pass.at(iWP)    ->Clone( (string("h_eff_b_")    + WPnames.at(iWP)).c_str() );
        h_eff_c.at(iWP)     = (TH2D*) h_c_pass.at(iWP)    ->Clone( (string("h_eff_c_")    + WPnames.at(iWP)).c_str() );
        h_eff_udsg.at(iWP)  = (TH2D*) h_udsg_pass.at(iWP) ->Clone( (string("h_eff_udsg_") + WPnames.at(iWP)).c_str() );

        h_eff_b.at(iWP)    ->Divide(h_b_tot.at(iWP));
        h_eff_c.at(iWP)    ->Divide(h_c_tot.at(iWP));
        h_eff_udsg.at(iWP) ->Divide(h_udsg_tot.at(iWP));
    }

    fOut->cd();
    for (uint iWP = 0; iWP < 3; ++iWP){
        h_b_pass.at(iWP)    ->Write();
        h_b_tot.at(iWP)     ->Write();
        h_c_pass.at(iWP)    ->Write();
        h_c_tot.at(iWP)     ->Write();
        h_udsg_pass.at(iWP) ->Write();
        h_udsg_tot.at(iWP)  ->Write();
    }
    
    for (uint iWP = 0; iWP < 3; ++iWP){
        h_eff_b.at(iWP)    ->Write();
        h_eff_c.at(iWP)    ->Write();
        h_eff_udsg.at(iWP) ->Write();
    }

    // check if there are empty bins
    for (uint iWP = 0; iWP < 3; ++iWP)
        for (uint ibinx = 1; ibinx < nbins_pt; ++ibinx)
            for (uint ibiny = 1; ibiny < nbins_eta; ++ibiny){
                
                if (h_b_pass.at(iWP)->GetBinContent(ibinx, ibiny) == 0)
                    cout << iWP << " " << ibinx << " " << ibiny << " " << h_b_pass.at(iWP)->GetName() << " is empty" << endl;
                if (h_b_tot.at(iWP)->GetBinContent(ibinx, ibiny) == 0)
                    cout << iWP << " " << ibinx << " " << ibiny << " " << h_b_tot.at(iWP)->GetName() << " is empty" << endl;
                
                if (h_c_pass.at(iWP)->GetBinContent(ibinx, ibiny) == 0)
                    cout << iWP << " " << ibinx << " " << ibiny << " " << h_c_pass.at(iWP)->GetName() << " is empty" << endl;
                if (h_c_tot.at(iWP)->GetBinContent(ibinx, ibiny) == 0)
                    cout << iWP << " " << ibinx << " " << ibiny << " " << h_c_tot.at(iWP)->GetName() << " is empty" << endl;
                
                if (h_udsg_pass.at(iWP)->GetBinContent(ibinx, ibiny) == 0)
                    cout << iWP << " " << ibinx << " " << ibiny << " " << h_udsg_pass.at(iWP)->GetName() << " is empty" << endl;
                if (h_udsg_tot.at(iWP)->GetBinContent(ibinx, ibiny) == 0)
                    cout << iWP << " " << ibinx << " " << ibiny << " " << h_udsg_tot.at(iWP)->GetName() << " is empty" << endl;
            }

    // TH2D* heff_b = new TH2D ("heff_b", "heff_b;pt;abseta", 100, 0, 1000, 10, 0, 2.5);

}