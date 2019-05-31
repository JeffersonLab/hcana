#ifndef ROOT_THcDC
#define ROOT_THcDC

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcDC                                                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THaTrackingDetector.h"
#include "THcHitList.h"
#include "THcRawDCHit.h"
#include "THcSpacePoint.h"
#include "THcDriftChamberPlane.h"
#include "THcDriftChamber.h"
#include "TMath.h"

#define NUM_FPRAY 4

//class THaScCalib;
class TClonesArray;

class THcDC : public THaTrackingDetector, public THcHitList {

public:
  THcDC( const char* name, const char* description = "",
		   THaApparatus* a = NULL );
  virtual ~THcDC();

  virtual Int_t      Decode( const THaEvData& );
  virtual EStatus    Init( const TDatime& run_time );
  virtual Int_t      End(THaRunBase* run=0);
  virtual Int_t      CoarseTrack( TClonesArray& tracks );
  virtual Int_t      FineTrack( TClonesArray& tracks );

  virtual Int_t      ApplyCorrections( void );

  //  Int_t GetNHits() const { return fNhit; }

  //  Int_t GetNTracks() const { return fNDCTracks; }
  //  const TClonesArray* GetTrackHits() const { return fTrackProj; }
  void SetFocalPlaneBestTrack(Int_t golden_track_index); // Called in THcHallCSpectrometer:

  Int_t GetNWires(Int_t plane) const { return fNWires[plane-1];}
  Int_t GetNChamber(Int_t plane) const { return fNChamber[plane-1];}
  Int_t GetWireOrder(Int_t plane) const { return fWireOrder[plane-1];}
  Double_t GetPitch(Int_t plane) const { return fPitch[plane-1];}
  Double_t GetCentralWire(Int_t plane) const { return fCentralWire[plane-1];}
  Int_t GetTdcWinMin(Int_t plane) const { return fTdcWinMin[plane-1];}
  Int_t GetTdcWinMax(Int_t plane) const { return fTdcWinMax[plane-1];}

  Double_t GetXPos(Int_t plane) const { return fXPos[plane-1];}
  Double_t GetYPos(Int_t plane) const { return fYPos[plane-1];}
  Double_t GetZPos(Int_t plane) const { return fZPos[plane-1];}
  Double_t GetAlphaAngle(Int_t plane) const { return fAlphaAngle[plane-1];}
  Double_t GetBetaAngle(Int_t plane) const { return fBetaAngle[plane-1];}
  Double_t GetGammaAngle(Int_t plane) const { return fGammaAngle[plane-1];}

  Int_t GetMinHits(Int_t chamber) const { return fMinHits[chamber-1];}
  Int_t GetMaxHits(Int_t chamber) const { return fMaxHits[chamber-1];}
  Int_t GetMinCombos(Int_t chamber) const { return fMinCombos[chamber-1];}
  Double_t GetSpacePointCriterion(Int_t chamber) const { return fSpace_Point_Criterion[chamber-1];}
  Double_t GetCentralTime(Int_t plane) const { return fCentralTime[plane-1];}
  Int_t GetDriftTimeSign(Int_t plane) const { return fDriftTimeSign[plane-1];}
  Int_t GetReadoutLR(Int_t plane) const { return fReadoutLR[plane-1];}
  Int_t GetReadoutTB(Int_t plane) const { return fReadoutTB[plane-1];}
  Int_t GetVersion() const {return fVersion;}


  Double_t GetPlaneTimeZero(Int_t plane) const { return fPlaneTimeZero[plane-1];}
  Double_t GetSigma(Int_t plane) const { return fSigma[plane-1];}
  Int_t GetFixPropagationCorrectionFlag() const {return fFixPropagationCorrection;}

  Double_t GetNSperChan() const { return fNSperChan;}

  Double_t GetCenter(Int_t plane) const {
    return
      //fXCenter[chamber]*sin(fAlphaAngle[plane-1]) +
      //fYCenter[chamber]*cos(fAlphaAngle[plane-1]);

      fXPos[plane-1]*sin(fAlphaAngle[plane-1]) +
      fYPos[plane-1]*cos(fAlphaAngle[plane-1]);
  }
  //  friend class THaScCalib;

  THcDC();  // for ROOT I/O
protected:
  Int_t fdebuglinkstubs;
  Int_t fdebugprintrawdc;
  Int_t fdebugflagpr;
  Int_t fdebugflagstubs;
  Int_t fdebugtrackprint;
  Int_t fdebugprintdecodeddc;
  Int_t fHMSStyleChambers;
  Int_t fTDC_RefTimeCut;

