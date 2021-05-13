// NOTE : this class is taken from: 
// https://raw.githubusercontent.com/pmandrik/VSEVA/master/HHWWgg/reweight/reweight_HH.C
// sadly most things are tangled up so the code really patches it to run the reweihgting @ NLO

#ifndef REWEIGHTMANDRIK_H
#define REWEIGHTMANDRIK_H

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>

using namespace std;

// TODO add links ...
class ReweightMandrik {
  public:
  vector< vector<double>> A_values_lo, A_values_nlo;

  ReweightMandrik(string error_set="", string input_lo="pm_pw_LO-Ais-13TeV_V2.txt", string input_nlo="pm_pw_NLO_Ais_13TeV_V2.txt"){
    error_set = "\""+error_set+"\"";
    LoadData( error_set, input_lo, A_values_lo );
    LoadData( error_set, input_nlo, A_values_nlo );
  }

  // vector<double> GetEFTBenchmark(string name, string year="2016", bool cms_fake = false){
  //   vector<double> couplings = get_CMS_EFT_benchmarks( name, year, cms_fake );
  //   return couplings;
  // }

  void LoadData( string error_set, string input_name, vector< vector<double>> & answer ){
    std::string line;
    std::ifstream infile( input_name );
    while (std::getline(infile, line)) {
      std::istringstream iss(line);
      string value;

      if( error_set != "\"V0\""){
        std::getline(iss, value, ',');
        if(value != error_set) continue;
      }

      answer.push_back( vector<double>() );
      while (std::getline(iss, value, ',')) {
        answer[ answer.size()-1 ].push_back( atof(value.c_str()) );
      }
      if( error_set == "\"V0\""){
        for(int i = 0, N = answer[ answer.size()-1 ].size() + 1; i < N; i++) answer[ answer.size()-1 ].push_back( 0 );
      }
    }
    cout << answer.size() << endl;
  }

  vector< pair<double, double> > GetBinCenters(string order="nlo"){
    vector< vector<double> > * A_map = & A_values_nlo;
    if(order=="lo") A_map            = & A_values_lo;

    vector< pair<double, double> > answer;
    for(unsigned int i = 0; i < A_map->size(); i++){
      vector<double> & values = A_map->at(i);
      double mass_bin_center = (values.at(0)+values.at(1))*0.5;
      double cos_bin_center  = (values.at(2)+values.at(3))*0.5;
      answer.push_back( make_pair(mass_bin_center, cos_bin_center) );
    }
    return answer;
  }

  double GetDiffXsection(double m_hh, double cos_theta, const vector<double> & couplings, const vector< vector<double> > * A_map){
    for(unsigned int i = 0; i < A_map->size(); i++){
      const vector<double> & values = A_map->at(i);
      double mass_bin_end = values.at(1);
      double cos_bin_end  = values.at(3);
      if(m_hh > mass_bin_end or cos_theta > cos_bin_end) continue;
      double dXsec = 0;
      for( unsigned int j = 0; j < couplings.size(); j++ ){
        dXsec += values.at(j+4) * couplings.at(j);
      }
      return dXsec;
    }
    return 0;
  }

  double GetDiffXsection(double m_hh, double cos_theta, const vector<double> & eft_parameters, string order="nlo"){
    if(m_hh < 250) return 0;
    double kl, kt, c2, cg, c2g;
    kl = eft_parameters[0];
    kt = eft_parameters[1];
    c2 = eft_parameters[2];
    cg = eft_parameters[3];
    c2g = eft_parameters[4];

    vector<double> couplings;
    if(order == "lo") couplings = { // LO
      pow(kt,4), pow(c2,2), pow(kt,2)*pow(kl,2), pow(cg,2)*pow(kl,2),
      pow(c2g, 2), c2*pow(kt, 2), kl*pow(kt, 3), kt*kl*c2, cg*kl*c2, c2*c2g,
      cg*kl*pow(kt, 2), c2g*pow(kt, 2), 
      pow(kl,2)*cg*kt, c2g*kt*kl, cg*c2g*kl };
    if(order == "nlo") couplings = { // LO
      pow(kt,4), pow(c2,2), pow(kt,2)*pow(kl,2), pow(cg,2)*pow(kl,2),
      pow(c2g, 2), c2*pow(kt, 2), kl*pow(kt, 3), kt*kl*c2, cg*kl*c2, c2*c2g,
      cg*kl*pow(kt, 2), c2g*pow(kt, 2), 
      pow(kl,2)*cg*kt, c2g*kt*kl, cg*c2g*kl,
      // NLO
      pow(kt,3)*cg, kt*c2*cg, kt*pow(cg,2)*kl, cg*kt*c2g, 
      pow(kt*cg,2), c2*pow(cg,2), pow(cg,3)*kl, pow(cg,2)*c2g };
    
    vector< vector<double> > * A_map = & A_values_lo;
    if(order=="nlo")           A_map = & A_values_nlo;

    return GetDiffXsection(m_hh, cos_theta, couplings, A_map);
  }

  double GetA(int index, double m_hh, double cos_theta, string order="nlo"){
    vector< vector<double> > * A_map = & A_values_lo;
    if(order=="nlo")           A_map = & A_values_nlo;
    
    for(unsigned int i = 0; i < A_map->size(); i++){
      const vector<double> & values = A_map->at(i);
      double mass_bin_end = values.at(1);
      double cos_bin_end  = values.at(3);
      if(m_hh > mass_bin_end or cos_theta > cos_bin_end) continue;
      return values.at(index+4);
    }
    return 0;
  }

};


// // P.Mandrik, IHEP, 2020
// // Return EFT benchmarks definition as https://arxiv.org/pdf/1710.08261.pdf
// vector<double> get_CMS_EFT_benchmarks( string name, string year, bool cms_fake = false ){
//   vector<double> answer;
//   if( name == "SM" or name == "sm" ) answer = {1, 1, 0, 0, 0};
//   if( name ==  "1" ) answer = {7.5, 1, -1, 0, 0};
//   if( name ==  "2" ) answer = {1.0, 1.0, 0.5, -0.8, 0.6};
//   if( name ==  "3" ) answer = {1.0, 1.0, -1.5, 0.0, -0.8};
//   if( name ==  "4" ) answer = {-3.5, 1.5, -3.0, 0.0, 0.0};
//   if( name ==  "5" ) answer = {1.0, 1.0, 0.0, 0.8, -1.0};
//   if( name ==  "6" ) answer = {2.4, 1.0, 0.0, 0.2, -0.2};
//   if( name ==  "7" ) answer = {5.0, 1.0, 0.0, 0.2, -0.2};
//   if( name ==  "8" ) answer = {15.0, 1.0, 0.0, -1.0, 1.0};
//   if( name ==  "9" ) answer = {1.0, 1.0, 1.0, -0.6, 0.6};
//   if( name ==  "10" ) answer = {10.0, 1.5, -1.0, 0.0, 0.0};
//   if( name ==  "11" ) answer = {2.4, 1.0, 0.0, 1.0, -1.0};
//   if( name ==  "12" ) answer = {15.0, 1.0, 1.0, 0.0, 0.0};

//   if( (year == "2017" or year == "2018") and answer.size() and cms_fake ){
//     answer[4] = 1.0;
//     double tr = answer[0];
//     answer[0] = answer[1];
//     answer[1] = tr;
//   }
//   if( (year == "2016") and answer.size() and cms_fake ){
//     double tr = answer[0];
//     answer[0] = answer[1];
//     answer[1] = tr;
//   }

//   if( name == "BOX" or name == "box" ) answer = {0, 1, 0, 0, 0};
//   if( name ==  "cHHH0" ) answer = {0.0,  1.0, 0.0, 0.0, 0.0};
//   if( name ==  "cHHH1" ) answer = {1.0,  1.0, 0.0, 0.0, 0.0};
//   if( name ==  "cHHH2" ) answer = {2.45, 1.0, 0.0, 0.0, 0.0};
//   if( name ==  "cHHH5" ) answer = {5.0,  1.0, 0.0, 0.0, 0.0};
  
//   // 8a from https://link.springer.com/article/10.1007/JHEP09(2018)057
//   if( name ==  "8a" ) answer = {1.0, 1.0, 0.5, 0.8/3, 0.0};

//   // New benchmarks from https://arxiv.org/pdf/1908.08923.pdf
//   if( name == "JHEP03(2020)091_1" or name == "1b") answer = {  3.94, 0.94, -1./3.,  0.5 * 1.5,  1./3. * (-3.) };
//   if( name == "JHEP03(2020)091_2" or name == "2b") answer = {  6.84, 0.61,  1./3.,  0.0 * 1.5, -1./3. * (-3.) };
//   if( name == "JHEP03(2020)091_3" or name == "3b") answer = {  2.21, 1.05, -1./3.,  0.5 * 1.5,   0.5  * (-3.) };
//   if( name == "JHEP03(2020)091_4" or name == "4b") answer = {  2.79, 0.61,  1./3., -0.5 * 1.5,  1./6. * (-3.) };
//   if( name == "JHEP03(2020)091_5" or name == "5b") answer = {  3.95, 1.17, -1./3., 1./6.* 1.5,  -0.5  * (-3.) };
//   if( name == "JHEP03(2020)091_6" or name == "6b") answer = {  5.68, 0.83,  1./3., -0.5 * 1.5,  1./3. * (-3.) };
//   if( name == "JHEP03(2020)091_7" or name == "7b") answer = { -0.10, 0.94,     1., 1./6.* 1.5, -1./6. * (-3.) };

//   return answer;
// }

// vector<string> get_benchmarks_names(){
//   vector<string> benchmars = {"sm", "box", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", 
//                               "8a", "cHHH0", "cHHH2", "cHHH5", 
//                               "JHEP03(2020)091_1", "JHEP03(2020)091_2", "JHEP03(2020)091_3", "JHEP03(2020)091_4", "JHEP03(2020)091_5", "JHEP03(2020)091_6", "JHEP03(2020)091_7"};
//   return benchmars;
// }

