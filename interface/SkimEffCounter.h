#ifndef SKIMEFFCOUNTER_H
#define SKIMEFFCOUNTER_H

/**
** class  : SkimEffCounter
** author : L. Cadamuro (UF)
** date   : 30/01/2018
** brief  : logs the numbner of processed events and the skim efficiency
**/

#include "TH1D.h"
#include <memory>
#include <iostream>

class SkimEffCounter {
    public:

        SkimEffCounter();
        ~SkimEffCounter(){};

        enum BinValue{
            kNtot_w    = 1,
            kNtot_uw   = 2,
            kNtrg_w    = 3,
            kNtrg_uw   = 4,
            kNsel_w    = 5,
            kNsel_uw   = 6,
        };

        void updateProcessed      (double evtW);
        void updateTriggered      (double evtW);
        void updateSelected       (double evtW);

        double   getSumWProcessed()      {return Ntot_w;}
        long int getNProcessed()         {return Ntot_uw;}
        double   getSumWTriggered()      {return Ntrg_w;}
        long int getNTriggered()         {return Ntrg_uw;}
        double   getSumWSelected()       {return Nsel_w;}
        long int getNSelected()          {return Nsel_uw;}
        
        void createHisto();
        int  write();

    private:
        std::unique_ptr<TH1D>  eff_histo_;

        // _w  : weighted
        // _uw : unweighted (plain ev count)
        
        double   Ntot_w;
        long int Ntot_uw;
        double   Ntrg_w;
        long int Ntrg_uw;
        double   Nsel_w;
        long int Nsel_uw;

};


#endif