  UInt_t fNDCTracks;
  TClonesArray* fDCTracks;     // Tracks found from stubs (THcDCTrack obj)
  // Calibration

  // Hall C Parameters
  char fPrefix[2];
  Int_t fNPlanes;              // Total number of DC planes
  char** fPlaneNames;
  UInt_t fNChambers;
  Int_t fFixLR;			// If 1, allow a given hit to have different LR
                                // for different space points
  Int_t fFixPropagationCorrection; // If 1, don't reapply (and accumulate) the
                                // propagation along the wire correction for
                                // each space point a hit occurs in.  Keep a
                                // separate correction for each space point.
  Int_t fProjectToChamber;	// If 1, project y position each stub back to it's own
                                // chamber before comparing y positions in LinkStubs
                                // Was used for SOS in ENGINE.

  // Per-event data
  Int_t fStubTest;
  Int_t fNhits;
  Int_t fNthits;
  Int_t fN_True_RawHits;
  Int_t fNSp;                   // Number of space points
  Int_t fNsp_best;                   // Number of space points for gloden track
  Double_t* fResiduals;         //[fNPlanes] Array of residuals
  Double_t* fResidualsExclPlane;         //[fNPlanes] Array of residuals with plane excluded
  Double_t* fWire_hit_did;      //[fNPlanes]
  Double_t* fWire_hit_should;   //[fNPlanes]

  Double_t fNSperChan;		/* TDC bin size */
  Double_t fWireVelocity;
  Int_t fSingleStub;		/* If 1, single stubs make tracks */
  Int_t fNTracksMaxFP;
  Double_t fXtTrCriterion;
  Double_t fYtTrCriterion;
  Double_t fXptTrCriterion;
  Double_t fYptTrCriterion;
  Int_t fVersion;

  // Each of these will be dimensioned with the number of chambers
  Double_t* fXCenter;
  Double_t* fYCenter;
  Int_t* fMinHits;
  Int_t* fMaxHits;
  Int_t* fMinCombos;
  Double_t* fSpace_Point_Criterion;

  // Each of these will be dimensioned with the number of planes
  // A THcDCPlane class object will need to access the value for
  // its plane number.  Should we have a Get method for each or
  Int_t* fTdcWinMin;
  Int_t* fTdcWinMax;
  Double_t* fCentralTime;
  Int_t* fNWires;		// Number of wires per plane
  Int_t* fNChamber;
  Int_t* fWireOrder;
  Int_t* fDriftTimeSign;
  Int_t* fReadoutTB;
  Int_t* fReadoutLR;


  Double_t* fXPos;
  Double_t* fYPos;
  Double_t* fZPos;
  Double_t* fAlphaAngle;
  Double_t* fBetaAngle;
  Double_t* fGammaAngle;
  Double_t* fPitch;
  Double_t* fCentralWire;
  Double_t* fPlaneTimeZero;
  Double_t* fSigma;
  Double_t** fPlaneCoeffs;
  //
  Double_t fX_fp_best;
  Double_t fY_fp_best;
  Double_t fXp_fp_best;
  Double_t fYp_fp_best;
  Double_t fChisq_best;
  Int_t fSp1_ID_best;
  Int_t fSp2_ID_best;
  Bool_t fInSideDipoleExit_best;
 // For accumulating statitics for efficiencies
  Int_t fTotEvents;
  Int_t* fNChamHits;
  Int_t* fPlaneEvents;

  // Pointer to global var indicating whether this spectrometer is triggered
  // for this event.
  Bool_t* fPresentP;

  // Useful derived quantities
  // double tan_angle, sin_angle, cos_angle;

  // Intermediate structure for building
  static const UInt_t MAXTRACKS = 50;

  std::vector<THcDriftChamberPlane*> fPlanes; // List of plane objects
  std::vector<THcDriftChamber*> fChambers; // List of chamber objects

  TClonesArray*  fTrackProj;  // projection of track onto scintillator plane
                              // and estimated match to TOF paddle
  void           ClearEvent();
  void           DeleteArrays();
  virtual Int_t  ReadDatabase( const TDatime& date );
  virtual Int_t  DefineVariables( EMode mode = kDefine );
  void           LinkStubs();
  void           TrackFit();
  Double_t       DpsiFun(Double_t ray[4], Int_t plane);
  void           EffInit();
  void           Eff();

  void Setup(const char* name, const char* description);
  void PrintSpacePoints();
  void PrintStubs();
  void EfficiencyPerWire(Int_t golden_track_index);
  ClassDef(THcDC,0)   // Set of Drift Chambers detector
};

////////////////////////////////////////////////////////////////////////////////

#endif
