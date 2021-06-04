#ifndef ROOT_THcDCTrack
#define ROOT_THcDCTrack

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcDCTrack                                                             //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

//#include "THaCluster.h"
#include "TVector3.h"
#include "TObject.h"

//class THaVDCCluster;
class THcDCPlane;
class THaTrack;
class THcDCHit;

class THcDCTrack : public TObject {

public:
  THcDCTrack(Int_t nplanes);
  virtual ~THcDCTrack() {};

  virtual void AddSpacePoint(THcSpacePoint* sp);

  //Get and Set Functions
  //  Int_t* GetSpacePoints()               {return fspID;}
  Int_t GetNSpacePoints()         const { return fnSP;}
  //  Int_t GetSpacePointID(Int_t i)  const {return fspID[i];}
  THcSpacePoint* GetSpacePoint(Int_t i) const {return fSp[i];}
  THcDCHit* GetHit(Int_t i)       const {return fHits[i].dchit;}
  Double_t GetHitDist(Int_t ihit) { return fHits[ihit].distCorr; };
  Int_t GetHitLR(Int_t ihit) { return fHits[ihit].lr; };
  Int_t GetNHits()                const {return fNHits;}
  Int_t GetNFree()                const {return fNfree;}
  Double_t GetCoord(Int_t ip)     const {return fCoords[ip];}
  Double_t GetResidual(Int_t ip)     const {return fResiduals[ip];}
  Double_t GetResidual1()     const {return fResiduals[0];}
  Double_t GetResidualExclPlane(Int_t ip)     const {return fResidualsExclPlane[ip];}
  void GetRay(Double_t *ray) const {ray[0]=fX_fp; ray[1]=fY_fp; ray[2]=fXp_fp; ray[3]=fYp_fp;}
  Double_t GetX()                 const {return fX_fp;}
  Double_t GetY()                 const {return fY_fp;}
  Double_t GetXP()                 const {return fXp_fp;}
  Double_t GetYP()                 const {return fYp_fp;}
  Double_t GetSp1_ID()                 const {return fSp1_ID;}
  Double_t GetSp2_ID()                 const {return fSp2_ID;}
  Double_t GetChisq()              const {return fChi2_fp;}
  void SetNFree(Int_t nfree)           {fNfree = nfree;}
  void SetCoord(Int_t ip, Double_t coord) {fCoords[ip] = coord;}
  void SetResidual(Int_t ip, Double_t coord) {fResiduals[ip] = coord;}
  void SetResidualExclPlane(Int_t ip, Double_t coord) {fResidualsExclPlane[ip] = coord;}
  void SetDoubleResidual(Int_t ip, Double_t coord) {fDoubleResiduals[ip] = coord;}
  void SetVector(Double_t x, Double_t y, Double_t z,
		 Double_t xp, Double_t yp) {fX_fp = x; fY_fp=y; fZ_fp=z; fXp_fp=xp; fYp_fp=yp;}
  void SetChisq(Double_t chi2)   {fChi2_fp = chi2;}
  void SetSp1_ID(Int_t isp1) {fSp1_ID= isp1;}
  void SetSp2_ID(Int_t isp2) {fSp2_ID= isp2;}
  void SetHitLR(Int_t ihit,Int_t lrtemp) { fHits[ihit].lr=lrtemp; }

  virtual void ClearHits( );

  // TObject functions redefined
  virtual void Clear( Option_t* opt="" );
  virtual void RemoveHit(Int_t RemoveHitIndex);

protected:
  Int_t fnSP; /* Number of space points in this track */
  THcSpacePoint* fSp[10];         /* List of space points in this track */
  Int_t fSp1_ID;
  Int_t fSp2_ID;
  Int_t fNHits;
  Int_t fNfree;		  /* Number of degrees of freedom */
  struct Hit {
    // This is the same structure as in THcSpacePoint.  Can we not
    // duplicate this?
    THcDCHit* dchit;
    Double_t distCorr;
    Int_t lr;
  };
  std::vector<Hit> fHits; /* List of hits for this track */
  //std::vector<THcDCHit*> fHits; /* List of hits for this track */
  std::vector<Double_t> fCoords; /* Coordinate on each plane */
  std::vector<Double_t> fResiduals; /* Residual on each plane */
  std::vector<Double_t> fResidualsExclPlane; /* Residual on each plane */
  std::vector<Double_t> fDoubleResiduals; /* Residual on each plane for single stub mode */
  Double_t fX_fp, fY_fp, fZ_fp;
  Double_t fXp_fp, fYp_fp;
  Double_t fChi2_fp;

  virtual void AddHit(THcDCHit * hit, Double_t dist, Int_t lr);
private:
  // Hide copy ctor and op=
  THcDCTrack( const THcDCTrack& );
  THcDCTrack& operator=( const THcDCTrack& );

  ClassDef(THcDCTrack,0)	// Full Drift Chamber track
};
#endif
