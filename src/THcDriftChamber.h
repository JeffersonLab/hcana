#ifndef ROOT_THcDriftChamber
#define ROOT_THcDriftChamber

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcDriftChamber                                                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THaSubDetector.h"
#include "THcDriftChamberPlane.h"
#include "TClonesArray.h"
#include "TMatrixD.h"

#include <map>
#include <vector>

#define MAX_SPACE_POINTS 100
#define MAX_HITS_PER_POINT 20

//#include "TMath.h"

//class THaScCalib;
class TClonesArray;
class THcSpacePoint;

class THcDriftChamber : public THaSubDetector {

public:
  THcDriftChamber( const char* name, const char* description, Int_t chambernum,
		   THaDetectorBase* parent = NULL );
  virtual ~THcDriftChamber();

  virtual Int_t Decode( const THaEvData& );
  virtual EStatus    Init( const TDatime& run_time );

  virtual void       AddPlane(THcDriftChamberPlane *plane);
  virtual Int_t      ApplyCorrections( void );
  virtual void       ProcessHits( void );
  virtual Int_t      FindSpacePoints( void ) ;
  virtual void       PrintDecode( void ) ;
  virtual void       CorrectHitTimes( void ) ;
  virtual void       LeftRight(void);


  virtual void   Clear( Option_t* opt="" );

  Int_t GetNHits() const { return fNhits; }
  Int_t GetNSpacePoints() const { return(fNSpacePoints);}
  Int_t GetNTracks() const { return fTrackProj->GetLast()+1; }
  const TClonesArray* GetTrackHits() const { return fTrackProj; }
  TClonesArray* GetSpacePointsP() const { return(fSpacePoints);}
  Int_t GetChamberNum() const { return fChamberNum;}
  Double_t GetZPos() const {return fZPos;}
  //  friend class THaScCalib;
  void SetHMSStyleFlag(Int_t flag) {fHMSStyleChambers = flag;}

  THcDriftChamber(); // for ROOT I/O
protected:
  Int_t f;

  // Calibration

  // Per-event data
  Int_t fNhits;
  Int_t fNthits;
  Int_t fN_True_RawHits;

  Int_t fNPlanes;		// Number of planes in the chamber

  Int_t fChamberNum;

  // HMS Specific
  Int_t YPlaneInd; 		// Index of Yplane for this chamber
  Int_t YPlanePInd; 		// Index of Yplanep for this chamber
  Int_t YPlaneNum;		// Absolute plane number of Yplane
  Int_t YPlanePNum;		// Absolute plane number of Yplanep
  // SOS Specific
  Int_t XPlaneInd; 		// Index of Xplane for this chamber
  Int_t XPlanePInd; 		// Index of Xplanep for this chamber
  Int_t XPlaneNum;		// Absolute plane number of Xplane
  Int_t XPlanePNum;		// Absolute plane number of Xplanep

  // Parameters
  Int_t fMinHits; 		// Minimum hits required to do something
  Int_t fMaxHits; 		// Maximum required to do something
  Int_t fMinCombos;             // Minimum # pairs in a space point
  Int_t fRemove_Sppt_If_One_YPlane;
  Double_t fWireVelocity;
  Int_t fSmallAngleApprox;
  Double_t fStubMaxXPDiff;
  Int_t fFixPropagationCorrection;
  Int_t fHMSStyleChambers;
  Int_t fhdebugflagpr;
  Int_t fdebugstubchisq;
  Double_t fRatio_xpfp_to_xfp; // Used in selecting stubs 
  Double_t fZPos;
  Double_t fXCenter;
  Double_t fYCenter;
  Double_t fSpacePointCriterion;
  Double_t fMaxDist; 		// Max dist used in EasySpacePoint methods
  Double_t* fSinBeta;
  Double_t* fCosBeta;
  Double_t* fTanBeta;
  Double_t* fSigma;
  Double_t* fPsi0;
  Double_t** fStubCoefs;

  std::vector<THcDriftChamberPlane*> fPlanes;
  //  THcDriftChamberPlane* fPlanes[20]; // List of plane objects

  TClonesArray*  fTrackProj;  // projection of track onto scintillator plane
                              // and estimated match to TOF paddle
  //  void           ClearEvent();
  void           DeleteArrays();
  virtual Int_t  ReadDatabase( const TDatime& date );
  virtual Int_t  DefineVariables( EMode mode = kDefine );

  void Setup(const char* name, const char* description);
  Int_t      FindEasySpacePoint_HMS(Int_t yplane_hitind, Int_t yplanep_hitind);
  Int_t      FindEasySpacePoint_SOS(Int_t xplane_hitind, Int_t xplanep_hitind);
  Int_t      FindHardSpacePoints(void);
  Int_t      DestroyPoorSpacePoints(void);
  Int_t      SpacePointMultiWire(void);
  void       ChooseSingleHit(void);
  void       SelectSpacePoints(void);
  UInt_t     Count1Bits(UInt_t x);
  Double_t   FindStub(Int_t nhits, THcSpacePoint *sp,
		      Int_t* plane_list, UInt_t bitpat,
		      Int_t* plusminus, Double_t* stub);

  std::vector<THcDCHit*> fHits;	/* All hits for this chamber */
  TClonesArray *fSpacePoints;
  Int_t fNSpacePoints;
  Int_t fEasySpacePoint;	/* This event is an easy space point */

  Double_t* stubcoef[4];
  std::map<int,TMatrixD> fAA3Inv;

  THaDetectorBase* fParent;

  ClassDef(THcDriftChamber,0)   // A single drift chamber
};

////////////////////////////////////////////////////////////////////////////////

#endif
