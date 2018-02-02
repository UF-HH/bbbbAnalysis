#include "NanoAODTree_SetBranchImpl.h"

bool NanoAODTree_SetBranchImpl::Next()
{
    ++nEv_;
    int got = chain_->GetEntry(nEv_);
    if (got == 0)
        return false;

    int t_tmp = chain_->GetTreeNumber();

    if (t_tmp != old_tree_nr_) // marks a chain transition
    {
        for (auto& rd : trg_reader_.getRefToReadersPtrVector())
            rd->Verify(chain_);

        chain_->GetEntry(nEv_); // get again the entry to reload the new branches
        old_tree_nr_ = t_tmp;
    }

    return true;
}
