#ifndef READERARRAYPATCH_H
#define READERARRAYPATCH_H

/**
** class  : ReaderArrayPatch
** author : L. Cadamuro (UF)
** date   : 31/01/2018
** brief  : a class to patch the seg fault in ROOT experienced when using TTreeReaderValue/Array
**        : it implements the same interfaces as a TTreeReaderArray, but internally stores the [plain] array pointed
**        : in this way hopefully I can run on the NanoAOD without problems
**        :
**        : Note that the ReaderArray links the ReaderValue and expects it to be of type UInt_t
**/

// FIXME: how / where to check if the array size in the tree is > than the max_size at construction (sizeIsEnough)?
// just after calling GetEntry() in the tree?

#include "TTree.h"
#include "ReaderValuePatch.h"
#include <string>

template <typename T>
class ReaderArrayPatch {
    public:
        // ReaderArrayPatch (TTreeReader &tr, const char *branchname);
        ReaderArrayPatch (TTree* tree, const char *branchname, ReaderValuePatch<UInt_t>& rvp_arr_size, size_t max_size = 50);
        ~ReaderArrayPatch(){};
        bool sizeIsEnough() {return (GetSize() < max_size_);}
        // void Verify(TTree* tree);
        bool IsValid() {return (vals_ ? true : false);}
        void setBranchStatus(TTree* tree, bool val);
        size_t GetSize() const {return *(*rvp_arr_size_);}
        // just forwarding the TTreeReaderArray functions - the underlying unique_ptr will throw if not initialized
        // getting a default value does not make much sense for arrays
        T& At (size_t idx);
        T& operator[] (size_t idx){return this->At[idx];} 
        
        // begin and end not implemented yet
        // auto  begin ();
        // auto  end ();

    private:
        std::unique_ptr<T[]> vals_;
        ReaderValuePatch<UInt_t>* rvp_arr_size_;
        size_t max_size_;
        std::string branchname_;
};

template <typename T>
ReaderArrayPatch<T>::ReaderArrayPatch (TTree* tree, const char *branchname, ReaderValuePatch<UInt_t>& rvp_arr_size, size_t max_size)
{
    max_size_     = max_size;
    branchname_   = branchname;
    rvp_arr_size_ = &rvp_arr_size;
    if (!tree->GetListOfBranches()->FindObject(branchname_.c_str()))
        return; // will leave the unique ptr uninitialized and hence the reader invalid

    vals_         = std::unique_ptr<T[]> (new T[max_size_]);
    tree->SetBranchAddress(branchname_.c_str(), vals_.get());
}

// template <typename T>
// ReaderArrayPatch<T>::~ReaderArrayPatch ()
// {
//     // delete[] vals_;
// }

template <typename T>
T& ReaderArrayPatch<T>::At(size_t idx)
{
    if (!vals_)
        throw std::runtime_error ("Accessing the invalid branch: " + branchname_);

    // NOTE: this does not check/prevent that memory is *written* bt the TTree at indexes beyond the max array size
    if (idx > *(*rvp_arr_size_))
    {
        throw std::runtime_error (
            "Accessing the ReaderArrayPatch of branch " + branchname_
            + " at idx " + std::to_string(idx)
            + " while its size in tree is "
            + std::to_string(*(*rvp_arr_size_))
        );
    }
    return vals_.get()[idx];
}

template <typename T>
void ReaderArrayPatch<T>::setBranchStatus(TTree* tree, bool val)
{
    if (!IsValid()) // at construction this branch was not found, so nothing to do there
        return;
    tree->SetBranchStatus(branchname_.c_str(), val);
}

#endif