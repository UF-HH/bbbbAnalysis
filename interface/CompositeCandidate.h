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
        CompositeCandidate() : Candidate(),cand1_(), cand2_() {p4_.SetPxPyPzE(0,0,0,0); isComposite_=true;}
        CompositeCandidate(const Candidate& c1, const Candidate& c2) : Candidate() {setComponents(c1,c2); isComposite_=true;}

        ~CompositeCandidate(){};
        CompositeCandidate(const CompositeCandidate& rhs); // copy ctor
        CompositeCandidate& operator = (const CompositeCandidate& rhs);        // assignment
        
        void setComponents(const Candidate& c1, const Candidate& c2);

        Candidate& getComponent1 () const {
            return (*cand1_);
        }
        Candidate& getComponent2 () const {
            return (*cand2_);
        }

        bool sharesComponentWith (const CompositeCandidate& cc) const;
        bool isValid() {return (cand1_ && cand2_);}
        std::unique_ptr<Candidate> clone() const {
            std::unique_ptr<CompositeCandidate> CompositeCandidateClone(new CompositeCandidate(this->getComponent1 (), this->getComponent2 ()));
            CompositeCandidateClone->setP4(this->P4()); //In case the P4 had been re-evaluated
            return CompositeCandidateClone;
        }
        void rebuildP4UsingRegressedPt(bool usePtRegressedCandidate1, bool usePtRegressedCandidate2);

    protected:
        void buildP4() {p4_ = cand1_->P4() + cand2_->P4();}; 
        std::unique_ptr<Candidate> cand1_;
        std::unique_ptr<Candidate> cand2_;
};

#endif