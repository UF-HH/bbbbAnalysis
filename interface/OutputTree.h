#ifndef OUTPUTTREE_H
#define OUTPUTTREE_H

/**
** class  : OutputTree
** author : L. Cadamuro (UF)
** date   : 10/01/2018
** brief  : the output tree of the skims
** NOTE   : for every new variable added:
**        : 1) add the member variable
**        : 2) add the corresponing branch in init_branches()
**        : 3) set the variable clearing in clear()
**        :
**        : remember that the variables are set (from the EventInfo) in the SkimUtils::fill_output_tree function
**        : so put the set instructions of the new variables there
**/

#include "TTree.h"
#include "TLorentzVector.h"
#include "UserValCollection.h"
#include <string>
#include <memory>

// helper: declares the m/pt/eta/phi/p4 of a variable OBJ
#define DECLARE_m_pt_eta_phi_p4(OBJ) \
    float OBJ ## _m; \
    float OBJ ## _pt; \
    float OBJ ## _eta; \
    float OBJ ## _phi; \
    TLorentzVector OBJ ## _p4;

// helper: declares the m/pt/eta/phi/p4 of a variable OBJ
#define DECLARE_m_pt_ptRegressed_eta_phi_p4(OBJ) \
    float OBJ ## _m; \
    float OBJ ## _pt; \
    float OBJ ## _ptRegressed; \
    float OBJ ## _eta; \
    float OBJ ## _phi; \
    TLorentzVector OBJ ## _p4;

class OutputTree {
    
    public:
        OutputTree (bool savetlv = false, std::string name = "bbbbTree", std::string title = "bbbbTree");
        ~OutputTree(){};
        
        void clear(); // to be called to reset the var values
        int fill()  {return tree_->Fill();}
        int write() {return tree_->Write();}
    
        // returns false if the branch could not be created, true if all ok
        // thje second optional value specifies what the branch should be reset to at clear()
        bool declareUserIntBranch   (std::string name, int defaultClearValue = 0);
        bool declareUserFloatBranch (std::string name, float defaultClearValue = 0.0);
        //XYH
        bool declareUserIntBranchList(std::vector<std::string> nameList, int defaultClearValue = 0);

        // throws an exception if the branch name was not declared
        int&   userInt   (std::string name) {return userInts_   . getVal(name);}
        float& userFloat (std::string name) {return userFloats_ . getVal(name);}

        //////////////////////////
        //// saved variables
        //////////////////////////

        unsigned int           Run;
        unsigned int           LumiSec;
        unsigned long long int Event;
        //////////////////----- reco H1 and H2
        DECLARE_m_pt_eta_phi_p4(H1)
        float H1_bb_DeltaR;
        DECLARE_m_pt_eta_phi_p4(H2)
        float H2_bb_DeltaR;
        DECLARE_m_pt_eta_phi_p4(HH)
        float HH_2DdeltaM;
        float HH_m_kinFit;

        //////////////////----- reco bs
        DECLARE_m_pt_eta_phi_p4(H1unregressed)
        DECLARE_m_pt_eta_phi_p4(H2unregressed)
        DECLARE_m_pt_eta_phi_p4(HHunregressed)
        DECLARE_m_pt_ptRegressed_eta_phi_p4(H1_b1)
        float H1_b1_deepCSV;
        float H1_b1_deepJet;
        int H1_b1_jetId;
        int H1_b1_puId;
        float H1_b1_qgl;
        DECLARE_m_pt_ptRegressed_eta_phi_p4(H1_b2)
        float H1_b2_deepCSV;
        float H1_b2_deepJet;
        int H1_b2_jetId;
        int H1_b2_puId;
        float H1_b2_qgl;
        DECLARE_m_pt_ptRegressed_eta_phi_p4(H2_b1)
        float H2_b1_deepCSV;
        float H2_b1_deepJet;
        int H2_b1_jetId;
        int H2_b1_puId;
        float H2_b1_qgl;
        DECLARE_m_pt_ptRegressed_eta_phi_p4(H2_b2)
        float H2_b2_deepCSV;
        float H2_b2_deepJet;
        int H2_b2_jetId;
        int H2_b2_puId;
        float H2_b2_qgl;
        //////////////////----- gen level
        float gen_mHH;
        float gen_mJJ;
        float gen_deltaEtaJJ;
        float gen_min_4b_deltaR;
        float gen_max_4b_deltaR;

