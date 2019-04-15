#ifndef ROOT_THcHodoscope
#define ROOT_THcHodoscope

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcHodoscope                                                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <vector>

#include "TClonesArray.h"
#include "TH1F.h"
#include "THaNonTrackingDetector.h"
#include "THcHitList.h"
#include "THcHodoHit.h"
#include "THcRawHodoHit.h"
#include "THcScintillatorPlane.h"
#include "THcCherenkov.h"

#include "THaTrackingDetector.h"
#include "THcHitList.h"
#include "THcRawDCHit.h"
#include "THcSpacePoint.h"
#include "THcDriftChamberPlane.h"
#include "THcDriftChamber.h"
#include "TMath.h"

#include "THaSubDetector.h"
#include "TClonesArray.h"
#include <iostream>
#include <fstream>


class THaScCalib;

class THcHodoscope : public THaNonTrackingDetector, public THcHitList {

public:
  THcHodoscope( const char* name, const char* description = "",
		   THaApparatus* a = NULL );
  virtual ~THcHodoscope();

  virtual Int_t      Decode( const THaEvData& );
  virtual EStatus    Init( const TDatime& run_time );
  virtual void       Clear( Option_t* opt="" );

  virtual Int_t      CoarseProcess( TClonesArray& tracks );
  virtual Int_t      FineProcess( TClonesArray& tracks );
  virtual Int_t      End(THaRunBase* run=0);

  void EstimateFocalPlaneTime(void);
  void OriginalTrackEffTest(void);
  void TrackEffTest(void);
  virtual Int_t      ApplyCorrections( void );
  Double_t GetStartTime() const { return fStartTime; }
  Bool_t IsStartTimeGood() const {return fGoodStartTime;};
  Int_t GetNfptimes() const {return fNfptimes;};
  Int_t GetScinIndex(Int_t nPlane, Int_t nPaddle);
  Int_t GetScinIndex(Int_t nSide, Int_t nPlane, Int_t nPaddle);
  Double_t GetPathLengthCentral();
  Int_t GetNTracks() const { return fTrackProj->GetLast()+1; }
  Double_t GetTdcMin() const {return fScinTdcMin;}
  Double_t GetTdcMax() const {return fScinTdcMax;}
  Double_t GetTofTolerance() const {return fTofTolerance;}
  Double_t GetTdcToTime() const {return fScinTdcToTime;}
  Double_t GetBetaNominal() const {return fBetaNominal;}
  Double_t GetHodoPosPhcCoeff(Int_t iii) const {return fHodoPosPhcCoeff[iii];}
  Double_t GetHodoNegPhcCoeff(Int_t iii) const {return fHodoNegPhcCoeff[iii];}
  Double_t GetHodoPosMinPh(Int_t iii) const {return fHodoPosMinPh[iii];}
  Double_t GetHodoNegMinPh(Int_t iii) const {return fHodoNegMinPh[iii];}
  Double_t GetHodoPosTimeOffset(Int_t iii) const {return fHodoPosTimeOffset[iii];}
  Double_t GetHodoNegTimeOffset(Int_t iii) const {return fHodoNegTimeOffset[iii];}
  Double_t GetHodoVelLight(Int_t iii) const {return fHodoVelLight[iii];}
  Double_t GetHodoPosInvAdcOffset(Int_t iii) const {return fHodoPosInvAdcOffset[iii];}
  Double_t GetHodoNegInvAdcOffset(Int_t iii) const {return fHodoNegInvAdcOffset[iii];}
  Double_t GetHodoPosInvAdcLinear(Int_t iii) const {return fHodoPosInvAdcLinear[iii];}
  Double_t GetHodoNegInvAdcLinear(Int_t iii) const {return fHodoNegInvAdcLinear[iii];}
  Double_t GetHodoPosInvAdcAdc(Int_t iii) const {return fHodoPosInvAdcAdc[iii];}
  Double_t GetHodoNegInvAdcAdc(Int_t iii) const {return fHodoNegInvAdcAdc[iii];}
  Double_t GetHodoPosAdcTimeWindowMax(Int_t iii) const {return fHodoPosAdcTimeWindowMax[iii];}
  Double_t GetHodoPosAdcTimeWindowMin(Int_t iii) const {return fHodoPosAdcTimeWindowMin[iii];}
  Double_t GetHodoNegAdcTimeWindowMax(Int_t iii) const {return fHodoNegAdcTimeWindowMax[iii];}
  Double_t GetHodoNegAdcTimeWindowMin(Int_t iii) const {return fHodoNegAdcTimeWindowMin[iii];}
 
