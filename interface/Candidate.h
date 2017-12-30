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

#define get_property(OBJ, NAME) OBJ.getNanoAODTree() -> NAME .At(OBJ.getIdx())

class Candidate
{
    public:
        Candidate(int idx, NanoAODTree* nat){idx_ = idx; nat_ = nat;}
        ~Candidate(){};
        TLorentzVector P4() const      {return p4_;}
        void setP4( TLorentzVector p4) {p4_ = p4;}
        int getIdx() const {return idx_;}
        NanoAODTree* getNanoAODTree() const {return nat_;}

    protected:
        virtual void buildP4(NanoAODTree* nat) = 0;
        int idx_;
        TLorentzVector p4_;
        NanoAODTree* nat_;
};

#endif