        int gen_etapairsign;
        DECLARE_m_pt_eta_phi_p4(gen_H1)
        DECLARE_m_pt_eta_phi_p4(gen_H2)
        DECLARE_m_pt_eta_phi_p4(gen_H1_last)
        DECLARE_m_pt_eta_phi_p4(gen_H2_last)
        DECLARE_m_pt_eta_phi_p4(gen_H1_b1)
        float gen_H1_b1_matchedflag;
        DECLARE_m_pt_eta_phi_p4(gen_H1_b2)
        float gen_H1_b2_matchedflag;
        DECLARE_m_pt_eta_phi_p4(gen_H2_b1)
        float gen_H2_b1_matchedflag;
        DECLARE_m_pt_eta_phi_p4(gen_H2_b2)
        float gen_H2_b2_matchedflag;
        DECLARE_m_pt_eta_phi_p4(gen_q1_in)
        DECLARE_m_pt_eta_phi_p4(gen_q2_in)
        DECLARE_m_pt_eta_phi_p4(gen_q1_out)
        DECLARE_m_pt_eta_phi_p4(gen_q2_out)

        //XYH stuff
        float recoJetMatchedToGenJet1;
        float recoJetMatchedToGenJet2;
        float recoJetMatchedToGenJet3;
        float recoJetMatchedToGenJet4;


        float triggerScaleFactor       ;
        float triggerDataEfficiency    ;
        float triggerMcEfficiency      ;
        float triggerScaleFactorUp     ;
        float triggerDataEfficiencyUp  ;
        float triggerMcEfficiencyUp    ;
        float triggerScaleFactorDown   ;
        float triggerDataEfficiencyDown;
        float triggerMcEfficiencyDown  ;


