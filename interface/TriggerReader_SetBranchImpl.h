#ifndef TRIGGERREADER_SETBRANCHIMPL_H
#define TRIGGERREADER_SETBRANCHIMPL_H

/*
** class   : TriggerReader_SetBranchImpl
** author  : L.Cadamuro (UF)
** date    : 01/02/2017
** brief   : interface to select and read trigger decision in NanoaOD
** note    : readers_ is a vector<unique_ptr<ReaderValuePatch>>, so that
**           readers_.at(i) returns a unique_ptr
**           *(readers_.at(i)) returns a ReaderValuePatch
**           *(*(readers_.at(i))) returns the true/false value
**           as syntax is very error-prone, a couple of helper functions to get the readers and the results directly are stored as private members
*/

#include <string>
#include <memory>
#include <map>
#include "ReaderValuePatch.h"

class TriggerReader_SetBranchImpl {
    public:
        TriggerReader_SetBranchImpl (TTree* tree);
        ~TriggerReader_SetBranchImpl(){};
        void addTrigger(std::string trgName);
        void setTriggers(std::vector<std::string> trgNames);
        std::vector<std::unique_ptr<ReaderValuePatch<bool>>>& getRefToReadersPtrVector(){return readers_;}
        bool getTrgOr();
        bool getTrgResult(std::string trgName);
        std::vector<std::string> getTrgPassed();

    private:
        ReaderValuePatch<bool>& getRefToReader(int idx) {return *(readers_.at(idx));}
        bool getTrgResult(int idx) {return *getRefToReader(idx);}

        std::vector<std::unique_ptr<ReaderValuePatch<bool>>> readers_;
        std::map<std::string, int> name_to_idx_;
        TTree* tree_;
};

#endif