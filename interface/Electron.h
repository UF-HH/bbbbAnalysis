#ifndef ELECTRON_H
#define ELECTRON_H

#include "Candidate.h"

class Electron : public Candidate
{
    public:
        Electron () : Candidate(){}
        Electron (int idx, NanoAODTree* nat) : Candidate(idx, nat){buildP4();}
        ~Electron(){};
        std::unique_ptr<Candidate> clone() const {
        	Electron *clonedElectron = new Electron(this->getIdx(), this->getNanoAODTree());
        	clonedElectron->setP4(this->P4());
        	return std::unique_ptr<Electron> (clonedElectron);
        }
    private:
        void buildP4(); 
};

#endif