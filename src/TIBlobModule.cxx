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
#include <sstream>
#include <cstring>   // memcpy

using namespace std;

namespace Decoder {

Module::TypeIter_t TIBlobModule::fgThisType =
        DoRegister( ModuleType( "Decoder::TIBlobModule" , 4 ));

//_____________________________________________________________________________
TIBlobModule::TIBlobModule()
  : TIBlobModule(0,0) {}

//_____________________________________________________________________________
TIBlobModule::TIBlobModule(UInt_t crate, UInt_t slot)
  : PipeliningModule(crate, slot)
  , fNfill{0}
  , fHasTimestamp{true}
{}

//_____________________________________________________________________________
TIBlobModule::~TIBlobModule() = default;

//_____________________________________________________________________________
void TIBlobModule::Init()
{
  Module::Init();
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

//_____________________________________________________________________________
UInt_t TIBlobModule::LoadSlot( THaSlotData* sldat, const UInt_t* evbuffer,
                                 const UInt_t* pstop )
{
    // Load from evbuffer between [evbuffer,pstop]

  return LoadSlot(sldat, evbuffer, 0, pstop + 1 - evbuffer);
}

//_____________________________________________________________________________
string TIBlobModule::Here( const char* function )
{
  ostringstream os;
  os << "TIBlobModule::" << function << ": slot/crate "
     << fSlot << "/" << fCrate << ": ";
  return os.str();
}

//_____________________________________________________________________________
UInt_t TIBlobModule::LoadSlot( THaSlotData* sldat, const UInt_t* evbuffer,
                               UInt_t pos, UInt_t len ) {
  // Load from bank data in evbuffer between [pos,pos+len)

  // evbuffer+pos points to the event header, evbuffer+pos+len is one past
  // the last word of the event, which is either the block trailer in single
  // block mode or for the last event in multi-block mode, or the next event
  // header in multi-block mode.

  const char* const here = "LoadSlot";

  // Clear event data and ensure there is enough space including optional data.
  // Don't call Clear() here; it would wipe out the bank-level info.
  fNumChan = 0;
  fData.assign(NTICHAN,0);

  if( len < 2 )
    return 0;

  // Event data:
  //  event[0]:  header: trigger type, event length
  //  event[1]:  trigger number lower 32 bits
  //  event[2]:  trigger time lower 32 bits (optional)
  //  event[3]:  maybe: tigger number/time upper 16 bits (optional, firmware-dependent)
  const auto* event = evbuffer + pos;
  UInt_t nwords = (event[0] & 0xFFFF) + 1;   // Number of words in this event
  if( nwords > len || nwords > NTICHAN ) {
    cerr << Here(here) << "Reported event length too long: hdr = "
         << nwords << ", buf = " << len << ", need <= " << NTICHAN
         << " and <= " << len << endl;
    return 0;
  }
  if( nwords < 2 ) {
    cerr << Here(here) << "Reported event length too short. Got "
         << nwords << ", need >= 2" << endl;
    return 0;
  }
  if( nwords == 2 && fHasTimestamp ) {
    cerr << Here(here) << "Inconsistent event length = " << nwords
         << " but timestamp present requires >= 3" << endl;
    return 0;
  }
  fData[0] = (event[0] >> 24) & 0xFF;  // Trigger type
  memcpy( fData.data()+1, event+1, (nwords-1)*sizeof(*event));
  for( UInt_t i = 0; i < nwords; i++ ) {
    sldat->loadData(i, fData[i], fData[i]);
    //	cout << " " << fData[i];
  }
  //      cout << endl;

  // Report actual number of valid data words, which is variable for this module
  // although it should not change from event to event
  fNumChan = nwords;
  return nwords;
}

//_____________________________________________________________________________
static inline
Long64_t FindBlockHeaderForSlot( const UInt_t* buf, UInt_t start, UInt_t len,
                                 UInt_t slot ) {
  // Search 'buf' for data type identifier word encoding 'type', 'slot',
  // and 'id'. The format is (T=type, S=slot, D=ID)
  //    1TTT TSSS SSDD DDXX XXXX XXXX XXXX XXXX
  // bit  28   24   20   16   12    8    4    0
  //
  // The buffer is searched between [start,start+len)
  // Returns the offset into 'buf' containing the word, or -1 if not found.

  const uint32_t ID = BIT(31) | (slot & 0x1F) << 22;
  const auto* p = buf + start;
  const auto* q = p + len;
  while( p != q && (*p & 0xFFFC0000) != ID )
    ++p;
  return (p != q) ? p - buf : -1;
}

//_____________________________________________________________________________
UInt_t TIBlobModule::LoadBank( Decoder::THaSlotData* sldat,
                               const UInt_t* evbuffer,
                               UInt_t pos, UInt_t len )
{
  // Load event block. If multi-block data, split the buffer into individual
  // events. This routine is called for buffers with bank structure.

  const char* const here = "LoadBank";

  // Note: Clear() has been called at this point

  if( len < 5 ) {
    cerr << here << "Bank too short. Expected >= 5, got " << len << " words."
         << endl;
    // Caller has no provision for error codes and doesn't handle exceptions.
    // Hopeless.
    return 0;
  }
  // Find block header for this module's slot
  // There should never be more than one TI module per bank per ROC, so this
  // really ought to match the very first word.
  auto ibeg = FindBlockHeaderForSlot(evbuffer, pos, len,
                                     fSlot);
  if( ibeg == -1 ) {
    // Block header not present. Should not happen.
    cerr << Here(here) << "No block header found in bank for slot " << fSlot
         << "Call expert." << endl;
    return 0;
  }
  if( ibeg != pos ) {
    cerr << Here(here) << "Warning: " << ibeg-pos << " words of leading garbage"
         << endl;
  }
  const UInt_t* p = evbuffer+ibeg;
  // Block header lower 18 bits
  fBlockHeader  = *p;
  block_size    = (*p & 0x000000FF);      // Number of events in block
  //fBlockNum     = (*p & 0x0003FF00) >> 8; // Block number

  // Block header #2
  UInt_t hdr2id = *++p & 0xFFFEFF00;
  if( hdr2id != 0xFF102000 ) {
    cerr << Here(here) << "Invalid block header #2 signature = "
         << hex << *p << dec << ", expected 0xFF1020xx or 0xFF1120xx" << endl;
    return 0;
  }
  fHasTimestamp = TESTBIT(*p, 16);
  UInt_t block_size2 = (*p & 0x000000FF);
  if( block_size != block_size2 ) {
    cerr << Here(here) << "Warning: block sizes in block headers #1 and #2 "
         << "disagree: " << block_size << ", " << block_size2 << endl;
  }

  // Find all the event headers (there should be block_size of them)
  // between here and the block trailer, save their positions,
  // then proceed with decoding the first block
  evtblk.reserve(block_size + 1);

  UInt_t blksz = block_size;
  ++p;
  while( blksz > 0 && p-evbuffer < pos+len) {
    UInt_t sig = (*p & 0x00FF0000) >> 16;
    if( sig != 0x01 ) {
      cerr << Here(here) << "Invalid event header signature for iblock "
           << block_size-blksz << ". Expected xx01xxxx, got "
           << hex << *p << dec << endl;
      return 0;
    }
    UInt_t nwords = *p & 0xFFFF;   // Number of 32-bit words following
    if( nwords < 1 ) {
      cerr << Here(here) << "Event data too short, expected >= 2, got "
           << nwords+1 << endl;
      return 0;
    }
    evtblk.push_back(p - evbuffer);
    p += nwords+1;
    --blksz;
  }

  if( evtblk.size() != block_size ) {
    cerr << Here(here) << "Incorrect number of events found, expected "
         << block_size << ", found " << evtblk.size() << endl;
    return 0;
  }
  // evtblk must have exactly block_size elements now
  evtblk.push_back(p-evbuffer); // include block trailer

  // Verify block trailer
  if( (*p & 0xFFC00000) != (BIT(31) | (1U << 27) | (fSlot & 0x1F) << 22) ) {
    cerr << Here(here) << "Warning: Missing block trailer" << endl;
  }
  UInt_t nwords_inblock = *p & 0x3FFFFF;
  Long64_t iend = p-evbuffer;   // Position of block trailer
  if( nwords_inblock != iend+1-ibeg ) {
    cerr << Here(here) << "Warning: block trailer word count mismatch, "
         << "got " << nwords_inblock << ", expected " << iend+1-ibeg << endl;
  }
  assert( ibeg >= pos && iend > ibeg && iend < pos+len ); // trivially

  // Consume filler words
  while( ++p-evbuffer < pos+len
         && (*p & 0xF7C00000) == (0xF0000000 | (fSlot & 0x1F) << 22) ) {
    ++fNfill;
  }
  if( p-evbuffer != pos+len ) {
    cerr << Here(here) << evbuffer+pos+len-p
         << " trailing garbage words" << endl;
  }

  // Multi-block event?
  fMultiBlockMode = ( block_size > 1 );

  if( fMultiBlockMode ) {
    // Multi-block: decode first event, using event positions from evtblk[]
    index_buffer = 0;
    return LoadNextEvBuffer(sldat)
           + 2;  // 2 block header words

  } else {
    // Single block: decode, starting at event header
    ibeg += 2;
    return LoadSlot(sldat, evbuffer, ibeg, iend-ibeg)
           + 3 // 2 block header words + 1 block trailer word
           + fNfill;
  }
  // not reached
}

//_____________________________________________________________________________
UInt_t TIBlobModule::LoadNextEvBuffer( Decoder::THaSlotData* sldat )
{
  // In multi-block mode, load the next event from the current block

  UInt_t ii = index_buffer;
  assert( ii+1 < evtblk.size() );

  // ibeg = event header, iend = one past last word of this event ( = next
  // event header if more events pending)
  auto ibeg = evtblk[ii], iend = evtblk[ii+1];
  assert(ibeg > 0 && iend > ibeg && static_cast<size_t>(iend) <= fBuffer.size());

  // Load slot starting with event header at ibeg
  ii = LoadSlot(sldat, fBuffer.data(), ibeg, iend-ibeg);

  // Next cached buffer. Set flag if we've exhausted the cache.
  ++index_buffer;
  if( index_buffer+1 >= evtblk.size() )
    fBlockIsDone = true;

  if( fBlockIsDone )
    ii += 1 + fNfill; // block trailer + filler words
  return ii;
}

//_____________________________________________________________________________
  /* Does anything use this method */
UInt_t TIBlobModule::GetData( UInt_t chan ) const
{
  if (chan >= fNumChan) return 0;
  return fData[chan];
}

//_____________________________________________________________________________
// Clear module's data in preparation for new event/new bank
void TIBlobModule::Clear(const Option_t* opt)
{
  PipeliningModule::Clear(opt);
  fNumChan = 0;  // For this module: number of data words in current event
  fData.assign(NTICHAN, 0);
  fNfill = 0;
  fHasTimestamp = true;
}

} // namespace Decoder

ClassImp(Decoder::TIBlobModule)
