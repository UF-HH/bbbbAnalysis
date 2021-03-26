#include <iostream>
#include <string>
#include <iomanip>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "CfgParser.h"
#include "NanoAODTree.h"
#include "EventInfo.h"

#include "SkimUtils.h"
namespace su = SkimUtils;

#include "OfflineProducerHelper.h"
// namespace oph = OfflineProducerHelper;

#include "OutputTree.h"
#include "SkimEffCounter.h"
#include "jsonLumiFilter.h"

#include "TFile.h"

#include "TROOT.h"

using namespace std;

#include <any>

// skim_ntuple.exe --cfg config/skim.cfg --input inputFiles/Samples_80X/VBF_HH_4b_10gen2018.txt --output test_bbbb_tree.root --xs 10 --is-signal

int main(int argc, char** argv)
{
    cout << "[INFO] ... starting program" << endl;

    ////////////////////////////////////////////////////////////////////////
    // Decalre command line options
    ////////////////////////////////////////////////////////////////////////
    
    po::options_description desc("Skim options");
    desc.add_options()
        ("help", "produce help message")
        // required
        ("cfg"   , po::value<string>()->required(), "skim config")
        ("input" , po::value<string>()->required(), "input file list")
        ("output", po::value<string>()->required(), "output file LFN")
        // optional
        ("xs"        , po::value<float>(), "cross section [pb]")
        ("maxEvts"   , po::value<int>()->default_value(-1), "max number of events to process")
        ("puWeight"  , po::value<string>()->default_value(""), "PU weight file name")
        ("seed"      , po::value<int>()->default_value(-1), "seed to be used in systematic uncertainties such as JEC, JER,etc")
        // ("kl-rew-list"  , po::value<std::vector<float>>()->multitoken()->default_value(std::vector<float>(0), "-"), "list of klambda values for reweight")
        ("kl-rew"    , po::value<float>(),  "klambda value for reweighting (default 1 if reweighting is performed)")
        ("kt-rew"    , po::value<float>(),  "ktop value for reweighting (default 1 if reweighting is performed)")
        ("c2-rew"    , po::value<float>(),  "c2 value for reweighting (default 0 if reweighting is performed)")
        ("cg-rew"    , po::value<float>(),  "cg value for reweighting (default 0 if reweighting is performed)")
        ("c2g-rew"    , po::value<float>(), "c2g value for reweighting (default 0 if reweighting is performed)")
        ("kl-map"    , po::value<string>()->default_value(""), "klambda input map for reweighting")
        // ("kl-histo"  , po::value<string>()->default_value("hhGenLevelDistr"), "klambda histogram name for reweighting")
        ("jes-shift-mult", po::value<double>()->default_value(1.0), "Magnify the JES sys by this factor (for JES shape studies). Defaults to 1 for standard systematics")
        ("jes-shift-syst", po::value<string>()->default_value("nominal"), "Name of the JES (scale) source uncertainty to be shifted. Usage as <name>:<up/down>. Pass -nominal- to not shift the jets")
        ("jer-shift-syst", po::value<string>()->default_value("nominal"), "Name of the JER (resolution) source uncertainty to be shifted. Usage as <jer/bjer>:<up/down>. Pass -nominal- to not shift the jets")
        ("bbbbChoice"    , po::value<string>()->default_value("BothClosestToDiagonal"), "bbbb pairing choice")
        ("mh1mh2"        , po::value<float>()->default_value(1.05), "Ratio Xo/Yo or 1/slope of the diagonal") 
        ("option"        , po::value<int>()->default_value(0), "Option: 0=Nominal, 1=Alternative 1, 2=Alternative 2") 
        // flags
        ("is-data",       po::value<bool>()->zero_tokens()->implicit_value(true)->default_value(false), "mark as a data sample (default is false)")
        ("is-signal",     po::value<bool>()->zero_tokens()->implicit_value(true)->default_value(false), "mark as a HH signal sample (default is false)")
        ("is-VBF-sig",    po::value<bool>()->zero_tokens()->implicit_value(true)->default_value(false), "mark as a HH VBF signal sample (default is false)")
        ("is-nlo-sig",    po::value<bool>()->zero_tokens()->implicit_value(true)->default_value(false), "mark as NL0-HH sample")
        ("save-p4",       po::value<bool>()->zero_tokens()->implicit_value(true)->default_value(false), "save the tlorentzvectors in the output")
    ;

    po::variables_map opts;
    try {
        po::store(parse_command_line(argc, argv, desc, po::command_line_style::unix_style ^ po::command_line_style::allow_short), opts);
        if (opts.count("help")) {
            cout << desc << "\n";
            return 1;
        }
        po::notify(opts);
    }    
    catch (po::error& e) {
        cerr << "** [ERROR] " << e.what() << endl;
        return 1;
    }

    ////////////////////////////////////////////////////////////////////////
    // Read config and other cmd line options for skims
    ////////////////////////////////////////////////////////////////////////

    const bool is_data = opts["is-data"].as<bool>();
    if (!is_data && !opts.count("xs")){
        cerr << "** [ERROR] please provide xs for this sample that is marked as MC" << endl;
        return 1;
    }
    cout << "[INFO] ... is a data sample? " << std::boolalpha << is_data << std::noboolalpha << endl;

    const bool is_signal = (is_data ? false : opts["is-signal"].as<bool>());
    cout << "[INFO] ... is a HH signal sample? " << std::boolalpha << is_signal << std::noboolalpha << endl;

    const bool is_VBF_sig = (is_data ? false : opts["is-VBF-sig"].as<bool>());
    cout << "[INFO] ... is a HH VBF sample? " << std::boolalpha << is_VBF_sig << std::noboolalpha << endl;

    const bool is_NLO_sig = (is_data ? false : opts["is-nlo-sig"].as<bool>());
    cout << "[INFO] ... is a HH NLO sample? " << std::boolalpha << is_NLO_sig << std::noboolalpha << endl;

    const float xs = (is_data ? 1.0 : opts["xs"].as<float>());
    cout << "[INFO] ... cross section is : " << xs << " pb" << endl;

    CfgParser config;
    if (!config.init(opts["cfg"].as<string>())) return 1;
    cout << "[INFO] ... using config file " << opts["cfg"].as<string>() << endl;
 
    ////////////////////////////////////////////////////////////////////////
    // Read needed fields from config file and pass them to the oph
    ////////////////////////////////////////////////////////////////////////

    std::map<std::string,std::any> parameterList;
    const string bbbbChoice  = config.readStringOpt("parameters::bbbbChoice");
    const string datasetname = config.readStringOpt("parameters::datasetname");
    parameterList.emplace("is_VBF_sig",is_VBF_sig);
    parameterList.emplace("is_NLO_sig",is_NLO_sig);
    parameterList.emplace("bbbbChoice",bbbbChoice);
    parameterList.emplace("datasetname",datasetname);  
 
    if(bbbbChoice == "OneClosestToMh"){
        parameterList.emplace("HiggsMass",config.readFloatOpt("parameters::HiggsMass"));
    }
    else if(bbbbChoice == "BothClosestToMh"){
        parameterList.emplace("HiggsMass",config.readFloatOpt("parameters::HiggsMass"));
    }
    else if(bbbbChoice == "BothClosestToDiagonal"){
        parameterList.emplace("TargetHiggsMass1Mass2",config.readFloatOpt("parameters::TargetHiggsMass1Mass2"));
        parameterList.emplace("Option",config.readIntOpt("parameters::Option"));
    }
    else if(bbbbChoice == "MinMassDifference"){
    //No parameters are needed
    }
    else if(bbbbChoice == "MostBackToBack"){
        parameterList.emplace("HiggsMass",config.readFloatOpt("parameters::HiggsMass"));
    }
    else if(bbbbChoice == "HighestCSVandClosestToMh"){
        parameterList.emplace("HiggsMassLMR"        ,config.readFloatOpt("parameters::HiggsMassLMR"        ));
        parameterList.emplace("HiggsMassMMR"        ,config.readFloatOpt("parameters::HiggsMassMMR"        ));
        parameterList.emplace("LMRToMMRTransition"  ,config.readFloatOpt("parameters::LMRToMMRTransition"  ));
        parameterList.emplace("UseAntiTagOnOneBjet" ,config.readBoolOpt("parameters::UseAntiTagOnOneBjet"  ));
        if(!is_data && any_cast<bool>(parameterList.at("UseAntiTagOnOneBjet")))
        {
            throw std::runtime_error("UseAntiTagOnOneBjet can be done only on data");
        }
        if(any_cast<bool>(parameterList.at("UseAntiTagOnOneBjet")) && config.readStringOpt("parameters::PreselectionCut") != "bJetCut")
        {
            throw std::runtime_error("UseAntiTagOnOneBjet can be done only using PreselectionCut = bJetCut");
        }
    }
    // else if(other selection type){
    //     parameters fo be retreived;
    // }  
    else throw std::runtime_error("cannot recognize bbbb pair choice strategy " + bbbbChoice);

    cout << "[INFO] ... chosing bb bb jet pairs with strategy : " << bbbbChoice << endl;
    if(bbbbChoice == "BothClosestToDiagonal") cout << "          ... the Xo/Yo ratio is " <<config.readFloatOpt("parameters::TargetHiggsMass1Mass2")<<", and option is "<<config.readIntOpt("parameters::Option")<<endl;

    const string preselectionCutStrategy = config.readStringOpt("parameters::PreselectionCut");
    parameterList.emplace("PreselectionCut",preselectionCutStrategy);
    if(preselectionCutStrategy == "bJetCut"){
        parameterList.emplace("MinDeepCSV"          ,config.readFloatOpt("parameters::MinDeepCSV"          ));
        parameterList.emplace("MinPt"               ,config.readFloatOpt("parameters::MinPt"               ));
        parameterList.emplace("MaxAbsEta"           ,config.readFloatOpt("parameters::MaxAbsEta"           ));
    }
    else if(preselectionCutStrategy == "VBFJetCut"){
        parameterList.emplace("bMinDeepCSV"             ,config.readFloatOpt("parameters::bMinDeepCSV"));
        parameterList.emplace("bMinDeepJet"             ,config.readFloatOpt("parameters::bMinDeepJet"));
        parameterList.emplace("bMinPt"                  ,config.readFloatOpt("parameters::bMinPt"     ));
        parameterList.emplace("bMaxAbsEta"              ,config.readFloatOpt("parameters::bMaxAbsEta" ));
        parameterList.emplace("bJetId"                  ,config.readIntOpt("parameters::bJetId"     ));
        parameterList.emplace("bPUId"                   ,config.readIntOpt("parameters::bPUId"     ));
        parameterList.emplace("jMinPt"                  ,config.readFloatOpt("parameters::jMinPt"     ));
        parameterList.emplace("jMaxAbsEta"              ,config.readFloatOpt("parameters::jMaxAbsEta" ));       
        parameterList.emplace("jJetId"                  ,config.readIntOpt("parameters::jJetId"     ));
        parameterList.emplace("jPUId"                   ,config.readIntOpt("parameters::jPUId"     ));
        parameterList.emplace("jEENoiseVeto"            ,config.readBoolOpt( "parameters::jEENoiseVeto"));
        parameterList.emplace("jEENoiseVetoHmumu"       ,config.readBoolOpt( "parameters::jEENoiseVetoHmumu"));
        parameterList.emplace("FourthAntiBTagInfo"      ,config.readBoolOpt( "parameters::FourthAntiBTagInfo"));
        parameterList.emplace("BjetRegression"          ,config.readBoolOpt( "parameters::BjetRegression"));
        parameterList.emplace("NewestBtaggingAlgorithm" ,config.readBoolOpt( "parameters::NewestBtaggingAlgorithm")); 
        parameterList.emplace("bMinDeepCSVTight"        ,config.readFloatOpt("parameters::bMinDeepCSVTight"));
        parameterList.emplace("bMinDeepJetTight"        ,config.readFloatOpt("parameters::bMinDeepJetTight"));
    }
    else if(preselectionCutStrategy == "None"){
    }  
    // else if(other selection type){
    //     parameters fo be retreived;
    // }  
    else throw std::runtime_error("cannot recognize event choice ObjectsForCut " + preselectionCutStrategy);


    const string objectsForCut = config.readStringOpt("parameters::ObjectsForCut");
    parameterList.emplace("ObjectsForCut",objectsForCut);
    if(objectsForCut == "WandZleptonDecays"){
        parameterList.emplace("WElectronMaxPfIso"   ,config.readFloatOpt("parameters::WElectronMaxPfIso"   ));
        parameterList.emplace("ZElectronMaxPfIso"   ,config.readFloatOpt("parameters::ZElectronMaxPfIso"   ));
        parameterList.emplace("WMuonMaxPfIso"       ,config.readFloatOpt("parameters::WMuonMaxPfIso"       ));
        parameterList.emplace("ZMuonMaxPfIso"       ,config.readFloatOpt("parameters::ZMuonMaxPfIso"       ));
        parameterList.emplace("MuonMaxDxy"          ,config.readFloatOpt("parameters::MuonMaxDxy"          ));
        parameterList.emplace("MuonMaxDz"           ,config.readFloatOpt("parameters::MuonMaxDz"           ));
    }
    else if(objectsForCut == "IsolatedLeptons"){
        parameterList.emplace("ElectronMinPt"        ,config.readFloatOpt("parameters::ElectronMinPt"       ));
        parameterList.emplace("ElectronMaxAbsEta"    ,config.readFloatOpt("parameters::ElectronMaxAbsEta"   ));
        parameterList.emplace("ElectronMaxPfIso"     ,config.readFloatOpt("parameters::ElectronMaxPfIso"    ));
        parameterList.emplace("ElectronMaxBarrelDxy" ,config.readFloatOpt("parameters::ElectronMaxBarrelDxy"));
        parameterList.emplace("ElectronMaxBarrelDz"  ,config.readFloatOpt("parameters::ElectronMaxBarrelDz" ));
        parameterList.emplace("ElectronMaxEndcapDxy" ,config.readFloatOpt("parameters::ElectronMaxEndcapDxy" ));
        parameterList.emplace("ElectronMaxEndcapDz"  ,config.readFloatOpt("parameters::ElectronMaxEndcapDz"  ));
        parameterList.emplace("ElectronID"             ,config.readIntOpt("parameters::ElectronID"          ));
        parameterList.emplace("MuonMinPt"            ,config.readFloatOpt("parameters::MuonMinPt"           ));
        parameterList.emplace("MuonMaxAbsEta"        ,config.readFloatOpt("parameters::MuonMaxAbsEta"       ));
        parameterList.emplace("MuonMaxPfIso"         ,config.readFloatOpt("parameters::MuonMaxPfIso"        ));
        parameterList.emplace("MuonMaxBarrelDxy"     ,config.readFloatOpt("parameters::MuonMaxBarrelDxy"    ));
        parameterList.emplace("MuonMaxBarrelDz"      ,config.readFloatOpt("parameters::MuonMaxBarrelDz"     ));
        parameterList.emplace("MuonMaxEndcapDxy"     ,config.readFloatOpt("parameters::MuonMaxEndcapDxy"     ));
        parameterList.emplace("MuonMaxEndcapDz"      ,config.readFloatOpt("parameters::MuonMaxEndcapDz"      ));
        parameterList.emplace("MuonID"                 ,config.readIntOpt("parameters::MuonID"              ));
    }
    else if(objectsForCut == "None"){
    }  
    // else if(other selection type){
    //     parameters fo be retreived;
    // }  
    else throw std::runtime_error("cannot recognize event choice ObjectsForCut " + objectsForCut);

    // MC only procedures
    if(!is_data){

        //Define a new random seed if needed and supplied
        const int randomseed = opts["seed"].as<int>();
        cout<<"[INFO] ... random seed to be used in MC corrections: "<<randomseed<<endl;

        //Btag Scale factors
        const string bTagscaleFactorMethod = config.readStringOpt("parameters::BTagScaleFactorMethod");
        parameterList.emplace("BTagScaleFactorMethod",bTagscaleFactorMethod);
        if(bTagscaleFactorMethod == "FourBtag_ScaleFactor"){
            parameterList.emplace("BJetScaleFactorsFile"               ,config.readStringOpt("parameters::BJetScaleFactorsFile"    ));
            parameterList.emplace("BJetScaleFactorsFileAlternative"    ,config.readStringOpt("parameters::BJetScaleFactorsFileAlternative"));
            parameterList.emplace("deepJet_WP_cut"                     ,config.readFloatListOpt("parameters::deepJet_WP_cut"));
            parameterList.emplace("deepCSV_WP_cut"                     ,config.readFloatListOpt("parameters::deepCSV_WP_cut"));
            parameterList.emplace("bTagEffFile"                        ,config.readStringOpt("parameters::bTagEffFile"));
        }
        else if(bTagscaleFactorMethod == "Reshaping"){
            parameterList.emplace("BJetScaleFactorsFile"               ,config.readStringOpt("parameters::BJetScaleFactorsFile"    ));
            parameterList.emplace("BJetScaleFactorsFileAlternative"    ,config.readStringOpt("parameters::BJetScaleFactorsFileAlternative"));
        }
        else if(bTagscaleFactorMethod == "None"){
        }  
        // else if(other selection type){
        //     parameters fo be retreived;
        // }  
        else throw std::runtime_error("cannot recognize event choice bTagScaleFactorMethod " + bTagscaleFactorMethod);

        //L1prefiring scale factor
        const string L1PrefiringSFMethod = config.readStringOpt("parameters::L1PrefiringSFMethod");
        parameterList.emplace("L1PrefiringSFMethod",L1PrefiringSFMethod);
        if(L1PrefiringSFMethod == "Standard"){
        cout << "[INFO] ... Standard L1Prefiring weights are added in the MC sample" << endl;  
        }
        else if(config.readStringOpt("parameters::L1PrefiringSFMethod") == "None"){
        cout << "[INFO] ... No L1Prefiring weights are added in the MC sample" << endl;      
        }  
        // else if(other selection type){
        //     parameters fo be retreived;
        // }  
        else throw std::runtime_error("cannot recognize event choice L1PrefiringScaleFactorMethod " + config.readStringOpt("parameters::L1PrefiringSFMethod"));

        //Generator weights
        const string weightMethod = config.readStringOpt("parameters::WeightMethod");
        parameterList.emplace("WeightMethod",weightMethod);
        if(weightMethod == "StandardWeight"){
            if(opts["puWeight"].as<string>()==""){
                cerr << "** [ERROR] please provide PU weight file needed for WeightMethod " << weightMethod  << endl;
                return 1;
            }
        }
        else if(weightMethod == "None"){
        }  
        // else if(other selection type){
        //     parameters fo be retreived;
        // }  
        else throw std::runtime_error("cannot recognize event choice WeightMethod " + weightMethod);

        //JER 
        const string JERstrategy = config.readStringOpt("parameters::JetEnergyResolution");
        parameterList.emplace("JetEnergyResolution",JERstrategy);
        if(JERstrategy == "StandardJER"){
            cout << "[INFO] ... Standard JER correction is applied to the jets in MC sample" << endl; 
            parameterList.emplace("JERComputeVariations" ,config.readBoolOpt  ("parameters::JERComputeVariations" ));
            if(randomseed>0) 
                parameterList.emplace("RandomGeneratorSeed"  ,randomseed);
            else
                parameterList.emplace("RandomGeneratorSeed"  ,config.readIntOpt   ("parameters::RandomGeneratorSeed"  ));                
            parameterList.emplace("JERResolutionFile"        ,config.readStringOpt("parameters::JERResolutionFile"    ));
            parameterList.emplace("JERScaleFactorFile"       ,config.readStringOpt("parameters::JERScaleFactorFile"   ));          
            parameterList.emplace("bRegressionSmearType"     ,config.readIntOpt   ("parameters::bRegressionSmearType" ));
        }
        else if(JERstrategy == "None"){
            cout << "[INFO] ... No JER correction is applied to the jets" << endl;
        }  
        // else if(other selection type){
        //     parameters fo be retreived;
        // }  
        else throw std::runtime_error("cannot recognize event choice ObjectsForCut " + JERstrategy);

        // as a temporary fix, to avoid clashes with cmd line options, disable the variations from JetEnergyCorrection method by Fabio
        const string JECstrategy = config.readStringOpt("parameters::JetEnergyCorrection");
        if(JECstrategy != "None"){
            throw std::runtime_error("For the resonant analysis the only JetEnergyCorrection strategy accepted is None, use the cmd line option instead of " + JERstrategy);
        }
        if (opts["jes-shift-syst"].as<string>() != OfflineProducerHelper::nominal_jes_syst_shift_name)
            parameterList.emplace("JECFileName", config.readStringOpt("parameters::JECFileName"));

    }

    OfflineProducerHelper oph;
    oph.initializeOfflineProducerHelper(&parameterList);


    ////////////////////////////////////////////////////////////////////////
    // Prepare event loop
    ////////////////////////////////////////////////////////////////////////

    cout << "[INFO] ... opening file list : " << opts["input"].as<string>().c_str() << endl;
    if ( access( opts["input"].as<string>().c_str(), F_OK ) == -1 ){
        cerr << "** [ERROR] The input file list does not exist, aborting" << endl;
        return 1;        
    }

    // Joining all the NANOAD input file in a TChain in order to be used like an unique three
    TChain ch("Events");
    int nfiles = su::appendFromFileList(&ch, opts["input"].as<string>());
    
    if (nfiles == 0){
        cerr << "** [ERROR] The input file list contains no files, aborting" << endl;
        return 1;
    }
    cout << "[INFO] ... file list contains " << nfiles << " files" << endl;

    cout << "[INFO] ... creating tree reader" << endl;

    // The TChain is passed to the NanoAODTree_SetBranchImpl to parse all the brances
    NanoAODTree nat (&ch, (!is_data && config.readBoolOpt("parameters::is2016Sample")));

    cout << "[INFO] ... loading the following triggers" << endl;
    for (auto trg : config.readStringListOpt("triggers::makeORof"))
        cout << "   - " << trg << endl;

    //---------Trigger section--begin  
    bool check_trigger =  config.readBoolOpt("triggers::checktrigger");
    bool skipTriggerCheck = !check_trigger;
    //Matching trigger objects
    std::vector<std::string> triggerAndNameVector;
    if(!skipTriggerCheck) triggerAndNameVector = config.readStringListOpt("triggers::makeORof");
    std::vector<std::string> triggerVector;
    // <triggerName , < objectBit, minNumber> >
    std::map<std::string, std::map< std::pair<int,int>, int > > triggerObjectAndMinNumberMap;
    

    for (auto & trigger : triggerAndNameVector)
    {
        if(trigger=="") continue;

        std::string delimiter = ":";
        size_t pos = 0;
        std::vector<std::string> triggerTokens;
        while ((pos = trigger.find(delimiter)) != std::string::npos)
        {
            triggerTokens.push_back(trigger.substr(0, pos));
            trigger.erase(0, pos + delimiter.length());
        }
        triggerTokens.push_back(trigger); // last part splitted
        if (triggerTokens.size() != 2)
        {
            throw std::runtime_error("** skim_ntuple : could not parse trigger entry " + trigger + " , aborting");
        }

        triggerVector.push_back(triggerTokens[1]);
        cout << "   - " << triggerTokens[1] << endl;

        if(!config.hasOpt( Form("triggers::%s_ObjectRequirements",triggerTokens[0].data()) ))
        {
            cout<<Form("triggers::%s_ObjectRequirements",triggerTokens[0].data())<<std::endl;
            cout<<"Trigger "<< triggerTokens[1] <<" does not have ObjectRequirements are not defined";
            continue;
        }

        triggerObjectAndMinNumberMap[triggerTokens[1]] = std::map< std::pair<int,int>, int>();   

        std::vector<std::string> triggerObjectMatchingVector = config.readStringListOpt(Form("triggers::%s_ObjectRequirements",triggerTokens[0].data()));

        for (auto & triggerObject : triggerObjectMatchingVector)
        {

            std::vector<std::string> triggerObjectTokens;
            while ((pos = triggerObject.find(delimiter)) != std::string::npos)
            {
                triggerObjectTokens.push_back(triggerObject.substr(0, pos));
                triggerObject.erase(0, pos + delimiter.length());
            }
            triggerObjectTokens.push_back(triggerObject); // last part splitted
            if (triggerObjectTokens.size() != 3)
            {
                throw std::runtime_error("** skim_ntuple : could not parse trigger entry " + triggerObject + " , aborting");
            }

            triggerObjectAndMinNumberMap[triggerTokens[1]][std::pair<int,int>(atoi(triggerObjectTokens[0].data()),atoi(triggerObjectTokens[1].data()))] = atoi(triggerObjectTokens[2].data());
        }

    }    

    parameterList.emplace("MaxDeltaR",                config.readFloatOpt("triggers::MaxDeltaR")     );     
    parameterList.emplace("MatchWithSelectedObjects", config.readBoolOpt("triggers::MatchWithSelectedObjects")     );
    bool useTriggerMatching         =  config.readBoolOpt("triggers::MatchWithSelectedObjects");
    bool useTriggerScaleFactors     =  config.readBoolOpt("triggers::UseScaleFactor");
    bool matchedTriggerScaleFactors =  config.readBoolOpt("triggers::MatchedScaleFactor");
    parameterList.emplace("UseTriggerScaleFactor", useTriggerScaleFactors);     

    if(useTriggerScaleFactors)
    {
        parameterList.emplace("SimulateTrigger", config.readBoolOpt("triggers::SimulateTrigger"));
        if(matchedTriggerScaleFactors)
        {
            parameterList.emplace("TriggerEfficiencyFileName", config.readStringOpt("triggers::TriggerEfficiencyFileName_TriggerMatched"));
        }
        else{
            parameterList.emplace("TriggerEfficiencyFileName", config.readStringOpt("triggers::TriggerEfficiencyFileName_NotTriggerMatched"));
        }

    }
    int datasetYear = config.readIntOpt("triggers::DatasetYear");
    parameterList.emplace("DatasetYear", datasetYear); 
    parameterList.emplace("TriggerObjectAndMinNumberMap", triggerObjectAndMinNumberMap);
    nat.triggerReader().setTriggers(triggerVector);
    //---------Trigger section--end

    ////////////////////////////////////////////////////////////////////////
    // Prepare the output
    ////////////////////////////////////////////////////////////////////////
    
    string outputFileName = opts["output"].as<string>();
    cout << "[INFO] ... saving output to file : " << outputFileName << endl;
    TFile outputFile(outputFileName.c_str(), "recreate");
    OutputTree ot(
        opts["save-p4"].as<bool>()
    );

    SkimEffCounter ec;

    oph.initializeObjectsForCuts(ot);
    ot.declareUserFloatBranch("XS",-1);
    
    if(!is_data)
    {
        oph.initializeApplyJERAndBregSmearing(opts["jer-shift-syst"].as<string>());
        oph.initializeApplyJESshift(opts["jes-shift-syst"].as<string>(), opts["jes-shift-mult"].as<double>());
        oph.initializeObjectsForEventWeight(ot,ec,opts["puWeight"].as<string>(),xs);
        oph.initializeObjectsBJetForScaleFactors(ot);
        oph.initializeObjectsL1PrefiringForScaleFactors(ot);
        if(useTriggerScaleFactors)
        {
          if(matchedTriggerScaleFactors){oph.initializeTriggerScaleFactors_TriggerMatched(nat, ot);}
          else{oph.initializeTriggerScaleFactors(nat, ot);} 
        }

        // MC reweight initialization
        if (
            opts.find("kl-rew")  != opts.end() ||
            opts.find("kt-rew")  != opts.end() ||
            opts.find("c2-rew")  != opts.end() ||
            opts.find("cg-rew")  != opts.end() ||
            opts.find("c2g-rew") != opts.end() 
        ) // a kl value was passed
        {
            float  kl_rew    = (opts.find("kl-rew")  != opts.end() ? opts["kl-rew"].as<float>()  : 1);  // target value
            float  kt_rew    = (opts.find("kt-rew")  != opts.end() ? opts["kt-rew"].as<float>()  : 1);  // target value
            float  c2_rew    = (opts.find("c2-rew")  != opts.end() ? opts["c2-rew"].as<float>()  : 0);  // target value
            float  cg_rew    = (opts.find("cg-rew")  != opts.end() ? opts["cg-rew"].as<float>()  : 0);  // target value
            float  c2g_rew   = (opts.find("c2g-rew") != opts.end() ? opts["c2g-rew"].as<float>() : 0);  // target value
            string kl_map    = opts["kl-map"].as<string>(); // sample map fname
            string kl_histo  = "hhGenLevelDistr";           // sample map histo
            string kl_coeffs = config.readStringOpt("hhreweight::coeff_file"); // coefficient file

            std::cout << "[INFO] ... initialising HH reweighter" << std::endl;
            std::cout << "   - coefficient file       : " << kl_coeffs  << std::endl;
            std::cout << "   - input sample map file  : " << kl_map     << std::endl;
            std::cout << "   - input sample map histo : " << kl_histo   << std::endl;
            std::cout << "   - kl  : " << kl_rew  << endl;
            std::cout << "   - kt  : " << kt_rew  << endl;
            std::cout << "   - c2  : " << c2_rew  << endl;
            std::cout << "   - cg  : " << cg_rew  << endl;
            std::cout << "   - c2g : " << c2g_rew << endl;

            oph.init_HH_reweighter(ot, kl_coeffs, kl_map, kl_histo);
            oph.hhreweighter_kl_  = kl_rew;
            oph.hhreweighter_kt_  = kt_rew;
            oph.hhreweighter_c2_  = c2_rew;
            oph.hhreweighter_cg_  = cg_rew;
            oph.hhreweighter_c2g_ = c2g_rew;
        }
    }

    // Initialize trigger matching in Data/MC
    if (useTriggerMatching)  oph.initializeTriggerMatching(ot, 4);

    jsonLumiFilter jlf;
    if (is_data)
        jlf.loadJSON(config.readStringOpt("data::lumimask")); // just read the info for data, so if I just skim MC I'm not forced to parse a JSON

    ////////////////////////////////////////////////////////////////////////
    // Execute event loop
    ////////////////////////////////////////////////////////////////////////

    int maxEvts = opts["maxEvts"].as<int>();
    if (maxEvts >= 0)
        cout << "[INFO] ... running on : " << maxEvts << " events" << endl;

    for (int iEv = 0; true; ++iEv)
    {
        if (maxEvts >= 0 && iEv >= maxEvts)
            break;

        if (!nat.Next()) break;
        if (iEv % 10000 == 0) cout << "... processing event " << iEv << endl;

        if (is_data && !jlf.isValid(*nat.run, *nat.luminosityBlock)){
            continue; // not a valid lumi
        }
        
        //To address problem in NLO signal genweight      
        bool weight_check = true;      
        if(!is_data && is_NLO_sig) weight_check = oph.checkEventWeight(nat, ot, ec);
        if(!weight_check) continue;

        ot.clear();
        EventInfo ei;

        // HH reweight needs the GEN info -> keep first
        if (is_signal){
            oph.select_gen_HH(nat, ei);
            oph.select_gen_bb_bb(nat, ei);            
            if (is_VBF_sig) {
                bool got_gen_VBF = oph.select_gen_VBF_partons(nat, ei);
                if (!got_gen_VBF){
                    cout << "Failed on iEv = " << iEv << " evt num = " << *(nat.event) << " run = " << *(nat.run) << endl;
                    continue;
                }
            }
        }

        //Event weights included in the denominator        
        double weight = 1.;
        if(!is_data) weight = oph.calculateEventWeight(nat, ei, ot, ec);
        ec.updateProcessed(weight);
        
        //Match gen-reco jets to quarks 
        //if (is_signal) oph.match_gen_recojets(nat, ei, is_VBF_sig);
        
        //Trigger cut (only if asked explicitly in cfg file)
        std::vector<std::string> listOfPassedTriggers = nat.getTrgPassed();
        if( listOfPassedTriggers.size() == 0  && triggerVector.size()>0 ) continue;
        ec.updateTriggered(weight);

        //Preselecting jets
        if (!oph.select_bbbbjj_jets(nat, ei, ot)) continue;

        //Calculate event weights not included in the denominator (Trigger, WP bTagSF, L1prefiring, XS)
        if (!is_data) oph.CalculateEventbyEventScaleFactors(nat,ot,ei,xs);
        
        //Trigger matching using preselected objects (only if asked explicitly in cfg file)
        if (useTriggerMatching) oph.TriggerMatchingModule(nat,ot,ei, listOfPassedTriggers);

        oph.save_objects_for_cut(nat, ot,ei);

        oph.AddBoostedVariables(nat, ei, datasetYear);

        ec.updateSelected(weight);
        su::fill_output_tree(ot, nat, ei);

    }

    outputFile.cd();
    ot.write();
    ec.write();

    // for reshaping scale factors, save additionally the extra histogram with the partial normalisations
    if (config.readStringOpt("parameters::BTagScaleFactorMethod") == "Reshaping"){
        oph.writebTagReshapingHisto();
    }

}


