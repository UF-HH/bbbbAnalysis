#ifndef FATJET_H
#define FATJET_H

#include "Candidate.h"

class FatJet : public Candidate
{
    public:
        FatJet (int idx, NanoAODTree* nat) : Candidate(idx, nat){buildP4(nat);}
        ~FatJet(){};
    private:
        void buildP4(NanoAODTree* nat); 
};

#endif