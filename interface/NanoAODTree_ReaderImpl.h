/**
** class  : NanoAODTree_ReaderImpl
** author : L. Cadamuro (UF)
** date   : 27/12/2017
** brief  : interface to the Nano AOD Event ntuple. It contains the TTreeReader and the single branch readers.
**        : comment out the branches you are not interested in to speed up
**        :
**        : NOTE: this is the implementation that uses the TTreeReader to run
**        : Unfortunately a ROOT bug causes it to go segfault when some branches are not read
**/

#ifndef NANOAODTREE_READERIMPL_H
#define NANOAODTREE_READERIMPL_H

#include "TROOT.h"
#include "TChain.h"
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"
#include "TriggerReader_ReaderImpl.h"
#include "NanoReaderValue.h"
#include "NanoReaderArray.h"

#include <vector>
#include <iostream>
#include <memory>

// #define ENABLE_JET      true
// #define ENABLE_FATJET   true
// #define ENABLE_ELECTRON true
// #define ENABLE_CaloMET  true

class NanoAODTree_ReaderImpl {
    public:
    
        // methods
        NanoAODTree_ReaderImpl(TChain* chain, bool isData = false) :
            fReader(chain)        ,
            trg_reader_(&fReader) ,
            old_tree_nr_(-1)      ,
            is_data_ (isData)     ,
            proc_first_ev_(false)
        {};
        
        ~NanoAODTree_ReaderImpl(){};

        TriggerReader_ReaderImpl& triggerReader(){return trg_reader_;}

        // bool Next() {return fReader.Next();}
        bool Next();

        // the chain and TTreeReader
        TTreeReader   fReader;

        // the trigger reader - a special care is needed for branches that can disappear
        TriggerReader_ReaderImpl trg_reader_;

        // the count of the current ttree number, to verify at Next()
        int old_tree_nr_;
        const bool is_data_;
        bool proc_first_ev_;

        // tree readers
        TTreeReaderValue<UInt_t>    run                                  {fReader, "run"};
        TTreeReaderValue<UInt_t>    luminosityBlock                      {fReader, "luminosityBlock"};
        TTreeReaderValue<ULong64_t> event                                {fReader, "event"};
        
        TTreeReaderValue<Float_t>   CaloMET_phi                          {fReader, "CaloMET_phi"};
        TTreeReaderValue<Float_t>   CaloMET_pt                           {fReader, "CaloMET_pt"};
        TTreeReaderValue<Float_t>   CaloMET_sumEt                        {fReader, "CaloMET_sumEt"};

