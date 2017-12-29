#include "TriggerReader.h"

TriggerReader::TriggerReader(TTreeReader* reader) : main_reader_(reader)
{}

void TriggerReader::addTrigger(std::string trgName)
{
    // skip if name is in the trigger list
    if (name_to_idx_.find(trgName) != name_to_idx_.end())
    {
        std::cout << "** INFO: TriggerReader : trigger " << trgName << " is already set, skipping" << std::endl;
        return;
    }

    std::unique_ptr<NanoReaderValue<bool>> readr (new NanoReaderValue<bool>(*main_reader_, trgName.c_str()));
    readr->SetReturnDefault(true, false); // if branch does not exist, return false
    readers_.push_back(std::move(readr));
    name_to_idx_[trgName] = readers_.size()-1;
}

void TriggerReader::setTriggers(std::vector<std::string> trgNames)
{
    for (std::string& s : trgNames)
        addTrigger(s);
}

bool TriggerReader::getTrgOr()
{
    bool the_or = false;
    for (auto& rd_ptr : readers_)
        the_or = (the_or || (*(rd_ptr->Get())) );
    return the_or;
}

bool TriggerReader::getTrgResult(std::string trgName)
{
    if (name_to_idx_.find(trgName) == name_to_idx_.end())
    {
        std::cout << "** WARNING: TriggerReader : the requested trigger " << trgName << " was not declared, returning false" << std::endl;
        return false;
    }

    int idx = name_to_idx_[trgName];
    return getTrgResult(idx);
}
