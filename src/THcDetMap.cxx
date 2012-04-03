//*-- Author :    Ole Hansen   16/05/00

//////////////////////////////////////////////////////////////////////////
//
// THaDetMap
//
// The standard detector map for a Hall A detector.
//
//////////////////////////////////////////////////////////////////////////

#include "THaDetMap.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdlib>

using namespace std;

const int THaDetMap::kDetMapSize;

// FIXME: load from db_cratemap
struct ModuleType {
  UInt_t  model;       // model identifier
  Bool_t  adc;         // true if ADC
  Bool_t  tdc;         // true if TDC
};

static const ModuleType module_list[] = {
  { 1875, 0, 1 },
  { 1877, 0, 1 },
  { 1881, 1, 0 },
  { 1872, 0, 1 },
  { 3123, 1, 0 },
  { 1182, 1, 0 },
  {  792, 1, 0 },
  {  775, 0, 1 },
  {  767, 0, 1 },
  { 3201, 0, 1 },
  { 6401, 0, 1 },
  { 1190, 0, 1 },
  { 0 }
};

//_____________________________________________________________________________
void THaDetMap::Module::SetModel( UInt_t m )
{
  model = m & kModelMask;
  const ModuleType* md = module_list;
  while ( md->model && model != md->model ) md++;
  if( md->adc ) MakeADC();
  if( md->tdc ) MakeTDC();
}

//_____________________________________________________________________________
void THaDetMap::Module::SetResolution( Double_t res )
{
  resolution = res;
}

//_____________________________________________________________________________
THaDetMap::THaDetMap() : fNmodules(0), fMap(0), fMaplength(0)
{
  // Default constructor. Creates an empty detector map.
}

//_____________________________________________________________________________
THaDetMap::THaDetMap( const THaDetMap& rhs )
{
  // Copy constructor

  fMaplength = rhs.fMaplength;
  fNmodules  = rhs.fNmodules;
  if( fMaplength > 0 ) {
    fMap = new Module[fMaplength];
    memcpy(fMap,rhs.fMap,fNmodules*sizeof(Module));
  }
}

//_____________________________________________________________________________
THaDetMap& THaDetMap::operator=( const THaDetMap& rhs )
{
  // THaDetMap assignment operator

  if ( this != &rhs ) {
    if ( fMaplength != rhs.fMaplength ) {
      delete [] fMap;
      fMaplength = rhs.fMaplength;
      if( fMaplength > 0 )
	fMap = new Module[fMaplength];
    }
    fNmodules = rhs.fNmodules;
    memcpy(fMap,rhs.fMap,fNmodules*sizeof(Module));
  }
  return *this;
}

//_____________________________________________________________________________
THaDetMap::~THaDetMap()
{
  // Destructor

  delete [] fMap;
}

//_____________________________________________________________________________
Int_t THaDetMap::AddModule( UShort_t crate, UShort_t slot, 
			    UShort_t chan_lo, UShort_t chan_hi,
			    UInt_t first, UInt_t model, Int_t refindex,
			    Int_t refchan )
{
  // Add a module to the map.

  if( fNmodules >= kDetMapSize ) return -1;  //Map is full
  // Logical channels can run either "with" or "against" the physical channel
  // numbers:
  // lo<=hi :    lo -> first
  //             hi -> first+hi-lo
  // 
  // lo>hi  :    hi -> first
  //             lo -> first+lo-hi
  // 
  // To indicate the second case, The flag "reverse" is set to true in the 
  // module. The fields lo and hi are reversed so that lo<=hi always.
  // 
  bool reverse = ( chan_lo > chan_hi );

  if ( fNmodules >= fMaplength ) { // need to expand the Map
    Int_t oldlen = fMaplength;
    fMaplength += 10;
    Module* tmpmap = new Module[fMaplength];   // expand in groups of 10
    if( oldlen > 0 ) {
      memcpy(tmpmap,fMap,oldlen*sizeof(Module));
      delete [] fMap;
    }
    fMap = tmpmap;
  }

  Module& m = fMap[fNmodules];
  m.crate = crate;
  m.slot  = slot;
  if( reverse ) {
    m.lo = chan_hi;
    m.hi = chan_lo;
  } else {
    m.lo = chan_lo;
    m.hi = chan_hi;
  }
  m.first = first;
  m.SetModel( model );
  m.refindex = refindex;
  m.refchan  = refchan;
  m.SetResolution( 0.0 );
  m.reverse = reverse;

  return ++fNmodules;
}

//_____________________________________________________________________________
THaDetMap::Module* THaDetMap::Find( UShort_t crate, UShort_t slot,
				    UShort_t chan )
{
  // Return the module containing crate, slot, and channel chan.
  // If several matching modules exist (which mean the map is misconfigured),
  // only the first one is returned. If none match, return NULL.
  // Since the map is usually small and not necessarily sorted, a simple
  // linear search is done.

  Module* found = NULL;
  for( UShort_t i = 0; i < fNmodules; ++i ) {
    Module* d = uGetModule(i);
    if( d->crate == crate && d->slot == slot && 
	d->lo <= chan && chan <= d->hi ) {
      found = d;
      break;
    }
  }
  return found;
}