// // HH xsection using expression from https://arxiv.org/abs/1507.02245
// double get_eft_xsec_Carvalho(double kl, double kt, double c2, double cg, double c2g){
//   vector<double> A_values_lo  = { 2.09, 10.15, 0.28, 0.10, 1.33, -8.51, -1.37, 2.83, 1.46, -4.92, -0.68, 1.86, 0.32, -0.84, -0.57 };
//   vector<double> couplings = { pow(kt, 4), pow(c2, 2), pow(kt, 2)*pow(kl, 2), pow(cg, 2)*pow(kl, 2), pow(c2g, 2), c2*pow(kt, 2), kl*pow(kt, 3),
//                                kt*kl*c2, cg*kl*c2, c2g*c2, cg*kl*kt*kt, c2g*kt*kt,
//                                kl*cg*kt*kl, c2g*kt*kl, cg*c2g*kl };

//   double xsec_sm =  16.60711696;
//   double answer_xsection = 0;
//   for(int i = 0; i < A_values_lo.size(); i++)
//     answer_xsection += A_values_lo.at(i) * couplings.at(i);
//   return answer_xsection * xsec_sm;
// }

// // Custom HH xsection from Powheg 13 Tev generation results
// // Validation https://indico.cern.ch/event/996681/contributions/4212133/
// double get_eft_xsec_13TeV(vector<double> kappas, string order, string uncertantie=""){
//     double answer_xsection = 0;
//     double kl  = kappas[0];
//     double kt  = kappas[1];
//     double c2  = kappas[2];
//     double cg  = kappas[3];
//     double c2g = kappas[4];
//     vector<double> couplings = { 
//             // LO
//             pow(kt,4), pow(c2,2), pow(kt,2)*pow(kl,2), pow(cg,2)*pow(kl,2),
//             pow(c2g, 2), c2*pow(kt, 2), kl*pow(kt, 3), kt*kl*c2, cg*kl*c2, c2*c2g,
//             cg*kl*pow(kt, 2), c2g*pow(kt, 2), 
//             pow(kl,2)*cg*kt, c2g*kt*kl, cg*c2g*kl,
//             // NLO
//             pow(kt,3)*cg, kt*c2*cg, kt*pow(cg,2)*kl, cg*kt*c2g, 
//             pow(kt*cg,2), c2*pow(cg,2), pow(cg,3)*kl, pow(cg,2)*c2g };

//     if( order == "lo" ){
//       vector<double> A_values_lo    = {35.0111,169.908,4.72866,2.38523,22.3288,-142.521,-22.996,47.2901,28.0101,-82.3576,-13.1345,31.2217,6.37158,-13.9821,-10.8268,};
//       if( uncertantie=="muR_UP")A_values_lo    = {29.339989,142.828652,3.944472,1.987152,18.841645,-119.547333,-19.223360,39.572711,23.399797,-69.040228,-10.957083,26.085625,5.308951,-11.681204,-9.055525,};
//       if( uncertantie=="muR_DN")A_values_lo    = {42.541636,205.687054,5.778218,2.919222,26.909724,-172.982077,-28.026622,57.564385,34.165875,-100.032567,-16.048800,38.075269,7.796551,-17.054354,-13.187661,};
//       if( uncertantie=="muF_UP")A_values_lo    = {33.214886,159.640761,4.551589,2.305399,20.727429,-134.819116,-21.986524,45.071311,26.839155,-78.057553,-12.641570,29.899841,6.155038,-13.395502,-10.335953,};
//       if( uncertantie=="muF_DN")A_values_lo    = {36.805367,180.705794,4.885372,2.452094,24.101164,-150.349033,-23.950697,49.443752,29.097654,-86.678003,-13.570472,32.448121,6.554115,-14.528044,-11.299634,};
//       if( uncertantie=="muRF_UP")A_values_lo    = {27.832322,134.173116,3.796522,1.920497,17.483549,-113.072653,-18.378290,37.711670,22.418837,-65.428160,-10.544753,24.981035,5.128215,-11.189653,-8.643234,};
//       if( uncertantie=="muRF_DN")A_values_lo    = {44.714854,218.701521,5.968957,3.000666,29.029593,-182.448410,-29.186181,60.175085,35.486974,-105.262740,-16.579853,39.569843,8.019070,-17.717366,-13.759752,};
//       if( uncertantie=="PDF4LHC15_nlo_30_pdfas_UP")A_values_lo    = {36.208893,176.168489,4.879759,2.459933,23.284642,-147.500053,-23.752158,48.877622,28.922822,-85.183450,-13.550707,32.220345,6.571496,-14.439991,-11.195454,};
//       if( uncertantie=="PDF4LHC15_nlo_30_pdfas_DN")A_values_lo    = {33.812066,163.645533,4.577366,2.310462,21.372602,-137.539060,-22.238752,45.701219,27.097318,-79.530065,-12.718469,30.222196,6.171466,-13.524284,-10.458108,};
//       if( uncertantie=="PDF4LHC15_nlo_30_pdfas_as_UP")A_values_lo    = {34.557787,167.736680,4.665718,2.353319,22.038584,-140.685346,-22.693751,46.672758,27.642437,-81.296870,-12.961223,30.811765,6.286366,-13.797880,-10.684346,};
//       if( uncertantie=="PDF4LHC15_nlo_30_pdfas_as_DN")A_values_lo    = {35.481751,172.242144,4.791610,2.416866,22.658916,-144.446484,-23.303070,47.923962,28.382176,-83.461994,-13.307699,31.634385,6.456062,-14.168891,-10.973232,};

//       for(int i = 0; i < A_values_lo.size(); i++)
//         answer_xsection += A_values_lo.at(i) * couplings.at(i);
//       return answer_xsection;
//     }
//     if( order == "lo_MadGraph" ){ // pm_mg_LO-Ais-13TeV.txt
//       vector<double> A_values_lo_mg = {27.7684,134.174,3.79328,1.35023,17.4785,-112.933,-18.3424,37.7066,19.5523,-65.5234,-9.08617,24.9774,4.31592,-11.2505,-7.59558,}; // pm_mg_LO-Ais-13TeV.txt
//       for(int i = 0; i < A_values_lo_mg.size(); i++)
//         answer_xsection += A_values_lo_mg.at(i) * couplings.at(i);
//       return answer_xsection;
//     }

//     // TODO add NLO
//     vector<double> A_values_nlo  = {62.5088,345.604,9.63451,4.34841,39.0143,-268.644,-44.2924,96.5595,53.515,-155.793,-23.678,54.5601,12.2273,-26.8654,-19.3723,-0.0904439,0.321092,0.452381,-0.0190758,-0.607163,1.27408,0.364487,-0.499263,};
//     for(int i = 0; i < A_values_nlo.size(); i++)
//       answer_xsection += A_values_nlo.at(i) * couplings.at(i);
//     return answer_xsection;
// }

// double get_eft_xsec_13TeV(string mark, string order, string year = "2016", bool cms_fake=false){
//   vector<double> answer = get_CMS_EFT_benchmarks(mark, year, cms_fake);
//   return  get_eft_xsec_13TeV(answer, order);
// }

// // HH xsection using expression from https://arxiv.org/abs/1806.05162
// double get_eft_xsec_14TeV(string mark, string order, string year = "2016", bool cms_fake=false){
//     vector<double> answer = get_CMS_EFT_benchmarks(mark, year, cms_fake);
//     double ct  = answer[1];
//     double ctt = answer[2];
//     double c3  = answer[0];
//     double cg  = answer[3] * 2. / 3.;
//     double cgg = answer[4] * (-1.) / 3.;

//     vector<double> A_values_lo  = {2.08059, 10.2011, 0.27814, 0.314043, 12.2731, -8.49307, -1.35873, 2.80251, 2.48018, 14.6908, -1.15916, -5.51183, 0.560503, 2.47982, 2.89431};
//     vector<double> A_values_nlo  = {2.23389, 12.4598, 0.342248, 0.346822, 13.0087, -9.6455, -1.57553, 3.43849, 2.86694, 16.6912, -1.25293, -5.81216, 0.649714, 2.85933, 3.14475, -0.00816241, 0.0208652, 0.0168157, 0.0298576, -0.0270253, 0.0726921, 0.0145232, 0.123291};
    
//     vector<double> couplings = { pow(ct,4), pow(ctt,2), pow(c3 * ct,2), pow(c3*cg,2), pow(cgg,2), ctt*pow(ct,2), pow(ct,3)*c3,
//                   ct*c3*ctt, cg*c3*ctt, ctt*cgg, pow(ct,2)*cg*c3, pow(ct,2)*cgg, 
//                   ct*pow(c3,2)*cg, ct*c3*cgg, cg*c3*cgg, 
//                   pow(ct,3)*cg, ct*ctt*cg, ct*c3*pow(cg,2), cg*ct*cgg, 
//                   pow(ct*cg,2), ctt*pow(cg,2), pow(cg,3)*c3, pow(cg,2)*cgg };

//     double sm_xsection_lo  = 19.85;
//     double sm_xsection_nlo = 32.95;
//     double answer_xsection = 0;
    
//     if( order == "lo" ){
//       for(int i = 0; i < A_values_lo.size(); i++)
//         answer_xsection += A_values_lo.at(i) * couplings.at(i);
//       return answer_xsection * sm_xsection_lo;
//     }
    
//     for(int i = 0; i < A_values_nlo.size(); i++)
//       answer_xsection += A_values_nlo.at(i) * couplings.at(i);
//     return answer_xsection * sm_xsection_nlo;
// }

// // TODO add links ...
// class ReweightMandrik {
//   public:
//   vector< vector<double>> A_values_lo, A_values_nlo;

