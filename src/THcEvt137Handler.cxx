#include "THcEvt137Handler.h"
#include "CodaDecoder.h"
#include "THaGlobals.h"
#include "THcGlobals.h"
#include "THaRunBase.h"
#include "THaEvData.h"
#include "DAQconfig.h"
#include "THcParmList.h"
#include "TROOT.h"
#include <iostream>
#include <sstream>
#include <bits/stdc++.h>

using namespace std;

/////////////////////////////////////////////////////////////////////
//
// Parse FADC250 and VTP config data (ev 137) -- regular text
// 
// Format: key value
// FADC250 config is listed for all slots for the given roc
// e.g. FADC250_SLOT 3
// e.g. FADC250_ALLCH_PED 386.917 412.167 ... (for 16 channels)
//
// FADC250 parameters (NSA, NSB) are read in THcHitList
// 
/////////////////////////////////////////////////////////////////////

//_______________________________________________________________
THcEvt137Handler::THcEvt137Handler( const char* name,
				    const char* description ) :
  THaEvtTypeHandler(name, description),
  fNDecoded(0),
  fConfigTree(nullptr),
  fMakeConfigTree(true),
  fMakeParms(false),
  fCounter(0)
{
}

//_______________________________________________________________
THcEvt137Handler::~THcEvt137Handler()
{
  if(fMakeParms) {
    for( auto& cfg : fConfigDataMap )
      gHcParms->RemoveName(Form("g_%s", cfg.first.data()));
  }

  if( !TROOT::Initialized()) {
    delete fConfigTree;
  }
  
}

//_______________________________________________________________
THaAnalysisObject::EStatus THcEvt137Handler::Init( const TDatime& date )
{

  // default event type 
  if( fEvtTypes.empty() ) {
    fEvtTypes.emplace_back(137);
  }

  return THaEvtTypeHandler::Init(date);
}

//_______________________________________________________________
Int_t THcEvt137Handler::End( THaRunBase* )
{

  // Save the tree, write into the output 
  if(fMakeConfigTree) { SaveConfigData(); }

  return 0;
}

//_______________________________________________________________
void THcEvt137Handler::AddEvtType( UInt_t evtype )
{
  // We don't want to add this event type to the evt type list of THaEvtTypeHandler
  // eventtypes from THaEvtTypeHandler is looked up by all inherited EvtTypeHandler classes
  // Instead, we set the event types only relevant for this class
  
  if( std::find(fEvtTypes.begin(), fEvtTypes.end(), evtype ) == fEvtTypes.end() )
    fEvtTypes.emplace_back(evtype);
}

//_______________________________________________________________
void THcEvt137Handler::MakeParms()
{
  for(auto &cfg : fConfigDataMap ) {
    string keyname = cfg.first;
    const auto &vals = cfg.second.pars;

    int nval = vals.size();

    gHcParms->RemoveName(Form("g_%s", keyname.data()));
    if(nval == 1)
      gHcParms->Define(Form("g_%s", keyname.data()), keyname.data(), vals[0]);
    else
      gHcParms->Define(Form("g_%s", keyname.data()), keyname.data(), vals); // vector type is supported
  }
}

//_______________________________________________________________
std::string THcEvt137Handler::GetInfo(const char* keyname )
{
  // return parameters as a single string 
  auto it = fConfigDataMap.find(keyname);
  if( it != fConfigDataMap.end() )
    return it->second.pars_str;
  else
    return "";
}

//_______________________________________________________________
int THcEvt137Handler::GetNSA( UInt_t crate, UInt_t slot )
{
  int val = -1;
  string keyname = "FADC250_NSA_" + std::to_string(crate) + "_" + std::to_string(slot);

  auto it = fConfigDataMap.find(keyname);
  if( it != fConfigDataMap.end() ) {
    val = static_cast<int>(it->second.pars[0])/4;
  }
  return val;
}

//_______________________________________________________________
int THcEvt137Handler::GetNSB( UInt_t crate, UInt_t slot )
{
  int val = -1;
  string keyname = "FADC250_NSB_" + std::to_string(crate) + "_" + std::to_string(slot);

  auto it = fConfigDataMap.find(keyname);
  if( it != fConfigDataMap.end() ) {
    val = static_cast<int>(it->second.pars[0])/4;
  }
  return val;
}


