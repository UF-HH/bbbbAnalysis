#ifndef EVENTINFO_H
#define EVENTINFO_H

/**
** class  : EventInfo
** author : L. Cadamuro (UF)
** date   : 10/01/2018
** brief  : Struct that contains all the inforrmation elaborated during the skim
**          (e.g., which jets are selected, whic triggers fire etc...)
**          each object is wrapped in a boost::optional class so that it can be autmatically unitialized
**/

#include <boost/optional.hpp>

#include "Jet.h"

struct EventInfo{
    boost::optional<int> idx_genH1;
    boost::optional<int> idx_genH2;

    boost::optional<Jet> jet_H1_1;
    boost::optional<Jet> jet_H1_2;
    boost::optional<Jet> jet_H2_1;
    boost::optional<Jet> jet_H2_2;

};

#endif