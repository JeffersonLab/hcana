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

   virtual UInt_t GetHeaderTriggerTime();
   inline virtual UInt_t GetTriggerNum() { return vtp_header_data.trig_num; }

   inline virtual std::vector<UInt_t> GetTriggerTime()  { return vtp_trigger_data.trigtime; }
   inline virtual std::vector<UInt_t> GetTriggerType0() { return vtp_trigger_data.trigtype0; }
   inline virtual std::vector<UInt_t> GetTriggerType1() { return vtp_trigger_data.trigtype1; }
   inline virtual std::vector<UInt_t> GetTriggerType2() { return vtp_trigger_data.trigtype2; }

   inline virtual std::vector<UInt_t> GetClusterEnergy() { return vtp_cluster_data.energy; }
   inline virtual std::vector<UInt_t> GetClusterTime()   { return vtp_cluster_data.time; }
   inline virtual std::vector<UInt_t> GetClusterSize()   { return vtp_cluster_data.nblocks; }
   inline virtual std::vector<UInt_t> GetClusterX()      { return vtp_cluster_data.xcoord; }
   inline virtual std::vector<UInt_t> GetClusterY()      { return vtp_cluster_data.ycoord; }

 private:

   struct vtp_header_data_struct {
     uint32_t slot_blk_hdr, iblock_num, nblock_events;             // Block header objects
     uint32_t slot_blk_trl, nwords_inblock;                        // Block trailer objects
     uint32_t slot_evt_hdr, evt_num, trig_num;                     // Event header objects 
     uint32_t trig_time_w1, trig_time_w2;                          // Trigger time objects
     uint64_t trig_time;                                           // Trigger time objects cont.
     uint32_t evnt_of_blk;                                         // VTP event block information
     void clear() { memset(this, 0, sizeof(vtp_header_data_struct)); }
   } __attribute__((aligned(128))) vtp_header_data;  
   
   struct vtp_trigger_data_struct {
     std::vector<uint32_t> trigtime, trigtype0, trigtype1, trigtype2;   
     void clear() {
       trigtime.clear(); trigtype0.clear(); trigtype1.clear(); trigtype2.clear(); 
     }
   } __attribute__((aligned(128))) vtp_trigger_data;
   
   struct vtp_cluster_data_struct {
     std::vector<uint32_t> energy, time, nblocks, xcoord, ycoord;
     void clear() {
       energy.clear(); time.clear(); nblocks.clear(); xcoord.clear(); ycoord.clear();
     }
   } __attribute__((aligned(128))) vtp_cluster_data;
   
   Bool_t block_header_found, block_trailer_found, event_header_found, slots_match;

   void ClearDataVectors();
   void PopulateDataVector( std::vector<uint32_t>& data_vector, uint32_t data ) const;
   void LoadTHaSlotDataObj( THaSlotData* sldat );
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
