#include "SkimEffCounter.h"
using namespace std;

SkimEffCounter::SkimEffCounter()
{
    binMap_["Ntot_uw"] = 1;
    binMap_["Ntot_w"]  = 2;
    binMap_["Ntrg_uw"] = 3;
    binMap_["Ntrg_w"] = 4;
    binMap_["Nsel_uw"] = 5;
    binMap_["Nsel_w"] = 6;

    binEntries_["Ntot_uw"] = 0.;
    binEntries_["Ntot_w"]  = 0.;
    binEntries_["Ntrg_uw"] = 0.;
    binEntries_["Ntrg_w"] = 0.;
    binEntries_["Nsel_uw"] = 0.;
    binEntries_["Nsel_w"] = 0.;

}

void SkimEffCounter::updateProcessed (double evtW)
{
    binEntries_.at("Ntot_uw") += 1   ;
    binEntries_.at("Ntot_w")  += evtW;
}

void SkimEffCounter::updateTriggered (double evtW)
{
    binEntries_.at("Ntrg_uw") += 1;
    binEntries_.at("Ntrg_w")  += evtW;
}


void SkimEffCounter::updateSelected (double evtW)
{
    binEntries_.at("Nsel_uw") += 1;
    binEntries_.at("Nsel_w")  += evtW;
}

void SkimEffCounter::createHisto()
{
    if (eff_histo_)
    {
        cout << "[WARNING] SkimEffCounter : createHisto : histogram already exists, cannot create a new one" << endl;
        return;
    }
    eff_histo_ = std::unique_ptr<TH1D> (new TH1D("eff_histo", "eff_histo", binMap_.size(), 0, binMap_.size()));

    for( const auto & element : binMap_)
    {
        eff_histo_->SetBinContent(element.second, binEntries_.at(element.first));
        eff_histo_->GetXaxis()->SetBinLabel(element.second, element.first.data());
    }
}

int SkimEffCounter::write()
{
    if (!eff_histo_)
        createHisto();

    return eff_histo_->Write();
}