//   ReweightMandrik(string error_set="", string input_lo="pm_pw_LO-Ais-13TeV_V2.txt", string input_nlo="pm_pw_NLO_Ais_13TeV_V2.txt"){
//     error_set = "\""+error_set+"\"";
//     LoadData( error_set, input_lo, A_values_lo );
//     LoadData( error_set, input_nlo, A_values_nlo );
//   }

//   vector<double> GetEFTBenchmark(string name, string year="2016", bool cms_fake = false){
//     vector<double> couplings = get_CMS_EFT_benchmarks( name, year, cms_fake );
//     return couplings;
//   }

//   void LoadData( string error_set, string input_name, vector< vector<double>> & answer ){
//     std::string line;
//     std::ifstream infile( input_name );
//     while (std::getline(infile, line)) {
//       std::istringstream iss(line);
//       string value;

//       if( error_set != "\"V0\""){
//         std::getline(iss, value, ',');
//         if(value != error_set) continue;
//       }

//       answer.push_back( vector<double>() );
//       while (std::getline(iss, value, ',')) {
//         answer[ answer.size()-1 ].push_back( atof(value.c_str()) );
//       }
//       if( error_set == "\"V0\""){
//         for(int i = 0, N = answer[ answer.size()-1 ].size() + 1; i < N; i++) answer[ answer.size()-1 ].push_back( 0 );
//       }
//     }
//     cout << answer.size() << endl;
//   }

//   vector< pair<double, double> > GetBinCenters(string order="nlo"){
//     vector< vector<double> > * A_map = & A_values_nlo;
//     if(order=="lo") A_map            = & A_values_lo;

//     vector< pair<double, double> > answer;
//     for(int i = 0; i < A_map->size(); i++){
//       vector<double> & values = A_map->at(i);
//       double mass_bin_center = (values.at(0)+values.at(1))*0.5;
//       double cos_bin_center  = (values.at(2)+values.at(3))*0.5;
//       answer.push_back( make_pair(mass_bin_center, cos_bin_center) );
//     }
//     return answer;
//   }

//   double GetDiffXsection(double m_hh, double cos_theta, const vector<double> & couplings, const vector< vector<double> > * A_map){
//     for(int i = 0; i < A_map->size(); i++){
//       const vector<double> & values = A_map->at(i);
//       double mass_bin_end = values.at(1);
//       double cos_bin_end  = values.at(3);
//       if(m_hh > mass_bin_end or cos_theta > cos_bin_end) continue;
//       double dXsec = 0;
//       for( int j = 0; j < couplings.size(); j++ ){
//         dXsec += values.at(j+4) * couplings.at(j);
//       }
//       return dXsec;
//     }
//     return 0;
//   }

//   double GetDiffXsection(double m_hh, double cos_theta, const vector<double> & eft_parameters, string order="nlo"){
//     if(m_hh < 250) return 0;
//     double kl, kt, c2, cg, c2g;
//     kl = eft_parameters[0];
//     kt = eft_parameters[1];
//     c2 = eft_parameters[2];
//     cg = eft_parameters[3];
//     c2g = eft_parameters[4];

//     vector<double> couplings;
//     if(order == "lo") couplings = { // LO
//       pow(kt,4), pow(c2,2), pow(kt,2)*pow(kl,2), pow(cg,2)*pow(kl,2),
//       pow(c2g, 2), c2*pow(kt, 2), kl*pow(kt, 3), kt*kl*c2, cg*kl*c2, c2*c2g,
//       cg*kl*pow(kt, 2), c2g*pow(kt, 2), 
//       pow(kl,2)*cg*kt, c2g*kt*kl, cg*c2g*kl };
//     if(order == "nlo") couplings = { // LO
//       pow(kt,4), pow(c2,2), pow(kt,2)*pow(kl,2), pow(cg,2)*pow(kl,2),
//       pow(c2g, 2), c2*pow(kt, 2), kl*pow(kt, 3), kt*kl*c2, cg*kl*c2, c2*c2g,
//       cg*kl*pow(kt, 2), c2g*pow(kt, 2), 
//       pow(kl,2)*cg*kt, c2g*kt*kl, cg*c2g*kl,
//       // NLO
//       pow(kt,3)*cg, kt*c2*cg, kt*pow(cg,2)*kl, cg*kt*c2g, 
//       pow(kt*cg,2), c2*pow(cg,2), pow(cg,3)*kl, pow(cg,2)*c2g };
    
//     vector< vector<double> > * A_map = & A_values_lo;
//     if(order=="nlo")           A_map = & A_values_nlo;

//     return GetDiffXsection(m_hh, cos_theta, couplings, A_map);
//   }

//   double GetA(int index, double m_hh, double cos_theta, string order="nlo"){
//     vector< vector<double> > * A_map = & A_values_lo;
//     if(order=="nlo")           A_map = & A_values_nlo;
    
//     for(int i = 0; i < A_map->size(); i++){
//       const vector<double> & values = A_map->at(i);
//       double mass_bin_end = values.at(1);
//       double cos_bin_end  = values.at(3);
//       if(m_hh > mass_bin_end or cos_theta > cos_bin_end) continue;
//       return values.at(index+4);
//     }
//     return 0;
//   }

// };

// // https://arxiv.org/abs/1806.05162 LO and NLO reweighting
// class ReweightGudrun {
//   public:
//   double bin_step;
//   vector< vector<double>> A_values_lo, A_values_nlo;

//   ReweightGudrun(string input_lo="LO-Ais-13TeV.csv", string input_nlo="NLO-Ais-13TeV.csv", int bin_step=20){
//     LoadData( input_lo, A_values_lo );;
//     LoadData( input_nlo, A_values_nlo );
//     this->bin_step = bin_step;
//   }

//   void LoadData( string input_name, vector< vector<double>> & answer ){
//     std::string line;
//     std::ifstream infile( input_name );
//     while (std::getline(infile, line)) {
//       std::istringstream iss(line);
//       answer.push_back( vector<double>() );
//       string value;
//       while (std::getline(iss, value, ',')) {
//         answer[ answer.size()-1 ].push_back( atof(value.c_str()) );
//       }
//     }
//   }

//   vector<double> CouplingsToEWChL(double k_lambda, double k_t, double c2, double cg_, double c2g){
//     double c3  = k_lambda;
//     double ct  = k_t;
//     double ctt = c2;
//     double cg  = cg_ * 2. / 3.;
//     double cgg = c2g * (-1.) / 3.;
//     vector<double> answer = { c3, ct, ctt, cg, cgg };
//     return answer;
//   }

//   vector<double> GetEFTBenchmark(string name, string year="2016", bool cms_fake = false){
//     vector<double> couplings = get_CMS_EFT_benchmarks( name, year, cms_fake );
//     return CouplingsToEWChL(couplings[0], couplings[1], couplings[2], couplings[3], couplings[4]);
//   }

//   double GetDiffXsection(double m_hh, const vector<double> & eft_parameters, string order="nlo"){
//     if(m_hh < 240) return 0;
//     double c3 , ct, ctt, cg, cgg;
//     c3 = eft_parameters[0];
//     ct = eft_parameters[1];
//     ctt = eft_parameters[2];
//     cg = eft_parameters[3];
//     cgg = eft_parameters[4];

//     // cout << c3 << " " <<  ct << " " << ctt << " " << cg << " " << cgg << endl;
//     vector<double> couplings = { pow(ct,4), pow(ctt,2), pow(c3 * ct,2), pow(c3*cg,2), pow(cgg,2), ctt*pow(ct,2), pow(ct,3)*c3,
//                   ct*c3*ctt, cg*c3*ctt, ctt*cgg, pow(ct,2)*cg*c3, pow(ct,2)*cgg, 
//                   ct*pow(c3,2)*cg, ct*c3*cgg, cg*c3*cgg, 
//                   pow(ct,3)*cg, ct*ctt*cg, ct*c3*pow(cg,2), cg*ct*cgg, 
//                   pow(ct*cg,2), ctt*pow(cg,2), pow(cg,3)*c3, pow(cg,2)*cgg };
    
//     vector< vector<double> > * A_map = & A_values_nlo;
//     if(order=="lo"){
//       A_map = & A_values_lo;
//     }

//     for(int i = 0; i < A_map->size(); i++){
//       vector<double> & values = A_map->at(i);
//       double mass = values.at(0) + bin_step/2;
//       // cout << m_hh << " " << mass << endl;
//       if(m_hh > mass) continue;
//       double dXsec = 0;
//       for( int j = 1; j < values.size(); j++ ){
//         // cout << values.at(j) << " " << couplings.at(j-1) << " " << values.at(j) * couplings.at(j-1) << endl;
//         dXsec += values.at(j) * couplings.at(j-1);
//       }
//       return dXsec;
//     }
//     return 0;
//   }

//   double GetXsection(const vector<double> & eft_parameters, string order="nlo"){
//     double c3 , ct, ctt, cg, cgg;
//     c3 = eft_parameters[0];
//     ct = eft_parameters[1];
//     ctt = eft_parameters[2];
//     cg = eft_parameters[3];
//     cgg = eft_parameters[4];
//     vector<double> couplings = { pow(ct,4), pow(ctt,2), pow(c3 * ct,2), pow(c3*cg,2), pow(cgg,2), ctt*pow(ct,2), pow(ct,3)*c3,
//                   ct*c3*ctt, cg*c3*ctt, ctt*cgg, pow(ct,2)*cg*c3, pow(ct,2)*cgg, 
//                   ct*pow(c3,2)*cg, ct*c3*cgg, cg*c3*cgg, 
//                   pow(ct,3)*cg, ct*ctt*cg, ct*c3*pow(cg,2), cg*ct*cgg, 
//                   pow(ct*cg,2), ctt*pow(cg,2), pow(cg,3)*c3, pow(cg,2)*cgg };
    
