#include "HHReweight5D_NLO.h"
#include <iostream>

void HHReweight5D_NLO::init(std::string coeffFileLO, std::string coeffFileNLO, const TH2* hInput, bool useAbsEta)
{
    // clone the input histogram
    TH2* cloneH = (TH2*) hInput->Clone("h_input");
    cloneH->SetDirectory(0);
    h_input_.reset(cloneH);

    useAbsEta_ = useAbsEta;

    cout << "[INFO] initializing the NLO reweighting with the following parameters" << endl;
    cout << " ... LO  coeff file (unused) : " << coeffFileLO << endl;
    cout << " ... NLO  coeff file :  " << coeffFileNLO << endl;
    cout << " ... use abs eta? :  " << useAbsEta_ << endl;
    cout << " ... input histo name :  " << hInput->GetName() << "(stored as " << h_input_->GetName() << ")" << endl;

    rm_pw_NLO_ = std::make_unique<ReweightMandrik> ("", coeffFileLO, coeffFileNLO);

    nEvtTot_ = h_input_->Integral();
}

// return the weight to be applied for the reweight
// NOTE: normalization is arbitrary you'll have to scale by the sum of weights
double HHReweight5D_NLO::getWeight(double kl, double kt, double c2, double cg, double c2g, double mhh, double cth)
{
    if (useAbsEta_)
        cth = std::abs(cth);

    auto bins = find2DBin(h_input_.get(), mhh, cth);
    double old_frac = h_input_->GetBinContent(bins.first, bins.second)/nEvtTot_;

    // kl, kt, c2, cg, c2g
    double bsm_bin_xs = rm_pw_NLO_->GetDiffXsection(mhh, cth, {kl, kt, c2, cg, c2g}, "nlo") / 1000.;//diff XS in [fb]
    // scale up by the bin width
    double bw_2d = h_input_->GetXaxis()->GetBinWidth(bins.first) * h_input_->GetYaxis()->GetBinWidth(bins.second);

    // since the weight is arbitrarily normalized, there is no need to scale by the total benchmark xs
    return bsm_bin_xs*bw_2d/old_frac;
}

// return bin in 2D isto wihtout under/over flow (e.g. if ibin > ibinmax , ibin = ibinmax)
pair<int,int> HHReweight5D_NLO::find2DBin(TH2* h, double x, double y)
{
    int ibinx = h->GetXaxis()->FindBin(x);
    int ibiny = h->GetYaxis()->FindBin(y);

    if (ibinx <= 0) ibinx = 1;
    if (ibinx > h->GetNbinsX()) ibinx = h->GetNbinsX();

    if (ibiny <= 0) ibiny = 1;
    if (ibiny > h->GetNbinsY()) ibiny = h->GetNbinsY();

    return make_pair(ibinx, ibiny);

}