        TTreeReaderValue<UInt_t>    nElectron                            {fReader, "nElectron"};
        TTreeReaderArray<Float_t>   Electron_deltaEtaSC                  {fReader, "Electron_deltaEtaSC"};
        TTreeReaderArray<Float_t>   Electron_dr03EcalRecHitSumEt         {fReader, "Electron_dr03EcalRecHitSumEt"};
        TTreeReaderArray<Float_t>   Electron_dr03HcalDepth1TowerSumEt    {fReader, "Electron_dr03HcalDepth1TowerSumEt"};
        TTreeReaderArray<Float_t>   Electron_dr03TkSumPt                 {fReader, "Electron_dr03TkSumPt"};
        TTreeReaderArray<Float_t>   Electron_dxy                         {fReader, "Electron_dxy"};
        TTreeReaderArray<Float_t>   Electron_dxyErr                      {fReader, "Electron_dxyErr"};
        TTreeReaderArray<Float_t>   Electron_dz                          {fReader, "Electron_dz"};
        TTreeReaderArray<Float_t>   Electron_dzErr                       {fReader, "Electron_dzErr"};
        TTreeReaderArray<Float_t>   Electron_eCorr                       {fReader, "Electron_eCorr"};
        TTreeReaderArray<Float_t>   Electron_eInvMinusPInv               {fReader, "Electron_eInvMinusPInv"};
        TTreeReaderArray<Float_t>   Electron_energyErr                   {fReader, "Electron_energyErr"};
        TTreeReaderArray<Float_t>   Electron_eta                         {fReader, "Electron_eta"};
        TTreeReaderArray<Float_t>   Electron_hoe                         {fReader, "Electron_hoe"};
        TTreeReaderArray<Float_t>   Electron_ip3d                        {fReader, "Electron_ip3d"};
        TTreeReaderArray<Float_t>   Electron_mass                        {fReader, "Electron_mass"};
        TTreeReaderArray<Float_t>   Electron_miniPFRelIso_all            {fReader, "Electron_miniPFRelIso_all"};
        TTreeReaderArray<Float_t>   Electron_miniPFRelIso_chg            {fReader, "Electron_miniPFRelIso_chg"};
        TTreeReaderArray<Float_t>   Electron_mvaSpring16GP               {fReader, "Electron_mvaSpring16GP"};
        TTreeReaderArray<Float_t>   Electron_mvaSpring16HZZ              {fReader, "Electron_mvaSpring16HZZ"};
        TTreeReaderArray<Float_t>   Electron_pfRelIso03_all              {fReader, "Electron_pfRelIso03_all"};
        TTreeReaderArray<Float_t>   Electron_pfRelIso03_chg              {fReader, "Electron_pfRelIso03_chg"};
        TTreeReaderArray<Float_t>   Electron_phi                         {fReader, "Electron_phi"};
        TTreeReaderArray<Float_t>   Electron_pt                          {fReader, "Electron_pt"};
        TTreeReaderArray<Float_t>   Electron_r9                          {fReader, "Electron_r9"};
        TTreeReaderArray<Float_t>   Electron_sieie                       {fReader, "Electron_sieie"};
        TTreeReaderArray<Float_t>   Electron_sip3d                       {fReader, "Electron_sip3d"};
        TTreeReaderArray<Float_t>   Electron_mvaTTH                      {fReader, "Electron_mvaTTH"};
        TTreeReaderArray<Int_t>     Electron_charge                      {fReader, "Electron_charge"};
        TTreeReaderArray<Int_t>     Electron_cutBased                    {fReader, "Electron_cutBased"};
        TTreeReaderArray<Int_t>     Electron_cutBased_HLTPreSel          {fReader, "Electron_cutBased_HLTPreSel"};
        TTreeReaderArray<Int_t>     Electron_jetIdx                      {fReader, "Electron_jetIdx"};
        TTreeReaderArray<Int_t>     Electron_pdgId                       {fReader, "Electron_pdgId"};
        TTreeReaderArray<Int_t>     Electron_photonIdx                   {fReader, "Electron_photonIdx"};
        TTreeReaderArray<Int_t>     Electron_tightCharge                 {fReader, "Electron_tightCharge"};
        TTreeReaderArray<Int_t>     Electron_vidNestedWPBitmap           {fReader, "Electron_vidNestedWPBitmap"};
        TTreeReaderArray<Bool_t>    Electron_convVeto                    {fReader, "Electron_convVeto"};
        TTreeReaderArray<Bool_t>    Electron_cutBased_HEEP               {fReader, "Electron_cutBased_HEEP"};
        TTreeReaderArray<Bool_t>    Electron_isPFcand                    {fReader, "Electron_isPFcand"};
        TTreeReaderArray<UChar_t>   Electron_lostHits                    {fReader, "Electron_lostHits"};
        TTreeReaderArray<Bool_t>    Electron_mvaSpring16GP_WP80          {fReader, "Electron_mvaSpring16GP_WP80"};
        TTreeReaderArray<Bool_t>    Electron_mvaSpring16GP_WP90          {fReader, "Electron_mvaSpring16GP_WP90"};
        TTreeReaderArray<Bool_t>    Electron_mvaSpring16HZZ_WPL          {fReader, "Electron_mvaSpring16HZZ_WPL"};

        // TTreeReaderArray<Int_t>     Electron_genPartIdx                  {fReader, "Electron_genPartIdx"};
        // TTreeReaderArray<UChar_t>   Electron_genPartFlav                 {fReader, "Electron_genPartFlav"};
        TTreeReaderArray<Int_t>     Electron_genPartIdx                   {fReader, "Electron_genPartIdx"};
        TTreeReaderArray<UChar_t>   Electron_genPartFlav                  {fReader, "Electron_genPartFlav"};

        TTreeReaderArray<UChar_t>   Electron_cleanmask                   {fReader, "Electron_cleanmask"};

        TTreeReaderValue<UInt_t>    nFatJet                              {fReader, "nFatJet"};
        TTreeReaderArray<Float_t>   FatJet_area                          {fReader, "FatJet_area"};
        TTreeReaderArray<Float_t>   FatJet_btagCMVA                      {fReader, "FatJet_btagCMVA"};
        TTreeReaderArray<Float_t>   FatJet_btagCSVV2                     {fReader, "FatJet_btagCSVV2"};
        TTreeReaderArray<Float_t>   FatJet_btagDeepB                     {fReader, "FatJet_btagDeepB"};
        TTreeReaderArray<Float_t>   FatJet_btagHbb                       {fReader, "FatJet_btagHbb"};
        TTreeReaderArray<Float_t>   FatJet_eta                           {fReader, "FatJet_eta"};
        TTreeReaderArray<Float_t>   FatJet_mass                          {fReader, "FatJet_mass"};
        TTreeReaderArray<Float_t>   FatJet_msoftdrop                     {fReader, "FatJet_msoftdrop"};
        TTreeReaderArray<Float_t>   FatJet_n2b1                          {fReader, "FatJet_n2b1"};
        TTreeReaderArray<Float_t>   FatJet_n3b1                          {fReader, "FatJet_n3b1"};
        TTreeReaderArray<Float_t>   FatJet_phi                           {fReader, "FatJet_phi"};
        TTreeReaderArray<Float_t>   FatJet_pt                            {fReader, "FatJet_pt"};
        TTreeReaderArray<Float_t>   FatJet_tau1                          {fReader, "FatJet_tau1"};
        TTreeReaderArray<Float_t>   FatJet_tau2                          {fReader, "FatJet_tau2"};
        TTreeReaderArray<Float_t>   FatJet_tau3                          {fReader, "FatJet_tau3"};
        TTreeReaderArray<Float_t>   FatJet_tau4                          {fReader, "FatJet_tau4"};
        TTreeReaderArray<Int_t>     FatJet_subJetIdx1                    {fReader, "FatJet_subJetIdx1"};
        TTreeReaderArray<Int_t>     FatJet_subJetIdx2                    {fReader, "FatJet_subJetIdx2"};