                              /////////////////---Non-resonant analysis and studies
        int btaggerID;
        int EventCount;
        float H1_b1_bRegRes;
        float H1_b2_bRegRes;
        float H2_b1_bRegRes;
        float H2_b2_bRegRes;
        float HH_b1_bRegRes;
        float HH_b2_bRegRes;
        float HH_b3_bRegRes;
        float HH_b4_bRegRes;        
        float HH_btag_b1_bRegRes;
        float HH_btag_b2_bRegRes;
        float HH_btag_b3_bRegRes;
        float HH_btag_b4_bRegRes;
        int H1_b1_genJetIdx;
        int H1_b2_genJetIdx;
        int H2_b1_genJetIdx;
        int H2_b2_genJetIdx;
        float H1_b1_rawpt;
        float H1_b2_rawpt;
        float H2_b1_rawpt;
        float H2_b2_rawpt;
        int gen_H1_b1_jetidx;
        int gen_H1_b2_jetidx;
        int gen_H2_b1_jetidx;
        int gen_H2_b2_jetidx;
        int gen_q1_out_jetidx;
        int gen_q2_out_jetidx;
         int gen_H1_b1_jetmatched;
         int gen_H1_b2_jetmatched;
         int gen_H2_b1_jetmatched;
         int gen_H2_b2_jetmatched;
        int gen_q1_out_jetmatched;
        int gen_q2_out_jetmatched;
        int gen_HH_qual;
        int gen_qq_qual;
        int H1_b1_partonFlavour;
        int H1_b2_partonFlavour;
        int H2_b1_partonFlavour;
        int H2_b2_partonFlavour;
        int H1_b1_hadronFlavour;
        int H1_b2_hadronFlavour;
        int H2_b1_hadronFlavour;
        int H2_b2_hadronFlavour;
        DECLARE_m_pt_ptRegressed_eta_phi_p4(HH_b1)
        float HH_b1_deepCSV;
        float HH_b1_deepJet;
        float HH_b1_qgl;
        float HH_b1_rawpt;
        int HH_b1_jetId;
        int HH_b1_puId;
        int HH_b1_genJetIdx;
        int HH_b1_partonFlavour;
        int HH_b1_hadronFlavour;
        DECLARE_m_pt_ptRegressed_eta_phi_p4(HH_b2)
        float HH_b2_deepCSV;
        float HH_b2_deepJet;
        float HH_b2_qgl;        
        float HH_b2_rawpt;
        int HH_b2_jetId;
        int HH_b2_puId;     
        int HH_b2_genJetIdx;
        int HH_b2_partonFlavour;
        int HH_b2_hadronFlavour;        
        DECLARE_m_pt_ptRegressed_eta_phi_p4(HH_b3)
        float HH_b3_deepCSV;
        float HH_b3_deepJet;
        float HH_b3_qgl;
        float HH_b3_rawpt;
        int HH_b3_jetId;
        int HH_b3_puId;
        int HH_b3_genJetIdx;
        int HH_b3_partonFlavour; 
        int HH_b3_hadronFlavour; 
        DECLARE_m_pt_ptRegressed_eta_phi_p4(HH_b4)
        float HH_b4_deepCSV;
        float HH_b4_deepJet;
        float HH_b4_qgl;
        float HH_b4_rawpt;
        int HH_b4_jetId;
        int HH_b4_puId;
        int HH_b4_genJetIdx;
        int HH_b4_partonFlavour;                
        int HH_b4_hadronFlavour; 
        DECLARE_m_pt_eta_phi_p4(JJ_j1)
        float JJ_j1_deepCSV;
        float JJ_j1_deepJet;
        float JJ_j1_qgl;
        float JJ_j1_rawpt;
        int JJ_j1_jetId;
        int JJ_j1_puId;
        int JJ_j1_genJetIdx;        
        int JJ_j1_partonFlavour;
        int JJ_j1_hadronFlavour;
        int JJ_j1_location;
        DECLARE_m_pt_eta_phi_p4(JJ_j2)
        float JJ_j2_deepCSV;
        float JJ_j2_deepJet;
        float JJ_j2_qgl;
        float JJ_j2_rawpt;
        int JJ_j2_jetId;
        int JJ_j2_puId;     
        int JJ_j2_genJetIdx;
        int JJ_j2_partonFlavour;
        int JJ_j2_hadronFlavour;        
        int JJ_j2_location;
        DECLARE_m_pt_eta_phi_p4(JJ)
        DECLARE_m_pt_ptRegressed_eta_phi_p4(HH_btag_b1)
        float HH_btag_b1_bscore;
        float HH_btag_b1_bres;
        DECLARE_m_pt_ptRegressed_eta_phi_p4(HH_btag_b2)
        float HH_btag_b2_bscore;
        float HH_btag_b2_bres;
        DECLARE_m_pt_ptRegressed_eta_phi_p4(HH_btag_b3)
        float HH_btag_b3_bscore;
        float HH_btag_b3_bres;
        DECLARE_m_pt_ptRegressed_eta_phi_p4(HH_btag_b4)
        float HH_btag_b4_bscore;
        float HH_btag_b4_bres;
        int VBFEvent;
        int nBtag;
        int VBFEventLocation;
        int nPVgood;
        int nJet_brl;
        int nJet_edc;
        int nJet_fwd;
        int nJet_tot;
        int nJet; 
        int nJet_ec;
        int nJet_hf;
        float HT_PT30;
        float HT_PT40;
        float j1etaj2eta;
        float maxj1etaj2eta;
        float BDT1; 
        float BDT2;
        float BDT3;
        float BDT3cat1;
        float BDT3cat2; 
        DECLARE_m_pt_eta_phi_p4(H1rand);
        DECLARE_m_pt_eta_phi_p4(H2rand);
        float abs_costh_H1_vbfcm   ;
        float abs_costh_H2_vbfcm   ;
        float abs_costh_HH_vbfcm   ;
        float abs_costh_JJ_vbfcm   ;
        float abs_costh_HH_b1_vbfcm;
        float abs_costh_HH_b2_vbfcm;
        float abs_costh_HH_b3_vbfcm;
        float abs_costh_HH_b4_vbfcm;
        float abs_costh_JJ_j1_vbfcm;
        float abs_costh_JJ_j2_vbfcm;        
        float abs_costh_HH_b1_ggfcm;
        float abs_costh_HH_b2_ggfcm;
        float abs_costh_HH_b3_ggfcm;
        float abs_costh_HH_b4_ggfcm;
        float abs_costh_H1_ggfcm   ;
        float abs_costh_H2_ggfcm   ;
        float abs_costh_H1_b1_h1cm ; 
        float abs_costh_H1_b2_h1cm ;
        float abs_costh_H2_b1_h2cm ;
        float abs_costh_H2_b2_h2cm ;
        float abs_costh_JJ_j1_jjcm ;
        float abs_costh_JJ_j2_jjcm ;
        float sum_4b_pt            ;
        float sum_3b_bscore        ;
        float sum_3b_bres          ;
        float min_4b_deltaR  ;
        float min_4b_deltaPhi;
        float min_4b_deltaEta;
        float max_4b_deltaR  ;
        float max_4b_deltaPhi;
        float max_4b_deltaEta;
        float min_4b_cm_deltaR  ;
        float min_4b_cm_deltaPhi;
        float min_4b_cm_deltaEta;
        float max_4b_cm_deltaR  ;
        float max_4b_cm_deltaPhi;
        float max_4b_cm_deltaEta;
        int nBtagTightonMediumWP;
        float min_hbb_deltaR  ;
        float max_hbb_deltaR  ;
        float gen_min_hbb_deltaR  ;
        float gen_max_hbb_deltaR  ;

