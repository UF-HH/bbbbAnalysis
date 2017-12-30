#include "FatJet.h"

#include "BuildP4.h"

void FatJet::buildP4(NanoAODTree* nat)
{
    p4_.BUILDP4(FatJet, nat);
}