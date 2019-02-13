#include "SkimUtils.h"

#include "TLorentzVector.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// helper: copies the pt/eta/phi/p4 branches from a candidate OBJ to the output tree
// NOTE: requires the matching of the names (and enforces it)
#define COPY_m_pt_eta_phi_p4(OBJ) \
    ot.OBJ ## _m   = ei. OBJ -> P4().M(); \
    ot.OBJ ## _pt  = ei. OBJ -> P4().Pt(); \
    ot.OBJ ## _eta = ei. OBJ -> P4().Eta(); \
    ot.OBJ ## _phi = ei. OBJ -> P4().Phi(); \
    ot.OBJ ## _p4  = ei. OBJ -> P4();

#define COPY_m_pt_ptRegressed_eta_phi_p4(OBJ) \
    ot.OBJ ## _m            = ei. OBJ -> P4().M(); \
    ot.OBJ ## _pt           = ei. OBJ -> P4().Pt(); \
    ot.OBJ ## _ptRegressed  = ei. OBJ -> P4Regressed().Pt(); \
    ot.OBJ ## _eta          = ei. OBJ -> P4().Eta(); \
    ot.OBJ ## _phi          = ei. OBJ -> P4().Phi(); \
    ot.OBJ ## _p4           = ei. OBJ -> P4();

//helperM same as above, but encloses the obj (a boost::optional is expected) in a if clause to check whether it is initialized
#define COPY_OPTIONAL_m_pt_eta_phi_p4(OBJ) \
    if (ei.OBJ) { \
        ot.OBJ ## _m   = ei. OBJ -> P4().M(); \
        ot.OBJ ## _pt  = ei. OBJ -> P4().Pt(); \
        ot.OBJ ## _eta = ei. OBJ -> P4().Eta(); \
        ot.OBJ ## _phi = ei. OBJ -> P4().Phi(); \
        ot.OBJ ## _p4  = ei. OBJ -> P4(); \
    }

