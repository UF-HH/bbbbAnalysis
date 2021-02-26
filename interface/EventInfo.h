#ifndef EVENTINFO_H
#define EVENTINFO_H

/**
** class  : EventInfo
** author : L. Cadamuro (UF)
** date   : 10/01/2018
** brief  : Struct that contains all the inforrmation elaborated during the skim
**          (e.g., which jets are selected, whic triggers fire etc...)
**          each object is wrapped in a boost::optional class so that it can be autmatically unitialized
**/

#include <boost/optional.hpp>

#include "CompositeCandidate.h"
#include "Jet.h"
#include "Muon.h"
#include "Electron.h"
#include "GenPart.h"

struct EventInfo{

    boost::optional<unsigned int>           Run;
    boost::optional<unsigned int>           LumiSec;
    boost::optional<unsigned long long int> Event;

    boost::optional<double> event_weight;

    boost::optional<GenPart> gen_X;
    boost::optional<GenPart> gen_H1;
    boost::optional<GenPart> gen_H2;
    boost::optional<GenPart> gen_X_last;
    boost::optional<GenPart> gen_H1_last;
    boost::optional<GenPart> gen_H2_last;

    boost::optional<GenPart> gen_H1_b1;
    boost::optional<GenPart> gen_H1_b2;
    boost::optional<GenPart> gen_H2_b1;
    boost::optional<GenPart> gen_H2_b2;

    //XYH stuff
    boost::optional<int>     gen_H1_b1_matchedflag;
    boost::optional<int>     gen_H1_b2_matchedflag;
    boost::optional<int>     gen_H2_b1_matchedflag;
    boost::optional<int>     gen_H2_b2_matchedflag;
    boost::optional<int>     recoJetMatchedToGenJet1;
    boost::optional<int>     recoJetMatchedToGenJet2;
    boost::optional<int>     recoJetMatchedToGenJet3;
    boost::optional<int>     recoJetMatchedToGenJet4;
    //XYH stuff

    // VBF incoming and outcoming partons
    // hard scatter gen particles
    boost::optional<GenPart> gen_q1_in;
    boost::optional<GenPart> gen_q2_in;
    boost::optional<GenPart> gen_q1_out;
    boost::optional<GenPart> gen_q2_out;
    // last copies after hadronisation
    // boost::optional<GenPart> gen_q1_out_lastcopy;
    // boost::optional<GenPart> gen_q2_out_lastcopy;

    boost::optional<CompositeCandidate> H1;
    boost::optional<float> H1_bb_DeltaR;
    boost::optional<CompositeCandidate> H2;
    boost::optional<float> H2_bb_DeltaR;
    boost::optional<CompositeCandidate> HH;
    boost::optional<float> HH_2DdeltaM;
    boost::optional<float> HH_m_kinFit;

    boost::optional<Jet> H1_b1;
    boost::optional<Jet> H1_b2;
    boost::optional<Jet> H2_b1;
    boost::optional<Jet> H2_b2;

