#include "OfflineProducerHelper.h"
#include <iostream>

using namespace std;

bool OfflineProducerHelper::select_bbbb_jets(NanoAODTree& nat, EventInfo& ei)
{
    if (*(nat.nJet) < 4)
        return false;

    std::vector<Jet> jets;
    jets.reserve(*(nat.nJet));
    for (uint ij = 0; ij < *(nat.nJet); ++ij)
        jets.push_back(Jet(ij, &nat));

    sort(jets.begin(), jets.end(), [](const Jet & a, const Jet & b) -> bool
    {
        return ( get_property(a, Jet_btagCSVV2) < get_property(b, Jet_btagCSVV2) );
    }); // sort by CSV (lowest to highest)

    ei.jet_H1_1 = *(jets.rbegin()+0);
    ei.jet_H1_2 = *(jets.rbegin()+1);
    ei.jet_H2_1 = *(jets.rbegin()+2);
    ei.jet_H2_2 = *(jets.rbegin()+3);

    return true;
}