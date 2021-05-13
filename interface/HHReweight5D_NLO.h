#ifndef HHREWEIGHT5D_NLO_H
#define HHREWEIGHT5D_NLO_H

// wrapper to the NLO reweighting class discussed here
// https://gitlab.cern.ch/hh/eft-benchmarks#reweight-procedure

#include "ReweightMandrik.h"
#include <memory>
#include "TH2.h"
#include <string>
#include <vector>
#include <utility>

class HHReweight5D_NLO{

    public:
        HHReweight5D_NLO(){}
        HHReweight5D_NLO(std::string coeffFileLO, std::string coeffFileNLO, const TH2* hInput, bool useAbsEta=true) {init(coeffFileLO, coeffFileNLO, hInput, useAbsEta);}
        ~HHReweight5D_NLO(){}
        void init(std::string coeffFileLO, std::string coeffFileNLO, const TH2* hInput, bool useAbsEta=true);

        // return the weight to be applied for the reweight
        // NOTE: normalization is arbitrary you'll have to scale by the sum of weights
        double getWeight(double kl, double kt, double c2, double cg, double c2g, double mhh, double cth);
        std::pair<int,int> find2DBin(TH2* h, double x, double y);

    private:
        std::unique_ptr<ReweightMandrik> rm_pw_NLO_;
        std::shared_ptr<TH2> h_input_;
        bool useAbsEta_;

        double nEvtTot_;
};

#endif