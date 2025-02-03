#ifndef ROOT_THcEvt137Handler
#define ROOT_THcEvt137Handler

#include "THaEvtTypeHandler.h"
#include <vector>
#include <string>
#include <map>
#include "TTree.h"

class THcEvt137Handler : public THaEvtTypeHandler {
 public:

  THcEvt137Handler( const char* name, const char* description = "" );
  virtual ~THcEvt137Handler();
  
  virtual Int_t   Analyze( THaEvData *evdata );
  virtual EStatus Init( const TDatime& date );
  virtual Int_t   End( THaRunBase* r=nullptr );
  virtual void    AddEvtType( UInt_t evtype );

  std::vector<UInt_t> GetEvtTypes() { return fEvtTypes; }
  std::string GetInfo( const char* keyname );

  void MakeConfigTree(bool make_tree) { fMakeConfigTree = make_tree; } // default is true
  void MakeParms(bool make_parms ) { fMakeParms = make_parms; } // default is true

  // Some useful getter functions for FADC250
  int GetNSA( UInt_t crate, UInt_t slot );
  int GetNSB( UInt_t crate, UInt_t slot );
  int GetNPED( UInt_t crate, UInt_t slot );
  Double_t GetPED( UInt_t crate, UInt_t slot, UInt_t ch );

 private:

  Int_t fNDecoded; // Counter for decoded type 137 event

  TTree* fConfigTree; // Output tree for config information
  bool fMakeConfigTree; // default is true
  bool fMakeParms;
  UInt_t fCounter; // rough counter of the number of parameters to save

  struct ConfigData {
    int roc;
    int slot;
    std::string parname;
    std::string pars_str; // data in a single string format
    std::vector<Double_t> pars;
  };

  // with a uinque keyname
  std::map<std::string, ConfigData> fConfigDataMap;

  std::vector<UInt_t> fRoc; // List of rocs with config info
  std::vector<UInt_t> fEvtTypes;

  void SaveConfigData();
  void MakeParms();

  ClassDef(THcEvt137Handler,0)
};

#endif
