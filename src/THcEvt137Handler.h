#ifndef ROOT_THcEvt137Handler
#define ROOT_THcEvt137Handler

#include "THaEvtTypeHandler.h"
#include <vector>
#include <string>
#include <map>

class THcEvt137Handler : public THaEvtTypeHandler {
 public:

  THcEvt137Handler( const char* name, const char* description = "" );
  virtual ~THcEvt137Handler();
  
  virtual Int_t   Analyze( THaEvData *evdata );
  virtual EStatus Init( const TDatime& date );
  virtual void    AddEvtType( UInt_t evtype );
  virtual void    AddParameter(std::string parname, std::string keyname);

  std::vector<UInt_t> GetEvtTypes() { return fEvtTypes; }
  std::string GetInfo( const char* keyname );

  // Some useful getter functions for FADC250
  UInt_t GetNSA( UInt_t crate, UInt_t slot );
  UInt_t GetNSB( UInt_t crate, UInt_t slot );
  UInt_t GetNPED( UInt_t crate, UInt_t slot );
  Double_t GetPED( UInt_t crate, UInt_t slot, UInt_t ch );

 private:

  Int_t fNDecoded; // Counter for decoded type 137 event
  const int NTHR = 16;

  std::vector<UInt_t> fEvtTypes;

  // General parm and key pairs
  struct ConfigParms {
      std::string par;
      std::string key;
  };

  std::vector<ConfigParms> fConfigParmList;
  std::map<std::string, std::string> fConfigData;

  virtual void MakeParms();

  ClassDef(THcEvt137Handler,0)
};

#endif
