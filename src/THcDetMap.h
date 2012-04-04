
#ifndef ROOT_THcDetMap
#define ROOT_THcDetMap

//////////////////////////////////////////////////////////////////////////
//
// THcDetMap
//
// Standard detector map.
//
// Add plane and signal information to Hall A standard.
//
//////////////////////////////////////////////////////////////////////////

//FIXME: Several things here are duplicates of information that already is
// (or should be) available from the cratemap:  specifically "model" and
// "resolution", as well as the "model map" in AddModule. We should get it from
// there -  to ensure consistency and have one less maintenance headache.

#include "THaDetMap.h"

class THcDetMap : public THaDetMap {

public:
  struct Module {
    UShort_t crate;
    UShort_t slot;
    UShort_t lo;
    UShort_t hi;
    UInt_t   first;  // logical number of first channel
    UInt_t   plane;  // Plane of detector
    UInt_t   signal; // Signal type (ADC/TDC/Left/Right)
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

  THcDetMap();
  THcDetMap( const THcDetMap& );
  THcDetMap& operator=( const THcDetMap& );
  virtual ~THcDetMap();
  
protected:
  Module*      fMap;       // Array of modules, each module is a 7-tuple
                           // (create,slot,chan_lo,chan_hi,first_logical,
                           // model,refindex)

  ClassDef(THcDetMap,0)   //The standard detector map
};

#endif
