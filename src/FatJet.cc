#include "FatJet.h"

#include "BuildP4.h"

void FatJet::buildP4()
{
    p4_.BUILDP4(FatJet, nat_);
}