        TTreeReaderValue<UInt_t>    nGenJetAK8                           {fReader, "nGenJetAK8"};
        TTreeReaderArray<Float_t>   GenJetAK8_eta                        {fReader, "GenJetAK8_eta"};
        TTreeReaderArray<Float_t>   GenJetAK8_mass                       {fReader, "GenJetAK8_mass"};
        TTreeReaderArray<Float_t>   GenJetAK8_phi                        {fReader, "GenJetAK8_phi"};
        TTreeReaderArray<Float_t>   GenJetAK8_pt                         {fReader, "GenJetAK8_pt"};
        
        TTreeReaderValue<UInt_t>    nGenJet                              {fReader, "nGenJet"};
        TTreeReaderArray<Float_t>   GenJet_eta                           {fReader, "GenJet_eta"};
        TTreeReaderArray<Float_t>   GenJet_mass                          {fReader, "GenJet_mass"};
        TTreeReaderArray<Float_t>   GenJet_phi                           {fReader, "GenJet_phi"};
        TTreeReaderArray<Float_t>   GenJet_pt                            {fReader, "GenJet_pt"};
        
        TTreeReaderValue<UInt_t>    nGenPart                             {fReader, "nGenPart"};
        TTreeReaderArray<Float_t>   GenPart_eta                          {fReader, "GenPart_eta"};
        TTreeReaderArray<Float_t>   GenPart_mass                         {fReader, "GenPart_mass"};
        TTreeReaderArray<Float_t>   GenPart_phi                          {fReader, "GenPart_phi"};
        TTreeReaderArray<Float_t>   GenPart_pt                           {fReader, "GenPart_pt"};
        TTreeReaderArray<Int_t>     GenPart_genPartIdxMother             {fReader, "GenPart_genPartIdxMother"};
        TTreeReaderArray<Int_t>     GenPart_pdgId                        {fReader, "GenPart_pdgId"};
        TTreeReaderArray<Int_t>     GenPart_status                       {fReader, "GenPart_status"};
        TTreeReaderArray<Int_t>     GenPart_statusFlags                  {fReader, "GenPart_statusFlags"};
        
        TTreeReaderValue<Float_t>   Generator_x1                         {fReader, "Generator_x1"};
        TTreeReaderValue<Float_t>   Generator_x2                         {fReader, "Generator_x2"};
        
        TTreeReaderValue<UInt_t>    nGenVisTau                           {fReader, "nGenVisTau"};
        TTreeReaderArray<Float_t>   GenVisTau_eta                        {fReader, "GenVisTau_eta"};
        TTreeReaderArray<Float_t>   GenVisTau_mass                       {fReader, "GenVisTau_mass"};
        TTreeReaderArray<Float_t>   GenVisTau_phi                        {fReader, "GenVisTau_phi"};
        TTreeReaderArray<Float_t>   GenVisTau_pt                         {fReader, "GenVisTau_pt"};
        TTreeReaderArray<Int_t>     GenVisTau_charge                     {fReader, "GenVisTau_charge"};
        TTreeReaderArray<Int_t>     GenVisTau_genPartIdxMother           {fReader, "GenVisTau_genPartIdxMother"};
        TTreeReaderArray<Int_t>     GenVisTau_status                     {fReader, "GenVisTau_status"};
        
        TTreeReaderValue<Float_t>   genWeight                            {fReader, "genWeight"};
        TTreeReaderValue<Float_t>   LHEWeight_originalXWGTUP             {fReader, "LHEWeight_originalXWGTUP"};
        TTreeReaderValue<UInt_t>    nLHEPdfWeight                        {fReader, "nLHEPdfWeight"};
        TTreeReaderArray<Float_t>   LHEPdfWeight                         {fReader, "LHEPdfWeight"};
        TTreeReaderValue<UInt_t>    nLHEScaleWeight                      {fReader, "nLHEScaleWeight"};
        TTreeReaderArray<Float_t>   LHEScaleWeight                       {fReader, "LHEScaleWeight"};
        