  //Get Time Walk Parameters
  Double_t GetHodoVelFit(Int_t iii) const {return fHodoVelFit[iii];}
  Double_t GetHodoCableFit(Int_t iii) const {return fHodoCableFit[iii];}
  Double_t GetHodoLCoeff(Int_t iii) const {return fHodo_LCoeff[iii];}


  Double_t GetHodoPos_c1(Int_t iii) const {return fHodoPos_c1[iii];}
  Double_t GetHodoNeg_c1(Int_t iii) const {return fHodoNeg_c1[iii];}
  Double_t GetHodoPos_c2(Int_t iii) const {return fHodoPos_c2[iii];}
  Double_t GetHodoNeg_c2(Int_t iii) const {return fHodoNeg_c2[iii];}
  Double_t GetTDCThrs() const {return fTdc_Thrs;}

  Double_t GetStartTimeCenter() const {return fStartTimeCenter;}
  Double_t GetStartTimeSlop() const {return fStartTimeSlop;}
  Double_t GetBetaNotrk() const {return fBetaNoTrk;}

  Int_t GetGoodRawPad(Int_t iii){return fTOFCalc[iii].good_raw_pad;}
  Int_t GetGoodRawPlane(Int_t iii){return fTOFCalc[iii].pindex;}
  Int_t GetNScinHits(Int_t iii){return fNScinHits[iii];}
  Int_t GetTotHits(){return fTOFCalc.size();}

  Int_t GetNPlanes() { return fNPlanes;}
  THcScintillatorPlane* GetPlane(Int_t ip) { return fPlanes[ip];}
  UInt_t GetNPaddles(Int_t ip) { return fNPaddle[ip];}
  Double_t GetHodoSlop(Int_t ip) { return fHodoSlop[ip];}
  Double_t GetPlaneCenter(Int_t ip) { return fPlaneCenter[ip];}
  Double_t GetPlaneSpacing(Int_t ip) { return fPlaneSpacing[ip];}
  Int_t GetTdcOffset(Int_t ip) const { return fTdcOffset[ip];}
  Double_t GetAdcTdcOffset(Int_t ip) const { return fAdcTdcOffset[ip];}


  Double_t GetBeta() const {return fBeta;}


  Double_t GetHodoPosSigma(Int_t iii) const {return fHodoPosSigma[iii];}
  Double_t GetHodoNegSigma(Int_t iii) const {return fHodoNegSigma[iii];}

  Bool_t GetFlags(Int_t itrack, Int_t iplane, Int_t ihit,
		  Bool_t& onTrack, Bool_t& goodScinTime,
		  Bool_t& goodTdcNeg, Bool_t& goodTdcPos) const {
    onTrack = fGoodFlags[itrack][iplane][ihit].onTrack;
    goodScinTime = fGoodFlags[itrack][iplane][ihit].goodScinTime;
    goodTdcNeg = fGoodFlags[itrack][iplane][ihit].goodTdcNeg;
    goodTdcPos = fGoodFlags[itrack][iplane][ihit].goodTdcPos;
    return(kTRUE);
  }

  const TClonesArray* GetTrackHits() const { return fTrackProj; }

  friend class THaScCalib;

  THcHodoscope();  // for ROOT I/O
protected:

  THcCherenkov* fCherenkov;

  Int_t fTDC_RefTimeCut;
  Int_t fADC_RefTimeCut;

  Int_t fAnalyzePedestals;

  Int_t fNHits;

  TH1F *hTime;
  // Calibration
  Double_t fRatio_xpfp_to_xfp;
  // Per-event data
  Bool_t fSHMS;
  Bool_t fGoodStartTime;
  Double_t fStartTime;
  Double_t fFPTimeAll;
  Int_t fNfptimes;
  Bool_t* fPresentP;
  Double_t fTimeHist_Peak;
  Double_t fTimeHist_Sigma;
  Double_t fTimeHist_Hits;

  Double_t     fBeta;

  Double_t     fBetaNoTrk;
  Double_t     fBetaNoTrkChiSq;
  // Per-event data

  // Potential Hall C parameters.  Mostly here for demonstration
  Int_t fNPlanes;		// Number of planes
  UInt_t fMaxScinPerPlane,fMaxHodoScin; // max number of scin/plane; product of the first two
  Double_t fStartTimeCenter, fStartTimeSlop, fScinTdcToTime;
  Double_t fTofTolerance;
  Int_t fCosmicFlag; //
  Int_t fNumPlanesBetaCalc; // Number of planes to use in beta calculation
  Double_t fPathLengthCentral;
  Double_t fScinTdcMin, fScinTdcMax; // min and max TDC values
  char** fPlaneNames;
  UInt_t* fNPaddle;		// Number of paddles per plane

