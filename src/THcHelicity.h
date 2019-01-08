#ifndef Podd_THcHelicity_h_
#define Podd_THcHelicity_h_

////////////////////////////////////////////////////////////////////////
//
// THcHelicity
//
// Helicity of the beam - from QWEAK electronics in delayed mode
// 
////////////////////////////////////////////////////////////////////////

#include "THaHelicityDet.h"
#include "THcHelicityReader.h"

class TH1F;

class THcHelicity : public THaHelicityDet, public THcHelicityReader {

public:

  THcHelicity( const char* name, const char* description, 
		 THaApparatus* a = NULL );
  THcHelicity();
  virtual ~THcHelicity();

  virtual EStatus Init(const TDatime& date);
  virtual void        MakePrefix();

  virtual Int_t  Begin( THaRunBase* r=0 );
  virtual void   Clear( Option_t* opt = "" );
  virtual Int_t  Decode( const THaEvData& evdata );
  virtual Int_t  End( THaRunBase* r=0 );
  virtual void   SetDebug( Int_t level );
  virtual Bool_t HelicityValid() const { return fValidHel; }

  void PrintEvent(Int_t evtnum);

protected:
  void Setup(const char* name, const char* description);
  std::string fKwPrefix;
  
  void  FillHisto();
  void  LoadHelicity(Int_t reportedhelicity, Int_t cyclecount, Int_t missedcycles);
  Int_t RanBit30(Int_t ranseed);
  Int_t GetSeed30(Int_t currentseed);

  // Fixed Parameters
  Int_t fRingSeed_reported_initial;
  Int_t fFirstCycle;
  Double_t fFreq;

  Double_t fTIPeriod;		// Reversal period in TI time units

  Bool_t fFirstEvProcessed;
  Int_t fLastReportedHelicity;
  Long64_t fFirstEvTime;
  Long64_t fLastEvTime;
  Long64_t fLastMPSTime;
  Int_t fReportedHelicity;
  Int_t fMPS;
  Int_t fPredictedHelicity;
  Int_t fActualHelicity;
  Int_t fQuartetStartHelicity;
  Int_t fQuartetStartPredictedHelicity;
  Bool_t fFoundMPS;
  Bool_t fFoundQuartet;		// True if quartet phase probably found.
  Bool_t fIsNewCycle;
  Int_t fNCycle;		// Count of # of helicity cycles
  Int_t fQuartet[4];		// For finding and checking quartet pattern
  Int_t fNBits;
  Int_t fnQrt;			// Position in quartet

  Int_t fRingSeed_reported;
  Int_t fRingSeed_actual;

  
  // Offset between the ring reported value and the reported value
  Int_t fHelDelay;  
  // delay of helicity (# windows)
  Int_t fMAXBIT; 
  //number of bit in the pseudo random helcity generator
  std::vector<Int_t> fPatternSequence; // sequence of 0 and 1 in the pattern
  Int_t fQWEAKNPattern; // maximum of event in the pattern
  Bool_t HWPIN;


  Int_t fQrt;
  Int_t fTSettle;
  Bool_t fValidHel;

  Int_t fHelicityLastTIR;
  Int_t fPatternLastTIR;
  void SetErrorCode(Int_t error);
  Double_t fErrorCode;
 
  Int_t fEvtype; // Current CODA event type
 
  static const Int_t NHIST = 2;
  TH1F* fHisto[NHIST];  

  virtual Int_t DefineVariables( EMode mode = kDefine );
  virtual Int_t ReadDatabase( const TDatime& date );

  ClassDef(THcHelicity,0)   // Beam helicity from QWEAK electronics in delayed mode

};

#endif