        TTreeReaderValue<UInt_t>    nJet                                 {fReader, "nJet"};
        TTreeReaderArray<Float_t>   Jet_area                             {fReader, "Jet_area"};
        TTreeReaderArray<Float_t>   Jet_btagCMVA                         {fReader, "Jet_btagCMVA"};
        TTreeReaderArray<Float_t>   Jet_btagCSVV2                        {fReader, "Jet_btagCSVV2"};
        TTreeReaderArray<Float_t>   Jet_btagDeepB                        {fReader, "Jet_btagDeepB"};
        TTreeReaderArray<Float_t>   Jet_btagDeepC                        {fReader, "Jet_btagDeepC"};
        TTreeReaderArray<Float_t>   Jet_chEmEF                           {fReader, "Jet_chEmEF"};
        TTreeReaderArray<Float_t>   Jet_chHEF                            {fReader, "Jet_chHEF"};
        TTreeReaderArray<Float_t>   Jet_eta                              {fReader, "Jet_eta"};
        TTreeReaderArray<Float_t>   Jet_mass                             {fReader, "Jet_mass"};
        TTreeReaderArray<Float_t>   Jet_neEmEF                           {fReader, "Jet_neEmEF"};
        TTreeReaderArray<Float_t>   Jet_neHEF                            {fReader, "Jet_neHEF"};
        TTreeReaderArray<Float_t>   Jet_phi                              {fReader, "Jet_phi"};
        TTreeReaderArray<Float_t>   Jet_pt                               {fReader, "Jet_pt"};
        TTreeReaderArray<Float_t>   Jet_qgl                              {fReader, "Jet_qgl"};
        TTreeReaderArray<Float_t>   Jet_rawFactor                        {fReader, "Jet_rawFactor"};
        TTreeReaderArray<Float_t>   Jet_bRegCorr                         {fReader, "Jet_bRegCorr"};
        TTreeReaderArray<Int_t>     Jet_electronIdx1                     {fReader, "Jet_electronIdx1"};
        TTreeReaderArray<Int_t>     Jet_electronIdx2                     {fReader, "Jet_electronIdx2"};
        TTreeReaderArray<Int_t>     Jet_jetId                            {fReader, "Jet_jetId"};
        TTreeReaderArray<Int_t>     Jet_muonIdx1                         {fReader, "Jet_muonIdx1"};
        TTreeReaderArray<Int_t>     Jet_muonIdx2                         {fReader, "Jet_muonIdx2"};
        TTreeReaderArray<Int_t>     Jet_nConstituents                    {fReader, "Jet_nConstituents"};
        TTreeReaderArray<Int_t>     Jet_nElectrons                       {fReader, "Jet_nElectrons"};
        TTreeReaderArray<Int_t>     Jet_nMuons                           {fReader, "Jet_nMuons"};
        TTreeReaderArray<Int_t>     Jet_puId                             {fReader, "Jet_puId"};

        TTreeReaderArray<Int_t>     Jet_genJetIdx                         {fReader, "Jet_genJetIdx"};
        TTreeReaderArray<Int_t>     Jet_hadronFlavour                     {fReader, "Jet_hadronFlavour"};
        TTreeReaderArray<Int_t>     Jet_partonFlavour                     {fReader, "Jet_partonFlavour"};

        TTreeReaderValue<Float_t>   LHE_HT                               {fReader, "LHE_HT"};
        TTreeReaderValue<Float_t>   LHE_HTIncoming                       {fReader, "LHE_HTIncoming"};
        TTreeReaderValue<Float_t>   LHE_Vpt                              {fReader, "LHE_Vpt"};
        TTreeReaderValue<UChar_t>   LHE_Njets                            {fReader, "LHE_Njets"};
        TTreeReaderValue<UChar_t>   LHE_Nb                               {fReader, "LHE_Nb"};
        TTreeReaderValue<UChar_t>   LHE_Nc                               {fReader, "LHE_Nc"};
        TTreeReaderValue<UChar_t>   LHE_Nuds                             {fReader, "LHE_Nuds"};
        TTreeReaderValue<UChar_t>   LHE_Nglu                             {fReader, "LHE_Nglu"};
        TTreeReaderValue<UChar_t>   LHE_NpNLO                            {fReader, "LHE_NpNLO"};
        TTreeReaderValue<UChar_t>   LHE_NpLO                             {fReader, "LHE_NpLO"};
        
        TTreeReaderValue<Float_t>    GenMET_phi                           {fReader, "GenMET_phi"};
        TTreeReaderValue<Float_t>    GenMET_pt                            {fReader, "GenMET_pt"};
        TTreeReaderValue<Float_t>   MET_MetUnclustEnUpDeltaX             {fReader, "MET_MetUnclustEnUpDeltaX"};
        TTreeReaderValue<Float_t>   MET_MetUnclustEnUpDeltaY             {fReader, "MET_MetUnclustEnUpDeltaY"};
        TTreeReaderValue<Float_t>   MET_covXX                            {fReader, "MET_covXX"};
        TTreeReaderValue<Float_t>   MET_covXY                            {fReader, "MET_covXY"};
        TTreeReaderValue<Float_t>   MET_covYY                            {fReader, "MET_covYY"};
        TTreeReaderValue<Float_t>   MET_phi                              {fReader, "MET_phi"};
        TTreeReaderValue<Float_t>   MET_pt                               {fReader, "MET_pt"};
        TTreeReaderValue<Float_t>   MET_significance                     {fReader, "MET_significance"};
        TTreeReaderValue<Float_t>   MET_sumEt                            {fReader, "MET_sumEt"};

        TTreeReaderValue<Float_t>   PuppiMET_phi                         {fReader, "PuppiMET_phi"};
        TTreeReaderValue<Float_t>   PuppiMET_pt                          {fReader, "PuppiMET_pt"};
        TTreeReaderValue<Float_t>   PuppiMET_sumEt                       {fReader, "PuppiMET_sumEt"};
        
