#include "SkimEffCounter.h"
using namespace std;

SkimEffCounter::SkimEffCounter()
{
    binMap_["Ntot_uw"] = 1;
    binMap_["Ntrg_uw"] = 2;
    binMap_["Nsel_uw"] = 3;
    binMap_["Ntot_w"]  = 4;

    binEntries_["Ntot_uw"] = 0.;
    binEntries_["Ntrg_uw"] = 0.;
    binEntries_["Nsel_uw"] = 0.;
    binEntries_["Ntot_w"]  = 0.;

}

void SkimEffCounter::updateProcessed (double evtW)
{
    binEntries_.at("Ntot_uw") += 1   ;
    binEntries_.at("Ntot_w")  += evtW;
}

void SkimEffCounter::updateTriggered ()
{
    binEntries_.at("Ntrg_uw") += 1;
}


void SkimEffCounter::updateSelected ()
{
    binEntries_.at("Nsel_uw") += 1;
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
