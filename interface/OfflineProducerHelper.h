#ifndef OFFLINEPRODUCERHELPER_H
#define OFFLINEPRODUCERHELPER_H

/**
** class  : OfflineProducerHelper
** author : L. Cadamuro (UF)
** date   : 10/01/2018
** brief  : A namespace containing a set of helper functions to analyze the events and process the skims
** note   : the prototype of the general function accepts as first input the NanoAODTree 
**          and as the second a reference to EventInto.
**          It's a flexible way to let the function know what it should know on the event, and possibly set extra information
**/

#include "NanoAODTree.h"
#include "EventInfo.h"

namespace OfflineProducerHelper {
    bool select_bbbb_jets(NanoAODTree& nat, EventInfo& ei);
};

#endif