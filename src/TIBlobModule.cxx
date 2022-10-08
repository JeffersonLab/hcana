/**
   \class TIBlobModule
   \ingroup Decoders

   \brief Decoder module to pull information out of the TI blob that is read in each ROC.

   The TI blob is identified by a bank with the tag 4.
*/

#include "TIBlobModule.h"
#include "THaSlotData.h"
#include <cassert>
#include <iostream>

using namespace std;

namespace Decoder {

  Module::TypeIter_t TIBlobModule::fgThisType =
    DoRegister( ModuleType( "Decoder::TIBlobModule" , 4 ));

  TIBlobModule::TIBlobModule()
    : TIBlobModule(0,0) {}

  TIBlobModule::TIBlobModule(UInt_t crate, UInt_t slot)
    : PipeliningModule(crate, slot) { fNumChan = NTICHAN; }

  TIBlobModule::~TIBlobModule() = default;

  void TIBlobModule::Init() {
    Module::Init();
    fNumChan=NTICHAN;
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

  UInt_t TIBlobModule::LoadSlot( THaSlotData* sldat, const UInt_t* evbuffer,
                                 const UInt_t* pstop )
  {
    // Load from evbuffer between [evbuffer,pstop]

    return LoadSlot(sldat, evbuffer, 0, pstop + 1 - evbuffer);
  }

  UInt_t TIBlobModule::LoadSlot( THaSlotData *sldat, const UInt_t* evbuffer,
                                 UInt_t pos, UInt_t len)
  {
    // Load from bank data in evbuffer between [pos,pos+len)

    // Fill data structures of this class using the event buffer of one "event".
    // An "event" is defined in the traditional way -- a scattering from a target, etc.

    Clear();

    // Only interpret data if fSlot agrees with the slot in the headers

    const auto* bank = evbuffer + pos;
    if( len > 0 ) {
      // The first word might be a filler word
      UInt_t ifill = (((bank[0] >> 27) & 0x1F) == 0x1F) ? 1 : 0;
      if( len >= 5 + ifill ) {// Need at least two headers and the trailer and 2 data words
        UInt_t header1 = bank[ifill];
        UInt_t slot_blk_hdr = (header1 >> 22) & 0x1F;  // Slot number (set by VME64x backplane), mask 5 bits
        if( fSlot != slot_blk_hdr ) {
          return len;
        }
        assert(fData.size() >= 3 && NTICHAN >= 3);  // else error in constructor
        fData[0] = (bank[2 + ifill] >> 24) & 0xFF;  // Trigger type
        fData[1] = bank[3 + ifill];                 // Trigger number
        //fData[2] = (len > 5 + ifill) ? bank[4 + ifill] : 0; // Trigger time

	// PipeliningModule might supply a too short event length.
	// This happens if the Trigger time looks like a block trailer
	// We have two ways to check that the trigger time is really present
	// 1.  Find the event length from event data word 1
	// 2.  Look for the Timestamp present bit in block header 2

	// Method 1
	//      if((bank[2+ifill]&0xffff) >= 2) {
	//	  fData[2] = bank[4+ifill];
	//	} else {
	//	  fData[2] = 0;
	//	}
	//	cout << "H: " << hex << bank[0+ifill] << " " << 
	//	     bank[1+ifill] << " " << 
	//	     bank[2+ifill] << " " << 
	//	     bank[3+ifill] << " " << 
	//	     bank[4+ifill] << dec << endl;
	// Method 2
	if((bank[1+ifill] & 0x10000) != 0) {
	fData[2] = bank[4+ifill];
	} else {
		  fData[2] = 0;
		}
        for( UInt_t i = 0; i < NTICHAN; i++ ) {
          sldat->loadData(i, fData[i], fData[i]);
          //	cout << " " << fData[i];
        }
        //      cout << endl;
      }
    }

    return len;
  }

  /* Does anything use this method */
UInt_t TIBlobModule::GetData( UInt_t chan ) const {
  if (chan >= fNumChan) return 0;
  return fData[chan];
}

void TIBlobModule::Clear(const Option_t* opt) {
  PipeliningModule::Clear(opt);
  fData.assign(fNumChan,0);
}

}

ClassImp(Decoder::TIBlobModule)
