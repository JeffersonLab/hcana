#ifndef VTPModule_
#define VTPModule_


/////////////////////////////////////////////////////////////////////
//
//   VTPModule
//
/////////////////////////////////////////////////////////////////////

#include "PipeliningModule.h"
#include <string>
#include <cstdint>
#include <cstring>  // for memset

namespace Decoder {

class VTPModule : public PipeliningModule {

public:

   VTPModule() : VTPModule(0,0) {}
   VTPModule(UInt_t crate, UInt_t slot);
   virtual ~VTPModule();

   using PipeliningModule::GetData;
   using PipeliningModule::Init;

   virtual void Clear( Option_t *opt="" );
   virtual void Init();
   virtual UInt_t LoadSlot( THaSlotData* sldat, const UInt_t* evbuffer, const UInt_t* pstop );
   virtual UInt_t LoadSlot( THaSlotData* sldat, const UInt_t* evbuffer, UInt_t pos, UInt_t len );
   virtual Int_t  Decode( const UInt_t* data );
   virtual UInt_t GetData( Decoder::EModuleType mtype, UInt_t chan, UInt_t ievent ) const;
   virtual UInt_t GetNumEvents( Decoder::EModuleType mtype, UInt_t ichan ) const;
   virtual UInt_t GetTriggerTime() const;
   
   
 private:
   
   struct vtp_data_struct {
     // Header data objects
     uint32_t slot_blk_hdr, iblock_num, nblock_events;             // Block header objects
     uint32_t slot_blk_trl, nwords_inblock;                        // Block trailer objects
     uint32_t slot_evt_hdr, evt_num, trig_num;                     // Event header objects 
     uint32_t trig_time_w1, trig_time_w2;                          // Trigger time objects
     uint64_t trig_time;                                           // Trigger time objects cont.
     uint32_t evnt_of_blk;                                         // VTP event block information
     uint32_t nps_e, nps_t, nps_n, nps_x, nps_y;                   // NPS cluster information
     uint32_t nps_trig0, nps_trig1, nps_trig2, nps_trigtime;       // NPS trigger information
     void clear() { memset(this, 0, sizeof(vtp_data_struct)); }
   } __attribute__((aligned(128))) vtp_data;  // vtp_data_struct
   
   Bool_t block_header_found, block_trailer_found, event_header_found, slots_match;

   //void LoadTHaSlotDataObj( THaSlotData* sldat );
   void PrintDataType() const;
   
   void DecodeBlockHeader( UInt_t pdat, uint32_t data_type_id );
   void DecodeBlockTrailer( UInt_t pdat );
   void DecodeEventHeader( UInt_t pdat );
   void DecodeTriggerTime( UInt_t pdat, uint32_t data_type_id );
   void UnsupportedType( UInt_t pdat, uint32_t data_type_id );
   void DecodeNPSCluster( UInt_t pdat, uint32_t data_type_id );
   void DecodeTriggerDecision( UInt_t pdat, uint32_t data_type_id );

   static TypeIter_t fgThisType;
   
   ClassDef(VTPModule,0)        // NPS VTP module
     
  };

}

#endif
