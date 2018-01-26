#ifndef JET_H
#define JET_H

#include "Candidate.h"

class Jet : public Candidate
{
    public:
        Jet () : Candidate(){}
        Jet (int idx, NanoAODTree* nat) : Candidate(idx, nat){buildP4(nat);}
        ~Jet(){};
        std::unique_ptr<Candidate> clone() const {return std::unique_ptr<Jet> (new Jet(this->getIdx(), this->getNanoAODTree()));}
    private:
        void buildP4(NanoAODTree* nat); 
};

#endif