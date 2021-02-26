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
    //TriggerSF
    tree_->Branch("triggerScaleFactor"       , &triggerScaleFactor       );
    tree_->Branch("triggerDataEfficiency"    , &triggerDataEfficiency    );
    tree_->Branch("triggerMcEfficiency"      , &triggerMcEfficiency      );
    tree_->Branch("triggerScaleFactorUp"     , &triggerScaleFactorUp     );
    tree_->Branch("triggerDataEfficiencyUp"  , &triggerDataEfficiencyUp  );
    tree_->Branch("triggerMcEfficiencyUp"    , &triggerMcEfficiencyUp    );
    tree_->Branch("triggerScaleFactorDown"   , &triggerScaleFactorDown   );
    tree_->Branch("triggerDataEfficiencyDown", &triggerDataEfficiencyDown);
    tree_->Branch("triggerMcEfficiencyDown"  , &triggerMcEfficiencyDown  );

    // reco b jets
    BRANCH_m_pt_ptRegressed_eta_phi_p4(H1_b1)
    tree_->Branch("H1_b1_deepCSV", &H1_b1_deepCSV);
    tree_->Branch("H1_b1_deepJet", &H1_b1_deepJet);
    tree_->Branch("H1_b1_bRegRes", &H1_b1_bRegRes);
    tree_->Branch("H1_b1_puId", &H1_b1_puId);
    tree_->Branch("H1_b1_jetId", &H1_b1_jetId);
    tree_->Branch("H1_b1_rawpt", &H1_b1_rawpt);
    tree_->Branch("H1_b1_qgl", &H1_b1_qgl);   
    tree_->Branch("H1_b1_genJetIdx", &HH_b1_genJetIdx);
    tree_->Branch("H1_b1_partonFlavour", &H1_b1_partonFlavour);    
    tree_->Branch("H1_b1_hadronFlavour", &H1_b1_hadronFlavour);   
    BRANCH_m_pt_ptRegressed_eta_phi_p4(H1_b2)
    tree_->Branch("H1_b2_deepCSV", &H1_b2_deepCSV);
    tree_->Branch("H1_b2_deepJet", &H1_b2_deepJet);
    tree_->Branch("H1_b2_bRegRes", &H1_b2_bRegRes);
    tree_->Branch("H1_b2_puId", &H1_b2_puId);
    tree_->Branch("H1_b2_jetId", &H1_b2_jetId);
    tree_->Branch("H1_b2_rawpt", &H1_b2_rawpt);
    tree_->Branch("H1_b2_qgl", &H1_b2_qgl);   
    tree_->Branch("H1_b2_genJetIdx", &H1_b2_genJetIdx);
    tree_->Branch("H1_b2_partonFlavour", &H1_b2_partonFlavour);
    tree_->Branch("H1_b2_hadronFlavour", &H1_b2_hadronFlavour);         
    BRANCH_m_pt_ptRegressed_eta_phi_p4(H2_b1)
    tree_->Branch("H2_b1_deepCSV", &H2_b1_deepCSV);
    tree_->Branch("H2_b1_deepJet", &H2_b1_deepJet);
    tree_->Branch("H2_b1_bRegRes", &H2_b1_bRegRes);
    tree_->Branch("H2_b1_puId", &H2_b1_puId);
    tree_->Branch("H2_b1_jetId", &H2_b1_jetId);
    tree_->Branch("H2_b1_rawpt", &H2_b1_rawpt);
    tree_->Branch("H2_b1_qgl", &H2_b1_qgl);   
    tree_->Branch("H2_b1_genJetIdx", &H2_b1_genJetIdx);
    tree_->Branch("H2_b1_partonFlavour", &H2_b1_partonFlavour);
    tree_->Branch("H2_b1_hadronFlavour", &H2_b1_hadronFlavour); 
    BRANCH_m_pt_ptRegressed_eta_phi_p4(H2_b2)
    tree_->Branch("H2_b2_deepCSV", &H2_b2_deepCSV);
    tree_->Branch("H2_b2_deepJet", &H2_b2_deepJet);    
    tree_->Branch("H2_b2_bRegRes", &H2_b2_bRegRes);
    tree_->Branch("H2_b2_puId", &H2_b2_puId);
    tree_->Branch("H2_b2_jetId", &H2_b2_jetId);
    tree_->Branch("H2_b2_rawpt", &H2_b2_rawpt);
    tree_->Branch("H2_b2_qgl", &H2_b2_qgl);   
    tree_->Branch("H2_b2_genJetIdx", &H2_b2_genJetIdx);
    tree_->Branch("H2_b2_partonFlavour", &H2_b2_partonFlavour);
    tree_->Branch("H2_b2_hadronFlavour", &H2_b2_hadronFlavour);
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
    tree_->Branch("btaggerID", &btaggerID);
    tree_->Branch("EventCount", &EventCount);
    BRANCH_m_pt_eta_phi_p4(H1unregressed)
    BRANCH_m_pt_eta_phi_p4(H2unregressed)
    BRANCH_m_pt_eta_phi_p4(HHunregressed)
    BRANCH_m_pt_ptRegressed_eta_phi_p4(HH_b1)
    tree_->Branch("HH_b1_deepCSV", &HH_b1_deepCSV);
    tree_->Branch("HH_b1_deepJet", &HH_b1_deepJet);
    tree_->Branch("HH_b1_bRegRes", &HH_b1_bRegRes);
    tree_->Branch("HH_b1_puId", &HH_b1_puId);
    tree_->Branch("HH_b1_jetId", &HH_b1_jetId);
    tree_->Branch("HH_b1_rawpt", &HH_b1_rawpt);
    tree_->Branch("HH_b1_qgl", &HH_b1_qgl);   
    tree_->Branch("HH_b1_genJetIdx", &HH_b1_genJetIdx);
    tree_->Branch("HH_b1_partonFlavour", &HH_b1_partonFlavour);
    tree_->Branch("HH_b1_hadronFlavour", &HH_b1_hadronFlavour);
    BRANCH_m_pt_ptRegressed_eta_phi_p4(HH_b2)
    tree_->Branch("HH_b2_deepCSV", &HH_b2_deepCSV);
    tree_->Branch("HH_b2_deepJet", &HH_b2_deepJet);
    tree_->Branch("HH_b2_bRegRes", &HH_b2_bRegRes);
    tree_->Branch("HH_b2_puId", &HH_b2_puId);
    tree_->Branch("HH_b2_jetId", &HH_b2_jetId);
    tree_->Branch("HH_b2_rawpt", &HH_b2_rawpt);
    tree_->Branch("HH_b2_qgl", &HH_b2_qgl);     
    tree_->Branch("HH_b2_genJetIdx", &HH_b2_genJetIdx);
    tree_->Branch("HH_b2_partonFlavour", &HH_b2_partonFlavour); 
    tree_->Branch("HH_b2_hadronFlavour", &HH_b2_hadronFlavour);       
    BRANCH_m_pt_ptRegressed_eta_phi_p4(HH_b3)
    tree_->Branch("HH_b3_deepCSV", &HH_b3_deepCSV);
    tree_->Branch("HH_b3_deepJet", &HH_b3_deepJet);
    tree_->Branch("HH_b3_bRegRes", &HH_b3_bRegRes);
    tree_->Branch("HH_b3_puId", &HH_b3_puId);
    tree_->Branch("HH_b3_jetId", &HH_b3_jetId);
    tree_->Branch("HH_b3_rawpt", &HH_b3_rawpt);
    tree_->Branch("HH_b3_qgl", &HH_b3_qgl);
    tree_->Branch("HH_b3_genJetIdx", &HH_b3_genJetIdx);
    tree_->Branch("HH_b3_partonFlavour", &HH_b3_partonFlavour);
    tree_->Branch("HH_b3_hadronFlavour", &HH_b3_hadronFlavour);
    BRANCH_m_pt_ptRegressed_eta_phi_p4(HH_b4)
    tree_->Branch("HH_b4_deepCSV", &HH_b4_deepCSV);
    tree_->Branch("HH_b4_deepJet", &HH_b4_deepJet);
    tree_->Branch("HH_b4_bRegRes", &HH_b4_bRegRes);
    tree_->Branch("HH_b4_puId", &HH_b4_puId);
    tree_->Branch("HH_b4_jetId", &HH_b4_jetId);
    tree_->Branch("HH_b4_rawpt", &HH_b4_rawpt);
    tree_->Branch("HH_b4_qgl", &HH_b4_qgl);
    tree_->Branch("HH_b4_genJetIdx", &HH_b4_genJetIdx);        
    tree_->Branch("HH_b4_partonFlavour", &HH_b4_partonFlavour);
    tree_->Branch("HH_b4_hadronFlavour", &HH_b4_hadronFlavour);
    BRANCH_m_pt_ptRegressed_eta_phi_p4(HH_btag_b1)
    tree_->Branch("HH_btag_b1_bscore", &HH_btag_b1_bscore);       
    tree_->Branch("HH_btag_b1_bres",   &HH_btag_b1_bres);
    BRANCH_m_pt_ptRegressed_eta_phi_p4(HH_btag_b2)
    tree_->Branch("HH_btag_b2_bscore", &HH_btag_b2_bscore);       
    tree_->Branch("HH_btag_b2_bres",   &HH_btag_b2_bres); 
    BRANCH_m_pt_ptRegressed_eta_phi_p4(HH_btag_b3)
    tree_->Branch("HH_btag_b3_bscore", &HH_btag_b3_bscore);       
    tree_->Branch("HH_btag_b3_bres",   &HH_btag_b3_bres);
    BRANCH_m_pt_ptRegressed_eta_phi_p4(HH_btag_b4)
    tree_->Branch("HH_btag_b4_bscore", &HH_btag_b4_bscore);       
    tree_->Branch("HH_btag_b4_bres",   &HH_btag_b4_bres);
    BRANCH_m_pt_eta_phi_p4(JJ_j1)
    tree_->Branch("JJ_j1_deepCSV", &JJ_j1_deepCSV);
    tree_->Branch("JJ_j1_deepJet", &JJ_j1_deepJet);
    tree_->Branch("JJ_j1_puId", &JJ_j1_puId);
    tree_->Branch("JJ_j1_jetId", &JJ_j1_jetId);
    tree_->Branch("JJ_j1_rawpt", &JJ_j1_rawpt);
    tree_->Branch("JJ_j1_qgl", &JJ_j1_qgl);       
    tree_->Branch("JJ_j1_genJetIdx", &JJ_j1_genJetIdx);
    tree_->Branch("JJ_j1_partonFlavour", &JJ_j1_partonFlavour);
    tree_->Branch("JJ_j1_hadronFlavour", &JJ_j1_hadronFlavour);    
    tree_->Branch("JJ_j1_location", &JJ_j1_location);
    BRANCH_m_pt_eta_phi_p4(JJ_j2)
    tree_->Branch("JJ_j2_deepCSV", &JJ_j2_deepCSV);
    tree_->Branch("JJ_j2_deepJet", &JJ_j2_deepJet);
    tree_->Branch("JJ_j2_puId", &JJ_j2_puId);
    tree_->Branch("JJ_j2_jetId", &JJ_j2_jetId);
    tree_->Branch("JJ_j2_rawpt", &JJ_j2_rawpt);
    tree_->Branch("JJ_j2_qgl", &JJ_j2_qgl);     
    tree_->Branch("JJ_j2_genJetIdx", &JJ_j2_genJetIdx); 
    tree_->Branch("JJ_j2_partonFlavour", &JJ_j2_partonFlavour);
    tree_->Branch("JJ_j2_hadronFlavour", &JJ_j2_hadronFlavour);
    tree_->Branch("JJ_j2_location", &JJ_j2_location);
    BRANCH_m_pt_eta_phi_p4(JJ)
    tree_->Branch("b1b2_deltaR",&b1b2_deltaR); 
    tree_->Branch("b1b3_deltaR",&b1b3_deltaR); 
    tree_->Branch("b1b4_deltaR",&b1b4_deltaR); 
    tree_->Branch("b1j1_deltaR",&b1j1_deltaR); 
    tree_->Branch("b1j2_deltaR",&b1j2_deltaR); 
    tree_->Branch("b2b3_deltaR",&b2b3_deltaR); 
    tree_->Branch("b2b4_deltaR",&b2b4_deltaR); 
    tree_->Branch("b2j1_deltaR",&b2j1_deltaR); 
    tree_->Branch("b2j2_deltaR",&b2j2_deltaR); 
    tree_->Branch("b3b4_deltaR",&b3b4_deltaR); 
    tree_->Branch("b3j1_deltaR",&b3j1_deltaR); 
    tree_->Branch("b3j2_deltaR",&b3j2_deltaR); 
    tree_->Branch("b4j1_deltaR",&b4j1_deltaR); 
    tree_->Branch("b4j2_deltaR",&b4j2_deltaR); 
    tree_->Branch("j1j2_deltaR",&j1j2_deltaR); 
    tree_->Branch("h1h2_deltaR",&h1h2_deltaR); 
    tree_->Branch("h1j1_deltaR",&h1j1_deltaR); 
    tree_->Branch("h1j2_deltaR",&h1j2_deltaR); 
    tree_->Branch("h2j1_deltaR",&h2j1_deltaR); 
    tree_->Branch("h2j2_deltaR",&h2j2_deltaR); 
    tree_->Branch("h1jj_deltaR",&h1jj_deltaR); 
    tree_->Branch("h2jj_deltaR",&h2jj_deltaR); 
    tree_->Branch("hhj1_deltaR",&hhj1_deltaR); 
    tree_->Branch("hhj2_deltaR",&hhj2_deltaR); 
    tree_->Branch("hhjj_deltaR",&hhjj_deltaR); 
    tree_->Branch("b1b2_deltaPhi",&b1b2_deltaPhi); 
    tree_->Branch("b1b3_deltaPhi",&b1b3_deltaPhi); 
    tree_->Branch("b1b4_deltaPhi",&b1b4_deltaPhi); 
    tree_->Branch("b1j1_deltaPhi",&b1j1_deltaPhi); 
    tree_->Branch("b1j2_deltaPhi",&b1j2_deltaPhi); 
    tree_->Branch("b2b3_deltaPhi",&b2b3_deltaPhi); 
    tree_->Branch("b2b4_deltaPhi",&b2b4_deltaPhi); 
    tree_->Branch("b2j1_deltaPhi",&b2j1_deltaPhi); 
    tree_->Branch("b2j2_deltaPhi",&b2j2_deltaPhi); 
    tree_->Branch("b3b4_deltaPhi",&b3b4_deltaPhi); 
    tree_->Branch("b3j1_deltaPhi",&b3j1_deltaPhi); 
    tree_->Branch("b3j2_deltaPhi",&b3j2_deltaPhi); 
    tree_->Branch("b4j1_deltaPhi",&b4j1_deltaPhi); 
    tree_->Branch("b4j2_deltaPhi",&b4j2_deltaPhi); 
    tree_->Branch("j1j2_deltaPhi",&j1j2_deltaPhi); 
    tree_->Branch("b1b2_deltaEta",&b1b2_deltaEta); 
    tree_->Branch("b1b3_deltaEta",&b1b3_deltaEta); 
    tree_->Branch("b1b4_deltaEta",&b1b4_deltaEta); 
    tree_->Branch("b1j1_deltaEta",&b1j1_deltaEta); 
    tree_->Branch("b1j2_deltaEta",&b1j2_deltaEta); 
    tree_->Branch("b2b3_deltaEta",&b2b3_deltaEta); 
    tree_->Branch("b2b4_deltaEta",&b2b4_deltaEta); 
    tree_->Branch("b2j1_deltaEta",&b2j1_deltaEta); 
    tree_->Branch("b2j2_deltaEta",&b2j2_deltaEta); 
    tree_->Branch("b3b4_deltaEta",&b3b4_deltaEta); 
    tree_->Branch("b3j1_deltaEta",&b3j1_deltaEta); 
    tree_->Branch("b3j2_deltaEta",&b3j2_deltaEta); 
    tree_->Branch("b4j1_deltaEta",&b4j1_deltaEta); 
    tree_->Branch("b4j2_deltaEta",&b4j2_deltaEta); 
    tree_->Branch("j1j2_deltaEta",&j1j2_deltaEta); 
    tree_->Branch("h1h2_deltaPhi",&h1h2_deltaPhi); 
    tree_->Branch("h1j1_deltaPhi",&h1j1_deltaPhi); 
    tree_->Branch("h1j2_deltaPhi",&h1j2_deltaPhi); 
    tree_->Branch("h2j1_deltaPhi",&h2j1_deltaPhi); 
    tree_->Branch("h2j2_deltaPhi",&h2j2_deltaPhi); 
    tree_->Branch("h1jj_deltaPhi",&h1jj_deltaPhi); 
    tree_->Branch("h2jj_deltaPhi",&h2jj_deltaPhi); 
    tree_->Branch("hhj1_deltaPhi",&hhj1_deltaPhi); 
    tree_->Branch("hhj2_deltaPhi",&hhj2_deltaPhi); 
    tree_->Branch("hhjj_deltaPhi",&hhjj_deltaPhi); 
    tree_->Branch("h1h2_deltaEta",&h1h2_deltaEta); 
    tree_->Branch("h1j1_deltaEta",&h1j1_deltaEta); 
    tree_->Branch("h1j2_deltaEta",&h1j2_deltaEta); 
    tree_->Branch("h2j1_deltaEta",&h2j1_deltaEta); 
    tree_->Branch("h2j2_deltaEta",&h2j2_deltaEta); 
    tree_->Branch("h1jj_deltaEta",&h1jj_deltaEta); 
    tree_->Branch("h2jj_deltaEta",&h2jj_deltaEta); 
    tree_->Branch("hhj1_deltaEta",&hhj1_deltaEta); 
    tree_->Branch("hhj2_deltaEta",&hhj2_deltaEta); 
    tree_->Branch("hhjj_deltaEta",&hhjj_deltaEta);
    tree_->Branch("hhjj_pt",      &hhjj_pt);
    tree_->Branch("VBFEvent", &VBFEvent);
    tree_->Branch("nBtag", &nBtag);
    tree_->Branch("VBFEventLocation", &VBFEventLocation);  
    tree_->Branch("nPVgood", &nPVgood);
    tree_->Branch("nJet",    &nJet);
    tree_->Branch("nJet_ec", &nJet_ec);
    tree_->Branch("nJet_hf", &nJet_hf);
    tree_->Branch("nJet_brl",&nJet_brl);
    tree_->Branch("nJet_edc",&nJet_edc);
    tree_->Branch("nJet_fwd",&nJet_fwd);
    tree_->Branch("nJet_tot",&nJet_tot);
    tree_->Branch("HT_PT30", &HT_PT30);
    tree_->Branch("HT_PT40", &HT_PT40);
    tree_->Branch("j1etaj2eta", &j1etaj2eta); 
    tree_->Branch("maxj1etaj2eta", &maxj1etaj2eta);    
    tree_->Branch("BDT1", &BDT1);
    tree_->Branch("BDT2", &BDT2);
    tree_->Branch("BDT3", &BDT3);
    tree_->Branch("BDT3cat1", &BDT3cat1);
    tree_->Branch("BDT3cat2", &BDT3cat2);
    BRANCH_m_pt_eta_phi_p4(H1rand)
    BRANCH_m_pt_eta_phi_p4(H2rand)
    tree_->Branch("H1_bb_deltaR", &H1_bb_deltaR);
    tree_->Branch("H2_bb_deltaR", &H2_bb_deltaR);    
    tree_->Branch("H1_bb_deltaPhi", &H1_bb_deltaPhi);
    tree_->Branch("H2_bb_deltaPhi", &H2_bb_deltaPhi);  
    tree_->Branch("H1_bb_deltaEta", &H1_bb_deltaEta);
    tree_->Branch("H2_bb_deltaEta", &H2_bb_deltaEta); 
    tree_->Branch("H1_b1_qid", &H1_b1_qid);
    tree_->Branch("H1_b2_qid", &H1_b2_qid);
    tree_->Branch("H2_b1_qid", &H2_b1_qid);
    tree_->Branch("H2_b2_qid", &H2_b2_qid);
    tree_->Branch("JJ_j1_qid", &JJ_j1_qid);
    tree_->Branch("JJ_j2_qid", &JJ_j2_qid);
    tree_->Branch("H1_b1_qual", &H1_b1_qual);
    tree_->Branch("H1_b2_qual", &H1_b2_qual);
    tree_->Branch("H2_b1_qual", &H2_b1_qual);
    tree_->Branch("H2_b2_qual", &H2_b2_qual);
    tree_->Branch("JJ_j1_qual", &JJ_j1_qual);
    tree_->Branch("JJ_j2_qual", &JJ_j2_qual);
    tree_->Branch("H1_qual", &H1_qual);
    tree_->Branch("H2_qual", &H2_qual);
    tree_->Branch("HH_qual", &HH_qual);
    tree_->Branch("JJ_qual", &JJ_qual);

    //EXTRA VARIABLES 
    tree_->Branch("sum_4b_pt", &sum_4b_pt);
    tree_->Branch("sum_3b_bscore", &sum_3b_bscore);
    tree_->Branch("sum_3b_bres", &sum_3b_bres);
    tree_->Branch("min_4b_deltaR",   &min_4b_deltaR);    
    tree_->Branch("min_4b_deltaEta", &min_4b_deltaEta); 
    tree_->Branch("min_4b_deltaPhi", &min_4b_deltaPhi);
    tree_->Branch("max_4b_deltaR",   &max_4b_deltaR);    
    tree_->Branch("max_4b_deltaEta", &max_4b_deltaEta); 
    tree_->Branch("max_4b_deltaPhi", &max_4b_deltaPhi);
    tree_->Branch("min_4b_cm_deltaR",   &min_4b_cm_deltaR);    
    tree_->Branch("min_4b_cm_deltaEta", &min_4b_cm_deltaEta); 
    tree_->Branch("min_4b_cm_deltaPhi", &min_4b_cm_deltaPhi);
    tree_->Branch("max_4b_cm_deltaR",   &max_4b_cm_deltaR);    
    tree_->Branch("max_4b_cm_deltaEta", &max_4b_cm_deltaEta); 
    tree_->Branch("max_4b_cm_deltaPhi", &max_4b_cm_deltaPhi);
    tree_->Branch("min_hbb_deltaR", &min_hbb_deltaR);
    tree_->Branch("max_hbb_deltaR", &max_hbb_deltaR);

    //EXTRA B-TAGGING
    tree_->Branch("nBtagTightonMediumWP", &nBtagTightonMediumWP);

    //COSTHETA IN VBF CM
    tree_->Branch("abs_costh_H1_vbfcm",    &abs_costh_H1_vbfcm);     
    tree_->Branch("abs_costh_H2_vbfcm",    &abs_costh_H2_vbfcm); 
    tree_->Branch("abs_costh_HH_vbfcm",    &abs_costh_HH_vbfcm); 
    tree_->Branch("abs_costh_JJ_vbfcm",    &abs_costh_JJ_vbfcm); 
    tree_->Branch("abs_costh_HH_b1_vbfcm", &abs_costh_HH_b1_vbfcm);    
    tree_->Branch("abs_costh_HH_b2_vbfcm", &abs_costh_HH_b2_vbfcm);
    tree_->Branch("abs_costh_HH_b3_vbfcm", &abs_costh_HH_b3_vbfcm);    
    tree_->Branch("abs_costh_HH_b4_vbfcm", &abs_costh_HH_b4_vbfcm);
    tree_->Branch("abs_costh_JJ_j1_vbfcm", &abs_costh_JJ_j1_vbfcm);    
    tree_->Branch("abs_costh_JJ_j2_vbfcm", &abs_costh_JJ_j2_vbfcm);
    //COSTHETA IN GGF CM
    tree_->Branch("abs_costh_H1_ggfcm",    &abs_costh_H1_ggfcm);     
    tree_->Branch("abs_costh_H2_ggfcm",    &abs_costh_H2_ggfcm); 
    tree_->Branch("abs_costh_HH_b1_ggfcm", &abs_costh_HH_b1_ggfcm);    
    tree_->Branch("abs_costh_HH_b2_ggfcm", &abs_costh_HH_b2_ggfcm);
    tree_->Branch("abs_costh_HH_b3_ggfcm", &abs_costh_HH_b3_ggfcm);    
    tree_->Branch("abs_costh_HH_b4_ggfcm", &abs_costh_HH_b4_ggfcm);
    //COSTHETA IN H1 CM
    tree_->Branch("abs_costh_H1_b1_h1cm",    &abs_costh_H1_b1_h1cm);     
    tree_->Branch("abs_costh_H1_b2_h1cm",    &abs_costh_H1_b2_h1cm); 
    //COSTHETA IN H2 CM
    tree_->Branch("abs_costh_H2_b1_h2cm",    &abs_costh_H2_b1_h2cm);     
    tree_->Branch("abs_costh_H2_b2_h2cm",    &abs_costh_H2_b2_h2cm); 
    //COSTHETA IN JJ CM
    tree_->Branch("abs_costh_JJ_j1_jjcm",    &abs_costh_JJ_j1_jjcm);     
    tree_->Branch("abs_costh_JJ_j2_jjcm",    &abs_costh_JJ_j2_jjcm);

    //generator level information
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
    tree_->Branch("gen_mJJ", &gen_mJJ);
    tree_->Branch("gen_etapairsign", &gen_etapairsign);
    tree_->Branch("gen_deltaEtaJJ", &gen_deltaEtaJJ);
    tree_->Branch("gen_mHH", &gen_mHH);
    tree_->Branch("gen_min_4b_deltaR",  &gen_min_4b_deltaR);
    tree_->Branch("gen_max_4b_deltaR",  &gen_max_4b_deltaR);
    tree_->Branch("gen_min_hbb_deltaR", &gen_min_hbb_deltaR);
    tree_->Branch("gen_max_hbb_deltaR", &gen_max_hbb_deltaR);

    tree_->Branch( "gen_H1_b1_jetidx",  &gen_H1_b1_jetidx);
    tree_->Branch( "gen_H1_b2_jetidx",  &gen_H1_b2_jetidx);
    tree_->Branch( "gen_H2_b1_jetidx",  &gen_H2_b1_jetidx);
    tree_->Branch( "gen_H2_b2_jetidx",  &gen_H2_b2_jetidx);
    tree_->Branch("gen_q1_out_jetidx", &gen_q1_out_jetidx);
    tree_->Branch("gen_q2_out_jetidx", &gen_q2_out_jetidx);
    tree_->Branch( "gen_H1_b1_jetmatched",  &gen_H1_b1_jetmatched);
    tree_->Branch( "gen_H1_b2_jetmatched",  &gen_H1_b2_jetmatched);
    tree_->Branch( "gen_H2_b1_jetmatched",  &gen_H2_b1_jetmatched);
    tree_->Branch( "gen_H2_b2_jetmatched",  &gen_H2_b2_jetmatched);
    tree_->Branch("gen_q1_out_jetmatched", &gen_q1_out_jetmatched);
    tree_->Branch("gen_q2_out_jetmatched", &gen_q2_out_jetmatched);
    tree_->Branch("gen_HH_qual", &gen_HH_qual);
    tree_->Branch("gen_qq_qual", &gen_qq_qual);

    //TTEMU EVents Studies
    BRANCH_m_pt_ptRegressed_eta_phi_p4(TT_b1)
    tree_->Branch("TT_b1_deepCSV", &TT_b1_deepCSV);
    BRANCH_m_pt_ptRegressed_eta_phi_p4(TT_b2)
    tree_->Branch("TT_b2_deepCSV", &TT_b2_deepCSV);
    BRANCH_m_pt_eta_phi_p4(TT_e)
    BRANCH_m_pt_eta_phi_p4(TT_mu)
    tree_->Branch("TT_m", &TT_m);         
    tree_->Branch("TT_nJet", &TT_nJet); 
    tree_->Branch("TT_nPV", &TT_nPV);
    tree_->Branch("TT_nPVgood", &TT_nPVgood);

    //XYH stugg
    tree_->Branch("gen_H1_b1_matchedflag", &gen_H1_b1_matchedflag);
    tree_->Branch("gen_H1_b2_matchedflag", &gen_H1_b2_matchedflag);
    tree_->Branch("gen_H2_b1_matchedflag", &gen_H2_b1_matchedflag);
    tree_->Branch("gen_H2_b2_matchedflag", &gen_H2_b2_matchedflag);
    tree_->Branch("recoJetMatchedToGenJet1", &recoJetMatchedToGenJet1);
    tree_->Branch("recoJetMatchedToGenJet2", &recoJetMatchedToGenJet2);
    tree_->Branch("recoJetMatchedToGenJet3", &recoJetMatchedToGenJet3);
    tree_->Branch("recoJetMatchedToGenJet4", &recoJetMatchedToGenJet4);

    //boosted classification
    tree_->Branch("pass_VBFboosted_sel",            &pass_VBFboosted_sel);
    tree_->Branch("pass_VBFboosted_sel_nodPhi",     &pass_VBFboosted_sel_nodPhi);
    tree_->Branch("pass_VBFboosted_sel_nodEta",     &pass_VBFboosted_sel_nodEta);
    tree_->Branch("pass_VBFboosted_sel_noang",      &pass_VBFboosted_sel_noang);
    tree_->Branch("n_fatjet_gt250",                 &n_fatjet_gt250);
    tree_->Branch("n_fatjet_gt300",                 &n_fatjet_gt300);
    tree_->Branch("n_fatjet_gt250_twosubj",         &n_fatjet_gt250_twosubj);
    tree_->Branch("n_fatjet_gt300_twosubj",         &n_fatjet_gt300_twosubj);
    tree_->Branch("n_fatjet_gt250_twosubj_msdgt30", &n_fatjet_gt250_twosubj_msdgt30);
    tree_->Branch("n_fatjet_gt300_twosubj_msdgt30", &n_fatjet_gt300_twosubj_msdgt30);

    //VBFjets
    tree_->Branch("VBFcandidates",            &VBFcandidates);
    tree_->Branch("VBFj1candidates",          &VBFj1candidates);
    tree_->Branch("VBFj2candidates",          &VBFj2candidates);
    tree_->Branch("VBFj1_qid",          &VBFj1_qid);
    tree_->Branch("VBFj2_qid",          &VBFj2_qid);
    tree_->Branch("VBFj2_opt1_qid",          &VBFj2_opt1_qid);
    tree_->Branch("VBFj2_opt2_qid",          &VBFj2_opt2_qid);

    tree_->Branch("VBF_JJ_m",                    &VBF_JJ_m);
    tree_->Branch("VBF_opt1_JJ_m",               &VBF_opt1_JJ_m);
    tree_->Branch("VBF_opt2_JJ_m",               &VBF_opt2_JJ_m);
    tree_->Branch("VBF_j1j2_dEta",               &VBF_j1j2_dEta);
    tree_->Branch("VBF_opt1_j1j2_dEta",          &VBF_opt1_j1j2_dEta);
    tree_->Branch("VBF_opt2_j1j2_dEta",          &VBF_opt2_j1j2_dEta);

    BRANCH_m_pt_eta_phi_p4(fatjet1);
    tree_->Branch("fatjet1_ParticleNetMD_probQCDb",      &fatjet1_ParticleNetMD_probQCDb);
    tree_->Branch("fatjet1_ParticleNetMD_probQCDbb",     &fatjet1_ParticleNetMD_probQCDbb);
    tree_->Branch("fatjet1_ParticleNetMD_probQCDc",      &fatjet1_ParticleNetMD_probQCDc);
    tree_->Branch("fatjet1_ParticleNetMD_probQCDcc",     &fatjet1_ParticleNetMD_probQCDcc);
    tree_->Branch("fatjet1_ParticleNetMD_probQCDothers", &fatjet1_ParticleNetMD_probQCDothers);
    tree_->Branch("fatjet1_ParticleNetMD_probXbb",       &fatjet1_ParticleNetMD_probXbb);
    tree_->Branch("fatjet1_ParticleNetMD_probXcc",       &fatjet1_ParticleNetMD_probXcc);
    tree_->Branch("fatjet1_ParticleNetMD_probXqq",       &fatjet1_ParticleNetMD_probXqq);

    BRANCH_m_pt_eta_phi_p4(fatjet2);
    tree_->Branch("fatjet2_ParticleNetMD_probQCDb",      &fatjet2_ParticleNetMD_probQCDb);
    tree_->Branch("fatjet2_ParticleNetMD_probQCDbb",     &fatjet2_ParticleNetMD_probQCDbb);
    tree_->Branch("fatjet2_ParticleNetMD_probQCDc",      &fatjet2_ParticleNetMD_probQCDc);
    tree_->Branch("fatjet2_ParticleNetMD_probQCDcc",     &fatjet2_ParticleNetMD_probQCDcc);
    tree_->Branch("fatjet2_ParticleNetMD_probQCDothers", &fatjet2_ParticleNetMD_probQCDothers);
    tree_->Branch("fatjet2_ParticleNetMD_probXbb",       &fatjet2_ParticleNetMD_probXbb);
    tree_->Branch("fatjet2_ParticleNetMD_probXcc",       &fatjet2_ParticleNetMD_probXcc);
    tree_->Branch("fatjet2_ParticleNetMD_probXqq",       &fatjet2_ParticleNetMD_probXqq);

    // note that the initialization of the user branches is made separately when calling declareUser*Branch
}