        TTreeReaderValue<Float_t>   RawMET_phi                           {fReader, "RawMET_phi"};
        TTreeReaderValue<Float_t>   RawMET_pt                            {fReader, "RawMET_pt"};
        TTreeReaderValue<Float_t>   RawMET_sumEt                         {fReader, "RawMET_sumEt"};
        
        TTreeReaderValue<Float_t>   TkMET_phi                            {fReader, "TkMET_phi"};
        TTreeReaderValue<Float_t>   TkMET_pt                             {fReader, "TkMET_pt"};
        TTreeReaderValue<Float_t>   TkMET_sumEt                          {fReader, "TkMET_sumEt"};

        
        TTreeReaderValue<UInt_t>    nMuon                                {fReader, "nMuon"};
        TTreeReaderArray<Float_t>   Muon_dxy                             {fReader, "Muon_dxy"};
        TTreeReaderArray<Float_t>   Muon_dxyErr                          {fReader, "Muon_dxyErr"};
        TTreeReaderArray<Float_t>   Muon_dz                              {fReader, "Muon_dz"};
        TTreeReaderArray<Float_t>   Muon_dzErr                           {fReader, "Muon_dzErr"};
        TTreeReaderArray<Float_t>   Muon_eta                             {fReader, "Muon_eta"};
        TTreeReaderArray<Float_t>   Muon_ip3d                            {fReader, "Muon_ip3d"};
        TTreeReaderArray<Float_t>   Muon_mass                            {fReader, "Muon_mass"};
        TTreeReaderArray<Float_t>   Muon_miniPFRelIso_all                {fReader, "Muon_miniPFRelIso_all"};
        TTreeReaderArray<Float_t>   Muon_miniPFRelIso_chg                {fReader, "Muon_miniPFRelIso_chg"};
        TTreeReaderArray<Float_t>   Muon_pfRelIso03_all                  {fReader, "Muon_pfRelIso03_all"};
        TTreeReaderArray<Float_t>   Muon_pfRelIso03_chg                  {fReader, "Muon_pfRelIso03_chg"};
        TTreeReaderArray<Float_t>   Muon_pfRelIso04_all                  {fReader, "Muon_pfRelIso04_all"};
        TTreeReaderArray<Float_t>   Muon_phi                             {fReader, "Muon_phi"};
        TTreeReaderArray<Float_t>   Muon_pt                              {fReader, "Muon_pt"};
        TTreeReaderArray<Float_t>   Muon_ptErr                           {fReader, "Muon_ptErr"};
        TTreeReaderArray<Float_t>   Muon_segmentComp                     {fReader, "Muon_segmentComp"};
        TTreeReaderArray<Float_t>   Muon_sip3d                           {fReader, "Muon_sip3d"};
        TTreeReaderArray<Float_t>   Muon_mvaTTH                          {fReader, "Muon_mvaTTH"};
        TTreeReaderArray<Int_t>     Muon_charge                          {fReader, "Muon_charge"};
        TTreeReaderArray<Int_t>     Muon_jetIdx                          {fReader, "Muon_jetIdx"};
        TTreeReaderArray<Int_t>     Muon_nStations                       {fReader, "Muon_nStations"};
        TTreeReaderArray<Int_t>     Muon_nTrackerLayers                  {fReader, "Muon_nTrackerLayers"};
        TTreeReaderArray<Int_t>     Muon_pdgId                           {fReader, "Muon_pdgId"};
        TTreeReaderArray<Int_t>     Muon_tightCharge                     {fReader, "Muon_tightCharge"};
        TTreeReaderArray<UChar_t>   Muon_highPtId                        {fReader, "Muon_highPtId"};
        TTreeReaderArray<Bool_t>    Muon_isPFcand                        {fReader, "Muon_isPFcand"};
        TTreeReaderArray<Bool_t>    Muon_mediumId                        {fReader, "Muon_mediumId"};
        TTreeReaderArray<Bool_t>    Muon_softId                          {fReader, "Muon_softId"};
        TTreeReaderArray<Bool_t>    Muon_tightId                         {fReader, "Muon_tightId"};
        
