
#ifndef ROOT_THaDetMap
#define ROOT_THaDetMap

//////////////////////////////////////////////////////////////////////////
//
// THaDetMap
//
// Standard detector map.
// The detector map defines the hardware channels that correspond to a
// single detector. Typically, "channels" are Fastbus or VMW addresses 
// characterized by
//
//   Crate, Slot, range of channels
//
//////////////////////////////////////////////////////////////////////////

//FIXME: Several things here are duplicates of information that already is
// (or should be) available from the cratemap:  specifically "model" and
// "resolution", as well as the "model map" in AddModule. We should get it from
// there -  to ensure consistency and have one less maintenance headache.

#include "Rtypes.h"
#include <vector>

class THaDetMap {

protected:
  static const UInt_t kADCBit = BIT(31);
  static const UInt_t kTDCBit = BIT(30);
  // Upper byte of model reserved for property bits
  static const UInt_t kModelMask = 0x00ffffff;
  
public:
  struct Module {
    UShort_t crate;
    UShort_t slot;
    UShort_t lo;
    UShort_t hi;
    UInt_t   first;  // logical number of first channel
    UInt_t   model;  // model number of module (for ADC/TDC identification).
    Int_t    refchan;    // for pipeline TDCs: reference channel number
    Int_t    refindex;   // for pipeline TDCs: index into reference channel map
    Double_t resolution; // Resolution (s/chan) for TDCs
    Bool_t   reverse;    // Indicates that "first" corresponds to hi, not lo

    bool   operator==( const Module& rhs ) const
    { return crate == rhs.crate && slot == rhs.slot; }
    bool   operator!=( const Module& rhs ) const
    { return !(*this==rhs); }
    Int_t  GetNchan() const { return hi-lo+1; }
    UInt_t GetModel() const { return model & kModelMask; }
    Bool_t IsTDC()    const { return model & kTDCBit; }
    Bool_t IsADC()    const { return model & kADCBit; }
    void   SetModel( UInt_t model );
    void   SetResolution( Double_t resolution );
    void   MakeTDC()  { model |= kTDCBit; }
    void   MakeADC()  { model |= kADCBit; }
  };

  // Flags for GetMinMaxChan()
  enum ECountMode {
    kLogicalChan         = 0,
    kRefIndex            = 1
  };
  // Flags for Fill()
  enum EFillFlags {
    kDoNotClear          = BIT(0),    // Don't clear the map first
    kFillLogicalChannel  = BIT(10),   // Parse the logical channel number
    kFillModel           = BIT(11),   // Parse the model number
    kFillRefIndex        = BIT(12),   // Parse the reference index
    kFillRefChan         = BIT(13)    // Parse the reference channel
  };

  THaDetMap();
  THaDetMap( const THaDetMap& );
  THaDetMap& operator=( const THaDetMap& );
  virtual ~THaDetMap();
  
  virtual Int_t     AddModule( UShort_t crate, UShort_t slot, 
			       UShort_t chan_lo, UShort_t chan_hi,
			       UInt_t first=0, UInt_t model=0,
			       Int_t refindex=-1, Int_t refchan = -1 );
          void      Clear()  { fNmodules = 0; }
  virtual Module*   Find( UShort_t crate, UShort_t slot, UShort_t chan );
  virtual Int_t     Fill( const std::vector<Int_t>& values, UInt_t flags = 0 );
          void      GetMinMaxChan( Int_t& min, Int_t& max,
				   ECountMode mode = kLogicalChan ) const;
          Module*   GetModule( UShort_t i ) const ;
          Int_t     GetNchan( UShort_t i ) const;
          Int_t     GetTotNumChan() const;
          Int_t     GetSize() const { return static_cast<Int_t>(fNmodules); }

          UInt_t    GetModel( UShort_t i ) const;
          Bool_t    IsADC( UShort_t i ) const;
          Bool_t    IsTDC( UShort_t i ) const;
  static  UInt_t    GetModel( Module* d );
  static  Bool_t    IsADC( Module* d );
  static  Bool_t    IsTDC( Module* d );

  virtual void      Print( Option_t* opt="" ) const;
  virtual void      Reset();
  virtual void      Sort();

  static const int kDetMapSize = (1<<16)-1;  //Sanity limit on map size

protected:
  UShort_t     fNmodules;  // Number of modules (=crate,slot) in the map

  Module*      fMap;       // Array of modules, each module is a 7-tuple
                           // (create,slot,chan_lo,chan_hi,first_logical,
                           // model,refindex)

  Int_t        fMaplength; // current size of the fMap array
  
  Module*      uGetModule( UShort_t i ) const { return fMap+i; }

  ClassDef(THaDetMap,0)   //The standard detector map
};

inline THaDetMap::Module* THaDetMap::GetModule( UShort_t i ) const {
  return i<fNmodules ? uGetModule(i) : NULL; 
}

inline Bool_t THaDetMap::IsADC(Module* d) {
  if( !d ) return kFALSE;
  return d->IsADC();
}

inline Bool_t THaDetMap::IsTDC(Module* d) {
  if( !d ) return kFALSE;
  return d->IsTDC();
}

inline UInt_t THaDetMap::GetModel(Module* d) {
  if( !d ) return 0;
  return d->GetModel();
}

inline Bool_t THaDetMap::IsADC( UShort_t i ) const {
  if( i >= fNmodules ) return kFALSE;
  return uGetModule(i)->IsADC();
}

inline Bool_t THaDetMap::IsTDC( UShort_t i ) const {
  if( i >= fNmodules ) return kFALSE;
  return uGetModule(i)->IsTDC();
}

inline UInt_t THaDetMap::GetModel( UShort_t i ) const {
  if( i >= fNmodules ) return 0;
  return uGetModule(i)->GetModel();
}

inline Int_t THaDetMap::GetNchan( UShort_t i ) const {
  // Return the number of channels of the i-th module
  if( i >= fNmodules ) return 0;
  return uGetModule(i)->GetNchan();
}

#endif
