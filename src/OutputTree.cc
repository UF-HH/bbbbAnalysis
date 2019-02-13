#include "OutputTree.h"

#include <iostream>

using namespace std;

// helper: creates the pt/eta/phi/p4 branches of a variable OBJ
#define BRANCH_m_pt_eta_phi_p4(OBJ) \
    tree_->Branch(#OBJ "_m",  &OBJ ## _m); \
    tree_->Branch(#OBJ "_pt",  &OBJ ## _pt); \
    tree_->Branch(#OBJ "_eta", &OBJ ## _eta); \
    tree_->Branch(#OBJ "_phi", &OBJ ## _phi); \
    if (savetlv_) tree_->Branch(#OBJ "_p4", &OBJ ## _p4);

#define CLEAR_m_pt_eta_phi_p4(OBJ) \
    OBJ ## _m    = -999.; \
    OBJ ## _pt   = -999.; \
    OBJ ## _eta  = -999.; \
    OBJ ## _phi  = -999.; \
    OBJ ## _p4 . SetPxPyPzE(0,0,0,0);

#define BRANCH_m_pt_ptRegressed_eta_phi_p4(OBJ) \
    tree_->Branch(#OBJ "_m"          ,  &OBJ ## _m); \
    tree_->Branch(#OBJ "_pt"         ,  &OBJ ## _pt); \
    tree_->Branch(#OBJ "_ptRegressed",  &OBJ ## _ptRegressed); \
    tree_->Branch(#OBJ "_eta"        , &OBJ ## _eta); \
    tree_->Branch(#OBJ "_phi"        , &OBJ ## _phi); \
    if (savetlv_) tree_->Branch(#OBJ "_p4", &OBJ ## _p4);

#define CLEAR_m_pt_ptRegressed_eta_phi_p4(OBJ) \
    OBJ ## _m             = -999.; \
    OBJ ## _pt            = -999.; \
    OBJ ## _ptRegressed   = -999.; \
    OBJ ## _eta           = -999.; \
    OBJ ## _phi           = -999.; \
    OBJ ## _p4            . SetPxPyPzE(0,0,0,0);

OutputTree::OutputTree (bool savetlv, string name, string title) :
savetlv_ (savetlv)
{
    tree_ = std::unique_ptr<TTree> (new TTree(name.c_str(), title.c_str()));
    
    init_branches();
    clear();
}

void OutputTree::init_branches()
{

    //event information
    tree_->Branch("Run", &Run);
    tree_->Branch("LumiSec", &LumiSec);
    tree_->Branch("Event", &Event);
    // reco b jets
    BRANCH_m_pt_ptRegressed_eta_phi_p4(H1_b1)
    tree_->Branch("H1_b1_deepCSV", &H1_b1_deepCSV);
    BRANCH_m_pt_ptRegressed_eta_phi_p4(H1_b2)
    tree_->Branch("H1_b2_deepCSV", &H1_b2_deepCSV);
    BRANCH_m_pt_ptRegressed_eta_phi_p4(H2_b1)
    tree_->Branch("H2_b1_deepCSV", &H2_b1_deepCSV);
    BRANCH_m_pt_ptRegressed_eta_phi_p4(H2_b2)
    tree_->Branch("H2_b2_deepCSV", &H2_b2_deepCSV);

    // reco H1 (H1_b1 + H1_b2)
    // reco H2 (H2_b1 + H2_b2)
    BRANCH_m_pt_eta_phi_p4(H1)
    tree_->Branch("H1_bb_DeltaR", &H1_bb_DeltaR);
    BRANCH_m_pt_eta_phi_p4(H2)
    tree_->Branch("H2_bb_DeltaR", &H2_bb_DeltaR);
    BRANCH_m_pt_eta_phi_p4(HH)
    tree_->Branch("HH_2DdeltaM", &HH_2DdeltaM);
    tree_->Branch("HH_m_kinFit", &HH_m_kinFit);

    // Non-resonant analysis and studies
    BRANCH_m_pt_ptRegressed_eta_phi_p4(HH_b1)
    tree_->Branch("HH_b1_deepCSV", &HH_b1_deepCSV);
    tree_->Branch("HH_b1_qgl", &HH_b1_qgl);
    tree_->Branch("HH_b1_quarkflag", &HH_b1_quarkflag);
    tree_->Branch("HH_b1_matchedflag", &HH_b1_matchedflag);    
    BRANCH_m_pt_ptRegressed_eta_phi_p4(HH_b2)
    tree_->Branch("HH_b2_deepCSV", &HH_b2_deepCSV);
    tree_->Branch("HH_b2_qgl", &HH_b2_qgl);    
    tree_->Branch("HH_b2_quarkflag", &HH_b2_quarkflag);
    tree_->Branch("HH_b2_matchedflag", &HH_b2_matchedflag);  
    BRANCH_m_pt_ptRegressed_eta_phi_p4(HH_b3)
    tree_->Branch("HH_b3_deepCSV", &HH_b3_deepCSV);
    tree_->Branch("HH_b3_qgl", &HH_b3_qgl);
    tree_->Branch("HH_b3_quarkflag", &HH_b3_quarkflag);
    tree_->Branch("HH_b3_matchedflag", &HH_b3_matchedflag);
    BRANCH_m_pt_ptRegressed_eta_phi_p4(HH_b4)
    tree_->Branch("HH_b4_deepCSV", &HH_b4_deepCSV);
    tree_->Branch("HH_b4_qgl", &HH_b4_qgl);
    tree_->Branch("HH_b4_quarkflag", &HH_b4_quarkflag);
    tree_->Branch("HH_b4_matchedflag", &HH_b4_matchedflag);     
    BRANCH_m_pt_eta_phi_p4(JJ_j1)
    tree_->Branch("JJ_j1_deepCSV", &JJ_j1_deepCSV);
    tree_->Branch("JJ_j1_qgl", &JJ_j1_qgl);    
    tree_->Branch("JJ_j1_quarkflag", &JJ_j1_quarkflag);    
    tree_->Branch("JJ_j1_matchedflag", &JJ_j1_matchedflag); 
    BRANCH_m_pt_eta_phi_p4(JJ_j2)
    tree_->Branch("JJ_j2_deepCSV", &JJ_j2_deepCSV);
    tree_->Branch("JJ_j2_qgl", &JJ_j2_qgl);     
    tree_->Branch("JJ_j2_quarkflag", &JJ_j2_quarkflag);    
    tree_->Branch("JJ_j2_matchedflag", &JJ_j2_matchedflag); 
    BRANCH_m_pt_eta_phi_p4(JJ)
    tree_->Branch("JJ_deltaEta", &JJ_deltaEta); 
    tree_->Branch("b1j1_deltaPhi", &b1j1_deltaPhi);
    tree_->Branch("b1b2_deltaPhi", &b1b2_deltaPhi);
    tree_->Branch("VBFEvent", &VBFEvent);              
    //generator level information
    tree_->Branch("gen_mJJ", &gen_mJJ);
    tree_->Branch("gen_deltaEtaJJ", &gen_deltaEtaJJ);
    tree_->Branch("gen_mHH", &gen_mHH);
    BRANCH_m_pt_eta_phi_p4(gen_H1)
    BRANCH_m_pt_eta_phi_p4(gen_H2)
    BRANCH_m_pt_eta_phi_p4(gen_H1_last)
    BRANCH_m_pt_eta_phi_p4(gen_H2_last)
    BRANCH_m_pt_eta_phi_p4(gen_H1_b1)
    BRANCH_m_pt_eta_phi_p4(gen_H1_b2)
    BRANCH_m_pt_eta_phi_p4(gen_H2_b1)
    BRANCH_m_pt_eta_phi_p4(gen_H2_b2)
    BRANCH_m_pt_eta_phi_p4(gen_q1_in)
    BRANCH_m_pt_eta_phi_p4(gen_q2_in)
    BRANCH_m_pt_eta_phi_p4(gen_q1_out)
    BRANCH_m_pt_eta_phi_p4(gen_q2_out)

    // note that the initialization of the user branches is made separately when calling declareUser*Branch

}

void OutputTree::clear()
{
    Run=0;
    LumiSec=0;
    Event=0;

    CLEAR_m_pt_ptRegressed_eta_phi_p4(H1_b1)
    H1_b1_deepCSV = 0.;
    CLEAR_m_pt_ptRegressed_eta_phi_p4(H1_b2)
    H1_b2_deepCSV = 0.;
    CLEAR_m_pt_ptRegressed_eta_phi_p4(H2_b1)
    H2_b1_deepCSV = 0.;
    CLEAR_m_pt_ptRegressed_eta_phi_p4(H2_b2)
    H2_b2_deepCSV = 0.;

    CLEAR_m_pt_eta_phi_p4(H1)
    H1_bb_DeltaR = 0.;
    CLEAR_m_pt_eta_phi_p4(H2)
    H2_bb_DeltaR = 0.;
    CLEAR_m_pt_eta_phi_p4(HH)
    HH_2DdeltaM = 0.;
    HH_m_kinFit = 0.;

    //Non-resonant analysis and studies
    CLEAR_m_pt_ptRegressed_eta_phi_p4(HH_b1)
    HH_b1_deepCSV = 0;
    HH_b1_qgl = -1;
    HH_b1_matchedflag = -1;
    HH_b1_quarkflag = -1;    
    CLEAR_m_pt_ptRegressed_eta_phi_p4(HH_b2)
    HH_b2_deepCSV = 0;
    HH_b2_qgl = -1;   
    HH_b2_matchedflag = -1;
    HH_b2_quarkflag = -1; 
    CLEAR_m_pt_ptRegressed_eta_phi_p4(HH_b3)
    HH_b3_deepCSV = 0;
    HH_b3_qgl = -1;
    HH_b3_matchedflag = -1;
    HH_b3_quarkflag = -1; 
    CLEAR_m_pt_ptRegressed_eta_phi_p4(HH_b4)
    HH_b4_deepCSV = 0;
    HH_b4_qgl = -1;
    HH_b4_matchedflag = -1;
    HH_b4_quarkflag = -1;   
    CLEAR_m_pt_eta_phi_p4(JJ_j1)
    JJ_j1_deepCSV = 0.;
    JJ_j1_qgl = -1;
    JJ_j1_matchedflag = -1;  
    JJ_j1_quarkflag = -1;
    CLEAR_m_pt_eta_phi_p4(JJ_j2)
    JJ_j2_deepCSV = 0.;    
    JJ_j2_qgl = -1;
    JJ_j2_matchedflag = -1;  
    JJ_j2_quarkflag = -1;
    CLEAR_m_pt_eta_phi_p4(JJ)
    JJ_deltaEta = 0.;
    b1j1_deltaPhi = 0.;
    b1b2_deltaPhi = 0.;
    VBFEvent =0.;

    //generator level information
    gen_mHH = 0.;    
    gen_mJJ = 0.;    
    gen_deltaEtaJJ = 0.;
    CLEAR_m_pt_eta_phi_p4(gen_H1)
    CLEAR_m_pt_eta_phi_p4(gen_H2)
    CLEAR_m_pt_eta_phi_p4(gen_H1_last)
    CLEAR_m_pt_eta_phi_p4(gen_H2_last)
    CLEAR_m_pt_eta_phi_p4(gen_H1_b1)
    CLEAR_m_pt_eta_phi_p4(gen_H1_b2)
    CLEAR_m_pt_eta_phi_p4(gen_H2_b1)
    CLEAR_m_pt_eta_phi_p4(gen_H2_b2)
    CLEAR_m_pt_eta_phi_p4(gen_q1_in)
    CLEAR_m_pt_eta_phi_p4(gen_q2_in)
    CLEAR_m_pt_eta_phi_p4(gen_q1_out)
    CLEAR_m_pt_eta_phi_p4(gen_q2_out)

    userFloats_.resetAll();
    userInts_.resetAll();

}

bool OutputTree::declareUserIntBranch (std::string name, int defaultClearValue)
{
    // check if the branch exists -- the check in the same collection is done by UserVal internally, but I have to do the cross-checks
    if (userFloats_.hasVal(name)){
        cout << "[WARNING] OutputTree : declareUserIntBranch : branch " << name << " was already found as a userFloat, cannot create it" << endl;
        return false;
    }
    
    if (!userInts_.addVal(name, defaultClearValue)){
        cout << "[WARNING] OutputTree : declareUserIntBranch : branch " << name << " was already found as a userInt, cannot create it" << endl;
        return false;
    }

    // set the branch
    tree_->Branch(name.c_str(), userInts_.getValPtr(name));
    return true;
}

bool OutputTree::declareUserFloatBranch (std::string name, float defaultClearValue)
{
    // check if the branch exists -- the check in the same collection is done by UserVal internally, but I have to do the cross-checks
    if (userInts_.hasVal(name)){
        cout << "[WARNING] OutputTree : declareUserFloatBranch : branch " << name << " was already found as a userInt, cannot create it" << endl;
        return false;
    }
    
    if (!userFloats_.addVal(name, defaultClearValue)){
        cout << "[WARNING] OutputTree : declareUserFloatBranch : branch " << name << " was already found as a userFloat, cannot create it" << endl;
        return false;
    }

    // set the branch
    tree_->Branch(name.c_str(), userFloats_.getValPtr(name));
    return true;
}