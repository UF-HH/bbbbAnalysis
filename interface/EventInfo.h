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

    boost::optional<unsigned int>           Run;
    boost::optional<unsigned int>           LumiSec;
    boost::optional<unsigned long long int> Event;

    boost::optional<GenPart> gen_H1;
    boost::optional<GenPart> gen_H2;
    boost::optional<GenPart> gen_H1_last;
    boost::optional<GenPart> gen_H2_last;

    boost::optional<GenPart> gen_H1_b1;
    boost::optional<int>     gen_H1_b1_matchedflag;
    boost::optional<GenPart> gen_H1_b2;
    boost::optional<int>     gen_H1_b2_matchedflag;
    boost::optional<GenPart> gen_H2_b1;
    boost::optional<int>     gen_H2_b1_matchedflag;
    boost::optional<GenPart> gen_H2_b2;
    boost::optional<int>     gen_H2_b2_matchedflag;


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
    boost::optional<float> H1_bb_DeltaR;
    boost::optional<CompositeCandidate> H2;
    boost::optional<float> H2_bb_DeltaR;
    boost::optional<CompositeCandidate> HH;
    boost::optional<float> HH_2DdeltaM;
    boost::optional<float> HH_m_kinFit;

    boost::optional<Jet> H1_b1;
    boost::optional<Jet> H1_b2;
    boost::optional<Jet> H2_b1;
    boost::optional<Jet> H2_b2;

    //Non-resonant analysis and studies
    boost::optional<Jet> JJ_j1;
    boost::optional<int> JJ_j1_quarkflag;    
    boost::optional<int> JJ_j1_matchedflag;
    boost::optional<Jet> JJ_j2;   
    boost::optional<int> JJ_j2_quarkflag;
    boost::optional<int> JJ_j2_matchedflag;
    boost::optional<Jet> HH_b1;
    boost::optional<int> HH_b1_quarkflag;
    boost::optional<int> HH_b1_matchedflag;    
    boost::optional<Jet> HH_b2;   
    boost::optional<int> HH_b2_quarkflag;
    boost::optional<int> HH_b2_matchedflag;    
    boost::optional<Jet> HH_b3;
    boost::optional<int> HH_b3_quarkflag; 
    boost::optional<int> HH_b3_matchedflag;       
    boost::optional<Jet> HH_b4;
    boost::optional<int> HH_b4_quarkflag;
    boost::optional<int> HH_b4_matchedflag;  
    boost::optional<CompositeCandidate> JJ;
    boost::optional<float> JJ_deltaEta;
    boost::optional<float> b1j1_deltaPhi;
    boost::optional<float> b1b2_deltaPhi;    
    boost::optional<int> VBFEvent;    
};

#endif