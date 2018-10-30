#include "SkimEffCounter.h"
using namespace std;

SkimEffCounter::SkimEffCounter()
{
    // extend with more bins if needed
    // eff_histo_ = std::unique_ptr<TH1D> (new TH1D("eff_histo", "eff_histo", 4, 0, 4));
    
    // eff_histo_->GetXaxis()->SetBinLabel(kNtot,            "Ntot");
    // eff_histo_->GetXaxis()->SetBinLabel(kNtot_unweighted, "Ntot_unweighted");
    // eff_histo_->GetXaxis()->SetBinLabel(kNsel,            "Nsel");
    // eff_histo_->GetXaxis()->SetBinLabel(kNsel_unweighted, "Nsel_unweighted");

    Ntot_w     = 0.0;
    Ntot_uw    = 0;
    Ntrg_w     = 0.0;
    Ntrg_uw    = 0;
    Nsel_w     = 0.0;
    Nsel_uw    = 0;
}

void SkimEffCounter::updateProcessed (double evtW)
{
    Ntot_w  += evtW;
    Ntot_uw += 1;
}

void SkimEffCounter::updateTriggered (double evtW)
{
    Ntrg_w  += evtW;
    Ntrg_uw += 1;
}


void SkimEffCounter::updateSelected (double evtW)
{
    Nsel_w  += evtW;
    Nsel_uw += 1;
}

void SkimEffCounter::createHisto()
{
    if (eff_histo_)
    {
        cout << "[WARNING] SkimEffCounter : createHisto : histogram already exists, cannot create a new one" << endl;
        return;
    }
    eff_histo_ = std::unique_ptr<TH1D> (new TH1D("eff_histo", "eff_histo", 6, 0, 6));

    eff_histo_->SetBinContent(kNtot_w,    (double) getSumWProcessed());
    eff_histo_->SetBinContent(kNtot_uw,   (double) getNProcessed());
    eff_histo_->SetBinContent(kNtrg_w,    (double) getSumWTriggered());
    eff_histo_->SetBinContent(kNtrg_uw,   (double) getNTriggered());
    eff_histo_->SetBinContent(kNsel_w,    (double) getSumWSelected());
    eff_histo_->SetBinContent(kNsel_uw,   (double) getNSelected());
    
    eff_histo_->GetXaxis()->SetBinLabel(kNtot_w,    "sumW_processed");
    eff_histo_->GetXaxis()->SetBinLabel(kNtot_uw,   "N_processed");
    eff_histo_->GetXaxis()->SetBinLabel(kNtrg_w,    "sumW_triggered");
    eff_histo_->GetXaxis()->SetBinLabel(kNtrg_uw,   "N_triggered");
    eff_histo_->GetXaxis()->SetBinLabel(kNsel_w,    "sumW_selected");
    eff_histo_->GetXaxis()->SetBinLabel(kNsel_uw,   "N_selected");
}

int SkimEffCounter::write()
{
    if (!eff_histo_)
        createHisto();

    return eff_histo_->Write();
}