  Double_t *fHodoNegAdcTimeWindowMin;    
  Double_t *fHodoNegAdcTimeWindowMax;
  Double_t *fHodoPosAdcTimeWindowMin;    
  Double_t *fHodoPosAdcTimeWindowMax;

  Double_t* fHodoVelLight;
  Double_t* fHodoPosSigma;
  Double_t* fHodoNegSigma;

  Double_t* fHodoPosMinPh;
  Double_t* fHodoNegMinPh;
  Double_t* fHodoPosPhcCoeff;
  Double_t* fHodoNegPhcCoeff;
  Double_t* fHodoPosTimeOffset;
  Double_t* fHodoNegTimeOffset;
  Int_t* fHodoPosPedLimit;
  Int_t* fHodoNegPedLimit;
  Int_t fTofUsingInvAdc;
  Double_t* fHodoPosInvAdcOffset;
  Double_t* fHodoNegInvAdcOffset;
  Double_t* fHodoPosInvAdcLinear;
  Double_t* fHodoNegInvAdcLinear;
  Double_t* fHodoPosInvAdcAdc;
  Double_t* fHodoNegInvAdcAdc;

  //New Time-Walk Calibration Parameters
  Double_t* fHodoVelFit;
  Double_t* fHodoCableFit;
  Double_t* fHodo_LCoeff;
  Double_t* fHodoPos_c1;
  Double_t* fHodoNeg_c1;
  Double_t* fHodoPos_c2;
  Double_t* fHodoNeg_c2;
  Double_t  fTdc_Thrs;  
  Double_t* fHodoSigmaPos;
  Double_t* fHodoSigmaNeg;

  Double_t fPartMass;		// Nominal particle mass
  Double_t fBetaNominal;	// Beta for central ray of nominal particle type

  THcScintillatorPlane** fPlanes; // List of plane objects

  TClonesArray*  fTrackProj;  // projection of track onto scintillator plane
                              // and estimated match to TOF paddle

  //--------------------------   Ahmed   -----------------------------


  Int_t        fCheckEvent;
  Int_t        fEventType;

  Int_t        fGoodTrack;
  Double_t     fScin2XZpos;
  Double_t     fScin2XdZpos;
  Double_t     fScin2YZpos;
  Double_t     fScin2YdZpos;

  Double_t     fChi2Min;
  Double_t*    fPlaneCenter;
  Double_t*    fPlaneSpacing;

  Double_t     fNormETot;
  Double_t     fNCerNPE;
  Double_t*    fHodoSlop;
  Int_t        *fTdcOffset;
  Double_t     *fAdcTdcOffset;
  Int_t        fdebugprintscinraw;
  Int_t        fTestSum;
  Int_t        fTrackEffTestNScinPlanes;
  Int_t        fGoodScinHits;
  Int_t*       fxLoScin;
  Int_t*       fxHiScin;
  Int_t*       fyLoScin;
  Int_t*       fyHiScin;
  Int_t        fNHodoscopes;

  Double_t fTOFCalib_shtrk_lo;
  Double_t fTOFCalib_shtrk_hi;
  Double_t fTOFCalib_cer_lo;
  Double_t fTOFCalib_beta_lo;
  Double_t fTOFCalib_beta_hi;
  Int_t        fDumpTOF;
  ofstream    fDumpOut;
  string       fTOFDumpFile;
  Bool_t      fGoodEventTOFCalib;


  Int_t fHitSweet1X;
  Int_t fHitSweet1Y;
  Int_t fHitSweet2X;
  Int_t fHitSweet2Y;

  Int_t fSweet1XScin;
  Int_t fSweet1YScin;
  Int_t fSweet2XScin;
  Int_t fSweet2YScin;

  //  Double_t**   fScinHit;                // [fNPlanes] Array

  Double_t*    fFPTime;               // [fNPlanes] Array


  Double_t* fSumPlaneTime; // [fNPlanes]

  Int_t* fNScinHits;  // [fNPlanes]
  Int_t* fNPlaneTime; // [fNPlanes]

  Bool_t* fGoodPlaneTime;  // [fNPlanes]

  //----------------------------------------------------------------

  // Useful derived quantities
  // double tan_angle, sin_angle, cos_angle;