//_______________________________________________________________
int THcEvt137Handler::GetNPED( UInt_t crate, UInt_t slot )
{
  int val = -1;
  string keyname = "FADC250_NPED_" + std::to_string(crate) + "_" + std::to_string(slot);

  auto it = fConfigDataMap.find(keyname);
  if( it != fConfigDataMap.end() ) {
    val = static_cast<int>(it->second.pars[0]);
  }
  return val;
}

//_______________________________________________________________
Double_t THcEvt137Handler::GetPED( UInt_t crate, UInt_t slot, UInt_t ch )
{
  Double_t ped = 0;
  string keyname = "FADC250_ALLCH_PED_" + std::to_string(crate) + "_" + std::to_string(slot);
  auto it = fConfigDataMap.find(keyname);
  if( it != fConfigDataMap.end() ) {
    ped = it->second.pars[ch];
  }
  return ped;
}

//_______________________________________________________________
void THcEvt137Handler::SaveConfigData()
{
  // Init output tree
  if( fMakeConfigTree && !fConfigTree ) {
      fConfigTree = new TTree("THC", "Config parameter tree");
  }    

  vector<Double_t> fVars;
  vector<vector<Double_t>> fArrays;

  fVars.resize(fCounter);
  fArrays.resize(fCounter);
  UInt_t ivar = 0, iarr = 0; // counter

  // Init tree
  fConfigTree->Branch("ROCNum", &fRoc);
  for( auto &cfg : fConfigDataMap ){
    string bname = cfg.first; // branch name
    auto bpars = cfg.second.pars; // 

    int npar = bpars.size();

    if(npar == 1) {
      fConfigTree->Branch(Form("%s", bname.data()), &fVars[ivar]);
      fVars[ivar] = bpars[0];
      ivar++;
    }
    else {
      fConfigTree->Branch(Form("%s", bname.data()), &fArrays[iarr]);
      fArrays[iarr] = bpars;
      iarr++;
    }
  }

  fConfigTree->Fill();
  fConfigTree->Write();
}

//_______________________________________________________________
Int_t THcEvt137Handler::Analyze( THaEvData* evdata )
{
  // Check event type
  UInt_t evtype = evdata->GetEvType();
  if( std::find(fEvtTypes.begin(), fEvtTypes.end(), evtype) == fEvtTypes.end() )
    return -1;

  // ROC
  UInt_t roc = gHaRun->GetDAQConfigTag(fNDecoded);
  fRoc.emplace_back(roc);

  auto cinfo = gHaRun->GetDAQConfig(fNDecoded);
  istringstream ifstr(cinfo);
  string line;
  string slot = "";

  while( getline(ifstr, line) ) {

    // skip blank lines
    if( line.find_first_not_of(" \t") == string::npos ) 
      continue;

    auto items = Podd::vsplit(line);
    if( !items.empty() ) {
      string& key = items[0];
      string val;
      val.reserve(line.size());

      std::vector<Double_t> v_val;
      for( size_t j = 1, e = items.size(); j < e; ++j ) {
	val.append(items[j]);
	if( j + 1 != e )
	  val.append(" ");

	//for simplicity make them all double vars
	v_val.emplace_back(std::stod(items[j]));
      }

      // Per slot config for FADC250
      // This assumes that SLOT information is the first line for FADC250 config list
      if( key == "FADC250_SLOT" ){
	slot.replace(0, slot.length(), val);
      }	  

      // Define new unique key for Parm list
      string new_key = key + "_" + std::to_string(roc);
      if( slot.length() > 0)
	new_key = new_key + "_" + slot;

      // Remove the existing element if the key already exists
      // and will overwrite with new parameters
      if( fConfigDataMap.find(new_key) != fConfigDataMap.end() )
	fConfigDataMap.erase(new_key);

      // Add to the container
      ConfigData config_data;
      config_data.roc = roc;
      config_data.slot = -1;
      if( slot.length() > 0 )
	config_data.slot = std::stoi(slot);
      config_data.parname = key;
      config_data.pars = v_val;
      config_data.pars_str = val;
      fConfigDataMap.emplace(std::move(new_key), std::move(config_data));

      fCounter++;
    }
  }// getline

  // Add to the Parm List
  if(fMakeParms)
    MakeParms();

  fNDecoded++;

  return fNDecoded;
}

ClassImp(THcEvt137Handler)
