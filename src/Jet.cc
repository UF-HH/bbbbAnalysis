#include "Jet.h"

#include "BuildP4.h"

void Jet::buildP4UnRegressed(){
    p4UnRegressed_.BUILDP4(Jet, nat_);
}

void Jet::buildP4()
{

	if(p4UnRegressed_.Pt()==0.) this->buildP4UnRegressed();

	p4_.SetPtEtaPhiM(
            p4UnRegressed_.Pt()*get_property((*this),Jet_bRegCorr),
            p4UnRegressed_.Eta(),
            p4UnRegressed_.Phi(),
            p4UnRegressed_.M()
        );
}
