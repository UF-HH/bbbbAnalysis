#ifndef FATJET_H
#define FATJET_H

#include "Candidate.h"

class FatJet : public Candidate
{
    public:
        FatJet() : Candidate(){}
        FatJet (int idx, NanoAODTree* nat) : Candidate(idx, nat){buildP4();}
        ~FatJet(){};
        std::unique_ptr<Candidate> clone() const {
        	FatJet *clonedFatJet = new FatJet(this->getIdx(), this->getNanoAODTree());
        	clonedFatJet->setP4(this->P4());
        	return std::unique_ptr<FatJet> (clonedFatJet);
        }

    private:
        void buildP4(); 
};

#endif