//     vector< vector<double> > * A_map = & A_values_nlo;
//     if(order=="lo") A_map = & A_values_lo;

//     double xsec = 0;
//     for(int i = 0; i < A_map->size(); i++){
//       vector<double> & values = A_map->at(i);
//       for( int j = 1; j < values.size(); j++ ){
//         xsec += values.at(j) * couplings.at(j-1);
//       }
//     }
//     return xsec * bin_step;
//   }

//   double GetA(int index, double m_hh, string order="nlo"){
//     vector< vector<double> > * A_map = & A_values_lo;
//     if(order=="nlo")           A_map = & A_values_nlo;
    
//     for(int i = 0; i < A_map->size(); i++){
//       vector<double> & values = A_map->at(i);
//       double mass = values.at(0) + bin_step/2;
//       if(m_hh > mass) continue;
//       return values.at(index+1);
//     }
//     return 0;
//   }
// };

// // https://arxiv.org/pdf/1710.08261.pdf LO reweighting
// class ReweightCarvalho { 
//   public:
//   vector< vector<double>> A_map_values;
//   vector< vector<double>> A_map_info;
//   vector<double>  mhh_bins, cos_theta_bins; // lover edges of the bins
//   double sm_xsection_lo;

//   ReweightCarvalho( string input_lo="coefficientsByBin_extended_3M_costHHSim_59-4.txt" ){
//     LoadData( input_lo, A_map_values, A_map_info );
//     mhh_bins = { 250.,    260.,    270.,    280.,    290.,    300.,    310.,
//             320.,    330.,    340.,    350.,    360.,    370.,    380.,
//             390.,    400.,    410.,    420.,    430.,    440.,    450.,
//             460.,    470.,    480.,    490.,    500.,    510.,    520.,
//             530.,    540.,    550.,    560.,    570.,    580.,    590.,
//             600.,    610.,    620.,    630.,    640.,    650.,    660.,
//             670.,    680.,    690.,    700.,    750.,    800.,    850.,
//             900.,    950.,   1000.,   1100.,   1200.,   1300.,   1400.,
//            1500.,   1750.,   2000.,  50000. };
//     cos_theta_bins = { 0.,  0.40000001,  0.60000002,  0.80000001,  1. };

//     // defined as xsection = sm_xsection * \sum A_i * f(i, kappas)
//     // assuming flat k-factors for all benchmarks : xsection_NNLO = sm_xsection_NNLO * \sum A_i * f(i, kappas)
//     sm_xsection_lo  = 16.60711696; 
//   }

//   void LoadData( string input_name, vector< vector<double>> & A_values, vector< vector<double>> & A_info ){
//     std::string line;
//     std::ifstream infile( input_name );
//     while (std::getline(infile, line)) {
//       std::istringstream iss(line);
//       A_info.push_back( vector<double>() );
//       A_values.push_back( vector<double>() );
//       string value;

//       std::getline(iss, value, ' '); // skip
//       std::getline(iss, value, ' '); // mass
//       A_info[ A_info.size()-1 ].push_back( atof(value.c_str()) );

//       std::getline(iss, value, ' '); // cos
//       A_info[ A_info.size()-1 ].push_back( atof(value.c_str()) );

//       std::getline(iss, value, ' '); // N SM events, hardcodded norm vactor is from the paper https://arxiv.org/pdf/1710.08261.pdf
//       A_info[ A_info.size()-1 ].push_back( atof(value.c_str()) / 3000000. );
//       std::getline(iss, value, ' '); // skip

//       while (std::getline(iss, value, ' ')) {
//         A_values[ A_values.size()-1 ].push_back( atof(value.c_str()) );
//       }
//     }
//   }

//   vector<double> GetEFTBenchmark(string name, string year="2016", bool cms_fake = false){
//     vector<double> couplings = get_CMS_EFT_benchmarks( name, year, cms_fake );
//     return couplings;
//   }

//   // https://phab.hepforge.org/source/rosettagit/browse/master/Rosetta/interfaces/dihiggs/AnalyticalReweighter.py
//   double AnalyticalFormula(double kl, double kt, double c2, double cg, double c2g, const vector<double> & A){
//     return A[0]*pow(kt,4) + A[1]*pow(c2,2) + (A[2]*pow(kt,2) + A[3]*pow(cg,2))*pow(kl,2) +
//            A[4]*pow(c2g, 2) + ( A[5]*c2 + A[6]*kt*kl )*pow(kt, 2)  + (A[7]*kt*kl + A[8]*cg*kl)*c2 + A[9]*c2*c2g +
//            (A[10]*cg*kl + A[11]*c2g)*pow(kt, 2)+ (A[12]*kl*cg + A[13]*c2g )*kt*kl  + A[14]*cg*c2g*kl ;
//   }

//   double GetDiffXsection(double m_hh, double cos_theta, const vector<double> & eft_parameters){
//     if(m_hh < 250) return 0;
//     cos_theta = abs( cos_theta );

//     double kl, kt, c2, cg, c2g;
//     kl = eft_parameters[0];
//     kt = eft_parameters[1];
//     c2 = eft_parameters[2];
//     cg = eft_parameters[3];
//     c2g = eft_parameters[4];

//     for(int mi = 1; mi < mhh_bins.size(); mi++){
//       double cos_bin_upper_edge, mass_bin_upper_edge = mhh_bins.at( mi );
//       if(m_hh > mass_bin_upper_edge) continue;
//       for(int ci = 1; ci < cos_theta_bins.size(); ci++){
//         cos_bin_upper_edge  = cos_theta_bins.at( ci );
//         if(cos_theta > cos_bin_upper_edge) continue;

//         const vector<double> & A_info   = A_map_info.at( (cos_theta_bins.size() - 1) * (mi - 1) + (ci - 1) );
//         const vector<double> & A_values = A_map_values.at( (cos_theta_bins.size() - 1) * (mi - 1) + (ci - 1) );
//         double mass_bin_center = A_info.at(0);
//         double cos_bin_center  = A_info.at(1);
//         double nevents         = A_info.at(2);

//         double mass_bin_lower_edge = mhh_bins.at( mi-1 );
//         double cos_bin_lower_edge  = cos_theta_bins.at( ci-1 );

//         double dm   = mass_bin_upper_edge - mass_bin_lower_edge;
//         double dcos = cos_bin_upper_edge - cos_bin_lower_edge;
        
//         // cout << m_hh << " [" << mass_bin_lower_edge << " " << mass_bin_center << " " << mass_bin_upper_edge << "]" << endl;
//         // cout << cos_theta << " [" << cos_bin_lower_edge << " " << cos_bin_center << " " << cos_bin_upper_edge << "]" << endl;
//         // cout << AnalyticalFormula(kl, kt, c2, cg, c2g, A_values) << " *" << nevents << " *" << sm_xsection_lo << " /" << dm << " /" << dcos;
//         // cout << " = " << AnalyticalFormula(kl, kt, c2, cg, c2g, A_values) * nevents * sm_xsection_lo / dm / dcos << endl;

//         return AnalyticalFormula(kl, kt, c2, cg, c2g, A_values) * nevents * sm_xsection_lo / dm / dcos;
//       }
//     }
//     return 0;
//   }
// };

// // test functions ==============================================================================
// void make_prediction_hists(){
//   ReweightGudrun   rg = ReweightGudrun();
//   ReweightCarvalho rc = ReweightCarvalho();

//   vector<string> bms = {"SM", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12"};
//   for(string bm : bms){
//     auto couplings_rc = rc.GetEFTBenchmark(bm);
//     auto couplings_rg = rg.GetEFTBenchmark(bm);

//     // Gudrun reweighting == >
//     double start_x = 240;
//     double end_x   = 1040;
//     double width   = 20;
//     int nbins   = int( (end_x-start_x)/width );
//     TH1D* h1 = new TH1D("1", ("EFT"+bm +" LO II").c_str(), nbins,  start_x, end_x);
//     TH1D* h2 = new TH1D("2", ("EFT"+bm +" NLO II").c_str(), nbins, start_x, end_x);

//     double dXsec;
//     for(int i = 0; i < nbins; i++){
//       double mass = start_x + width * i + width/2;
//       dXsec   = rg.GetDiffXsection( mass, couplings_rg, "lo" );
//       // cout << mass << " " << dXsec << endl;
//       h1->Fill( start_x + 20 * i, dXsec );
//       dXsec  = rg.GetDiffXsection( mass, couplings_rg, "nlo" );
//       h2->Fill( start_x + 20 * i, dXsec );
//     }

//     // Carvalho reweighting == >
//     double kl, kt, c2, cg, c2g;
//     kl = couplings_rc[0];
//     kt = couplings_rc[1];
//     c2 = couplings_rc[2];
//     cg = couplings_rc[3];
//     c2g = couplings_rc[4];
//     TGraphErrors * h0 = new TGraphErrors( rc.mhh_bins.size() - 1);
//     for(int mi = 1; mi < rc.mhh_bins.size(); mi++){
//       double mass_bin_upper_edge = rc.mhh_bins.at( mi );
//       double mass_bin_lower_edge = rc.mhh_bins.at( mi-1 );
//       double mass_bin_center     = (mass_bin_upper_edge + mass_bin_lower_edge) * 0.5;
//       double dXsec = 0;
//       for(int ci = 1; ci < rc.cos_theta_bins.size(); ci++){
//         double cos_bin_upper_edge = rc.cos_theta_bins.at( ci );
//         double cos_bin_lower_edge  = rc.cos_theta_bins.at( ci-1 );
//         double dcos = cos_bin_upper_edge - cos_bin_lower_edge;
//         dXsec += rc.GetDiffXsection( mass_bin_center, (cos_bin_upper_edge+cos_bin_lower_edge)*0.5, couplings_rc ) * dcos;
//         // cout << mass_bin_center << " " << (cos_bin_upper_edge+cos_bin_lower_edge)*0.5 << " " << rc.GetDiffXsection( mass_bin_center, (cos_bin_upper_edge+cos_bin_lower_edge)*0.5, couplings_rc ) << endl;
//       }
//       h0->SetPoint(mi-1, mass_bin_center, dXsec );
//       h0->SetPointError(mi-1, (mass_bin_upper_edge-mass_bin_lower_edge)/2, 0);
//     }

