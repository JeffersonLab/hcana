#include "THcEvt137Handler.h"
#include "CodaDecoder.h"
#include "THaGlobals.h"
#include "THcGlobals.h"
#include "THaRunBase.h"
#include "THaEvData.h"
#include "DAQconfig.h"
#include "THcParmList.h"
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
  THaEvtTypeHandler(name, description), fNDecoded(0)
{
}

//_______________________________________________________________
THcEvt137Handler::~THcEvt137Handler()
{
  for( auto& cfg : fConfigParmList ) {
    gHcParms->RemoveString(cfg.par);
  }
}

//_______________________________________________________________
THaAnalysisObject::EStatus THcEvt137Handler::Init( const TDatime& date )
{

  // default event type 
  if( fEvtTypes.empty() ) {
    fEvtTypes.push_back(137);
  }

  return THaEvtTypeHandler::Init(date);
}

//_______________________________________________________________
void THcEvt137Handler::AddEvtType( UInt_t evtype )
{
  // We don't want to add this event type to the evt type list of THaEvtTypeHandler
  // eventtypes from THaEvtTypeHandler is looked up by all inherited EvtTypeHandler classes
  // Instead, we set the event types only relevant for this class
  
  if( std::find(fEvtTypes.begin(), fEvtTypes.end(), evtype ) == fEvtTypes.end() )
    fEvtTypes.push_back(evtype);
}

//_______________________________________________________________
void THcEvt137Handler::AddParameter(std::string parname, std::string keyname)
{
  fConfigParmList.push_back( {Form("g_%s", parname.data()), keyname} );
}
 
//_______________________________________________________________
void THcEvt137Handler::MakeParms()
{
  // all data parsed as a single string varaible 

  for( auto& cfg : fConfigParmList ) {
    if( !GetInfo(cfg.key.data()).empty() ) {
      gHcParms->RemoveString(cfg.par);
      gHcParms->AddString(cfg.par, GetInfo(cfg.key.data()) );
    }
  }      
}

//_______________________________________________________________
std::string THcEvt137Handler::GetInfo(const char* keyname )
{
  auto it = fConfigData.find(keyname);
  if( it != fConfigData.end() )
    return it->second;
  else
    return "";
}

//_______________________________________________________________
UInt_t THcEvt137Handler::GetNSA( UInt_t crate, UInt_t slot )
{
  int nsa = 0;
  string keyname = "FADC250_NSA_" + std::to_string(crate) + "_" + std::to_string(slot);
  string val = GetInfo(keyname.data());
  if( val.length() > 0 )
    nsa = std::stoi(val);

  return nsa;
}

//_______________________________________________________________
UInt_t THcEvt137Handler::GetNSB( UInt_t crate, UInt_t slot )
{
  int nsb = 0;
  string keyname = "FADC250_NSB_" + std::to_string(crate) + "_" + std::to_string(slot);
  string val = GetInfo(keyname.data());
  if( val.length() > 0 )
    nsb = std::stoi(val);
 
  return nsb;
}


//_______________________________________________________________
UInt_t THcEvt137Handler::GetNPED( UInt_t crate, UInt_t slot )
{
  int nped = 0;
  string keyname = "FADC250_NPED_" + std::to_string(crate) + "_" + std::to_string(slot);
  string val = GetInfo(keyname.data());
  if( val.length() > 0 )
    nped = std::stoi(val);
 
  return nped;
}

//_______________________________________________________________
Double_t THcEvt137Handler::GetPED( UInt_t crate, UInt_t slot, UInt_t ch )
{
  vector<Double_t> v_ped;

  string keyname = "FADC250_ALLCH_PED_" + std::to_string(crate) + "_" + std::to_string(slot);
  string val = GetInfo(keyname.data());

  if( val.length() > 0 ) {
    istringstream ifstr(val);
    Double_t ped;
    while(ifstr >> ped) {
      v_ped.push_back(ped);      
    }      
  }

  if( ch+1 > v_ped.size() )
    return 0;
  else
    return v_ped[ch];
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

      for( size_t j = 1, e = items.size(); j < e; ++j ) {
	val.append(items[j]);
	if( j + 1 != e )
	  val.append(" ");
      }

      // Per slot config for FADC250
      if( key == "FADC250_SLOT" ){
	slot.replace(0, slot.length(), val);
      }	  

      // Define new key for Parm list
      string new_key = key + "_" + std::to_string(roc) + "_" + slot;

      // Add Parameters
      fConfigParmList.push_back( {Form("g_%s", new_key.data()), new_key} );
      fConfigData.emplace( std::move(new_key), std::move(val) );
    }
  }// getline

  // Add to the gHcParm list
  MakeParms();

  fNDecoded++;

  return 0;
}

ClassImp(THcEvt137Handler)
