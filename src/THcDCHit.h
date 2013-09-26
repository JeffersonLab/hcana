#ifndef ROOT_THcDCHit
#define ROOT_THcDCHit

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcDCHit                                                                 //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "THcDCWire.h"
#include "THcDriftChamberPlane.h"
#include "THcDriftChamber.h"
#include <cstdio>

class THcDCHit : public TObject {

public:
  THcDCHit( THcDCWire* wire=NULL, Int_t rawtime=0, Double_t time=0.0,
	    THcDriftChamberPlane* wp=0) : 
    fWire(wire), fRawTime(rawtime), fTime(time), fWirePlane(wp),
    fDist(0.0), ftrDist(kBig) {
    ConvertTimeToDist();
    fCorrected = 0;
  }
  virtual ~THcDCHit() {}

  virtual Double_t ConvertTimeToDist();
  Int_t  Compare ( const TObject* obj ) const;
  Bool_t IsSortable () const { return kTRUE; }
  virtual void Print( Option_t* opt="" ) const;
  
  // Get and Set Functions
  THcDCWire* GetWire() const { return fWire; }
  Int_t    GetWireNum() const { return fWire->GetNum(); }
  Int_t    GetRawTime() const { return fRawTime; }
  Double_t GetTime()    const { return fTime; }
  Double_t GetDist()    const { return fDist; }
  Double_t GetPos()     const { return fWire->GetPos(); } //Position of hit wire
  Double_t GetCoord()   const { return fCoord; }
  Double_t GetdDist()   const { return fdDist; }
  Int_t    GetCorrectedStatus() const { return fCorrected; }

  THcDriftChamberPlane* GetWirePlane() const { return fWirePlane; }
  

  void     SetWire(THcDCWire * wire) { fWire = wire; }
  void     SetRawTime(Int_t time)     { fRawTime = time; }
  void     SetTime(Double_t time)     { fTime = time; }
  void     SetDist(Double_t dist)     { fDist = dist; }
  void     SetLeftRight(Int_t lr)   { fCoord = GetPos() + lr*fDist; fLR=lr;}
  Int_t    GetLR() { return fLR; }
  void     SetdDist(Double_t ddist)   { fdDist = ddist; }
  void     SetFitDist(Double_t dist)  { ftrDist = dist; }
  Int_t    GetPlaneNum() const { return fWirePlane->GetPlaneNum(); }
  Int_t    GetPlaneIndex() const { return fWirePlane->GetPlaneIndex(); }
  Int_t    GetChamberNum() const { return fWirePlane->GetChamberNum(); }
  void     SetCorrectedStatus(Int_t c) { fCorrected = c; }
  
protected:
  static const Double_t kBig;  //!
  
  THcDCWire*  fWire;     // Wire on which the hit occurred
  Int_t       fRawTime;  // TDC value (channels)
  Double_t    fTime;     // Time corrected for time offset of wire (s)
  THcDriftChamberPlane* fWirePlane; //! Pointer to parent wire plane
  Double_t    fDist;     // (Perpendicular) Drift Distance
  Int_t       fLR;       // +1/-1 which side of wire
  Double_t    fCoord;    // Actual coordinate of hit
  Double_t    fdDist;    // uncertainty in fDist (for chi2 calc)
  Double_t    ftrDist;   // (Perpendicular) distance from the track
  Int_t       fCorrected; // Has this hit been corrected?

  THcDriftChamber* fChamber; //! Pointer to parent wire plane

  
private:
  THcDCHit( const THcDCHit& );
  THcDCHit& operator=( const THcDCHit& );
  
  ClassDef(THcDCHit,2)             // VDCHit class
};

////////////////////////////////////////////////////////////////////////////////

#endif
