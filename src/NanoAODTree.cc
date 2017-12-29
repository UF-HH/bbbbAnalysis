#include "NanoAODTree.h"

bool NanoAODTree::Next()
{
    bool next = fReader.Next();
    if (!next) return next; // it's the end
    
    int t_tmp = fReader.GetTree()->GetTreeNumber();
    
    if (t_tmp != old_tree_nr_) // marks a chain transition
    {
        Long64_t entry = fReader.GetCurrentEntry();
        fReader.Restart();
        for (auto& rd : trg_reader_.getRefToReadersPtrVector())
            rd->Verify(fReader.GetTree());

        fReader.SetEntry(entry);
        old_tree_nr_ = t_tmp;
    }
    return next;
}