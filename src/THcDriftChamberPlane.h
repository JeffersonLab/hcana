#ifndef ROOT_THcDriftChamberPlane
#define ROOT_THcDriftChamberPlane

//////////////////////////////////////////////////////////////////////////////
//                         
// THcDriftChamberPlane
//
// A Hall C scintillator plane
//
// May want to later inherit from a THcPlane class if there are similarities
// in what a plane is shared with other detector types (shower, etc.)
// 
//////////////////////////////////////////////////////////////////////////////

#include "THaSubDetector.h"
#include "TClonesArray.h"
#include <cassert>

class THaEvData;
class THcDCWire;
class THcDCHit;
class THcDCTimeToDistConv;
class THcHodoscope;

/*class THaSignalHit;*/

class THcDriftChamberPlane : public THaSubDetector {
  
 public:
  THcDriftChamberPlane( const char* name, const char* description,
			Int_t planenum, THaDetectorBase* parent = NULL);
  virtual ~THcDriftChamberPlane();

  virtual void    Clear( Option_t* opt="" );
  virtual Int_t Decode( const THaEvData& );
  virtual EStatus Init( const TDatime& run_time );

  virtual Int_t CoarseProcess( TClonesArray& tracks );
  virtual Int_t FineProcess( TClonesArray& tracks );
          Bool_t   IsTracking() { return kFALSE; }
  virtual Bool_t   IsPid()      { return kFALSE; }

  virtual Int_t ProcessHits(TClonesArray* rawhits, Int_t nexthit);

  // Get and Set functions
  Int_t        GetNWires()   const { return fWires->GetLast()+1; }
  THcDCWire*  GetWire(Int_t i) const
  { assert( i>=1 && i<=GetNWires() );
    return (THcDCWire*)fWires->UncheckedAt(i-1); }

 protected:

  TClonesArray* fParentHitList;

  TClonesArray* fHits;
  TClonesArray* fWires;

  Int_t fPlaneNum;
  Int_t fNChamber;
  Int_t fNWires;
  Int_t fWireOrder;
  Int_t fTdcWinMin;
  Int_t fTdcWinMax;
  Double_t fPitch;
  Double_t fCentralWire;
  Double_t fPlaneTimeZero;

  Double_t fCenter;

  Double_t fNSperChan;		/* TDC bin size */

  virtual Int_t  ReadDatabase( const TDatime& date );
  virtual Int_t  DefineVariables( EMode mode = kDefine );

  THcDCTimeToDistConv* fTTDConv;  // Time-to-distance converter for this plane's wires

  THcHodoscope* fglHod;		// Hodoscope to get start time

  ClassDef(THcDriftChamberPlane,0)
};
#endif