//     // Drawing == >
//     TCanvas * canv = new TCanvas("canv", "canv", 640, 480);
//     h1->SetLineColor(2);
//     h2->SetLineColor(3);
//     h0->SetLineColor(4);
    
//     h2->SetTitle( ("EFT benchmark " + bm).c_str() );

//     h2->SetLineWidth(3);
//     h1->SetLineWidth(3);
//     h0->SetLineWidth(3);

//     h2->GetXaxis()->SetTitle("M_{HH} [GeV]");
//     h2->GetYaxis()->SetTitle("d#sigma/dm [fb/GeV]");
//     h2->SetStats(false);

//     h2->Draw("hist");
//     h1->Draw("hist same");
//     h0->Draw("p");

//     h2->Scale( h1->Integral() / h2->Integral() );
//     h2->GetYaxis()->SetRangeUser(0., max(h2->GetMaximum(), h1->GetMaximum())*1.5);

//     TLegend * legend = new TLegend(0.55,0.75,0.90,0.88);
//     legend->SetLineWidth(3);
//     legend->AddEntry(h0, "LO I", "lp");
//     legend->AddEntry(h1, "LO II", "lp");
//     legend->AddEntry(h2, "NLO II (normalised to LO)", "lp");
//     legend->Draw();

//     canv->Print( ("RewComp_" + bm + ".png").c_str() );
//   }
// }

// // create hists with the k-factors ==============================================================================
// void make_reweighting_hists(){
//   ReweightGudrun  rg = ReweightGudrun();
//   vector<string> bms = {"SM", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "box", 
//                         "8a", "cHHH0", "cHHH2", "cHHH5",
//                         "1b", "2b", "3b", "4b", "5b", "6b", "7b"};

//   TFile * file  = new TFile("reweight_HH.root", "RECREATE");
//   TH1D  * hxsec = new TH1D("xsections", "xsections", 2*bms.size()+10,  0, 2*bms.size()+10);
//   hxsec->SetOption("hist");

//   for(string bm : bms){
//     auto couplings_rg = rg.GetEFTBenchmark(bm);

//     // gudrun reweighting == >
//     double start_x = 240;
//     double end_x   = 1040;
//     double width   = 20;
//     int nbins   = int( (end_x-start_x)/width );
//     TH1D* h1 = new TH1D(("EFT_"+bm +"_LO").c_str(), ("EFT_"+bm +"_LO").c_str(), nbins,  start_x, end_x);
//     TH1D* h2 = new TH1D(("EFT_"+bm +"_NLO").c_str(), ("EFT_"+bm +"_NLO").c_str(), nbins, start_x, end_x);
//     TH1D* h3 = new TH1D(("EFT_"+bm +"_NLO_div_LO").c_str(), ("EFT_"+bm +"_LO_to_NLO").c_str(), nbins, start_x, end_x);
//     h1->SetOption("hist");
//     h2->SetOption("hist");
//     h3->SetOption("hist");

//     double dXsec;
//     for(int i = 0; i < nbins; i++){
//       double mass = start_x + width * i + width/2;
//       dXsec   = rg.GetDiffXsection( mass, couplings_rg, "lo" );
//       h1->Fill( start_x + 20 * i, dXsec );
//       dXsec  = rg.GetDiffXsection( mass, couplings_rg, "nlo" );
//       h2->Fill( start_x + 20 * i, dXsec );
//     }

//     h3->Add( h2 );
//     h3->Divide( h1 );

//     cout << bm << " " << get_eft_xsec_13TeV(bm, "lo") << " " << get_eft_xsec_13TeV(bm, "nlo") << endl;
//     hxsec->Fill( ("EFT_"+bm +"_LO").c_str(),  get_eft_xsec_13TeV(bm, "lo") );
//     hxsec->Fill( ("EFT_"+bm +"_NLO").c_str(), get_eft_xsec_13TeV(bm, "nlo") );
//   }
//   file->Write();
//   file->Close();
// }

// // create hists with the k-factors for CMS fake benchmarks ==============================================================================
// // OLD VERSION --- PLEASU USE make_reweighting_hists_from_fake_v4()
// void make_reweighting_hists_from_fake(){ 
//   ReweightGudrun   rg = ReweightGudrun();
//   vector<string> bms = {"SM", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12"};

//   TFile * file  = new TFile("reweight_HH_fake.root", "RECREATE");
//   TH1D  * hxsec_fake = new TH1D("xsections_fake", "xsections_fake", 3*bms.size()+10,  0, 3*bms.size()+10);
//   TH1D  * hxsec      = new TH1D("xsections", "xsections", 3*bms.size()+10,  0, 3*bms.size()+10);
//   vector<string> years = {"2016", "2017", "2018"};

//   double start_x = 240;
//   double end_x   = 1040;
//   double width   = 20;
//   int nbins   = int( (end_x-start_x)/width );

//   for(string year : years){
//     vector<TH1D*> start_points;
//     vector<TH1D*> end_points;
//     for(string bm : bms){
//       auto couplings_rg_fake = rg.GetEFTBenchmark(bm, year, true);
//       auto couplings_rg      = rg.GetEFTBenchmark(bm, year, false);

//       TH1D* h1 = new TH1D((year + "_"+bm +"_LO_fake").c_str(), (year + "_"+bm +"_LO_fake").c_str(), nbins,  start_x, end_x);
//       TH1D* h2 = new TH1D((year + "_"+bm +"_NLO").c_str(),     (year + "_"+bm +"_NLO").c_str(),     nbins,  start_x, end_x);
//       h1->SetOption("hist");
//       h2->SetOption("hist");

//       double dXsec;
//       for(int i = 0; i < nbins; i++){
//         double mass = start_x + width * i + width/2;
//         dXsec   = rg.GetDiffXsection( mass, couplings_rg_fake, "lo"  );
//         h1->Fill( start_x + 20 * i, dXsec );
//         dXsec   = rg.GetDiffXsection ( mass, couplings_rg, "nlo" );
//         h2->Fill( start_x + 20 * i, dXsec );
//       }

//       start_points.push_back( h1 );
//       end_points.push_back( h2 );
      
//       hxsec_fake->Fill( (year + "_EFT_" + bm + "_LO_fake" ).c_str(), get_eft_xsec_13TeV(bm, "lo" , year, true ) );
//       hxsec->Fill( (year + "_EFT_" + bm + "_NLO").c_str(), get_eft_xsec_13TeV(bm, "nlo", year, false) );

//       cout << bm << " " << get_eft_xsec_13TeV(bm, "lo" , year, true ) << " " << get_eft_xsec_13TeV(bm, "nlo", year, false) << endl;
//     }

//     for(int i = 0; i < start_points.size(); i++){
//       for(int j = 0; j < end_points.size(); j++){
//         TH1D* in = start_points.at(i);
//         TH1D* fi = end_points.at(j);
//         string name = year + "_" + bms.at(j) + "_NLO_div_" + bms.at(i) + "_LO_fake" ;
//         TH1D* h3    = new TH1D(name.c_str(), name.c_str(), nbins, start_x, end_x);
//         h3->SetOption("hist");
//         h3->Add( fi );
//         h3->Divide( in );
//       }
//     }
//   }
//   file->Write();
//   file->Close();
// }

// void make_reweighting_hists_from_fake_v2(){
//   ReweightGudrun  rg = ReweightGudrun();
//   ReweightMandrik rm_mg = ReweightMandrik("pm_mg_LO-Ais-13TeV.txt");
//   ReweightMandrik rm_pw = ReweightMandrik("pm_pw_LO-Ais-13TeV.txt");
//   vector<string> bms = {"SM", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12"};

//   TFile * file       = new TFile("reweight_HH_fake_v2.root", "RECREATE");
//   TH1D  * hxsec_1 = new TH1D("xsections_MadGraph_LO_fake", "xsections_MadGraph_LO_fake", 3*bms.size()+10,  0, 3*bms.size()+10);
//   TH1D  * hxsec_2 = new TH1D("xsections_MadGraph_LO", "xsections_MadGraph_LO", 3*bms.size()+10,  0, 3*bms.size()+10);
//   TH1D  * hxsec_3 = new TH1D("xsections_Powheg_LO",  "xsections_Powheg_LO",  3*bms.size()+10,  0, 3*bms.size()+10);
//   TH1D  * hxsec_4 = new TH1D("xsections_Powheg_NLO", "xsections_Powheg_NLO", 3*bms.size()+10,  0, 3*bms.size()+10);

//       hxsec_1->GetYaxis()->SetTitle("#sigma [fb]");
//       hxsec_2->GetYaxis()->SetTitle("#sigma [fb]");
//       hxsec_3->GetYaxis()->SetTitle("#sigma [fb]");
//       hxsec_4->GetYaxis()->SetTitle("#sigma [fb]");

//   vector<string> years = {"2016", "2017", "2018"};

//   const Int_t NBINS = 36;
//   Double_t edges[NBINS + 1] = {250.,    270.,    290.,    310., 330.,    350.,    370.,    
//                                390.,    410.,    430.,    450., 470.,    490.,    510.,    
//                                530.,    550.,    570.,    590., 610.,    630.,    650.,    
//                                670.,    700.,    750.,    800., 850.,    900.,    950.,   
//                                1000.,   1100.,   1200.,   1300.,1400.,  1500.,   1750.,   
//                                2000.,  5000.};
  