    //Non-resonant analysis and studies
    boost::optional<int>  btaggerID;
    boost::optional<int>  EventCount;
    boost::optional<CompositeCandidate> H1unregressed;
    boost::optional<CompositeCandidate> H2unregressed;
    boost::optional<CompositeCandidate> HHunregressed;
    boost::optional<int>  gen_H1_b1_jetidx;
    boost::optional<int>  gen_H1_b2_jetidx;
    boost::optional<int>  gen_H2_b1_jetidx;
    boost::optional<int>  gen_H2_b2_jetidx;
    boost::optional<int> gen_q1_out_jetidx;
    boost::optional<int> gen_q2_out_jetidx;
    boost::optional<int>  gen_H1_b1_jetmatched;
    boost::optional<int>  gen_H1_b2_jetmatched;
    boost::optional<int>  gen_H2_b1_jetmatched;
    boost::optional<int>  gen_H2_b2_jetmatched;
    boost::optional<int> gen_q1_out_jetmatched;
    boost::optional<int> gen_q2_out_jetmatched;
    boost::optional<int> gen_HH_qual;
    boost::optional<int> gen_qq_qual;
    boost::optional<int> H1_b1_genJetIdx;
    boost::optional<int> H1_b2_genJetIdx;
    boost::optional<int> H2_b1_genJetIdx;
    boost::optional<int> H2_b2_genJetIdx;   
    boost::optional<float> H1_b1_rawpt;
    boost::optional<float> H1_b2_rawpt;
    boost::optional<float> H2_b1_rawpt;
    boost::optional<float> H2_b2_rawpt;  
    boost::optional<float> H1_b1_qgl;
    boost::optional<float> H1_b2_qgl;
    boost::optional<float> H2_b1_qgl;
    boost::optional<float> H2_b2_qgl; 
    boost::optional<int> H1_b1_partonFlavour;
    boost::optional<int> H1_b2_partonFlavour;
    boost::optional<int> H2_b1_partonFlavour;
    boost::optional<int> H2_b2_partonFlavour;
    boost::optional<int> H1_b1_hadronFlavour;
    boost::optional<int> H1_b2_hadronFlavour;
    boost::optional<int> H2_b1_hadronFlavour;
    boost::optional<int> H2_b2_hadronFlavour;
    boost::optional<Jet> JJ_j1;  
    boost::optional<int> JJ_j1_genJetIdx;
    boost::optional<int> JJ_j1_partonFlavour;
    boost::optional<int> JJ_j1_hadronFlavour;    
    boost::optional<float> JJ_j1_qgl;       
    boost::optional<float> JJ_j1_rawpt; 
    boost::optional<int> JJ_j1_location; 
    boost::optional<Jet> JJ_j2;   
    boost::optional<int> JJ_j2_genJetIdx;
    boost::optional<float> JJ_j2_qgl;
    boost::optional<float> JJ_j2_rawpt;
    boost::optional<int> JJ_j2_partonFlavour;
    boost::optional<int> JJ_j2_hadronFlavour;    
    boost::optional<int> JJ_j2_location;
    boost::optional<Jet> HH_b1;   
    boost::optional<int> HH_b1_genJetIdx;
    boost::optional<int> HH_b1_partonFlavour; 
    boost::optional<int> HH_b1_hadronFlavour; 
    boost::optional<float> HH_b1_rawpt;       
    boost::optional<Jet> HH_b2;      
    boost::optional<int> HH_b2_genJetIdx;
    boost::optional<int> HH_b2_partonFlavour;
    boost::optional<int> HH_b2_hadronFlavour;    
    boost::optional<float> HH_b2_rawpt; 
    boost::optional<Jet> HH_b3;      
    boost::optional<int> HH_b3_genJetIdx;
    boost::optional<int> HH_b3_partonFlavour;
    boost::optional<int> HH_b3_hadronFlavour;        
    boost::optional<float> HH_b3_rawpt; 
    boost::optional<Jet> HH_b4;
    boost::optional<int> HH_b4_genJetIdx;      
    boost::optional<int> HH_b4_partonFlavour;
    boost::optional<int> HH_b4_hadronFlavour;
    boost::optional<float> HH_b4_rawpt; 
    boost::optional<CompositeCandidate> JJ;
    boost::optional<Jet> HH_btag_b1;
    boost::optional<Jet> HH_btag_b2;
    boost::optional<Jet> HH_btag_b3;
    boost::optional<Jet> HH_btag_b4;     
    boost::optional<float> HH_btag_b1_bscore;
    boost::optional<float> HH_btag_b2_bscore;
    boost::optional<float> HH_btag_b3_bscore;
    boost::optional<float> HH_btag_b4_bscore; 
    boost::optional<float> HH_btag_b1_bres;
    boost::optional<float> HH_btag_b2_bres;
    boost::optional<float> HH_btag_b3_bres;
    boost::optional<float> HH_btag_b4_bres; 
    boost::optional<float> b1b2_deltaR; 
    boost::optional<float> b1b3_deltaR; 
    boost::optional<float> b1b4_deltaR; 
    boost::optional<float> b1j1_deltaR; 
    boost::optional<float> b1j2_deltaR; 
    boost::optional<float> b2b3_deltaR; 
    boost::optional<float> b2b4_deltaR; 
    boost::optional<float> b2j1_deltaR; 
    boost::optional<float> b2j2_deltaR; 
    boost::optional<float> b3b4_deltaR; 
    boost::optional<float> b3j1_deltaR; 
    boost::optional<float> b3j2_deltaR; 
    boost::optional<float> b4j1_deltaR; 
    boost::optional<float> b4j2_deltaR; 
    boost::optional<float> j1j2_deltaR; 
    boost::optional<float> h1h2_deltaR; 
    boost::optional<float> h1j1_deltaR; 
    boost::optional<float> h1j2_deltaR; 
    boost::optional<float> h2j1_deltaR; 
    boost::optional<float> h2j2_deltaR; 
    boost::optional<float> h1jj_deltaR; 
    boost::optional<float> h2jj_deltaR; 
    boost::optional<float> hhj1_deltaR; 
    boost::optional<float> hhj2_deltaR; 
    boost::optional<float> hhjj_deltaR;
    boost::optional<float> b1b2_deltaPhi; 
    boost::optional<float> b1b3_deltaPhi; 
    boost::optional<float> b1b4_deltaPhi; 
    boost::optional<float> b1j1_deltaPhi; 
    boost::optional<float> b1j2_deltaPhi; 
    boost::optional<float> b2b3_deltaPhi; 
    boost::optional<float> b2b4_deltaPhi; 
    boost::optional<float> b2j1_deltaPhi; 
    boost::optional<float> b2j2_deltaPhi; 
    boost::optional<float> b3b4_deltaPhi; 
    boost::optional<float> b3j1_deltaPhi; 
    boost::optional<float> b3j2_deltaPhi; 
    boost::optional<float> b4j1_deltaPhi; 
    boost::optional<float> b4j2_deltaPhi; 
    boost::optional<float> j1j2_deltaPhi; 
    boost::optional<float> b1b2_deltaEta; 
    boost::optional<float> b1b3_deltaEta; 
    boost::optional<float> b1b4_deltaEta; 
    boost::optional<float> b1j1_deltaEta; 
    boost::optional<float> b1j2_deltaEta; 
    boost::optional<float> b2b3_deltaEta; 
    boost::optional<float> b2b4_deltaEta; 
    boost::optional<float> b2j1_deltaEta; 
    boost::optional<float> b2j2_deltaEta; 
    boost::optional<float> b3b4_deltaEta; 
    boost::optional<float> b3j1_deltaEta; 
    boost::optional<float> b3j2_deltaEta; 
    boost::optional<float> b4j1_deltaEta; 
    boost::optional<float> b4j2_deltaEta; 
    boost::optional<float> j1j2_deltaEta;  
    boost::optional<float> h1h2_deltaPhi; 
    boost::optional<float> h1j1_deltaPhi; 
    boost::optional<float> h1j2_deltaPhi; 
    boost::optional<float> h2j1_deltaPhi; 
    boost::optional<float> h2j2_deltaPhi; 
    boost::optional<float> h1jj_deltaPhi; 
    boost::optional<float> h2jj_deltaPhi; 
    boost::optional<float> hhj1_deltaPhi; 
    boost::optional<float> hhj2_deltaPhi; 
    boost::optional<float> hhjj_deltaPhi; 
    boost::optional<float> h1h2_deltaEta; 
    boost::optional<float> h1j1_deltaEta; 
    boost::optional<float> h1j2_deltaEta; 
    boost::optional<float> h2j1_deltaEta; 
    boost::optional<float> h2j2_deltaEta; 
    boost::optional<float> h1jj_deltaEta; 
    boost::optional<float> h2jj_deltaEta; 
    boost::optional<float> hhj1_deltaEta; 
    boost::optional<float> hhj2_deltaEta; 
    boost::optional<float> hhjj_deltaEta;
    boost::optional<float> hhjj_pt;
    boost::optional<float> abs_costh_H1_vbfcm   ;
    boost::optional<float> abs_costh_H2_vbfcm   ;
    boost::optional<float> abs_costh_HH_vbfcm   ;
    boost::optional<float> abs_costh_JJ_vbfcm   ;
    boost::optional<float> abs_costh_HH_b1_vbfcm;
    boost::optional<float> abs_costh_HH_b2_vbfcm;
    boost::optional<float> abs_costh_HH_b3_vbfcm;
    boost::optional<float> abs_costh_HH_b4_vbfcm;
    boost::optional<float> abs_costh_JJ_j1_vbfcm;
    boost::optional<float> abs_costh_JJ_j2_vbfcm;
    boost::optional<float> abs_costh_HH_b1_ggfcm;
    boost::optional<float> abs_costh_HH_b2_ggfcm;
    boost::optional<float> abs_costh_HH_b3_ggfcm;
    boost::optional<float> abs_costh_HH_b4_ggfcm;
    boost::optional<float> abs_costh_H1_ggfcm   ;
    boost::optional<float> abs_costh_H2_ggfcm   ; 
    boost::optional<float> abs_costh_H1_b1_h1cm ; 
    boost::optional<float> abs_costh_H1_b2_h1cm ;
    boost::optional<float> abs_costh_H2_b1_h2cm ;
    boost::optional<float> abs_costh_H2_b2_h2cm ;
    boost::optional<float> abs_costh_JJ_j1_jjcm ;
    boost::optional<float> abs_costh_JJ_j2_jjcm ;
    boost::optional<float> sum_4b_pt            ;
    boost::optional<float> sum_3b_bscore        ;
    boost::optional<float> sum_3b_bres          ;
    boost::optional<float> min_4b_deltaR  ;
    boost::optional<float> min_4b_deltaPhi;
    boost::optional<float> min_4b_deltaEta;
    boost::optional<float> max_4b_deltaR  ;
    boost::optional<float> max_4b_deltaPhi;
    boost::optional<float> max_4b_deltaEta;
    boost::optional<float> min_4b_cm_deltaR  ;
    boost::optional<float> min_4b_cm_deltaPhi;
    boost::optional<float> min_4b_cm_deltaEta;
    boost::optional<float> max_4b_cm_deltaR  ;
    boost::optional<float> max_4b_cm_deltaPhi;
    boost::optional<float> max_4b_cm_deltaEta;
    boost::optional<float> min_hbb_deltaR;
    boost::optional<float> max_hbb_deltaR;
    boost::optional<int> nBtagTightonMediumWP;
    boost::optional<int> nJet_brl;
    boost::optional<int> nJet_edc;
    boost::optional<int> nJet_fwd;
    boost::optional<int> nJet_tot;
    boost::optional<int> nJet;
    boost::optional<int> nJet_ec;
    boost::optional<int> nJet_hf;
    boost::optional<float> HT_PT30;
    boost::optional<float> HT_PT40;    
    boost::optional<float> j1etaj2eta;
    boost::optional<float> maxj1etaj2eta;    
    boost::optional<int> VBFEvent;
    boost::optional<int> nBtag;
    boost::optional<int> VBFEventLocation;  
    boost::optional<CompositeCandidate> H1rand;
    boost::optional<CompositeCandidate> H2rand;
    boost::optional<float> H1_bb_deltaR;
    boost::optional<float> H2_bb_deltaR;   
    boost::optional<float> H1_bb_deltaPhi;
    boost::optional<float> H2_bb_deltaPhi;   
    boost::optional<float> H1_bb_deltaEta;
    boost::optional<float> H2_bb_deltaEta;
    boost::optional<int> H1_b1_qual;
    boost::optional<int> H1_b2_qual;
    boost::optional<int> H2_b1_qual;
    boost::optional<int> H2_b2_qual;
    boost::optional<int> JJ_j1_qual;
    boost::optional<int> JJ_j2_qual;
    boost::optional<int> H1_qual;
    boost::optional<int> H2_qual;
    boost::optional<int> HH_qual;
    boost::optional<int> JJ_qual;
    boost::optional<int> H1_b1_qid;
    boost::optional<int> H1_b2_qid;
    boost::optional<int> H2_b1_qid;
    boost::optional<int> H2_b2_qid;
    boost::optional<int> JJ_j1_qid;
    boost::optional<int> JJ_j2_qid;
    boost::optional<float> BDT1;
    boost::optional<float> BDT2;
    boost::optional<float> BDT3;
    boost::optional<float> BDT3cat1;
    boost::optional<float> BDT3cat2;
    boost::optional<int> nPVgood;   

