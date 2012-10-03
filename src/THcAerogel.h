#ifndef ROOT_THcHodoscope
#define ROOT_THcHodoscope

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcHodoscope                                                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "TClonesArray.h"
#include "THaNonTrackingDetector.h"
#include "THcHitList.h"
#include "THcAerogelHit.h"

class THcHodoscope : public THaNonTrackingDetector, public THcHitList {

 public:
  THcHodoscope( const char* name, const char* description = "",
		THaApparatus* a = NULL );
  virtual ~THcHodoscope();
  
  virtual void 	     Clear( Option_t* opt="" );
  virtual Int_t      Decode( const THaEvData& );
  virtual EStatus    Init( const TDatime& run_time );
  virtual Int_t      CoarseProcess( TClonesArray& tracks );
  virtual Int_t      FineProcess( TClonesArray& tracks );
  
  virtual Int_t      ApplyCorrections( void );

 protected:
  
  ClassDef(THcHodoscope,0)   // Generic hodoscope class
};

#endif
