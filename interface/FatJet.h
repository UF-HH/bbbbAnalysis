#ifndef FATJET_H
#define FATJET_H

#include "Candidate.h"

class FatJet : public Candidate
{
    public:
        FatJet() : Candidate(){}
        FatJet (int idx, NanoAODTree* nat) : Candidate(idx, nat){buildP4(nat);}
        ~FatJet(){};
        std::unique_ptr<Candidate> clone() const {return std::unique_ptr<FatJet> (new FatJet(this->getIdx(), this->getNanoAODTree()));}
    private:
        void buildP4(NanoAODTree* nat); 
};

#endif