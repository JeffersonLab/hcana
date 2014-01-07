#ifndef ROOT_THcRaster
#define ROOT_THcRaster


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcRaster                                                                 //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THaBeamDet.h"

class THcRaster : public THaBeamDet {

 public:

  THcRaster( const char* name, const char* description ="" ,
	     THaApparatus* a = NULL );

  virtual ~THaRaster();

  virtual Int_t      Decode( const THaEvData& );
  virtual Int_t      Process();

  virtual TVector3 GetPosition()  const { return fPosition[2]; }
  virtual TVector3 GetDirection() const { return NULL; } // Hall C we don't use raster direction yet.

  Double_t GetCurrentX() { return fRawPos(0); }
  Double_t GetCurrentY() { return fRawPos(1); }

 protected:

  void           ClearEvent();
  virtual Int_t  ReadDatabase( const TDatime& date );

  TVector        fRawPos[2];        // current in Raster ADCs for position
  TVector3       fPosition[2];   // Beam position at 1st, 2nd BPM or at the target (meters)
  TVector3  fPosOff[2]; // pedestals 


 private:

  ClassDef(THcRaster, 0);	// add THcRaster to ROOT library
};  

////////////////////////////////////////////////////////////////////////////////


#endif
