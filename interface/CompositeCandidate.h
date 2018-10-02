#ifndef COMPOSITECANDIDATE_H
#define COMPOSITECANDIDATE_H

/*
** class  : CompositeCandidate
** author : L. Cadamuro (UF)
** date   : 23/01/2018
** brief  : an object composed by a pair of Candidates
** note   : the candidate stores the combined P4 and keep reference of its components
**        : this reference is in the form of the two idx of the components, plus the two components themselves
**        : The CompositeCandidate own completely the two components and any copy operation also creates two new components
**        : This should not cause a problem as the == comparison between to Candidates is based on their index
**        : but be careful in case you want to compare the pointers
*/

#include "TLorentzVector.h"
#include "Candidate.h"
#include <utility>

class CompositeCandidate : public Candidate
{
    public:
        CompositeCandidate() : Candidate(),cand1_(), cand2_() {p4_.SetPxPyPzE(0,0,0,0); idx1_ = -1; idx2_ = -1;}
        // CompositeCandidate() :cand1_(), cand2_() {p4_.SetPxPyPzE(0,0,0,0); idx1_ = -1; idx2_ = -1;}
        CompositeCandidate(const Candidate& c1, const Candidate& c2) : Candidate(-1, nullptr) {setComponents(c1,c2);}
        // CompositeCandidate(const Candidate& c1, const Candidate& c2) {setComponents(c1,c2);}
        ~CompositeCandidate(){};
        CompositeCandidate(const CompositeCandidate& rhs); // copy ctor
        CompositeCandidate& operator = (const CompositeCandidate& rhs);        // assignment
        
        TLorentzVector P4() const      {return p4_;}
        
        void setComponents(const Candidate& c1, const Candidate& c2);

        int                getIdxComponent1 () const {return idx1_;}
        int                getIdxComponent2 () const {return idx2_;}
        std::pair<int,int> getIdxComponents () const {return std::make_pair(getIdxComponent1(), getIdxComponent2());}

        Candidate&                       getComponent1 () const {return (*cand1_);}
        Candidate&                       getComponent2 () const {return (*cand2_);}
        
        bool sharesComponentWith (const CompositeCandidate& cc) const;
        bool isValid() {return (cand1_ && cand2_);}
        std::unique_ptr<Candidate> clone() const {return std::unique_ptr<CompositeCandidate> (new CompositeCandidate(this->getComponent1 (), this->getComponent2 ()));}

        // void setP4( TLorentzVector p4) {p4_ = p4;}
        // int getIdx() const {return idx_;}
        // NanoAODTree* getNanoAODTree() const {return nat_;}

    protected:
        // virtual void buildP4(NanoAODTree* nat) = 0;
        // int idx_;
        void buildP4() {p4_ = cand1_->P4() + cand2_->P4();}; 
        // TLorentzVector p4_;
        int idx1_;
        int idx2_;
        std::unique_ptr<Candidate> cand1_;
        std::unique_ptr<Candidate> cand2_;
        // NanoAODTree* nat_;
};

#endif