        TTreeReaderValue<UInt_t>    nPhoton                              {fReader, "nPhoton"};
        TTreeReaderArray<Float_t>   Photon_eCorr                         {fReader, "Photon_eCorr"};
        TTreeReaderArray<Float_t>   Photon_energyErr                     {fReader, "Photon_energyErr"};
        TTreeReaderArray<Float_t>   Photon_eta                           {fReader, "Photon_eta"};
        TTreeReaderArray<Float_t>   Photon_hoe                           {fReader, "Photon_hoe"};
        TTreeReaderArray<Float_t>   Photon_mass                          {fReader, "Photon_mass"};
        TTreeReaderArray<Float_t>   Photon_mvaID                         {fReader, "Photon_mvaID"};
        TTreeReaderArray<Float_t>   Photon_pfRelIso03_all                {fReader, "Photon_pfRelIso03_all"};
        TTreeReaderArray<Float_t>   Photon_pfRelIso03_chg                {fReader, "Photon_pfRelIso03_chg"};
        TTreeReaderArray<Float_t>   Photon_phi                           {fReader, "Photon_phi"};
        TTreeReaderArray<Float_t>   Photon_pt                            {fReader, "Photon_pt"};
        TTreeReaderArray<Float_t>   Photon_r9                            {fReader, "Photon_r9"};
        TTreeReaderArray<Float_t>   Photon_sieie                         {fReader, "Photon_sieie"};
        TTreeReaderArray<Int_t>     Photon_charge                        {fReader, "Photon_charge"};
        TTreeReaderArray<Int_t>     Photon_cutBased                      {fReader, "Photon_cutBased"};
        TTreeReaderArray<Int_t>     Photon_electronIdx                   {fReader, "Photon_electronIdx"};
        TTreeReaderArray<Int_t>     Photon_jetIdx                        {fReader, "Photon_jetIdx"};
        TTreeReaderArray<Int_t>     Photon_pdgId                         {fReader, "Photon_pdgId"};
        TTreeReaderArray<Int_t>     Photon_vidNestedWPBitmap             {fReader, "Photon_vidNestedWPBitmap"};
        TTreeReaderArray<Bool_t>    Photon_electronVeto                  {fReader, "Photon_electronVeto"};
        TTreeReaderArray<Bool_t>    Photon_mvaID_WP80                    {fReader, "Photon_mvaID_WP80"};
        TTreeReaderArray<Bool_t>    Photon_mvaID_WP90                    {fReader, "Photon_mvaID_WP90"};
        TTreeReaderArray<Bool_t>    Photon_pixelSeed                     {fReader, "Photon_pixelSeed"};
        
        TTreeReaderValue<Int_t>      Pileup_nPU                           {fReader, "Pileup_nPU"};
        TTreeReaderValue<Int_t>      Pileup_nTrueInt                      {fReader, "Pileup_nTrueInt"};
        
        TTreeReaderValue<Float_t>   fixedGridRhoFastjetAll               {fReader, "fixedGridRhoFastjetAll"};
        TTreeReaderValue<Float_t>   fixedGridRhoFastjetCentralCalo       {fReader, "fixedGridRhoFastjetCentralCalo"};
        TTreeReaderValue<Float_t>   fixedGridRhoFastjetCentralNeutral    {fReader, "fixedGridRhoFastjetCentralNeutral"};
        
        TTreeReaderValue<UInt_t>     nGenDressedLepton                    {fReader, "nGenDressedLepton"};
        TTreeReaderArray<Float_t>    GenDressedLepton_eta                 {fReader, "GenDressedLepton_eta"};
        TTreeReaderArray<Float_t>    GenDressedLepton_mass                {fReader, "GenDressedLepton_mass"};
        TTreeReaderArray<Float_t>    GenDressedLepton_phi                 {fReader, "GenDressedLepton_phi"};
        TTreeReaderArray<Float_t>    GenDressedLepton_pt                  {fReader, "GenDressedLepton_pt"};
        TTreeReaderArray<Int_t>      GenDressedLepton_pdgId               {fReader, "GenDressedLepton_pdgId"};
        
        TTreeReaderValue<UInt_t>    nSoftActivityJet                     {fReader, "nSoftActivityJet"};
        TTreeReaderArray<Float_t>   SoftActivityJet_eta                  {fReader, "SoftActivityJet_eta"};
        TTreeReaderArray<Float_t>   SoftActivityJet_phi                  {fReader, "SoftActivityJet_phi"};
        TTreeReaderArray<Float_t>   SoftActivityJet_pt                   {fReader, "SoftActivityJet_pt"};
        TTreeReaderValue<Float_t>   SoftActivityJetHT                    {fReader, "SoftActivityJetHT"};
        TTreeReaderValue<Float_t>   SoftActivityJetHT10                  {fReader, "SoftActivityJetHT10"};
        TTreeReaderValue<Float_t>   SoftActivityJetHT2                   {fReader, "SoftActivityJetHT2"};
        TTreeReaderValue<Float_t>   SoftActivityJetHT5                   {fReader, "SoftActivityJetHT5"};
        TTreeReaderValue<Int_t>     SoftActivityJetNjets10               {fReader, "SoftActivityJetNjets10"};
        TTreeReaderValue<Int_t>     SoftActivityJetNjets2                {fReader, "SoftActivityJetNjets2"};
        TTreeReaderValue<Int_t>     SoftActivityJetNjets5                {fReader, "SoftActivityJetNjets5"};
        
