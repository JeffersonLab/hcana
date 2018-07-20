/**
   \class TIBlobModule
   \ingroup Decoders

   \brief Decoder module to pull information out of the TI blob that is read in each ROC.

   The TI blob is identified by a bank with the tag 4.
*/

#include "TIBlobModule.h"
#include "THaSlotData.h"
#include <iostream>

using namespace std;

namespace Decoder {

  Module::TypeIter_t TIBlobModule::fgThisType =
    DoRegister( ModuleType( "Decoder::TIBlobModule" , 4 ));

  TIBlobModule::TIBlobModule(Int_t crate, Int_t slot) : PipeliningModule(crate, slot) {
    fDebugFile=0;
    Init();
  }

  TIBlobModule::~TIBlobModule() {
  }

  void TIBlobModule::Init() {
    Module::Init();
    fNumChan=NTICHAN;
    for (Int_t i=0; i<fNumChan; i++) fData.push_back(0);
#if defined DEBUG && defined WITH_DEBUG
    // This will make a HUGE output
    delete fDebugFile; fDebugFile = 0;
    fDebugFile = new ofstream;
    fDebugFile->open(string("TIBlob_debug.txt"));
#endif
    //fDebugFile=0;
    Clear();
    IsInit = kTRUE;
    
    fName = "TIBlob";
  }

  Int_t TIBlobModule::LoadSlot(THaSlotData *sldat, const UInt_t* evbuffer, const UInt_t *pstop) {
    // the 3-arg verison of LoadSlot

    std::vector< UInt_t > evb;
    while(evbuffer < pstop) evb.push_back(*evbuffer++);

    // Note, methods SplitBuffer, GetNextBlock are defined in PipeliningModule
    // SplitBuffer needs to be modified a bit for the TI

    SplitBuffer(evb);
    return LoadThisBlock(sldat, GetNextBlock());
  }

  Int_t TIBlobModule::LoadSlot(THaSlotData *sldat, const UInt_t *evbuffer, Int_t pos, Int_t len) {
    // the 4-arg verison of LoadSlot.  Let it call the 3-arg version.
    // Do we need this or is it historical?

    return LoadSlot(sldat, evbuffer+pos, evbuffer+pos+len);
  }

  Int_t TIBlobModule::LoadNextEvBuffer(THaSlotData *sldat) {
    // Note, GetNextBlock belongs to PipeliningModule
    return LoadThisBlock(sldat, GetNextBlock());
  }

  Int_t TIBlobModule::LoadThisBlock(THaSlotData *sldat, std::vector< UInt_t>evbuffer) {

    // Fill data structures of this class using the event buffer of one "event".
    // An "event" is defined in the traditional way -- a scattering from a target, etc.
    
    Clear();

    // Only interpret data if fSlot agrees with the slot in the headers
    
    Int_t evlen = evbuffer.size();
    if(evlen>0) {
      // The first word might be a filler word
      Int_t ifill = (((evbuffer[0]>>27)&0x1F) == 0x1F) ? 1 : 0;
      if (evlen>=5+ifill) {// Need at least two headers and the trailer and 2 data words
	UInt_t header1=evbuffer[ifill];
	Int_t slot_blk_hdr=(header1 >> 22) & 0x1F;  // Slot number (set by VME64x backplane), mask 5 bits
	if(fSlot != slot_blk_hdr) {
	  return evlen;
	}
	fData[0] = (evbuffer[2+ifill]>>24)&0xFF; // Trigger type
	fData[1] = evbuffer[3+ifill];		 // Trigger number
	fData[2] = (evlen>5+ifill) ? evbuffer[4+ifill] : 0; // Trigger time
	//      cout << "TIBlob Slot " << fSlot << ": ";
	for(Int_t i=0;i<3;i++) {
	  sldat->loadData(i, fData[i], fData[i]);
	  //	cout << " " << fData[i];
	}
	//      cout << endl;
      }
    }
      
    return evlen;

  }

  Int_t TIBlobModule::SplitBuffer(std::vector< UInt_t > codabuffer ) {

    // Split a CODA buffer into blocks.   A block is data from a traditional physics event.
    // In MultiBlock Mode, a pipelining module can have several events in each CODA buffer.
    // If block level is 1, then the buffer is a traditional physics event.
    // If finding >1 block, this will set fMultiBlockMode = kTRUE

    // Copied from PipeliningModule and customized for TI

    std::vector<UInt_t > oneEventBuffer;
    eventblock.clear();
    fBlockIsDone = kFALSE;
    //    Int_t eventnum = 1;
    //    Int_t evt_num_modblock;

    //    if ((fFirstTime == kFALSE) && (IsMultiBlockMode() == kFALSE)) {
      eventblock.push_back(codabuffer);
      index_buffer=1;
      return 1;
      //    }
      //    return 0;
    // The rest of this is only use if we are in multiblock mode
    // It still needs to be developed.
#if 0
    int debug=1;

    Int_t slot_blk_hdr, slot_evt_hdr, slot_blk_trl;
    Int_t iblock_num, nblock_events, nwords_inblock, evt_num;
    Int_t BlockStart=0;

    slot_blk_hdr = 0;
    slot_evt_hdr = 0;
    slot_blk_trl = 0;
    nblock_events = 0;

    if(codabuffer.size() > 2) {	// Need at least the two headers and the trailer
      UInt_t header1=codabuffer[0];
      UInt_t header2=codabuffer[1];

      if((header1&0xF8000000)==0x80000000 &&
	 (header1&0xF8000000)==0xF8000000) {	// Header words
	slot_blk_hdr = (header1 >> 22) & 0x1F;  // Slot number (set by VME64x backplane), mask 5 bits
	// iblock_num = (data >> 8) & 0x3FF;
	nblock_events = header1 & 0xFF;
	if(nblock_events > 1) fMultiBlockMode = kTRUE;
	UInt_t i=2;
	while(i+1 < codabuffer.size()) {
	  // This should be event header
	  
      
...
#endif
}


  /* Does anything use this method */
UInt_t TIBlobModule::GetData(Int_t chan) const {
  if (chan < 0 || chan > fNumChan) return 0;
  return fData[chan];
}

void TIBlobModule::Clear(const Option_t* opt) {
  VmeModule::Clear(opt);
  for (Int_t i=0; i<fNumChan; i++) fData[i]=0;
}

}

ClassImp(Decoder::TIBlobModule)
