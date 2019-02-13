#include "Jet.h"
#include <iostream>
#include "BuildP4.h"

void Jet::buildP4(){
    p4_.BUILDP4(Jet, nat_);
}

void Jet::buildP4Regressed()
{
//    std::cout<<"Step1"<<std::endl;
	if(p4_.Pt()==0.) this->buildP4();
//    std::cout<<"Step2"<<std::endl;
//    std::cout<<this->getNanoAODTree()<<std::endl;
//    std::cout<<"Step2.1"<<std::endl;
//    std::cout<<this->getNanoAODTree()->Jet_bRegCorr.GetSize()<<std::endl;
//    std::cout<<"Step2.2"<<std::endl;
//    std::cout<<this->getIdx()<<std::endl;
//    std::cout<<"Step2.3"<<std::endl;
//    auto x = get_property((*this),Jet_bRegCorr);
//    std::cout<<"Step2.4: "<<x<<std::endl;
	p4Regressed_.SetPtEtaPhiM(
            p4_.Pt()*get_property((*this),Jet_bRegCorr),
            p4_.Eta(),
            p4_.Phi(),
            p4_.M()
        );
//    std::cout<<"Step3"<<std::endl;
}

