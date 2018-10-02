#include "SkimUtils.h"

#include "TLorentzVector.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// helper: copies the pt/eta/phi/p4 branches from a candidate OBJ to the output tree
// NOTE: requires the matching of the names (and enforces it)
#define COPY_pt_eta_phi_p4(OBJ) \
    ot.OBJ ## _m   = ei. OBJ -> P4().M(); \
    ot.OBJ ## _pt  = ei. OBJ -> P4().Pt(); \
    ot.OBJ ## _eta = ei. OBJ -> P4().Eta(); \
    ot.OBJ ## _phi = ei. OBJ -> P4().Phi(); \
    ot.OBJ ## _p4  = ei. OBJ -> P4();

//helperM same as above, but encloses the obj (a boost::optional is expected) in a if clause to check whether it is initialized
#define COPY_OPTIONAL_pt_eta_phi_p4(OBJ) \
    if (ei.OBJ) { \
        ot.OBJ ## _m   = ei. OBJ -> P4().M(); \
        ot.OBJ ## _pt  = ei. OBJ -> P4().Pt(); \
        ot.OBJ ## _eta = ei. OBJ -> P4().Eta(); \
        ot.OBJ ## _phi = ei. OBJ -> P4().Phi(); \
        ot.OBJ ## _p4  = ei. OBJ -> P4(); \
    }

// --- - --- - --- - --- - --- - --- - --- - --- - --- - --- - --- - --- - 

int SkimUtils::appendFromFileList (TChain* chain, string filename)
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


void SkimUtils::fill_output_tree(OutputTree& ot, NanoAODTree& nat, EventInfo& ei)
{
    // set the variables
    COPY_pt_eta_phi_p4(H1_b1)
    COPY_pt_eta_phi_p4(H1_b2)
    COPY_pt_eta_phi_p4(H2_b1)
    COPY_pt_eta_phi_p4(H2_b2)

    COPY_pt_eta_phi_p4(H1)
    ot.H1_bb_DeltaR  = *ei.H1_bb_DeltaR;
    COPY_pt_eta_phi_p4(H2)
    ot.H2_bb_DeltaR  = *ei.H2_bb_DeltaR;
    COPY_pt_eta_phi_p4(HH)
    ot.HH_2DdeltaM  = *ei.HH_2DdeltaM;
    
    // gen info are not stored for all samples --> set only if initialized (macro checks if object is initialized, else does not set)
    COPY_OPTIONAL_pt_eta_phi_p4(gen_H1)
    COPY_OPTIONAL_pt_eta_phi_p4(gen_H2)
    COPY_OPTIONAL_pt_eta_phi_p4(gen_H1_last)
    COPY_OPTIONAL_pt_eta_phi_p4(gen_H2_last)

    if (ei.gen_H1 && ei.gen_H2)
    {
        TLorentzVector p4_HH = ei.gen_H1->P4() + ei.gen_H2->P4();
        ot.gen_mHH = p4_HH.M();
    }

    COPY_OPTIONAL_pt_eta_phi_p4(gen_H1_b1)
    COPY_OPTIONAL_pt_eta_phi_p4(gen_H1_b2)
    COPY_OPTIONAL_pt_eta_phi_p4(gen_H2_b1)
    COPY_OPTIONAL_pt_eta_phi_p4(gen_H2_b2)
    COPY_OPTIONAL_pt_eta_phi_p4(gen_q1_in)
    COPY_OPTIONAL_pt_eta_phi_p4(gen_q2_in)
    COPY_OPTIONAL_pt_eta_phi_p4(gen_q1_out)
    COPY_OPTIONAL_pt_eta_phi_p4(gen_q2_out)

    // fill the tree
    ot.fill();
}