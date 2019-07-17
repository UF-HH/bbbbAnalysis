#ifndef BDTEVAL_H
#define BDTEVAL_H

/*
** class  : BDTEval
** author : L. Cadamuro (UF)
** brief  : interface for the evaluation of a BDT
** note   : the class internally stores the float and integer variables used for the BDT evaluation
**          in case more variables are , extra types can be added (e.g. double) to match the BDT needs
*           IMPORTANT: after creation in ctor, vectors have not to be modified (apadrt from assignment in eval() ), or the link to input variables will be broken
*/

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

#include "TMVA/GeneticAlgorithm.h"
#include "TMVA/GeneticFitter.h"
#include "TMVA/IFitterTarget.h"
#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Reader.h"

class BDTEval {

    public:
        BDTEval(std::vector<std::string> floatVarsList, std::vector<std::string> intVarsList);
        ~BDTEval();
        void init(std::string method, std::string weights);
        float eval(); 

        // public, so that the user can access these maps and fill them with the needed values
        // NOTE: it's up to the user to remember refreshing the variables before calling eval()
        std::unordered_map<std::string, float> floatVarsMap;
        std::unordered_map<std::string, int>   intVarsMap;

    private:
        std::vector<std::string> floatVarsList_;
        std::vector<std::string> intVarsList_;

        std::vector<float> floatVars_;
        std::vector<int>   intVars_;

        std::unique_ptr<TMVA::Reader> reader_;
        std::string method_;
        std::string weights_;
};

#endif