  //  static const char NDEST = 2;
  //  struct DataDest {
  //    Int_t*    nthit;
  //    Int_t*    nahit;
  //    Double_t*  tdc;
  //    Double_t*  tdc_c;
  //    Double_t*  adc;
  //    Double_t*  adc_p;
  //    Double_t*  adc_c;
  //    Double_t*  offset;
  //    Double_t*  ped;
  //    Double_t*  gain;
  //  } fDataDest[NDEST];     // Lookup table for decoder

  // Inforamtion for each plane
  //  struct NoTrkPlaneInfo {
  //    Bool_t goodplanetime;
  //    NoTrkPlaneInfo () : goodplanetime(kFALSE) {}
  //  };
  //  std::vector<NoTrkPlaneInfo> fNoTrkPlaneInfo;

  // Inforamtion for each plane
  //  struct NoTrkHitInfo {
  //    Bool_t goodtwotimes;
  //    Bool_t goodscintime;
  //    NoTrkHitInfo () : goodtwotimes(kFALSE) {}
  //  };
  //  std::vector<NoTrkHitInfo> fNoTrkHitInfo;


  // Used in TOF calculation (FineProcess) to hold information about hits
  // within a given plane
  struct TOFPInfo {
    Bool_t onTrack;
    Bool_t keep_pos;
    Bool_t keep_neg;
    Double_t time_pos; // Times also corrected for particle
    Double_t time_neg; // flight time
    Double_t scin_pos_time; // Times corrected for position on
    Double_t scin_neg_time; // the bar
    Double_t pathp;
    Double_t pathn;
    Double_t zcor;
    Double_t scinTrnsCoord;
    Double_t scinLongCoord;
    Int_t planeIndex;
    Int_t hitNumInPlane;
    THcHodoHit *hit;
    TOFPInfo () : onTrack(kFALSE), keep_pos(kFALSE), keep_neg(kFALSE),
		  time_pos(-99.0), time_neg(-99.0), scin_pos_time(0.0),
		  scin_neg_time(0.0) {}
  };
  std::vector<TOFPInfo> fTOFPInfo;

  // Used to hold information about all hits within the hodoscope for the TOF
  struct TOFCalc {
    Int_t hit_paddle;
    Int_t pindex;		// Plane index
    Int_t good_raw_pad;
    Bool_t good_scin_time;
    Bool_t good_tdc_pos;
    Bool_t good_tdc_neg;
    Double_t scin_time;
    Double_t scin_time_fp;
    Double_t scin_sigma;
    Double_t dedx;
    TOFCalc() : good_scin_time(kFALSE), good_tdc_pos(kFALSE),
		good_tdc_neg(kFALSE) {}
  };
  std::vector<TOFCalc> fTOFCalc;
    // This doesn't work because we clear this structure each track
    // Do we need an vector of vectors of structures?
    // Start with a separate vector of vectors for now.
  std::vector<std::vector<Double_t> > fdEdX;	        // Vector over track #
  std::vector<Int_t > fNScinHit;		        // # scins hit for the track
  std::vector<std::vector<Int_t> > fScinHitPaddle;	// Vector over hits in a plane #
  std::vector<Int_t > fNClust;		                // # scins clusters for the plane
  std::vector<std::vector<Int_t> > fClustSize;		                // # scin cluster size
  std::vector<std::vector<Double_t> > fClustPos;		                // # scin cluster position
  std::vector<Int_t > fThreeScin;	                // # scins three clusters for the plane
  std::vector<Int_t > fGoodScinHitsX;                   // # hits in fid x range
  // Could combine the above into a structure

  struct GoodFlags {
    Bool_t onTrack;
    Bool_t goodScinTime;
    Bool_t goodTdcNeg;
    Bool_t goodTdcPos;
    GoodFlags() : onTrack(false), goodScinTime(false),
		  goodTdcNeg(false), goodTdcPos(false) {}
  };
  std::vector<std::vector<std::vector<GoodFlags> > > fGoodFlags;
  //

  void           DeleteArrays();
  virtual Int_t  ReadDatabase( const TDatime& date );
  virtual Int_t  DefineVariables( EMode mode = kDefine );
  enum ESide { kLeft = 0, kRight = 1 };

  virtual  Double_t TimeWalkCorrection(const Int_t& paddle,
					   const ESide side);
  void Setup(const char* name, const char* description);

  ClassDef(THcHodoscope,0)   // Hodoscope detector
};

////////////////////////////////////////////////////////////////////////////////

#endif
