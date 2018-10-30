#ifndef MUON_H
#define MUON_H

#include "Candidate.h"

class Muon : public Candidate
{
    public:
        Muon () : Candidate(){}
        Muon (int idx, NanoAODTree* nat) : Candidate(idx, nat){buildP4();}
        ~Muon(){};
        std::unique_ptr<Candidate> clone() const {
        	Muon *clonedMuon = new Muon(this->getIdx(), this->getNanoAODTree());
        	clonedMuon->setP4(this->P4());
        	return std::unique_ptr<Muon> (clonedMuon);
        }
    private:
        void buildP4(); 
};

#endif