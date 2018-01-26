#ifndef CANDIDATE_H
#define CANDIDATE_H

/*
** class  : Candidate
** author : L. Cadamuro (UF)
** date   : 30/12/2017
** brief  : lightweight class representing a general physics object in nanoAOD.
**          Each object has an index (in its respective parent vector collection) and a p4 built from it
** note   : the macro get_property simulates an introspection by giving access to a user defined object property
**          through an access from the NanoAODTree
**          The #define ensures it is automatically propagated to all codes using Candidate or its derived
*/

#include "TLorentzVector.h"
#include "NanoAODTree.h"
#include <memory>

#define get_property(OBJ, NAME) (OBJ.isValid() ?  OBJ.getNanoAODTree() -> NAME .At(OBJ.getIdx()) : throw std::runtime_error("get property " #NAME " from invalid object"))

class Candidate
{
    public:
        Candidate(){nat_ = nullptr; idx_ = -1;} // creates an invalid Candidate
        Candidate(int idx, NanoAODTree* nat){idx_ = idx; nat_ = nat;} // standard ctor to be used for NanoAODTree inspection
        ~Candidate(){};
        
        TLorentzVector P4() const      {return p4_;}
        void setP4( TLorentzVector p4) {p4_ = p4;}
        
        int getIdx() const {return idx_;}
        NanoAODTree* getNanoAODTree() const {return nat_;}
        
        bool isValid() const {return idx_ >= 0;}
        virtual std::unique_ptr<Candidate> clone() const = 0;

    protected:
        virtual void buildP4(NanoAODTree* nat) = 0;
        int idx_;
        TLorentzVector p4_;
        NanoAODTree* nat_;
};

#endif