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

using namespace std;

/////////////////////////////////////////////////////////////////////
//
// Parse FADC250 and VTP config data (ev 137) -- regular text
// 
/////////////////////////////////////////////////////////////////////

//_______________________________________________________________
THcEvt137Handler::THcEvt137Handler( const char* name,
				    const char* description ) :
  THaEvtTypeHandler(name, description), fNDecoded(0),
  fConfigTree(nullptr), fMakeConfigTree(true), fMakeParms(true),
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
  cout << "THcEvt137Handler::End" << endl;

  // Save the tree into the output 
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

    // Define global variables
    gHcParms->RemoveName(Form("g_%s", keyname.data()));
    if(nval == 1)
      gHcParms->Define(Form("g_%s", keyname.data()), keyname.data(), vals[0]);
    else
      gHcParms->Define(Form("g_%s", keyname.data()), keyname.data(), vals); // vector type is supported
  }

  /*
  const auto* pvar = gHcParms->Find("g_FADC250_ALLCH_PED_2_9");
  if(pvar){
    if(pvar->IsVector()){
      auto v1 = pvar->GetValues();
      cout << pvar->GetLen() << endl;
      cout << v1[0] << endl;
      cout << v1[1] << endl;
      cout << v1[15] << endl;
    }
  }
  */
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
  UInt_t evtype = evdata->GetEvType();

  // Check event type
  if( std::find(fEvtTypes.begin(), fEvtTypes.end(), evtype) == fEvtTypes.end() )
    return -1;

  UInt_t evlen = evdata->GetEvLength();
  auto bankinfo = Decoder::CodaDecoder::GetBank(evdata->GetRawDataBuffer(), 0 , evlen);
  if( bankinfo.status_ != Decoder::CodaDecoder::BankInfo::kOK ) {
    ostringstream ostr;
    ostr << "THcEv137Handler: CODA3 bank decoder error \""
	 << bankinfo.Errtxt() << "\"";
    throw Decoder::CodaDecoder::coda_format_error(ostr.str() );
  }

  UInt_t roc = bankinfo.tag_;
  fRoc.emplace_back(roc);

  auto* ifo = DAQInfoExtra::GetFrom(evdata->GetExtra());
  if( !ifo ) return -1;

  string slot = "";

  auto this_info = ifo->strings[fNDecoded];
  istringstream ifstr(this_info);
  string line;
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

      // Define new key for Parm list
      string new_key = key + "_" + std::to_string(roc);
      if( slot.length() > 0)
	new_key = new_key + "_" + slot;

      // Remove the existing element if the key already exists
      // and will override with new parameters
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

  return 0;
}

ClassImp(THcEvt137Handler)
