#ifndef JET_H
#define JET_H

#include "Candidate.h"

class Jet : public Candidate
{
    public:
        Jet () : Candidate(){}
        Jet (int idx, NanoAODTree* nat) : Candidate(idx, nat){buildP4();}
        ~Jet(){};
        std::unique_ptr<Candidate> clone() const {
        	Jet *clonedJet = new Jet(this->getIdx(), this->getNanoAODTree());
        	clonedJet->setP4(this->P4());
        	return std::unique_ptr<Jet> (clonedJet);
        }
    private:
        void buildP4(); 
};

#endif