    //TTEMU Events
    boost::optional<Jet> TT_b1;
    boost::optional<Jet> TT_b2;
    boost::optional<Electron> TT_e;
    boost::optional<Muon> TT_mu;     
    boost::optional<float> TT_m;
    boost::optional<int> TT_nPV;
    boost::optional<int> TT_nPVgood;
    boost::optional<int> TT_nJet;        

    //boosted classification
    boost::optional<int> pass_VBFboosted_sel;
    boost::optional<int> pass_VBFboosted_sel_nodPhi;
    boost::optional<int> pass_VBFboosted_sel_nodEta;
    boost::optional<int> pass_VBFboosted_sel_noang;
    boost::optional<int> n_fatjet_gt250;
    boost::optional<int> n_fatjet_gt300;
    boost::optional<int> n_fatjet_gt250_twosubj;
    boost::optional<int> n_fatjet_gt300_twosubj;
    boost::optional<int> n_fatjet_gt250_twosubj_msdgt30;
    boost::optional<int> n_fatjet_gt300_twosubj_msdgt30;

    //VBF jets
    boost::optional<int> VBFcandidates;
    boost::optional<int> VBFj1candidates;
    boost::optional<int> VBFj2candidates;
    boost::optional<int> VBFj1_qid;
    boost::optional<int> VBFj2_qid;
    boost::optional<int> VBFj2_opt1_qid;
    boost::optional<int> VBFj2_opt2_qid;
    boost::optional<float> VBF_JJ_m;
    boost::optional<float> VBF_opt1_JJ_m;
    boost::optional<float> VBF_opt2_JJ_m;
    boost::optional<float> VBF_j1j2_dEta;
    boost::optional<float> VBF_opt1_j1j2_dEta;
    boost::optional<float> VBF_opt2_j1j2_dEta;

};

#endif