        TTreeReaderValue<UInt_t>    nSubJet                              {fReader, "nSubJet"};
        TTreeReaderArray<Float_t>   SubJet_btagCMVA                      {fReader, "SubJet_btagCMVA"};
        TTreeReaderArray<Float_t>   SubJet_btagCSVV2                     {fReader, "SubJet_btagCSVV2"};
        TTreeReaderArray<Float_t>   SubJet_btagDeepB                     {fReader, "SubJet_btagDeepB"};
        TTreeReaderArray<Float_t>   SubJet_eta                           {fReader, "SubJet_eta"};
        TTreeReaderArray<Float_t>   SubJet_mass                          {fReader, "SubJet_mass"};
        TTreeReaderArray<Float_t>   SubJet_n2b1                          {fReader, "SubJet_n2b1"};
        TTreeReaderArray<Float_t>   SubJet_n3b1                          {fReader, "SubJet_n3b1"};
        TTreeReaderArray<Float_t>   SubJet_phi                           {fReader, "SubJet_phi"};
        TTreeReaderArray<Float_t>   SubJet_pt                            {fReader, "SubJet_pt"};
        TTreeReaderArray<Float_t>   SubJet_tau1                          {fReader, "SubJet_tau1"};
        TTreeReaderArray<Float_t>   SubJet_tau2                          {fReader, "SubJet_tau2"};
        TTreeReaderArray<Float_t>   SubJet_tau3                          {fReader, "SubJet_tau3"};
        TTreeReaderArray<Float_t>   SubJet_tau4                          {fReader, "SubJet_tau4"};
        
        TTreeReaderValue<UInt_t>    nTau                                 {fReader, "nTau"};
        TTreeReaderArray<Float_t>   Tau_chargedIso                       {fReader, "Tau_chargedIso"};
        TTreeReaderArray<Float_t>   Tau_dxy                              {fReader, "Tau_dxy"};
        TTreeReaderArray<Float_t>   Tau_dz                               {fReader, "Tau_dz"};
        TTreeReaderArray<Float_t>   Tau_eta                              {fReader, "Tau_eta"};
        TTreeReaderArray<Float_t>   Tau_footprintCorr                    {fReader, "Tau_footprintCorr"};
        TTreeReaderArray<Float_t>   Tau_leadTkDeltaEta                   {fReader, "Tau_leadTkDeltaEta"};
        TTreeReaderArray<Float_t>   Tau_leadTkDeltaPhi                   {fReader, "Tau_leadTkDeltaPhi"};
        TTreeReaderArray<Float_t>   Tau_leadTkPtOverTauPt                {fReader, "Tau_leadTkPtOverTauPt"};
        TTreeReaderArray<Float_t>   Tau_mass                             {fReader, "Tau_mass"};
        TTreeReaderArray<Float_t>   Tau_neutralIso                       {fReader, "Tau_neutralIso"};
        TTreeReaderArray<Float_t>   Tau_phi                              {fReader, "Tau_phi"};
        TTreeReaderArray<Float_t>   Tau_photonsOutsideSignalCone         {fReader, "Tau_photonsOutsideSignalCone"};
        TTreeReaderArray<Float_t>   Tau_pt                               {fReader, "Tau_pt"};
        TTreeReaderArray<Float_t>   Tau_puCorr                           {fReader, "Tau_puCorr"};
        TTreeReaderArray<Float_t>   Tau_rawAntiEle                       {fReader, "Tau_rawAntiEle"};
        TTreeReaderArray<Float_t>   Tau_rawIso                           {fReader, "Tau_rawIso"};
        TTreeReaderArray<Float_t>   Tau_rawMVAnewDM                      {fReader, "Tau_rawMVAnewDM"};
        TTreeReaderArray<Float_t>   Tau_rawMVAoldDM                      {fReader, "Tau_rawMVAoldDM"};
        TTreeReaderArray<Float_t>   Tau_rawMVAoldDMdR03                  {fReader, "Tau_rawMVAoldDMdR03"};
        TTreeReaderArray<Int_t>     Tau_charge                           {fReader, "Tau_charge"};
        TTreeReaderArray<Int_t>     Tau_decayMode                        {fReader, "Tau_decayMode"};
        TTreeReaderArray<Int_t>     Tau_jetIdx                           {fReader, "Tau_jetIdx"};
        TTreeReaderArray<Int_t>     Tau_rawAntiEleCat                    {fReader, "Tau_rawAntiEleCat"};
        TTreeReaderArray<UChar_t>   Tau_idAntiEle                        {fReader, "Tau_idAntiEle"};
        TTreeReaderArray<UChar_t>   Tau_idAntiMu                         {fReader, "Tau_idAntiMu"};
        TTreeReaderArray<Bool_t>    Tau_idDecayMode                      {fReader, "Tau_idDecayMode"};
        TTreeReaderArray<Bool_t>    Tau_idDecayModeNewDMs                {fReader, "Tau_idDecayModeNewDMs"};
        TTreeReaderArray<UChar_t>   Tau_idMVAnewDM                       {fReader, "Tau_idMVAnewDM"};
        TTreeReaderArray<UChar_t>   Tau_idMVAoldDM                       {fReader, "Tau_idMVAoldDM"};
        TTreeReaderArray<UChar_t>   Tau_idMVAoldDMdR03                   {fReader, "Tau_idMVAoldDMdR03"};
                
