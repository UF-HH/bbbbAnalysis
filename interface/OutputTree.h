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
#include <string>
#include <memory>

// helper: declares the pt/eta/phi/p4 of a variable OBJ
#define DECLARE_pt_eta_phi_p4(OBJ) \
    float OBJ ## _pt; \
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
    

        //////////////////////////
        //// saved variables
        //////////////////////////

        //////////////////----- reco H1 and H2
        DECLARE_pt_eta_phi_p4(H1)
        DECLARE_pt_eta_phi_p4(H2)

        //////////////////----- reco bs
        DECLARE_pt_eta_phi_p4(H1_b1)
        DECLARE_pt_eta_phi_p4(H1_b2)
        DECLARE_pt_eta_phi_p4(H2_b1)
        DECLARE_pt_eta_phi_p4(H2_b2)

        //////////////////----- gen level
        float gen_mHH;

        DECLARE_pt_eta_phi_p4(gen_H1)
        DECLARE_pt_eta_phi_p4(gen_H2)
        DECLARE_pt_eta_phi_p4(gen_H1_last)
        DECLARE_pt_eta_phi_p4(gen_H2_last)
        DECLARE_pt_eta_phi_p4(gen_H1_b1)
        DECLARE_pt_eta_phi_p4(gen_H1_b2)
        DECLARE_pt_eta_phi_p4(gen_H2_b1)
        DECLARE_pt_eta_phi_p4(gen_H2_b2)
        DECLARE_pt_eta_phi_p4(gen_q1_in)
        DECLARE_pt_eta_phi_p4(gen_q2_in)
        DECLARE_pt_eta_phi_p4(gen_q1_out)
        DECLARE_pt_eta_phi_p4(gen_q2_out)

    
    private:
        void init_branches();
        std::unique_ptr<TTree> tree_;
        const bool savetlv_;
};

#endif