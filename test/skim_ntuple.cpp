/*
** class  : skim_ntuple.cpp
** author : L. Cadamuro (UF)
** date   : 31/12/2017
** brief  : transforms a NanoAOD into a bbbb ntuple for the subsequent plots/analysis
*/

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
namespace oph = OfflineProducerHelper;

#include "OutputTree.h"
#include "SkimEffCounter.h"
#include "jsonLumiFilter.h"

#include "TFile.h"

using namespace std;

#include <experimental/any>
 
using namespace std::experimental;

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
        ("xs"       , po::value<float>(), "cross section [pb]")
        ("maxEvts"  , po::value<int>()->default_value(-1), "max number of events to process")
        // flags
        ("is-data",    po::value<bool>()->zero_tokens()->implicit_value(true)->default_value(false), "mark as a data sample (default is false)")
        ("is-signal",  po::value<bool>()->zero_tokens()->implicit_value(true)->default_value(false), "mark as a HH signal sample (default is false)")
        ("is-VBF-sig", po::value<bool>()->zero_tokens()->implicit_value(true)->default_value(false), "mark as a HH VBF signal sample (default is false)")
        ("save-p4",    po::value<bool>()->zero_tokens()->implicit_value(true)->default_value(false), "save the tlorentzvectors in the output")
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

    const float xs = (is_data ? 1.0 : opts["xs"].as<float>());
    cout << "[INFO] ... cross section is : " << xs << " pb" << endl;

    CfgParser config;
    if (!config.init(opts["cfg"].as<string>())) return 1;
    cout << "[INFO] ... using config file " << opts["cfg"].as<string>() << endl;

    //Read needed fields from config file

    std::map<std::string,any> parameterList;

    const string bbbbChoice = config.readStringOpt("parameters::bbbbChoice");
    
    parameterList.emplace("bbbbChoice",bbbbChoice);
    if(bbbbChoice == "OneClosestToMh"){
        parameterList.emplace("bbbbChoice""HiggsMass",config.readFloatOpt("parameters::HiggsMass"));
    }
    else if(bbbbChoice == "BothClosestToMh"){
        parameterList.emplace("HiggsMass",config.readFloatOpt("parameters::HiggsMass"));
    }
    else if(bbbbChoice == "MostBackToBack"){
        parameterList.emplace("HiggsMass",config.readFloatOpt("parameters::HiggsMass"));
    }
    else if(bbbbChoice == "HighestCSVandClosestToMh"){
        parameterList.emplace("HiggsMassLMR"        ,config.readFloatOpt("parameters::HiggsMassLMR"        ));
        parameterList.emplace("HiggsMassMMR"        ,config.readFloatOpt("parameters::HiggsMassMMR"        ));
        parameterList.emplace("LMRToMMRTransition"  ,config.readFloatOpt("parameters::LMRToMMRTransition"  ));
        parameterList.emplace("deepCSVcut"          ,config.readFloatOpt("parameters::deepCSVcut"          ));
    }
    // else if(other selection type){
    //     parameters fo be retreived;
    // }  
    else throw std::runtime_error("cannot recognize bbbb pair choice strategy " + bbbbChoice);

    cout << "[INFO] ... chosing bb bb jet pairs with strategy : " << bbbbChoice << endl;

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
    // else if(other selection type){
    //     parameters fo be retreived;
    // }  
    else throw std::runtime_error("cannot recognize event choice ObjectsForCut " + objectsForCut);


    oph::setParameterList(&parameterList);

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
    NanoAODTree nat (&ch, is_data);

    cout << "[INFO] ... loading the following triggers" << endl;
    for (auto trg : config.readStringListOpt("triggers::makeORof"))
        cout << "   - " << trg << endl;
    nat.triggerReader().setTriggers(config.readStringListOpt("triggers::makeORof"));

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

    //REMOVE_ME_BEGIN
    map<unsigned int, TH1F*> mapDeepCVSHistograms;
    for(unsigned int i=1; i<=4; ++i){
        mapDeepCVSHistograms[i] = new TH1F(Form("JetDeepCSVOrdered_DeepCSV_%i",i),Form("JetDeepCSVOrdered_DeepCSV_%i",i),25,0.,1.);
    }
    oph::setHistoMap(&mapDeepCVSHistograms);
    //REMOVE_ME_END

    oph::initializeUserDefinedBranches(ot);

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
        

        ot.clear();
        EventInfo ei;

        double weight = 1.;
        if(!is_data){
            // !!!!!!!!!!!  -------  FIXME: compute the weights -------  !!!!!!!!!!!
            weight=0.75;
        }
        ec.updateProcessed(weight);
        
        if( !nat.getTrgOr() ) continue;

        ec.updateTriggered(weight);

        if (!oph::select_bbbb_jets(nat, ei)) continue;

        if (is_signal){
            oph::select_gen_HH(nat, ei);
            if (!oph::select_gen_bb_bb(nat, ei))
                return 1;
            
        }

        if (is_VBF_sig){
            bool got_gen_VBF = oph::select_gen_VBF_partons(nat, ei);
            if (!got_gen_VBF){
                cout << "Failed on iEv = " << iEv << " evt num = " << *(nat.event) << " run = " << *(nat.run) << endl;
                return 1;
            }
        }

        oph::save_objects_for_cut(nat, ot);

        ec.updateSelected(weight);
        su::fill_output_tree(ot, nat, ei);

    }

    outputFile.cd();
    ot.write();
    ec.write();

    //REMOVE_ME_BEGIN
    for(unsigned int i=1; i<=4; ++i){
        mapDeepCVSHistograms[i]->Write();
    }
    //REMOVE_ME_END

}