        TTreeReaderValue<UInt_t>    nTrigObj                             {fReader, "nTrigObj"};
        TTreeReaderArray<Float_t>   TrigObj_pt                           {fReader, "TrigObj_pt"};
        TTreeReaderArray<Float_t>   TrigObj_eta                          {fReader, "TrigObj_eta"};
        TTreeReaderArray<Float_t>   TrigObj_phi                          {fReader, "TrigObj_phi"};
        TTreeReaderArray<Float_t>   TrigObj_l1pt                         {fReader, "TrigObj_l1pt"};
        TTreeReaderArray<Float_t>   TrigObj_l1pt_2                       {fReader, "TrigObj_l1pt_2"};
        TTreeReaderArray<Float_t>   TrigObj_l2pt                         {fReader, "TrigObj_l2pt"};
        TTreeReaderArray<Int_t>     TrigObj_id                           {fReader, "TrigObj_id"};
        TTreeReaderArray<Int_t>     TrigObj_l1iso                        {fReader, "TrigObj_l1iso"};
        TTreeReaderArray<Int_t>     TrigObj_l1charge                     {fReader, "TrigObj_l1charge"};
        TTreeReaderArray<Int_t>     TrigObj_filterBits                   {fReader, "TrigObj_filterBits"};
        
        TTreeReaderValue<Int_t>      genTtbarId                           {fReader, "genTtbarId"};
        
        TTreeReaderValue<UInt_t>    nOtherPV                             {fReader, "nOtherPV"};
        TTreeReaderArray<Float_t>   OtherPV_z                            {fReader, "OtherPV_z"};
        
        TTreeReaderValue<Float_t>   PV_ndof                              {fReader, "PV_ndof"};
        TTreeReaderValue<Float_t>   PV_x                                 {fReader, "PV_x"};
        TTreeReaderValue<Float_t>   PV_y                                 {fReader, "PV_y"};
        TTreeReaderValue<Float_t>   PV_z                                 {fReader, "PV_z"};
        TTreeReaderValue<Float_t>   PV_chi2                              {fReader, "PV_chi2"};
        TTreeReaderValue<Float_t>   PV_score                             {fReader, "PV_score"};
        TTreeReaderValue<Int_t>     PV_npvs                              {fReader, "PV_npvs"};
        
        TTreeReaderValue<UInt_t>    nSV                                  {fReader, "nSV"};
        TTreeReaderArray<Float_t>   SV_dlen                              {fReader, "SV_dlen"};
        TTreeReaderArray<Float_t>   SV_dlenSig                           {fReader, "SV_dlenSig"};
        TTreeReaderArray<Float_t>   SV_pAngle                            {fReader, "SV_pAngle"};
                
        TTreeReaderArray<Int_t>     GenJetAK8_partonFlavour               {fReader, "GenJetAK8_partonFlavour"};
        TTreeReaderArray<UChar_t>   GenJetAK8_hadronFlavour               {fReader, "GenJetAK8_hadronFlavour"};
        TTreeReaderArray<Int_t>     GenJet_partonFlavour                  {fReader, "GenJet_partonFlavour"};
        TTreeReaderArray<UChar_t>   GenJet_hadronFlavour                  {fReader, "GenJet_hadronFlavour"};
        
        TTreeReaderArray<Int_t>     Muon_genPartIdx                       {fReader, "Muon_genPartIdx"};
        TTreeReaderArray<UChar_t>   Muon_genPartFlav                      {fReader, "Muon_genPartFlav"};
        
        TTreeReaderArray<Int_t>     Photon_genPartIdx                     {fReader, "Photon_genPartIdx"};
        TTreeReaderArray<UChar_t>   Photon_genPartFlav                    {fReader, "Photon_genPartFlav"};

        TTreeReaderArray<Int_t>     Tau_genPartIdx                        {fReader, "Tau_genPartIdx"};
        TTreeReaderArray<UChar_t>   Tau_genPartFlav                       {fReader, "Tau_genPartFlav"};
        
        TTreeReaderValue<Float_t>   MET_fiducialGenPhi                    {fReader, "MET_fiducialGenPhi"};
        TTreeReaderValue<Float_t>   MET_fiducialGenPt                     {fReader, "MET_fiducialGenPt"};
        
        TTreeReaderArray<UChar_t>   Jet_cleanmask                        {fReader, "Jet_cleanmask"};
        TTreeReaderArray<UChar_t>   Muon_cleanmask                       {fReader, "Muon_cleanmask"};
        TTreeReaderArray<UChar_t>   Photon_cleanmask                     {fReader, "Photon_cleanmask"};
        TTreeReaderArray<UChar_t>   Tau_cleanmask                        {fReader, "Tau_cleanmask"};
        
        TTreeReaderArray<Float_t>   SV_chi2                              {fReader, "SV_chi2"};
        TTreeReaderArray<Float_t>   SV_eta                               {fReader, "SV_eta"};
        TTreeReaderArray<Float_t>   SV_mass                              {fReader, "SV_mass"};
        TTreeReaderArray<Float_t>   SV_ndof                              {fReader, "SV_ndof"};
        TTreeReaderArray<Float_t>   SV_phi                               {fReader, "SV_phi"};
        TTreeReaderArray<Float_t>   SV_pt                                {fReader, "SV_pt"};
        TTreeReaderArray<Float_t>   SV_x                                 {fReader, "SV_x"};
        TTreeReaderArray<Float_t>   SV_y                                 {fReader, "SV_y"};
        TTreeReaderArray<Float_t>   SV_z                                 {fReader, "SV_z"};
};

#endif