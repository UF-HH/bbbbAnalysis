#include "BDTEval.h"

BDTEval::BDTEval(std::vector<std::string> floatVarsList, std::vector<std::string> intVarsList)
{
    floatVarsList_ = floatVarsList;
    intVarsList_   = intVarsList;

    floatVars_.resize(floatVarsList_.size());
    intVars_.resize(intVarsList_.size());

    reader_  = std::unique_ptr<TMVA::Reader> (new TMVA::Reader("!Color:Silent"));

    // add the variables - floats
    for (uint iv = 0; iv < floatVarsList_.size(); ++iv)
    {
        std::string vname = floatVarsList_.at(iv);
        reader_->AddVariable(vname.c_str(), &(floatVars_.at(iv)) );        
    }

    // add the variables - integers
    for (uint iv = 0; iv < intVarsList_.size(); ++iv)
    {
        std::string vname = intVarsList_.at(iv);
        reader_->AddVariable(vname.c_str(), &(intVars_.at(iv)) );        
    }

    // init the maps for the input variables
    for (std::string s : floatVarsList_)
        floatVarsMap [s] = 0.0; 
    for (std::string s : intVarsList_)    
        intVarsMap   [s] = 0;
}

void BDTEval::init(std::string method, std::string weights)
{
    method_  = method;
    weights_ = weights;
    reader_->BookMVA(method_.c_str(), weights.c_str());
}

BDTEval::~BDTEval()
{
    floatVarsMap.clear();
    intVarsMap.clear();
}

float BDTEval::eval()
{
    // copy the values from the buffer map to the vector that is attached to the reader
    for (uint iv = 0; iv < floatVarsList_.size(); ++iv)
    {
        std::string vname = floatVarsList_.at(iv);
        floatVars_.at(iv) = floatVarsMap.at(vname);
    }

    for (uint iv = 0; iv < intVarsList_.size(); ++iv)
    {
        std::string vname = intVarsList_.at(iv);
        intVars_.at(iv) = intVarsMap.at(vname);
    }

    return reader_->EvaluateMVA(method_.c_str());
}