void OutputTree::clear()
{
    Run=0;
    LumiSec=0;
    Event=0;

    CLEAR_m_pt_ptRegressed_eta_phi_p4(H1_b1)
    H1_b1_deepCSV = 0.;
    H1_b1_deepJet = 0.;
    H1_b1_bRegRes = 0.;
    H1_b1_puId =-1;
    H1_b1_jetId =-1;
    H1_b1_partonFlavour=-1;
    H1_b1_hadronFlavour=-1;
    H1_b1_qgl = -1;
    H1_b1_genJetIdx = -2;  
    CLEAR_m_pt_ptRegressed_eta_phi_p4(H1_b2)
    H1_b2_deepCSV = 0.;
    H1_b2_deepJet = 0.;
    H1_b2_bRegRes = 0.;
    H1_b2_qgl = -1;
    H1_b2_puId =-1;
    H1_b2_jetId =-1;
    H1_b2_partonFlavour=-1;
    H1_b2_hadronFlavour=-1;
    H1_b2_genJetIdx = -2; 
    CLEAR_m_pt_ptRegressed_eta_phi_p4(H2_b1)
    H2_b1_deepCSV = 0.;
    H2_b1_deepJet = 0.;
    H2_b1_bRegRes = 0.;
    H2_b1_qgl = -1;
    H2_b1_puId =-1;
    H2_b1_jetId =-1;
    H2_b1_partonFlavour=-1;
    H2_b1_hadronFlavour=-1;
    H2_b1_genJetIdx = -2; 
    CLEAR_m_pt_ptRegressed_eta_phi_p4(H2_b2)
    H2_b2_deepCSV = 0.;
    H2_b2_deepJet = 0.;
    H2_b2_bRegRes = 0.;
    H2_b2_qgl = -1;
    H2_b2_puId =-1;
    H2_b2_jetId =-1;
    H2_b2_partonFlavour=-1;
    H2_b2_hadronFlavour=-1;
    H2_b2_genJetIdx = -2; 
    CLEAR_m_pt_eta_phi_p4(H1)
    H1_bb_DeltaR = 0.;
    CLEAR_m_pt_eta_phi_p4(H2)
    H2_bb_DeltaR = 0.;
    CLEAR_m_pt_eta_phi_p4(HH)
    HH_2DdeltaM = 0.;
    HH_m_kinFit = 0.;

    //Unregressed Higgs reconstruction
    CLEAR_m_pt_eta_phi_p4(H1)
    CLEAR_m_pt_eta_phi_p4(H2)
    CLEAR_m_pt_eta_phi_p4(HH)
    //Non-resonant analysis and studies
    btaggerID = -1;
    EventCount = -1;
    H1_b1_deepCSV = 0.;
    H1_b1_deepJet = 0.;
    H1_b1_rawpt = 0.;
    H1_b2_deepCSV = 0.;
    H1_b2_deepJet = 0.;
    H1_b2_rawpt = 0.;
    H2_b1_deepCSV = 0.;
    H2_b1_deepJet = 0.;
    H2_b1_rawpt = 0.;
    H2_b2_deepCSV = 0.;
    H2_b2_deepJet = 0.;
    H2_b2_rawpt = 0.;
    H1_b1_genJetIdx = -2;
    H1_b2_genJetIdx = -2;
    H2_b1_genJetIdx = -2;
    H2_b2_genJetIdx = -2;
    CLEAR_m_pt_ptRegressed_eta_phi_p4(HH_b1)
    HH_b1_deepCSV = 0;
    HH_b1_deepJet = 0;
    HH_b1_bRegRes = 0.;
    HH_b1_qgl = -1;
    HH_b1_puId =-1;
    HH_b1_jetId =-1;
    HH_b1_partonFlavour=-1;
    HH_b1_hadronFlavour=-1;
    HH_b1_rawpt = 0; 
    HH_b1_genJetIdx = -2;
    CLEAR_m_pt_ptRegressed_eta_phi_p4(HH_b2)
    HH_b2_deepCSV = 0;
    HH_b2_deepJet = 0;
    HH_b2_bRegRes = 0.;
    HH_b2_qgl = -1;   
    HH_b2_puId =-1;
    HH_b2_jetId =-1;
    HH_b2_partonFlavour=-1;
    HH_b2_hadronFlavour=-1;
    HH_b2_rawpt = 0;
    HH_b2_genJetIdx = -2;
    CLEAR_m_pt_ptRegressed_eta_phi_p4(HH_b3)
    HH_b3_deepCSV = 0;
    HH_b3_deepJet = 0;
    HH_b3_bRegRes = 0.;
    HH_b3_rawpt = 0;
    HH_b3_qgl = -1; 
    HH_b3_puId =-1;
    HH_b3_jetId =-1;
    HH_b3_partonFlavour=-1;
    HH_b3_hadronFlavour=-1;
    HH_b3_genJetIdx = -2;
    CLEAR_m_pt_ptRegressed_eta_phi_p4(HH_b4)
    HH_b4_deepCSV = 0;
    HH_b4_deepJet = 0;
    HH_b4_bRegRes = 0.;
    HH_b4_rawpt = 0;
    HH_b4_qgl = -1;
    HH_b4_puId =-1;
    HH_b4_jetId =-1;
    HH_b4_partonFlavour=-1;
    HH_b4_hadronFlavour=-1;
    HH_b4_genJetIdx = -2;  
    CLEAR_m_pt_eta_phi_p4(JJ_j1)
    JJ_j1_deepCSV = 0.;
    JJ_j1_deepJet = 0.;
    JJ_j1_qgl = -1;
    JJ_j1_puId =-1;
    JJ_j1_jetId =-1;
    JJ_j1_partonFlavour=-1;
    JJ_j1_hadronFlavour=-1;
    JJ_j1_rawpt = 0;
    JJ_j1_genJetIdx = -2;
    JJ_j1_location = 0;
    CLEAR_m_pt_eta_phi_p4(JJ_j2)
    JJ_j2_deepCSV = 0.;    
    JJ_j2_deepJet = 0.;
    JJ_j2_qgl = -1;  
    JJ_j2_puId =-1;
    JJ_j2_jetId =-1;
    JJ_j2_partonFlavour=-1;
    JJ_j2_hadronFlavour=-1;
    JJ_j2_rawpt = 0;
    JJ_j2_genJetIdx = -2; 
    JJ_j2_location = 0; 
    CLEAR_m_pt_eta_phi_p4(JJ)
    CLEAR_m_pt_ptRegressed_eta_phi_p4(HH_btag_b1)
    HH_btag_b1_bscore = 0;
    HH_btag_b1_bres = 0;
    CLEAR_m_pt_ptRegressed_eta_phi_p4(HH_btag_b2)
    HH_btag_b2_bscore = 0;
    HH_btag_b2_bres = 0;
    CLEAR_m_pt_ptRegressed_eta_phi_p4(HH_btag_b3)
    HH_btag_b3_bscore = 0;
    HH_btag_b3_bres = 0;
    CLEAR_m_pt_ptRegressed_eta_phi_p4(HH_btag_b4)
    HH_btag_b4_bscore = 0;
    HH_btag_b4_bres = 0;

    nJet_brl = -1.;
    nJet_edc = -1.;
    nJet_fwd = -1.;
    nJet_tot = -1.;
    nJet_ec = -1.;
    nJet_hf = -1.;
    nJet    = -1.;
    HT_PT30 = -1.;
    HT_PT40 = -1.;
    VBFEvent =-1.;
    nBtag =-1.;
    VBFEventLocation = 0;
    nJet = -1;
    nPVgood=0;
    BDT1 =-1.2;
    BDT2 =-1.2;
    BDT3 =-1.2;
    BDT3cat1 =-1.2;
    BDT3cat2 =-1.2;
    maxj1etaj2eta=-99;
    j1etaj2eta=-99;
    CLEAR_m_pt_eta_phi_p4(H1rand)
    CLEAR_m_pt_eta_phi_p4(H2rand)
    H1_bb_deltaR = 0.;
    H2_bb_deltaR = 0.;
    H1_bb_deltaEta = 0.;
    H2_bb_deltaEta = 0.;
    H1_bb_deltaPhi = 0.;
    H2_bb_deltaPhi = 0.;
    b1b2_deltaR = 0.; 
    b1b3_deltaR = 0.; 
    b1b4_deltaR = 0.; 
    b1j1_deltaR = 0.; 
    b1j2_deltaR = 0.; 
    b2b3_deltaR = 0.; 
    b2b4_deltaR = 0.; 
    b2j1_deltaR = 0.; 
    b2j2_deltaR = 0.; 
    b3b4_deltaR = 0.; 
    b3j1_deltaR = 0.; 
    b3j2_deltaR = 0.; 
    b4j1_deltaR = 0.; 
    b4j2_deltaR = 0.; 
    j1j2_deltaR = 0.; 
    h1h2_deltaR = 0.; 
    h1j1_deltaR = 0.; 
    h1j2_deltaR = 0.; 
    h2j1_deltaR = 0.; 
    h2j2_deltaR = 0.; 
    h1jj_deltaR = 0.; 
    h2jj_deltaR = 0.; 
    hhj1_deltaR = 0.; 
    hhj2_deltaR = 0.; 
    hhjj_deltaR = 0.;
    b1b2_deltaPhi = 0.; 
    b1b3_deltaPhi = 0.; 
    b1b4_deltaPhi = 0.; 
    b1j1_deltaPhi = 0.; 
    b1j2_deltaPhi = 0.; 
    b2b3_deltaPhi = 0.; 
    b2b4_deltaPhi = 0.; 
    b2j1_deltaPhi = 0.; 
    b2j2_deltaPhi = 0.; 
    b3b4_deltaPhi = 0.; 
    b3j1_deltaPhi = 0.; 
    b3j2_deltaPhi = 0.; 
    b4j1_deltaPhi = 0.; 
    b4j2_deltaPhi = 0.; 
    j1j2_deltaPhi = 0.; 
    b1b2_deltaEta = 0.; 
    b1b3_deltaEta = 0.; 
    b1b4_deltaEta = 0.; 
    b1j1_deltaEta = 0.; 
    b1j2_deltaEta = 0.; 
    b2b3_deltaEta = 0.; 
    b2b4_deltaEta = 0.; 
    b2j1_deltaEta = 0.; 
    b2j2_deltaEta = 0.; 
    b3b4_deltaEta = 0.; 
    b3j1_deltaEta = 0.; 
    b3j2_deltaEta = 0.; 
    b4j1_deltaEta = 0.; 
    b4j2_deltaEta = 0.; 
    j1j2_deltaEta = 0.;  
    h1h2_deltaPhi = 0.; 
    h1j1_deltaPhi = 0.; 
    h1j2_deltaPhi = 0.; 
    h2j1_deltaPhi = 0.; 
    h2j2_deltaPhi = 0.; 
    h1jj_deltaPhi = 0.; 
    h2jj_deltaPhi = 0.; 
    hhj1_deltaPhi = 0.; 
    hhj2_deltaPhi = 0.; 
    hhjj_deltaPhi = 0.; 
    h1h2_deltaEta = 0.; 
    h1j1_deltaEta = 0.; 
    h1j2_deltaEta = 0.; 
    h2j1_deltaEta = 0.; 
    h2j2_deltaEta = 0.; 
    h1jj_deltaEta = 0.; 
    h2jj_deltaEta = 0.; 
    hhj1_deltaEta = 0.; 
    hhj2_deltaEta = 0.; 
    hhjj_deltaEta = 0.;
    hhjj_pt       = 0.;
    sum_4b_pt     = 0.;
    sum_3b_bscore = 0.;
    sum_3b_bres   = 0.;
    min_4b_deltaR   = 0.;
    min_4b_deltaPhi = 0.;
    min_4b_deltaEta = 0.;
    max_4b_deltaR   = 0.;
    max_4b_deltaPhi = 0.;
    max_4b_deltaEta = 0.;
    min_4b_cm_deltaR   = 0.;
    min_4b_cm_deltaPhi = 0.;
    min_4b_cm_deltaEta = 0.;
    max_4b_cm_deltaR   = 0.;
    max_4b_cm_deltaPhi = 0.;
    max_4b_cm_deltaEta = 0.;
    nBtagTightonMediumWP = 0.;
    abs_costh_H1_vbfcm=0;       
    abs_costh_H2_vbfcm=0;   
    abs_costh_HH_vbfcm=0;   
    abs_costh_JJ_vbfcm=0;   
    abs_costh_HH_b1_vbfcm=0;    
    abs_costh_HH_b2_vbfcm=0;
    abs_costh_HH_b3_vbfcm=0;    
    abs_costh_HH_b4_vbfcm=0;
    abs_costh_JJ_j1_vbfcm=0;    
    abs_costh_JJ_j2_vbfcm=0;
    abs_costh_H1_ggfcm=0;       
    abs_costh_H2_ggfcm=0; 
    abs_costh_HH_b1_ggfcm=0;    
    abs_costh_HH_b2_ggfcm=0;
    abs_costh_HH_b3_ggfcm=0;    
    abs_costh_HH_b4_ggfcm=0;
    abs_costh_H1_b1_h1cm=0;       
    abs_costh_H1_b2_h1cm=0; 
    abs_costh_H2_b1_h2cm=0;       
    abs_costh_H2_b2_h2cm=0;     
    abs_costh_JJ_j1_jjcm=0;       
    abs_costh_JJ_j2_jjcm=0; 
    H1_b1_qid=-1;
    H1_b2_qid=-1;
    H2_b1_qid=-1;
    H2_b2_qid=-1;
    JJ_j1_qid=-1;
    JJ_j2_qid=-1;
    H1_b1_qual=-1;
    H1_b2_qual=-1;
    H2_b1_qual=-1;
    H2_b2_qual=-1;
    JJ_j1_qual=-1;
    JJ_j2_qual=-1;
    H1_qual=-1;
    H2_qual=-1;
    HH_qual=-1;
    JJ_qual=-1;

    //generator level information
    gen_mHH = 0.;    
    gen_mJJ = 0.; 
    gen_etapairsign = 0;   
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
    gen_min_4b_deltaR = -1;
    gen_max_4b_deltaR = -1;
    gen_min_hbb_deltaR = -1;
    gen_max_hbb_deltaR = -1;

    gen_q1_out_jetidx=-1;
    gen_q2_out_jetidx=-1;
    gen_H1_b1_jetidx=-1;
    gen_H1_b2_jetidx=-1;
    gen_H2_b1_jetidx=-1;
    gen_H2_b2_jetidx=-1;
    gen_q1_out_jetmatched=-1;
    gen_q2_out_jetmatched=-1;
    gen_H1_b1_jetmatched=-1;
    gen_H1_b2_jetmatched=-1;
    gen_H2_b1_jetmatched=-1;
    gen_H2_b2_jetmatched=-1;
    gen_HH_qual = -1;
    gen_qq_qual = -1;


    ///TTEMU events
    CLEAR_m_pt_ptRegressed_eta_phi_p4(TT_b1)
    TT_b1_deepCSV = 0.;
    CLEAR_m_pt_ptRegressed_eta_phi_p4(TT_b2)
    TT_b2_deepCSV = 0.;    
    CLEAR_m_pt_eta_phi_p4(TT_e)
    CLEAR_m_pt_eta_phi_p4(TT_mu)
    TT_m=0; 
    TT_nPV=0;
    TT_nPVgood=0;
    TT_nJet=0;     

    //TriggerSF
    triggerScaleFactor        = 1.;
    triggerDataEfficiency     = 1.;
    triggerMcEfficiency       = 1.;
    triggerScaleFactorUp      = 1.;
    triggerDataEfficiencyUp   = 1.;
    triggerMcEfficiencyUp     = 1.;
    triggerScaleFactorDown    = 1.;
    triggerDataEfficiencyDown = 1.;
    triggerMcEfficiencyDown   = 1.;

    //XYH stuff
    gen_H1_b1_matchedflag = -1;
    gen_H1_b2_matchedflag = -1;
    gen_H2_b1_matchedflag = -1;
    gen_H2_b2_matchedflag = -1;
    recoJetMatchedToGenJet1 = -1;
    recoJetMatchedToGenJet2 = -1;
    recoJetMatchedToGenJet3 = -1;
    recoJetMatchedToGenJet4 = -1;    

    //boosted classification
    pass_VBFboosted_sel            = 0;
    pass_VBFboosted_sel_nodPhi     = 0;
    pass_VBFboosted_sel_nodEta     = 0;
    pass_VBFboosted_sel_noang      = 0;
    n_fatjet_gt250                 = 0;
    n_fatjet_gt300                 = 0;
    n_fatjet_gt250_twosubj         = 0;
    n_fatjet_gt300_twosubj         = 0;
    n_fatjet_gt250_twosubj_msdgt30 = 0;
    n_fatjet_gt300_twosubj_msdgt30 = 0;

<<<<<<< HEAD
    //VBFjets
    VBFcandidates=0;
    VBFj1candidates=0;
    VBFj2candidates=0;
    VBFj1_qid=0;
    VBFj2_qid=0;
    VBFj2_opt1_qid=0;
    VBFj2_opt2_qid=0;
    VBF_JJ_m=0;
    VBF_opt1_JJ_m=0;
    VBF_opt2_JJ_m=0;
    VBF_j1j2_dEta=0;
    VBF_opt1_j1j2_dEta=0;
    VBF_opt2_j1j2_dEta=0;
=======
    CLEAR_m_pt_eta_phi_p4(fatjet1);
    fatjet1_ParticleNetMD_probQCDb      = -999;
    fatjet1_ParticleNetMD_probQCDbb     = -999;
    fatjet1_ParticleNetMD_probQCDc      = -999;
    fatjet1_ParticleNetMD_probQCDcc     = -999;
    fatjet1_ParticleNetMD_probQCDothers = -999;
    fatjet1_ParticleNetMD_probXbb       = -999;
    fatjet1_ParticleNetMD_probXcc       = -999;
    fatjet1_ParticleNetMD_probXqq       = -999;

    CLEAR_m_pt_eta_phi_p4(fatjet2);
    fatjet2_ParticleNetMD_probQCDb      = -999;
    fatjet2_ParticleNetMD_probQCDbb     = -999;
    fatjet2_ParticleNetMD_probQCDc      = -999;
    fatjet2_ParticleNetMD_probQCDcc     = -999;
    fatjet2_ParticleNetMD_probQCDothers = -999;
    fatjet2_ParticleNetMD_probXbb       = -999;
    fatjet2_ParticleNetMD_probXcc       = -999;
    fatjet2_ParticleNetMD_probXqq       = -999;
>>>>>>> 3d09b30bde87cabcedcc075a557e6a8ff7c5f282

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

    cout << "[INFO] OutputTree : creating userIntBranch " << name << " (" << defaultClearValue << ")" << endl;

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

    cout << "[INFO] OutputTree : creating userFloatBranch " << name << " (" << defaultClearValue << ")" << endl;

    // set the branch
    tree_->Branch(name.c_str(), userFloats_.getValPtr(name));
    return true;
}

//XYH stuff
bool OutputTree::declareUserIntBranchList(std::vector<std::string> nameList, int defaultClearValue)
{
    for(const auto& name : nameList)
    {
        bool success = declareUserIntBranch(name, defaultClearValue);
        if(!success) return false;
    }
    return true;
}
//XYH stuff
