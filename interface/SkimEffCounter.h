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

        void updateProcessed (double evtW);
        void updateSelected  (double evtW);

        double   getSumWProcessed() {return Ntot_w;}
        long int getNProcessed()    {return Ntot_uw;}
        double   getSumWSelected()  {return Nsel_w;}
        long int getNSelected()     {return Nsel_uw;}

        void createHisto();
        int  write();

    private:
        enum BinValue{
            kNtot_w  = 1,
            kNtot_uw = 2,
            kNsel_w  = 3,
            kNsel_uw = 4,
        };
        std::unique_ptr<TH1D>  eff_histo_;

        // _w  : weighted
        // _uw : unweighted (plain ev count)
        
        double   Ntot_w;
        long int Ntot_uw;
        double   Nsel_w;
        long int Nsel_uw;

};


#endif