//_____________________________________________________________________________
Int_t THaDetMap::Fill( const vector<Int_t>& values, UInt_t flags )
{
  // Fill the map with 'values'. Depending on 'flags', the values vector
  // is interpreted as a 4-, 5-, 6- or 7-tuple:
  //
  // The first 4 values are interpreted as (crate,slot,start_chan,end_chan)
  // Each of the following flags causes one more value to be used as part of
  // the tuple for each module:
  // 
  // kFillLogicalChannel - Logical channel number for 'start_chan'.
  // kFillModel          - The module's hardware model number (see AddModule())
  // kFillRefChan        - Reference channel (for pipeline TDCs etc.)
  // kFillRefIndex       - Reference index (for pipeline TDCs etc.)
  //
  // If more than one flag is present, the numbers will be interpreted
  // in the order the flags are listed above. 
  // Example:
  //      flags = kFillModel | kFillRefChan
  // ==>
  //      the vector is interpreted as a series of 6-tuples in the order
  //      (crate,slot,start_chan,end_chan,model,refchan).
  //
  // If kFillLogicalChannel is not set then the first logical channel numbers
  // are automatically calculated for each module, assuming the numbers are 
  // sequential.
  // 
  // By default, an existing map is overwritten. If the flag kDoNotClear 
  // is present, then the data are appended.
  //
  // The return value is the number of modules successfully added,
  // or negative if an error occurred.

  typedef vector<Int_t>::size_type vsiz_t;

  if( (flags & kDoNotClear) == 0 )
    Clear();

  vsiz_t tuple_size = 4;
  if( flags & kFillLogicalChannel )
    tuple_size++;
  if( flags & kFillModel )
    tuple_size++;
  if( flags & kFillRefChan )
    tuple_size++;
  if( flags & kFillRefIndex )
    tuple_size++;

  UInt_t prev_first = 0, prev_nchan = 0;
  // Defaults for optional values
  UInt_t first = 0, model = 0;
  Int_t rchan = -1, ref = -1;

  Int_t ret = 0;
  for( vsiz_t i = 0; i < values.size(); i += tuple_size ) {
    // For compatibility with older maps, crate < 0 means end of data
    if( values[i] < 0 )
      break;
    // Now we require a full tuple
    if( i+tuple_size > values.size() ) {
      ret = -127;
      break;
    }

    vsiz_t k = 4;
    if( flags & kFillLogicalChannel )
      first = values[i+k++];
    else {
      first = prev_first + prev_nchan;
    }
    if( flags & kFillModel )
      model = values[i+k++];
    if( flags & kFillRefChan )
      rchan = values[i+k++];
    if( flags & kFillRefIndex )
      ref   = values[i+k++];

    ret = AddModule( values[i], values[i+1], values[i+2], values[i+3],
		     first, model, ref, rchan );
    if( ret<=0 )
      break;
    prev_first = first;
    prev_nchan = GetNchan( ret-1 );
  }
  
  return ret;
}

//_____________________________________________________________________________
Int_t THaDetMap::GetTotNumChan() const
{
  // Get sum of the number of channels of all modules in the map. This is 
  // typically the total number of hardware channels used by the detector.

  Int_t sum = 0;
  for( UShort_t i = 0; i < fNmodules; i++ )
    sum += GetNchan(i);
    
  return sum;
}


//_____________________________________________________________________________
void THaDetMap::GetMinMaxChan( Int_t& min, Int_t& max, ECountMode mode ) const
{
  // Put the minimum and maximum logical or reference channel numbers 
  // into min and max. If refidx is true, check refindex, else check logical
  // channel numbers.

  min = kMaxInt;
  max = kMinInt;
  bool do_ref = ( mode == kRefIndex );
  for( UShort_t i = 0; i < fNmodules; i++ ) {
    Module* m = GetModule(i);
    Int_t m_min = do_ref ? m->refindex : m->first;
    Int_t m_max = do_ref ? m->refindex : m->first + m->hi - m->lo;
    if( m_min < min )
      min = m_min;
    if( m_max > max )
      max = m_max;
  }
}


//_____________________________________________________________________________
void THaDetMap::Print( Option_t* ) const
{
  // Print the contents of the map

  cout << "Size: " << fNmodules << endl;
  for( UShort_t i=0; i<fNmodules; i++ ) {
    Module* m = GetModule(i);
    cout << " " 
	 << setw(5) << m->crate
	 << setw(5) << m->slot 
	 << setw(5) << m->lo 
	 << setw(5) << m->hi 
	 << setw(5) << m->first
	 << setw(5) << GetModel(m);
    if( IsADC(m) )
      cout << setw(4) << " ADC";
    if( IsTDC(m) )
      cout << setw(4) << " TDC";
    cout << setw(5) << m->refchan
	 << setw(5) << m->refindex
	 << setw(8) << m->resolution
	 << endl;
  }
}

//_____________________________________________________________________________
void THaDetMap::Reset()
{
  // Clear() the map and reset the array size to zero, freeing memory.

  Clear();
  delete [] fMap;
  fMap = NULL;
  fMaplength = 0;
}

//_____________________________________________________________________________
static int compare_modules( const void* p1, const void* p2 )
{
  // Helper function for sort

  const THaDetMap::Module* lhs = static_cast<const THaDetMap::Module*>(p1);
  const THaDetMap::Module* rhs = static_cast<const THaDetMap::Module*>(p2);
  
  if( lhs->crate < rhs->crate )  return -1;
  if( lhs->crate > rhs->crate )  return  1;
  if( lhs->slot < rhs->slot )    return -1;
  if( lhs->slot > rhs->slot )    return  1;
  if( lhs->lo < rhs->lo )        return -1;
  if( lhs->lo > rhs->lo )        return  1;
  return 0;
}

//_____________________________________________________________________________
void THaDetMap::Sort()
{
  // Sort the map by crate/slot/low channel
  
  if( fMap && fNmodules )
    qsort( fMap, fNmodules, sizeof(Module), compare_modules );

}

//_____________________________________________________________________________
ClassImp(THaDetMap)

