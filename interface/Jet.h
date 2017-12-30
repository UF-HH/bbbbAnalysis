#ifndef JET_H
#define JET_H

#include "Candidate.h"

class Jet : public Candidate
{
    public:
        Jet (int idx, NanoAODTree* nat) : Candidate(idx, nat){buildP4(nat);}
        ~Jet(){};
    private:
        void buildP4(NanoAODTree* nat); 
};

#endif