//   for(string year : years){
//     for(string bm : bms){
//       auto couplings_rm_fake = rm_mg.GetEFTBenchmark(bm, year, true);
//       auto couplings_rm      = rm_mg.GetEFTBenchmark(bm, year, false);
//       auto couplings_rg      = rg.GetEFTBenchmark(bm, year, false);

//       TH1D* h1 = new TH1D((year + "_"+bm +"_MadGraph_LO_fake").c_str(), (year + "_"+bm +"_MadGraph_LO_fake").c_str(), NBINS,  edges);
//       TH1D* h2 = new TH1D((year + "_"+bm +"_MadGraph_LO").c_str(),      (year + "_"+bm +"_MadGraph_LO").c_str(),      NBINS,  edges);
//       TH1D* h3 = new TH1D((year + "_"+bm +"_Powheg_LO").c_str(),        (year + "_"+bm +"_Powheg_LO").c_str(),        NBINS,  edges);
//       TH1D* h4 = new TH1D((year + "_"+bm +"_Powheg_NLO").c_str(),       (year + "_"+bm +"_Powheg_NLO").c_str(),       NBINS,  edges);
//       h1->SetOption("hist");
//       h2->SetOption("hist");
//       h3->SetOption("hist");
//       h4->SetOption("hist");

//       h1->GetYaxis()->SetTitle("d #sigma /dmHH [fb/GeV]");
//       h2->GetYaxis()->SetTitle("d #sigma /dmHH [fb/GeV]");
//       h3->GetYaxis()->SetTitle("d #sigma /dmHH [fb/GeV]");
//       h4->GetYaxis()->SetTitle("d #sigma /dmHH [fb/GeV]");

//       double dXsec;
//       double start_x = edges[0], end_x = edges[NBINS];
//       double step = 1;
//       for(double mass = 0; mass < (end_x-start_x)/step; mass+=step){
//         // cout << year << " " << bm << " " << mass << endl;
//         dXsec   = rm_mg.GetDiffXsection( mass, 0.5, couplings_rm_fake, "lo"  );
//         h1->Fill( mass, dXsec*step );

//         dXsec   = rm_mg.GetDiffXsection( mass, 0.5, couplings_rm, "lo"  );
//         h2->Fill( mass, dXsec*step );

//         dXsec   = rm_pw.GetDiffXsection( mass, 0.5, couplings_rm, "lo"  );
//         h3->Fill( mass, dXsec*step );

//         dXsec   = rg.GetDiffXsection( mass, couplings_rg, "nlo" );
//         h4->Fill( mass, dXsec*step );
//       }

//       for(int i = 1; i <= NBINS; i++ ){
//         h1->SetBinContent( i, double(h1->GetBinContent(i)) /h1->GetBinWidth(i) );
//         h2->SetBinContent( i, double(h2->GetBinContent(i)) /h2->GetBinWidth(i) );
//         h3->SetBinContent( i, double(h3->GetBinContent(i)) /h3->GetBinWidth(i) );
//         h4->SetBinContent( i, double(h4->GetBinContent(i)) /h4->GetBinWidth(i) );
//       }

      
//       hxsec_1->Fill( (year + "_EFT_" + bm + "_MadGraph_LO_fake" ).c_str(), get_eft_xsec_13TeV(bm, "lo_MadGraph" , year, true  ) );
//       hxsec_2->Fill( (year + "_EFT_" + bm + "_MadGraph_LO" ).c_str(),      get_eft_xsec_13TeV(bm, "lo_MadGraph" , year, false ) );
//       hxsec_3->Fill( (year + "_EFT_" + bm + "_Powheg_LO").c_str(),         get_eft_xsec_13TeV(bm, "lo",  year, false) );
//       hxsec_4->Fill( (year + "_EFT_" + bm + "_Powheg_NLO").c_str(),        get_eft_xsec_13TeV(bm, "nlo", year, false) );

//       cout << bm << " " << get_eft_xsec_13TeV(bm, "lo_MadGraph" , year, true ) << " -> " << get_eft_xsec_13TeV(bm, "lo_MadGraph" , year, false );
//       cout << " -> "    << get_eft_xsec_13TeV(bm, "lo",  year, false)          << " -> " << get_eft_xsec_13TeV(bm, "nlo", year, false) << endl;
//     }
//   }
//   file->Write();
//   file->Close();
// }

// void make_reweighting_hists_from_fake_v3(){
//   ReweightGudrun  rg = ReweightGudrun();
//   ReweightMandrik rm_mg     = ReweightMandrik("V0", "pm_mg_LO-Ais-13TeV.txt");
//   ReweightMandrik rm_pw     = ReweightMandrik("V0", "pm_pw_LO-Ais-13TeV.txt");
//   ReweightMandrik rm_pw_NLO = ReweightMandrik("", "pm_pw_LO-Ais-13TeV_V2.txt", "pm_pw_NLO_Ais_13TeV_V2.txt");
//   vector<string> bms = {"SM", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "cHHH0", "cHHH1", "cHHH2", "cHHH5", "1b", "2b", "3b", "4b", "5b", "6b", "7b"};

//   TFile * file       = new TFile("reweight_HH_fake_v3.root", "RECREATE");

//   TH1D  * hxsec_2 = new TH1D("xsections_MadGraph_LO", "xsections_MadGraph_LO", 3*bms.size()+10,  0, 3*bms.size()+10);
//   TH1D  * hxsec_3 = new TH1D("xsections_Powheg_LO",  "xsections_Powheg_LO",  3*bms.size()+10,  0, 3*bms.size()+10);
//   TH1D  * hxsec_4 = new TH1D("xsections_Powheg_NLO", "xsections_Powheg_NLO", 3*bms.size()+10,  0, 3*bms.size()+10);

//       hxsec_2->GetYaxis()->SetTitle("#sigma [fb]");
//       hxsec_3->GetYaxis()->SetTitle("#sigma [fb]");
//       hxsec_4->GetYaxis()->SetTitle("#sigma [fb]");

//   vector<string> years = {"2016", "2017", "2018"};

//   const Int_t NBINS = 29;
//   Double_t edges[NBINS + 1] = {250.,    270.,    290.,    310., 330.,    350.,    370.,    390.,    410.,    430.,    450., 470.,    490.,    510.,    530.,    550.,    570.,    590., 640.,    680.,    720.,    760.,    800.,    900.,   1000., 1200.,  1400.,    1800.,   2500.,   5000.};

//   const Int_t COS_NBINS = 5;
//   Double_t cos_edges[COS_NBINS + 1] = { 0.,  0.4000000,  0.6000000,  0.8000000,  1. };
  
//   for(string year : years){
//     TH1D  * hxsec_fake = new TH1D( (year+"_xsections_MadGraph_LO_fake").c_str(), (year+"_xsections_MadGraph_LO_fake").c_str(), 3*bms.size()+10,  0, 3*bms.size()+10);
//     hxsec_fake->GetYaxis()->SetTitle("#sigma [fb]");
//     for(string bm : bms){
//       auto couplings_rm_fake = rm_mg.GetEFTBenchmark(bm, year, true);
//       TH1D* h1 = new TH1D((year + "_"+bm +"_MadGraph_LO_fake").c_str(), (year + "_"+bm +"_MadGraph_LO_fake").c_str(), NBINS,  edges);
//       h1->SetOption("hist");
//       h1->GetYaxis()->SetTitle("d #sigma /dmHH [fb/GeV]");

//       double dXsec;
//       double start_x = edges[0], end_x = edges[NBINS];
//       double step = 1;
//       for(double mass = 0; mass < (end_x-start_x)/step; mass+=step){
//         // cout << year << " " << bm << " " << mass << endl;
//         dXsec   = rm_mg.GetDiffXsection( mass, 0.5, couplings_rm_fake, "lo"  );
//         h1->Fill( mass, dXsec*step );
//       }
//       for(int i = 1; i <= NBINS; i++ ){
//         h1->SetBinContent( i, double(h1->GetBinContent(i)) /h1->GetBinWidth(i) );
//       }

//       hxsec_fake->Fill( (year + "_EFT_" + bm + "_MadGraph_LO_fake" ).c_str(), get_eft_xsec_13TeV(bm, "lo_MadGraph" , year, true  ) );
//     }
//   }

//     string year = "2017";
//     for(string bm : bms){
//       auto couplings_rm      = rm_mg.GetEFTBenchmark(bm, year, false);
//       auto couplings_rg      = rg.GetEFTBenchmark(bm, year, false);

//       TH1D* h2 = new TH1D((bm +"_MadGraph_LO").c_str(),      (bm +"_MadGraph_LO").c_str(),      NBINS,  edges);
//       TH1D* h3 = new TH1D((bm +"_Powheg_LO").c_str(),        (bm +"_Powheg_LO").c_str(),        NBINS,  edges);
//       TH1D* h4 = new TH1D((bm +"_Powheg_NLO").c_str(),       (bm +"_Powheg_NLO").c_str(),       NBINS,  edges);
//       TH1D* h5 = new TH1D((bm +"_Powheg_NLO_V2").c_str(),    (bm +"_Powheg_NLO_V2").c_str(),    NBINS,  edges);
//       h2->SetOption("hist");
//       h3->SetOption("hist");
//       h4->SetOption("hist");
//       h5->SetOption("hist");

//       h2->GetYaxis()->SetTitle("d #sigma /dmHH [fb/GeV]");
//       h3->GetYaxis()->SetTitle("d #sigma /dmHH [fb/GeV]");
//       h4->GetYaxis()->SetTitle("d #sigma /dmHH [fb/GeV]");
//       h5->GetYaxis()->SetTitle("d #sigma /dmHH [fb/GeV]");

