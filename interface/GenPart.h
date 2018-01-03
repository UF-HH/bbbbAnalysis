#ifndef GENPART_H
#define GENPART_H

#include "Candidate.h"
#include <map>

class GenPart : public Candidate
{
    public:
        GenPart (int idx, NanoAODTree* nat) : Candidate(idx, nat){buildP4(nat);}
        ~GenPart(){};
    private:
        void buildP4(NanoAODTree* nat); 
        static const std::map<int, float> gen_mass_;
};

#endif