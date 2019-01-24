#ifndef HH4B_KINFIT_H
#define HH4B_KINFIT_H

#include "TLorentzVector.h"
#include <iostream>

namespace HH4b_kinFit {

  double ErrpT_Signal(double Et, double eta);

  double ErrEta_Signal(double Et);

  double ErrPhi_Signal(double Et);

  double constrainHH_signalMeasurement(TLorentzVector *j1, TLorentzVector *j2, TLorentzVector *j3, TLorentzVector *j4, double mass1=125, double mass2=125);

}

#endif