//       double dXsec;
//       double start_x = edges[0], end_x = edges[NBINS];
//       double step = 1;
//       for(double mass = 0; mass < (end_x-start_x)/step; mass+=step){
//         dXsec   = rm_mg.GetDiffXsection( mass, 0.5, couplings_rm, "lo"  );
//         h2->Fill( mass, dXsec*step );

//         dXsec   = rm_pw.GetDiffXsection( mass, 0.5, couplings_rm, "lo"  );
//         h3->Fill( mass, dXsec*step );

//         dXsec   = rg.GetDiffXsection( mass, couplings_rg, "nlo" );
//         h4->Fill( mass, dXsec*step );

//         dXsec = 0;
//         for(int i = 0; i < COS_NBINS-1; i++){
//           double cos_max = cos_edges[i + 1];
//           double cos_min = cos_edges[i];
//           double dcos = cos_max - cos_min;
//           dXsec += rm_pw_NLO.GetDiffXsection(mass, (cos_max+cos_min)/2., couplings_rm, "nlo") * dcos;
//         }
//         h5->Fill( mass, dXsec*step );
//       }

//       for(int i = 1; i <= NBINS; i++ ){
//         h2->SetBinContent( i, double(h2->GetBinContent(i)) /h2->GetBinWidth(i) );
//         h3->SetBinContent( i, double(h3->GetBinContent(i)) /h3->GetBinWidth(i) );
//         h4->SetBinContent( i, double(h4->GetBinContent(i)) /h4->GetBinWidth(i) );
//         h5->SetBinContent( i, double(h5->GetBinContent(i)/1000. ) /h5->GetBinWidth(i) );
//       }

//       hxsec_2->Fill( ("EFT_" + bm + "_MadGraph_LO" ).c_str(),      get_eft_xsec_13TeV(bm, "lo_MadGraph" , year, false ) );
//       hxsec_3->Fill( ("EFT_" + bm + "_Powheg_LO").c_str(),         get_eft_xsec_13TeV(bm, "lo",  year, false) );
//       hxsec_4->Fill( ("EFT_" + bm + "_Powheg_NLO").c_str(),        get_eft_xsec_13TeV(bm, "nlo", year, false) );

//       cout << bm << " " << get_eft_xsec_13TeV(bm, "lo_MadGraph" , year, true ) << " -> " << get_eft_xsec_13TeV(bm, "lo_MadGraph" , year, false );
//       cout << " -> "    << get_eft_xsec_13TeV(bm, "lo",  year, false)          << " -> " << get_eft_xsec_13TeV(bm, "nlo", year, false) << endl;
//     }
//   file->Write();
//   file->Close();
// }

// void make_reweighting_hists_from_fake_v4(){
//   ReweightMandrik rm_mg     = ReweightMandrik("", "pm_mg_LO-Ais-13TeV_V2.txt");
//   ReweightMandrik rm_pw     = ReweightMandrik("", "pm_pw_LO-Ais-13TeV_V2.txt", "pm_pw_NLO_Ais_13TeV_V2.txt");
//   vector<string> bms = {"SM", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "cHHH0", "cHHH1", "cHHH2", "cHHH5"};

//   TFile * file    = new TFile("reweight_HH_fake_v4.root", "RECREATE");
//   TH1D  * hxsec_2 = new TH1D("xsections_MadGraph_LO", "xsections_MadGraph_LO", 3*bms.size()+10,  0, 3*bms.size()+10);
//   TH1D  * hxsec_3 = new TH1D("xsections_Powheg_LO",  "xsections_Powheg_LO",  3*bms.size()+10,  0, 3*bms.size()+10);
//   TH1D  * hxsec_4 = new TH1D("xsections_Powheg_NLO", "xsections_Powheg_NLO", 3*bms.size()+10,  0, 3*bms.size()+10);

//   hxsec_2->GetYaxis()->SetTitle("#sigma [fb]");
//   hxsec_3->GetYaxis()->SetTitle("#sigma [fb]");
//   hxsec_4->GetYaxis()->SetTitle("#sigma [fb]");

//   vector<string> years = {"2016", "2017", "2018"};

//   const Int_t NBINS = 29;
//   Double_t edges[NBINS + 1] = {250.,    270.,    290.,    310., 330.,    350.,    370.,    390.,    410.,    430.,    450., 470.,    490.,    510.,    530.,    550.,    570.,    590., 640.,    680.,    720.,    760.,    800.,    900.,   1000., 1200.,  1400.,    1800.,   2500.,   5000.};

//   const Int_t COS_NBINS = 5;
//   Double_t cos_edges[COS_NBINS + 1] = { 0.,  0.4000000,  0.6000000,  0.8000000,  1. };
  
//   for(string year : years){
//     TH1D  * hxsec_fake = new TH1D( (year+"_xsections_MadGraph_LO_fake").c_str(), (year+"_xsections_MadGraph_LO_fake").c_str(), 3*bms.size()+10,  0, 3*bms.size()+10);
//     hxsec_fake->GetYaxis()->SetTitle("#sigma [fb]");
//     for(string bm : bms){
//       auto couplings_rm_fake = rm_mg.GetEFTBenchmark(bm, year, true);
//       for(int cos_bin = 0; cos_bin < COS_NBINS-1; cos_bin++){
//         double cos_start =  cos_edges[ cos_bin ];
//         double cos_end   =  cos_edges[ cos_bin+1 ];
//         double dcos = cos_end - cos_start;
//         double cos = (cos_start+cos_end)*0.5;

//         string title = "#cos #theta #in [" + to_string(cos_start) + "," + to_string(cos_end) + "]";
//         TH1D* h1 = new TH1D((year + "_"+bm +"_MadGraph_LO_fake_cosBin_" + to_string(cos_bin)).c_str(), title.c_str(), NBINS,  edges);
//         h1->SetOption("hist");
//         h1->GetYaxis()->SetTitle("d #sigma /dmHH [fb/GeV]");

//         double dXsec;
//         double start_x = edges[0], end_x = edges[NBINS];
//         double step = 1;
//         for(double mass = 0; mass < (end_x-start_x)/step; mass+=step) h1->Fill( mass, rm_mg.GetDiffXsection( mass, cos, couplings_rm_fake, "lo"  )*step );
//         for(int i = 1; i <= NBINS; i++ ) h1->SetBinContent( i, double(h1->GetBinContent(i)/1000.)*dcos/h1->GetBinWidth(i) );
//       }
//       hxsec_fake->Fill( (year + "_EFT_" + bm + "_MadGraph_LO_fake" ).c_str(), get_eft_xsec_13TeV(bm, "lo_MadGraph" , year, true  ) );
//     }
//   }

//     string year = "";
//     for(string bm : bms){
//       auto couplings_rm      = rm_mg.GetEFTBenchmark(bm, year, false);
//       double integral = 0;
//       double integral_lo = 0;
//       for(int cos_bin = 0; cos_bin < COS_NBINS-1; cos_bin++){
//         double cos_start =  cos_edges[ cos_bin ];
//         double cos_end   =  cos_edges[ cos_bin+1 ];
//         double dcos = cos_end - cos_start;
//         double cos = (cos_start+cos_end)*0.5;

//         string title = "#cos #theta #in [" + to_string(cos_start) + "," + to_string(cos_end) + "]";
//         TH1D* h2 = new TH1D((bm +"_MadGraph_LO_cosBin_" + to_string(cos_bin)).c_str(),   title.c_str(),    NBINS,  edges);
//         TH1D* h3 = new TH1D((bm +"_Powheg_LO_cosBin_" + to_string(cos_bin)).c_str(),     title.c_str(),    NBINS,  edges);
//         TH1D* h5 = new TH1D((bm +"_Powheg_NLO_cosBin_" + to_string(cos_bin)).c_str(),    title.c_str(),    NBINS,  edges);
//         h2->SetOption("hist");
//         h3->SetOption("hist");
//         h5->SetOption("hist");

//         h2->GetYaxis()->SetTitle("d #sigma/dmHH [fb/GeV]");
//         h3->GetYaxis()->SetTitle("d #sigma/dmHH [fb/GeV]");
//         h5->GetYaxis()->SetTitle("d #sigma/dmHH [fb/GeV]");

//         double dXsec;
//         double start_x = edges[0], end_x = edges[NBINS];
//         double step = 1;
//         for(double mass = 0; mass < (end_x-start_x)/step; mass+=step){
//           dXsec = rm_mg.GetDiffXsection( mass, cos, couplings_rm, "lo"  );
//           h2->Fill( mass, dXsec*step );

//           dXsec = rm_pw.GetDiffXsection( mass, cos, couplings_rm, "lo"  );
//           h3->Fill( mass, dXsec*step );

//           dXsec = rm_pw.GetDiffXsection(mass, cos, couplings_rm, "nlo");
//           h5->Fill( mass, dXsec*step );
//         }

//         for(int i = 1; i <= NBINS; i++ ){
//           h2->SetBinContent( i, double(h2->GetBinContent(i)/1000.)*dcos/h2->GetBinWidth(i) );
//           h3->SetBinContent( i, double(h3->GetBinContent(i)/1000.)*dcos/h3->GetBinWidth(i) );
//           h5->SetBinContent( i, double(h5->GetBinContent(i)/1000.)*dcos/h5->GetBinWidth(i) );
//         }

//         integral_lo += h3->Integral("width");
//         integral += h5->Integral("width");
//       }

//       hxsec_2->Fill( ("EFT_" + bm + "_MadGraph_LO" ).c_str(),      get_eft_xsec_13TeV(bm, "lo_MadGraph" , year, false ) );
//       hxsec_3->Fill( ("EFT_" + bm + "_Powheg_LO").c_str(),         get_eft_xsec_13TeV(bm, "lo",  year, false) );
//       hxsec_4->Fill( ("EFT_" + bm + "_Powheg_NLO").c_str(),        get_eft_xsec_13TeV(bm, "nlo", year, false) );