        float H1_bb_deltaR;
        float H2_bb_deltaR;
        float H1_bb_deltaPhi;
        float H2_bb_deltaPhi;
        float H1_bb_deltaEta;
        float H2_bb_deltaEta;
        float b1b2_deltaR; 
        float b1b3_deltaR; 
        float b1b4_deltaR; 
        float b1j1_deltaR; 
        float b1j2_deltaR; 
        float b2b3_deltaR; 
        float b2b4_deltaR; 
        float b2j1_deltaR; 
        float b2j2_deltaR; 
        float b3b4_deltaR; 
        float b3j1_deltaR; 
        float b3j2_deltaR; 
        float b4j1_deltaR; 
        float b4j2_deltaR; 
        float j1j2_deltaR; 
        float h1h2_deltaR; 
        float h1j1_deltaR; 
        float h1j2_deltaR; 
        float h2j1_deltaR; 
        float h2j2_deltaR; 
        float h1jj_deltaR; 
        float h2jj_deltaR; 
        float hhj1_deltaR; 
        float hhj2_deltaR; 
        float hhjj_deltaR;
        float b1b2_deltaPhi; 
        float b1b3_deltaPhi; 
        float b1b4_deltaPhi; 
        float b1j1_deltaPhi; 
        float b1j2_deltaPhi; 
        float b2b3_deltaPhi; 
        float b2b4_deltaPhi; 
        float b2j1_deltaPhi; 
        float b2j2_deltaPhi; 
        float b3b4_deltaPhi; 
        float b3j1_deltaPhi; 
        float b3j2_deltaPhi; 
        float b4j1_deltaPhi; 
        float b4j2_deltaPhi; 
        float j1j2_deltaPhi; 
        float b1b2_deltaEta; 
        float b1b3_deltaEta; 
        float b1b4_deltaEta; 
        float b1j1_deltaEta; 
        float b1j2_deltaEta; 
        float b2b3_deltaEta; 
        float b2b4_deltaEta; 
        float b2j1_deltaEta; 
        float b2j2_deltaEta; 
        float b3b4_deltaEta; 
        float b3j1_deltaEta; 
        float b3j2_deltaEta; 
        float b4j1_deltaEta; 
        float b4j2_deltaEta; 
        float j1j2_deltaEta; 
        float h1h2_deltaPhi; 
        float h1j1_deltaPhi; 
        float h1j2_deltaPhi; 
        float h2j1_deltaPhi; 
        float h2j2_deltaPhi; 
        float h1jj_deltaPhi; 
        float h2jj_deltaPhi; 
        float hhj1_deltaPhi; 
        float hhj2_deltaPhi; 
        float hhjj_deltaPhi; 
        float h1h2_deltaEta; 
        float h1j1_deltaEta; 
        float h1j2_deltaEta; 
        float h2j1_deltaEta; 
        float h2j2_deltaEta; 
        float h1jj_deltaEta; 
        float h2jj_deltaEta; 
        float hhj1_deltaEta; 
        float hhj2_deltaEta; 
        float hhjj_deltaEta;    
        float hhjj_pt;
        int   H1_b1_qid;
        int   H1_b2_qid;
        int   H2_b1_qid;
        int   H2_b2_qid;
        int   JJ_j1_qid;
        int   JJ_j2_qid;
        int   H1_b1_qual;
        int   H1_b2_qual;
        int   H2_b1_qual;
        int   H2_b2_qual;
        int   JJ_j1_qual;
        int   JJ_j2_qual;
        int   H1_qual;
        int   H2_qual;
        int   HH_qual;
        int   JJ_qual;

        //TTEMU events
        DECLARE_m_pt_ptRegressed_eta_phi_p4(TT_b1)
        float TT_b1_deepCSV;
        DECLARE_m_pt_ptRegressed_eta_phi_p4(TT_b2)
        float TT_b2_deepCSV;
        DECLARE_m_pt_eta_phi_p4(TT_e)
        DECLARE_m_pt_eta_phi_p4(TT_mu)
        float TT_m;
        int TT_nPV;
        int TT_nPVgood;
        int TT_nJet;

        //boosted classification
        int pass_VBFboosted_sel;
        int pass_VBFboosted_sel_nodPhi;
        int pass_VBFboosted_sel_nodEta;
        int pass_VBFboosted_sel_noang;
        int n_fatjet_gt250;
        int n_fatjet_gt300;
        int n_fatjet_gt250_twosubj;
        int n_fatjet_gt300_twosubj;
        int n_fatjet_gt250_twosubj_msdgt30;
        int n_fatjet_gt300_twosubj_msdgt30;

        //VBF jets
        int VBFcandidates;
        int VBFj2candidates;
        int VBFj1candidates;
        int VBFj2_qid;
        int VBFj1_qid;        
        int VBFj2_opt1_qid;
        int VBFj2_opt2_qid;
        float VBF_JJ_m;        
        float VBF_opt1_JJ_m;
        float VBF_opt2_JJ_m;
        float VBF_j1j2_dEta;        
        float VBF_opt1_j1j2_dEta;
        float VBF_opt2_j1j2_dEta;
    private:
        void init_branches();
        std::unique_ptr<TTree> tree_;
        const bool savetlv_;
        
        // for user declared branches
        UserValCollection<float> userFloats_;
        UserValCollection<int>   userInts_;
};

#endif