#define COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(OBJ) \
    if (ei.OBJ) { \
    ot.OBJ ## _m            = ei. OBJ -> P4().M(); \
    ot.OBJ ## _pt           = ei. OBJ -> P4().Pt(); \
    ot.OBJ ## _ptRegressed  = ei. OBJ -> P4Regressed().Pt(); \
    ot.OBJ ## _eta          = ei. OBJ -> P4().Eta(); \
    ot.OBJ ## _phi          = ei. OBJ -> P4().Phi(); \
    ot.OBJ ## _p4           = ei. OBJ -> P4();\
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

    // set the variables for resonant analysis
    ot.Run  = *ei.Run;
    ot.LumiSec  = *ei.LumiSec;
    ot.Event  = *ei.Event;

    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(H1_b1)
    if (ei.H1_b1) ot.H1_b1_deepCSV = get_property(ei.H1_b1.get(),Jet_btagDeepB);   
    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(H1_b2)
    if (ei.H1_b2) ot.H1_b2_deepCSV = get_property(ei.H1_b2.get(),Jet_btagDeepB);
    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(H2_b1)
    if (ei.H2_b1) ot.H2_b1_deepCSV = get_property(ei.H2_b1.get(),Jet_btagDeepB);
    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(H2_b2)
    if (ei.H2_b2) ot.H2_b2_deepCSV = get_property(ei.H2_b2.get(),Jet_btagDeepB);

    COPY_OPTIONAL_m_pt_eta_phi_p4(H1)
    if(ei.H1_bb_DeltaR) ot.H1_bb_DeltaR  = *ei.H1_bb_DeltaR;
    COPY_OPTIONAL_m_pt_eta_phi_p4(H2)
    if(ei.H2_bb_DeltaR) ot.H2_bb_DeltaR  = *ei.H2_bb_DeltaR;
    COPY_OPTIONAL_m_pt_eta_phi_p4(HH)
    if(ei.HH_2DdeltaM) ot.HH_2DdeltaM  = *ei.HH_2DdeltaM;
    if(ei.HH_m_kinFit) ot.HH_m_kinFit  = *ei.HH_m_kinFit;
 
    //set the variables for non-resonant analysis and studies
    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(HH_b1)
    if(ei.HH_b1)             ot.HH_b1_deepCSV = get_property(ei.HH_b1.get(),Jet_btagDeepB);
    if(ei.HH_b1)             ot.HH_b1_qgl = get_property(ei.HH_b1.get(),Jet_qgl);
    if(ei.HH_b1_quarkflag)   ot.HH_b1_quarkflag = *ei.HH_b1_quarkflag;
    if(ei.HH_b1_matchedflag) ot.HH_b1_matchedflag  = *ei.HH_b1_matchedflag;
    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(HH_b2)
    if(ei.HH_b2)                ot.HH_b2_deepCSV = get_property(ei.HH_b2.get(),Jet_btagDeepB);
    if(ei.HH_b2)                ot.HH_b2_qgl = get_property(ei.HH_b2.get(),Jet_qgl);
    if(ei.HH_b2_quarkflag)      ot.HH_b2_quarkflag = *ei.HH_b2_quarkflag ;
    if(ei.HH_b2_matchedflag)    ot.HH_b2_matchedflag  = *ei.HH_b2_matchedflag ;
    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(HH_b3)
    if(ei.HH_b3)                ot.HH_b3_deepCSV = get_property(ei.HH_b3.get(),Jet_btagDeepB);
    if(ei.HH_b3)                ot.HH_b3_qgl = get_property(ei.HH_b3.get(),Jet_qgl);
    if(ei.HH_b3_quarkflag)      ot.HH_b3_quarkflag = *ei.HH_b3_quarkflag ;
    if(ei.HH_b3_matchedflag)    ot.HH_b3_matchedflag  = *ei.HH_b3_matchedflag ;
    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(HH_b4)
    if(ei.HH_b4)                ot.HH_b4_deepCSV = get_property(ei.HH_b4.get(),Jet_btagDeepB);
    if(ei.HH_b4)                ot.HH_b4_qgl = get_property(ei.HH_b4.get(),Jet_qgl);
    if(ei.HH_b4_quarkflag)      ot.HH_b4_quarkflag = *ei.HH_b4_quarkflag ;
    if(ei.HH_b4_matchedflag)    ot.HH_b4_matchedflag  = *ei.HH_b4_matchedflag ;
    COPY_OPTIONAL_m_pt_eta_phi_p4(JJ_j1)
    if(ei.JJ_j1)                ot.JJ_j1_deepCSV = get_property(ei.JJ_j1.get(),Jet_btagDeepB);
    if(ei.JJ_j1)                ot.JJ_j1_qgl = get_property(ei.JJ_j1.get(),Jet_qgl);
    if(ei.JJ_j1_quarkflag)      ot.JJ_j1_quarkflag  = *ei.JJ_j1_quarkflag;
    if(ei.JJ_j1_matchedflag)    ot.JJ_j1_matchedflag  = *ei.JJ_j1_matchedflag;
    COPY_OPTIONAL_m_pt_eta_phi_p4(JJ_j2)
    if(ei.JJ_j2)                ot.JJ_j2_deepCSV = get_property(ei.JJ_j2.get(),Jet_btagDeepB);
    if(ei.JJ_j2)                ot.JJ_j2_qgl = get_property(ei.JJ_j2.get(),Jet_qgl);
    if(ei.JJ_j2_quarkflag)      ot.JJ_j2_quarkflag  = *ei.JJ_j2_quarkflag;
    if(ei.JJ_j2_matchedflag)    ot.JJ_j2_matchedflag  = *ei.JJ_j2_matchedflag;    
    COPY_OPTIONAL_m_pt_eta_phi_p4(JJ)
    if(ei.JJ_deltaEta) ot.JJ_deltaEta  = *ei.JJ_deltaEta;
    if(ei.b1j1_deltaPhi) ot.b1j1_deltaPhi  = *ei.b1j1_deltaPhi;
    if(ei.b1b2_deltaPhi) ot.b1b2_deltaPhi  = *ei.b1b2_deltaPhi;
    if(ei.VBFEvent) ot.VBFEvent  = *ei.VBFEvent;
    // gen info are not stored for all samples --> set only if initialized (macro checks if object is initialized, else does not set)
    COPY_OPTIONAL_m_pt_eta_phi_p4(gen_H1)
    COPY_OPTIONAL_m_pt_eta_phi_p4(gen_H2)
    COPY_OPTIONAL_m_pt_eta_phi_p4(gen_H1_last)
    COPY_OPTIONAL_m_pt_eta_phi_p4(gen_H2_last)

    if (ei.gen_H1 && ei.gen_H2)
    {
        TLorentzVector p4_HH = ei.gen_H1->P4() + ei.gen_H2->P4();
        ot.gen_mHH = p4_HH.M();
    }

    COPY_OPTIONAL_m_pt_eta_phi_p4(gen_H1_b1)
    COPY_OPTIONAL_m_pt_eta_phi_p4(gen_H1_b2)
    COPY_OPTIONAL_m_pt_eta_phi_p4(gen_H2_b1)
    COPY_OPTIONAL_m_pt_eta_phi_p4(gen_H2_b2)
    COPY_OPTIONAL_m_pt_eta_phi_p4(gen_q1_in)
    COPY_OPTIONAL_m_pt_eta_phi_p4(gen_q2_in)
    COPY_OPTIONAL_m_pt_eta_phi_p4(gen_q1_out)
    COPY_OPTIONAL_m_pt_eta_phi_p4(gen_q2_out)

    if (ei.gen_q1_out && ei.gen_q2_out)
    {
        TLorentzVector p4_JJ = ei.gen_q1_out->P4() + ei.gen_q2_out->P4();
        ot.gen_mJJ = p4_JJ.M();
    }    

    if (ei.gen_q1_out && ei.gen_q2_out)
    {
        ot.gen_deltaEtaJJ = abs( ei.gen_q1_out->P4().Eta() - ei.gen_q2_out->P4().Eta() );
    }    

    // fill the tree
    ot.fill();
}