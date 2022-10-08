#ifndef Podd_THcHelicityReader_h_
#define Podd_THcHelicityReader_h_

//////////////////////////////////////////////////////////////////////////
//
// THcHelicityReader
//
// Routines for decoding QWEAK helicity hardware
//
//////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "Decoder.h"
#include "Fadc250Module.h"

class THaEvData;
class TDatime;
class TH1F;

class THcHelicityReader {
  
public:
  THcHelicityReader();
  virtual ~THcHelicityReader();
  
  struct ROCinfo {
    Int_t  roc;               // ROC to read out
    Int_t  slot;            // Headers to search for (0 = ignore)
    Int_t  index;             // Index into buffer
  };
  
protected:

  // Used by ReadDatabase
  enum EROC { kHel = 0, kHelm, kMPS, kQrt, kTime, kCount };
  Int_t SetROCinfo( EROC which, Int_t roc, Int_t slot, Int_t index );

  virtual void  Clear( Option_t* opt="" );
  virtual Int_t ReadData( const THaEvData& evdata );
  Int_t         ReadDatabase( const char* dbfilename, const char* prefix,
			      const TDatime& date, int debug_flag = 0 );
  void Begin();
  void End();

  ULong64_t fTITime;
  UInt_t fTITime_last;
  UInt_t fTITime_rollovers;
  
  // Reported Helicity status for the event
  Bool_t fIsMPS;
  Bool_t fIsQrt;
  Bool_t fIsHelp;
  Bool_t fIsHelm;

  Int_t fADCThreshold;		// Threshold for On/Off of helicity signals
  Int_t fADCRawSamples;		// Use raw sample data if true

  Decoder::Fadc250Module* fFADCModule; // Module object to get trigger time
  Int_t fTTimeDiff;		       // Difference between TI and ADC times
  
  ROCinfo  fROCinfo[kCount];

  Int_t    fQWEAKDebug;          // Debug level
  Bool_t   fHaveROCs;         // Required ROCs are defined
  Bool_t   fNegGate;          // Invert polarity of gate, TO DO implement this functionality
  static const Int_t NHISTR = 12;
  //  TH1F*    fHistoR[12];  // Histograms

private:

  ClassDef(THcHelicityReader,0) // Helper class for reading QWEAK helicity data

};

#endif