//       cout << bm << " " << get_eft_xsec_13TeV(bm, "lo_MadGraph" , year, true ) << " -> " << get_eft_xsec_13TeV(bm, "lo_MadGraph" , year, false );
//       cout << " -> "    << get_eft_xsec_13TeV(bm, "lo",  year, false) << "( " << integral_lo << ") -> " << get_eft_xsec_13TeV(bm, "nlo", year, false) << " (" << integral << ")" << endl;
//     }
//   file->Write();
//   file->Close();
// }

// // usage example ==============================================================================
// void reweight_example(){
//   // preparation ... ==============================================================================
//   string year = "2017";
//   string initial_benchmark = "sm";
//   vector<string> out_benchmars = {"sm", "box", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12"};

//   // ReweightGudrun and ReweightCarvalho will provide 
//   ReweightGudrun   r_gudrun = ReweightGudrun("LO-Ais-13TeV.csv", "NLO-Ais-13TeV.csv");
//   ReweightCarvalho r_carval = ReweightCarvalho("coefficientsByBin_extended_3M_costHHSim_59-4.txt");

//   // Couplings values of the EFT benchmarks have different basis in Gudrin and Carvalho theoretical papers
//   // obtain initial EFT points and total cross sections
//   vector<double> benchmark_coupling_gudrun_initial = r_gudrun.GetEFTBenchmark( initial_benchmark, year, true  ) ;
//   vector<double> benchmark_coupling_carval_initial = r_carval.GetEFTBenchmark( initial_benchmark, year, true  ) ;
//   double xsec_initial                = get_eft_xsec_13TeV(initial_benchmark, "lo", year, true);

//   // set final EFT points and total cross sections
//   vector< vector<double> > benchmark_couplings_gudrun_final, benchmark_couplings_carval_final;
//   vector<double> xsecs_final_lo, xsecs_final_nlo;
//   for(int i = 0; i < out_benchmars.size(); i++){
//     benchmark_couplings_gudrun_final.push_back   ( r_gudrun.GetEFTBenchmark( out_benchmars.at(i), year, false ) );
//     benchmark_couplings_carval_final.push_back   ( r_carval.GetEFTBenchmark( out_benchmars.at(i), year, false ) );
//     xsecs_final_lo.push_back ( get_eft_xsec_13TeV(out_benchmars.at(i), "lo", year, false) );
//     xsecs_final_nlo.push_back( get_eft_xsec_13TeV(out_benchmars.at(i), "nlo", year, false) );
//   }

//   // event loop ... ==============================================================================
//   double zero_weights_events = 0, total_events = 100;
//   for(int N = 0; N < total_events; N++){
//     // put the gen level m_HH and cos^{CS}(H, Oz) values here 
//     double m_HH  = 300;
//     double cos_H = 0.2;

//     // initial differential crossections
//     double dXsection_gudrun_lo_initial  = r_gudrun.GetDiffXsection( m_HH, benchmark_coupling_gudrun_initial, "lo" );
//     double dXsection_carval_lo_initial  = r_carval.GetDiffXsection( m_HH, cos_H, benchmark_coupling_carval_initial );

//     // check if m_HH and cos_H within defined reweighting range, otherwise the dxsec is 0
//     if( dXsection_gudrun_lo_initial < 0.000001){
//       zero_weights_events += 1;
//       continue;
//     }

//     // calculate the reweighting factor from initial EFT benchmark to every other
//     for(int i = 0; i < out_benchmars.size(); i++){
//       double dXsection_gudrun_nlo_final = r_gudrun.GetDiffXsection( m_HH, benchmark_couplings_gudrun_final.at(i), "nlo" );
//       double dXsection_gudrun_lo_final  = r_gudrun.GetDiffXsection( m_HH, benchmark_couplings_gudrun_final.at(i), "lo" );
//       double dXsection_carval_lo_final  = r_carval.GetDiffXsection( m_HH, cos_H, benchmark_couplings_carval_final.at(i) );

//       // if we have a set of N generated events the probability to obtain N_i events in the Mgg bin i is a DeltaM dXsec(Mgg_i) / Xsec 
//       // where DeltaM is a bin width, dXsec is a differential cross section and Xsec is a cross section. 
//       // So, N_i = N DeltaM dXsec(Mgg_i) / Xsec (eq 1). The another point in EFT space will have the different cross sections 
//       // dXsec' and Xsec' and expected number of events N_i' = N DeltaM dXsec'(Mgg_i) / Xsec' (eq 2). 
//       // From (eq 1) and (eq 2) N_i' = N_i (dXsec'(Mgg_i) / dXsec(Mgg_i)) (Xsec / Xsec'). Sum of the events in bin i is a N_i by definition. 
//       // So, the sum of the events in bin i with weight w_i =(dXsec'(Mgg_i) / dXsec(Mgg_i)) (Xsec / Xsec') is the N_i'.
//       double weight_gudrun_nlo = dXsection_gudrun_nlo_final / dXsection_gudrun_lo_initial * xsec_initial / xsecs_final_nlo.at( i );
//       double weight_gudrun_lo  = dXsection_gudrun_lo_final  / dXsection_gudrun_lo_initial * xsec_initial / xsecs_final_lo.at( i );
//       double weight_carval_lo  = dXsection_carval_lo_final  / dXsection_carval_lo_initial * xsec_initial / xsecs_final_lo.at( i );

//       // cout << initial_benchmark << " -> " << out_benchmars.at(i) << " " << dXsection_gudrun_nlo_final << " " << dXsection_gudrun_lo_final << " " << dXsection_carval_lo_final << endl;
//       cout << N << " " << initial_benchmark << " -> " << out_benchmars.at(i) << " " << weight_gudrun_nlo << " " << weight_gudrun_lo << " " << weight_carval_lo << endl;
//     }
//   }

//   // because the initial set of generated events from MadGraph is defined in the range m_HH [250, +inf]
//   // while the reweighting is defined in the continuum range [250, X]
//   // we need to apply an extra normalisation weight to take this into account
//   double extra_weight = total_events / (total_events - zero_weights_events);
// }

// // validation of the xsecs ==============================================================================
// #include "get_EFT_grid_points.C"
// void make_xsecs_validation(){
//   // reference xsections from Powheg 13 TeV
//   vector<double> xsecs_LO = {16.75, 86.31, 6.21, 443.58, 3815.01, 26.61, 11.04, 70.17, 60.33, 52.41, 246.25, 76.62, 1491.43, 213.36, 40.62, 149.19, 360.46, 183.15, 451.67, 230.91, 28.09, 124.02, 32.73, 85.39, 516.26, 282.86, 632.99, 356.15, 115.08, 36.70, 135.86, 48.59, 27.98, 178.89, 102.32, 33.61, 72.61, 53.77, 135.86, 40.98, 162.93, 28.26, 50.82, 76.18, 0, 0, 0, 0, 0, 0};
//   vector<double> xsecs_NLO = {27.863093217833576, 162.34559845377984, 11.85291826556827, 878.6187252093916, 7549.180890835765, 48.64120522963421, 20.83237541006396, 145.35134659502975, 129.81352406212272, 121.94791854865879, 490.81110611529976, 147.4954973856642, 3090.75808017093, 436.91966277055894, 93.99182504291637, 312.2855477838231, 686.1142808875294, 359.29975040999915, 854.2958230301776, 450.42213252002364, 63.93826048375961, 252.40544416086908, 67.16306892018427, 178.17356760336108, 997.2039271414974, 561.9159742113031, 1212.65828182493, 700.1650825293005, 249.45269386725693, 83.64561912545486, 281.1741511515414, 112.29545931515453, 61.47122089871927, 368.57225344995544, 210.2004795854609, 77.25016949594657, 164.0334727713831, 119.26514006254244, 280.56761261746203, 67.530835837366, 294.7078397059687, 52.01353896031721, 94.11995469419227, 129.34885971898277, 0, 0, 0, 0, 0, 0};

//   for(int i = 0; i <= 43; i++){
//     double kl, kt, c2, cg, c2g;
//     set_couplings(to_string(i), kl, kt, c2, cg, c2g);
//     vector<double> kappas = { kl, kt, c2, cg, c2g };
//     double xsec_an_lo  = get_eft_xsec_13TeV( kappas, "lo" );
//     double xsec_an_nlo = get_eft_xsec_13TeV( kappas, "nlo" );

//     double xsec_pw_lo  = xsecs_LO.at(i);
//     double xsec_pw_nlo = xsecs_NLO.at(i);

//     string bname = to_string(i);
//     if( i == 0 ) bname = "SM";
//     if( i > 12 ) bname = bname + "'";
      
//     string kappas_str = "";
//       std::ostringstream out1;
//       out1 << std::fixed << std::setprecision(2) << kl;
//       std::ostringstream out2;
//       out2 << std::fixed << std::setprecision(2) << kt;
//       std::ostringstream out3;
//       out3 << std::fixed << std::setprecision(2) << c2;
//       std::ostringstream out4;
//       out4 << std::fixed << std::setprecision(2) << cg;
//       std::ostringstream out5;
//       out5 << std::fixed << std::setprecision(2) << c2g;
//       kappas_str = out1.str() + ", " + out2.str() + ", " + out3.str() + ", " + out4.str() + ", " + out5.str();
//     cout << bname << "|" << kappas_str << " | " << xsec_pw_lo << " " << xsec_an_lo << " | " << xsec_pw_nlo << " " << xsec_an_nlo << endl;
//   }
// }

// void reweight_HH(){
//   // make_xsecs_validation();
//   // make_prediction_hists();
//   // make_reweighting_hists();
//   // make_reweighting_hists_from_fake();
//   // make_reweighting_hists_from_fake_v2();
//   // make_reweighting_hists_from_fake_v3();
//   make_reweighting_hists_from_fake_v4();
//   // reweight_example();
// }


#endif