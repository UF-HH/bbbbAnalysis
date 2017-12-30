#include <iostream>

#include "TFile.h"
#include "TChain.h"

#include "Jet.h"

#include "NanoAODTree.h"
#include "SkimUtils.h"

namespace su = SkimUtils;
using namespace std;

// c++ -lm -o test_reader test_reader.cpp `root-config --glibs --cflags` -lTreePlayer -I ../interface

int main(int argc, char** argv)
{
    cout << "... starting program" << endl;

    TChain* ch          = new TChain("Events");
    // TTreeReader* reader = new TTreeReader(ch);

    cout << "... appending file list to chain" << endl;

    // su::appendFromFileList(ch, "/uscms/home/lcadamur/nobackup/NanoAOD/CMSSW_9_4_1/src/bbbbAnalysis/inputFiles/Samples_80X/VBF_HH_4b.txt");
    su::appendFromFileList(ch, "/uscms/home/lcadamur/nobackup/NanoAOD/CMSSW_9_4_1/src/bbbbAnalysis/inputFiles/Samples_80X/debugFileList.txt");

    cout << "... making tree reader" << endl;

    NanoAODTree nat (ch);

    cout << "... loading the triggers" << endl;
    std::vector<std::string> vTriggers = {"HLT_AK8PFJet40", "HLT_AK8PFJet200"};
    nat.triggerReader().setTriggers(vTriggers);

    cout << "... starting loop" << endl;

    int ntot = 0;
    int nPFJet40 = 0;
    int nPFJet200 = 0;
    
    for (uint iEv = 0; true; ++iEv)
    {
        if (!nat.Next()) break;
        if (iEv % 10000 == 0) cout << "... processing event " << iEv << endl;

        ++ntot;
        
        if (nat.triggerReader().getTrgResult("HLT_AK8PFJet40"))
            ++nPFJet40;

        if (nat.triggerReader().getTrgResult("HLT_AK8PFJet200"))
            ++nPFJet200;

        // ----------------
        std::vector<Jet> jets;
        for (uint ij = 0; ij < nat.Jet_pt.GetSize(); ++ij)
        {
            Jet j (ij, &nat);
            jets.push_back(j);
        }

        for (uint ij = 0; ij < nat.Jet_pt.GetSize(); ++ij)
        {
            Jet& jet = jets.at(ij);
            TLorentzVector p4;
            p4.SetPtEtaPhiM(nat.Jet_pt.At(ij), nat.Jet_eta.At(ij), nat.Jet_phi.At(ij), nat.Jet_mass.At(ij));
            cout << ij << " " << jet.getIdx() << " "
                 << p4.Px() << " " << jet.P4().Px() << " "
                 << p4.Py() << " " << jet.P4().Py() << " "
                 << p4.Pz() << " " << jet.P4().Pz() << " "
                 << p4.E() << " " << jet.P4().E() << " "
                 << p4.M() << " " << jet.P4().M() << " "
                 << endl;
        }


        // cout << *(nat->run) << endl;

        // cout << nat->GenJet_hadronFlavour.GetSize() << endl;

        // cout << *(nat->CaloMET_pt) << endl;

        // cout << " ---------------- " << endl;
        // for (uint igj = 0; igj < nat->GenJet_hadronFlavour.GetSize(); ++igj){
        //     cout << " --- " << igj << "-" << (int)(nat->GenJet_hadronFlavour).At(igj) << "-" << std::boolalpha << ((nat->GenJet_hadronFlavour).At(igj) == 5) << endl;
        // }

        // -------> some branches have unsigned char types and thus are not printable ==>> remember to cast them to integers

        // cout << " ---------------- " << endl;
        // for (uint ijet = 0; ijet < nat->Jet_area.GetSize(); ++ijet){
        //     cout << " --- " << ijet << endl;
        //     cout << (nat->Jet_area).At(ijet) << endl;
        // }

        /// ----- gen flags NEEDED!!

        // int nStat23 = 0;
        // int nStat71 = 0;
        // cout << " ------------------------- N GEN PARTS: " << *(nat.nGenPart) << endl;
        // for (uint igp = 0; igp < nat.GenPart_pt.GetSize(); ++igp)
        // {
        //     auto id = abs(nat.GenPart_pdgId.At(igp));
        //     if (id > 4) continue;
        //     if (nat.GenPart_status.At(igp) == 23) ++nStat23;
        //     if (nat.GenPart_status.At(igp) == 71) ++nStat71;
        //     // cout << igp
        //     //      << " pdg: " << setw(5) << nat.GenPart_pdgId.At(igp)
        //     //      << " , status: " << setw(3) << nat.GenPart_status.At(igp)
        //     //      << " , moth id: " << setw(3) << nat.GenPart_genPartIdxMother.At(igp)
        //     //      << " , pt : " << setw(8) << nat.GenPart_pt.At(igp)
        //     //      << " , eta : " << setw(8) << nat.GenPart_eta.At(igp)
        //     //      << " , phi : " << setw(8) << nat.GenPart_phi.At(igp)
        //     //      << endl;
        // }

        // if (nStat23 != 2 || nStat71 != 2)
        // {
        //     cout << "EV " << iEv << " ??? 23 : " << nStat23 << " 71 : " << nStat71 << endl;
        //     for (uint igp = 0; igp < nat.GenPart_pt.GetSize(); ++igp)
        //     {
        //         auto status = nat.GenPart_status.At(igp);
        //         bool highl = (status == 71 || status == 23) && (abs(nat.GenPart_pdgId.At(igp)) <= 4);
        //         cout << igp
        //              << " pdg: " << setw(5) << nat.GenPart_pdgId.At(igp)
        //              << " , status: " << setw(3) << nat.GenPart_status.At(igp)
        //              << " , moth id: " << setw(3) << nat.GenPart_genPartIdxMother.At(igp)
        //              << " , pt : " << setw(8) << nat.GenPart_pt.At(igp)
        //              << " , eta : " << setw(8) << nat.GenPart_eta.At(igp)
        //              << " , phi : " << setw(8) << nat.GenPart_phi.At(igp);
        //         if (highl) cout << " <=== " << status;
        //              cout << endl;
        //     }

        // }

    }

    cout << "ntot : " << ntot << endl;
    cout << "nPFJet40 : " << nPFJet40 << endl;
    cout << "nPFJet200 : " << nPFJet200 << endl;

}