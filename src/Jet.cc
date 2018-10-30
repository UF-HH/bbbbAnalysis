#include "Jet.h"

#include "BuildP4.h"

void Jet::buildP4()
{
    p4_.BUILDP4(Jet, nat_);
}