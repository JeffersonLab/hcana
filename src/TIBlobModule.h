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

namespace Decoder {

class TIBlobModule : public PipeliningModule {

public:

   TIBlobModule() : PipeliningModule() {}
   TIBlobModule(UInt_t crate, UInt_t slot);
   virtual ~TIBlobModule();

   using Module::GetData;

   virtual UInt_t GetData(UInt_t chan) const;
   virtual void Init();
   virtual void Clear(const Option_t *opt="");
   virtual Int_t Decode(const UInt_t*) { return 0; }
   virtual UInt_t LoadSlot( THaSlotData *sldat, const UInt_t *evbuffer, const UInt_t *pstop );
   virtual UInt_t LoadSlot( THaSlotData *sldat, const UInt_t* evbuffer, UInt_t pos, UInt_t len);
   virtual UInt_t LoadNextEvBuffer(THaSlotData *sldat);
   virtual UInt_t LoadThisBlock( THaSlotData *sldat, const std::vector<UInt_t>& evbuffer);
 
 protected:
   virtual Int_t SplitBuffer( const std::vector<UInt_t>& bigbuffer);

 private:

   static const size_t NTICHAN = 3;

   static TypeIter_t fgThisType;
   ClassDef(TIBlobModule,0)  //  TIBlob of a module;

};

}

#endif

