#include "CompositeCandidate.h"

void CompositeCandidate::setComponents(const Candidate& c1, const Candidate& c2)
{
    // p4_ = (c1.P4() + c2.P4());
    idx1_ = c1.getIdx();
    idx2_ = c2.getIdx();
    cand1_ = c1.clone();
    cand2_ = c2.clone();
    buildP4();
    return;
}

bool CompositeCandidate::sharesComponentWith(const CompositeCandidate& cc) const
{
    if (this->getIdxComponent1() == cc.getIdxComponent1()) return true;
    if (this->getIdxComponent1() == cc.getIdxComponent2()) return true;
    if (this->getIdxComponent2() == cc.getIdxComponent1()) return true;
    if (this->getIdxComponent2() == cc.getIdxComponent2()) return true;
    return false;
}

CompositeCandidate::CompositeCandidate(const CompositeCandidate& rhs)
{
    this -> p4_ = rhs.p4_;
    this -> idx1_ = rhs.idx1_;
    this -> idx2_ = rhs.idx2_;
    this -> cand1_ = rhs.cand1_->clone();
    this -> cand2_ = rhs.cand2_->clone();
}

CompositeCandidate& CompositeCandidate::operator = (const CompositeCandidate& rhs)
{
    this -> p4_ = rhs.p4_;
    this -> idx1_ = rhs.idx1_;
    this -> idx2_ = rhs.idx2_;
    this -> cand1_ = rhs.cand1_->clone();
    this -> cand2_ = rhs.cand2_->clone();
    return (*this);
}
