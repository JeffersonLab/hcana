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
   TIBlobModule(Int_t crate, Int_t slot);
   virtual ~TIBlobModule();

   using Module::GetData;
   using Module::LoadSlot;

   virtual UInt_t GetData(Int_t chan) const;
   virtual void Init();
   virtual void Clear(const Option_t *opt="");
   virtual Int_t Decode(const UInt_t*) { return 0; }
   virtual Int_t LoadSlot(THaSlotData *sldat,  const UInt_t *evbuffer, const UInt_t *pstop );
   virtual Int_t LoadSlot(THaSlotData *sldat, const UInt_t* evbuffer, Int_t pos, Int_t len);
   Int_t LoadNextEvBuffer(THaSlotData *sldat);
   Int_t LoadThisBlock(THaSlotData *sldat, std::vector<UInt_t > evb);
 
 protected:
   Int_t SplitBuffer(std::vector< UInt_t > bigbuffer);

 private:

   static const size_t NTICHAN = 3;

   static TypeIter_t fgThisType;
   ClassDef(TIBlobModule,0)  //  TIBlob of a module;

};

}

#endif

