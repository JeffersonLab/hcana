#ifndef TIBlobModule_
#define TIBlobModule_


/////////////////////////////////////////////////////////////////////
//
//   TIBlobModule
//   TI Trigger information
//     This version just pulls out
//       Trigger type
//       Event number (just bottum 32 bits)
//       4ns clock
//
/////////////////////////////////////////////////////////////////////

#include "PipeliningModule.h"
#include <string>

namespace Decoder {

class TIBlobModule : public PipeliningModule {

public:

   TIBlobModule();
   TIBlobModule(UInt_t crate, UInt_t slot);
   virtual ~TIBlobModule();

   using Module::GetData;

   virtual UInt_t GetData(UInt_t chan) const;
   virtual void Init();
   virtual void Init( const char* configstr );
   virtual void Clear(const Option_t *opt="");
   virtual Int_t Decode(const UInt_t*) { return 0; }
   virtual UInt_t LoadSlot( THaSlotData *sldat, const UInt_t *evbuffer, const UInt_t *pstop );
   virtual UInt_t LoadSlot( THaSlotData *sldat, const UInt_t* evbuffer, UInt_t pos, UInt_t len);
   virtual UInt_t LoadBank( THaSlotData* sldat, const UInt_t* evbuffer, UInt_t pos, UInt_t len );

   // Index into fData[]. Usage: GetData(Decoder::TIBlobModule::kTimestamp)
   enum EInfoType {
     kTriggerType = 0, kTriggerNum, kTimestamp, kCounterBits, kTriggerBits,
     kNTICHAN       /* indicates number of channels */
   };

   bool HasTriggerType() const { return TESTBIT(fDataAvail, kTriggerType); }
   bool HasTriggerNum()  const { return TESTBIT(fDataAvail, kTriggerNum); }
   bool HasTimestamp()   const { return TESTBIT(fDataAvail, kTimestamp); }
   bool HasCounterBits() const { return TESTBIT(fDataAvail, kCounterBits); }
   bool HasTriggerBits() const { return TESTBIT(fDataAvail, kTriggerBits); }

   // Set type of 4th data word (3rd word if there is no timestamp).
   // This only applies if the TI is configured to generate either the counter
   // bits or the trigger bit pattern word, but not both. This case cannot
   // be unambiguously decoded automatically.
   // Values can be 0 = automatic, kCounterBits, or kTriggerBits.
   void SetWord4Type( UInt_t type );

protected:
  virtual UInt_t LoadNextEvBuffer( THaSlotData *sldat );

private:
   UInt_t fNfill;        // Number of filler words at end of current bank
   UInt_t fDataAvail;    // Bitfield of data read (see EInfoType)
   UInt_t fWord4Type;    // Which data contained in event data word4
   Bool_t fHasTimestamp; // Event data contain timestamp (default)

   std::string Here( const char* function );
   void CheckWarnTrigBits( UInt_t word, UInt_t nopt, const char* here );

   static TypeIter_t fgThisType;
   ClassDef(TIBlobModule,0)        // Trigger Interface module
};

}

#endif
