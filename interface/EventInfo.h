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

#include "CompositeCandidate.h"
#include "Jet.h"
#include "GenPart.h"

struct EventInfo{
    boost::optional<GenPart> gen_H1;
    boost::optional<GenPart> gen_H2;
    boost::optional<GenPart> gen_H1_last;
    boost::optional<GenPart> gen_H2_last;

    boost::optional<GenPart> gen_H1_b1;
    boost::optional<GenPart> gen_H1_b2;
    boost::optional<GenPart> gen_H2_b1;
    boost::optional<GenPart> gen_H2_b2;


    // VBF incoming and outcoming partons
    // hard scatter gen particles
    boost::optional<GenPart> gen_q1_in;
    boost::optional<GenPart> gen_q2_in;
    boost::optional<GenPart> gen_q1_out;
    boost::optional<GenPart> gen_q2_out;
    // last copies after hadronisation
    // boost::optional<GenPart> gen_q1_out_lastcopy;
    // boost::optional<GenPart> gen_q2_out_lastcopy;

    boost::optional<CompositeCandidate> H1;
    boost::optional<CompositeCandidate> H2;

    boost::optional<Jet> H1_b1;
    boost::optional<Jet> H1_b2;
    boost::optional<Jet> H2_b1;
    boost::optional<Jet> H2_b2;

};

#endif