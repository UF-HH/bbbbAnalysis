#include "CompositeCandidate.h"

void CompositeCandidate::setComponents(const Candidate& c1, const Candidate& c2)
{
    cand1_ = c1.clone();
    cand2_ = c2.clone();
    buildP4();

    if(c1.getIsComposite()) for(const int & id : c1.getIdxParents()) parentIdxVector_.emplace_back(id);
    else parentIdxVector_.emplace_back(c1.getIdx());

    if(c2.getIsComposite()) for(const int & id : c2.getIdxParents()) parentIdxVector_.emplace_back(id);
    else parentIdxVector_.emplace_back(c2.getIdx());
    
    return;
}

bool CompositeCandidate::sharesComponentWith(const CompositeCandidate& cc) const
{
    std::vector<int> thisParentVector = this->getIdxParents();
    std::vector<int> ccParentVector = cc.getIdxParents();

    for(int thisParent : thisParentVector){
        for(int ccParent : ccParentVector){
            if(thisParent==ccParent) return true;
        }
    }

    return false;

}


CompositeCandidate::CompositeCandidate(const CompositeCandidate& rhs)
{
    
    this -> p4_ = rhs.p4_;
    this -> isComposite_ = rhs.isComposite_;
    this -> parentIdxVector_ = rhs.parentIdxVector_;
    this -> cand1_ = rhs.cand1_->clone();
    this -> cand2_ = rhs.cand2_->clone();
}

CompositeCandidate& CompositeCandidate::operator = (const CompositeCandidate& rhs)
{
    this -> p4_ = rhs.p4_;
    this -> isComposite_ = rhs.isComposite_;
    this -> parentIdxVector_ = rhs.parentIdxVector_;
    this -> cand1_ = rhs.cand1_->clone();
    this -> cand2_ = rhs.cand2_->clone();
    return (*this);
}
