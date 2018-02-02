#ifndef READERVALUEPATCH_H
#define READERVALUEPATCH_H

/**
** class  : ReaderValuePatch
** author : L. Cadamuro (UF)
** date   : 31/01/2018
** brief  : a class to patch the seg fault in ROOT experienced when using TTreeReaderValue/Array
**        : it implements the same interfaces as a TTreeReaderArray, but internally stores the value pointed
**        : in this way hopefully I can run on the NanoAOD without problems
**/

#include "TTree.h"
#include <string>
#include <iostream>

template <typename T>
class ReaderValuePatch {
    public:
        ReaderValuePatch(TTree* tree, const char *branchname);
        ~ReaderValuePatch(){}
        T* Get          ();
        T& operator *   () {return *Get();}
        T* operator ->  () {return Get();}
        bool IsValid() {return is_valid_;}
        void SetReturnDefault(bool ret_do, T ret_val = T()) {do_return_default_ = ret_do; val_return_default_ = ret_val;}
        void setBranchStatus(TTree* tree, bool val);
        void Verify(TTree* tree);


    private:
        T value_;
        std::string branchname_;
        bool is_valid_;
        bool do_return_default_;
        T    val_return_default_;

};

template <typename T>
ReaderValuePatch<T>::ReaderValuePatch (TTree* tree, const char *branchname)
{
    branchname_ = branchname;
    do_return_default_ = false;
    val_return_default_ = T();
    
    if (!tree->GetListOfBranches()->FindObject(branchname_.c_str())){
        is_valid_ = false;
        return;
    }

    is_valid_ = true;
    tree->SetBranchAddress(branchname_.c_str(), &(this->value_));
}

template <typename T>
T* ReaderValuePatch<T>::Get()
{
    
    if (!IsValid()){
        if (do_return_default_)
            return &val_return_default_;
        else{
            std::cerr << "** ReaderValuePatch::Get() : [ERROR] reader of branch " << branchname_
                      << " is currently not valid, aborting (you can use SetReturnDefault(true, T ret_val) to set a default return value)" << std::endl;
            throw std::runtime_error ("ReaderValuePatch not configured");
        }
    }
    return &value_;
}

template <typename T>
void ReaderValuePatch<T>::setBranchStatus(TTree* tree, bool val)
{
    if (IsValid()) // only makes sense if the branch exists
        tree->SetBranchStatus(branchname_.c_str(), val);
}

template <typename T>
void ReaderValuePatch<T>::Verify (TTree* tree)
{

    // std::cout << "ReaderValuePatch :: I am verifying value : " << branchname_ << std::endl;

    if (tree->GetListOfBranches()->FindObject(branchname_.c_str())) {
        if (!IsValid()) // not yet initialized
        {   
            tree->SetBranchAddress(branchname_.c_str(), &(this->value_));
            is_valid_ = true;
        }
    }

    else {
        if (IsValid()) // branch disappeared, thus disable reader
            is_valid_ = false;
    }

}

#endif