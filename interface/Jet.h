#ifndef JET_H
#define JET_H

#include "Candidate.h"

class Jet : public Candidate
{
    public:
        Jet () : Candidate(){}
        Jet (int idx, NanoAODTree* nat) : Candidate(idx, nat){
            buildP4UnRegressed();
            buildP4(); 
        }
        ~Jet(){};
        std::unique_ptr<Candidate> clone() const {
        	Jet *clonedJet = new Jet(this->getIdx(), this->getNanoAODTree());
        	clonedJet->setP4UnRegressed(this->P4UnRegressed());
            clonedJet->setP4(this->P4());
        	return std::unique_ptr<Jet> (clonedJet);
        }
        TLorentzVector P4UnRegressed() const      {return p4UnRegressed_;}
        void setP4UnRegressed( TLorentzVector p4UnRegressed_) {p4UnRegressed_ = p4UnRegressed_;}
        

    private:
        void buildP4UnRegressed(); 
        void buildP4(); 
        TLorentzVector p4UnRegressed_;

};

#endif