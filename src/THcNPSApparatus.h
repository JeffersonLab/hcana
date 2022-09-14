#ifndef ROOT_THcNPSApparatus
#define ROOT_THcNPSApparatus

#include "TString.h"

#include "THaAnalysisObject.h"
#include "THaApparatus.h"
#include "TClonesArray.h"

class TDatime;


class THcNPSApparatus : public THaApparatus {
public:
  THcNPSApparatus(const char* name, const char* description);
  virtual ~THcNPSApparatus();

  virtual Int_t CoarseReconstruct();
  virtual Int_t Reconstruct();
  Double_t GetNPSAngle() {return fTheta_lab;};

  TString GetKwPrefix();

  enum EStagesDone {
    kCoarseTrack = BIT(0),
    kCoarseRecon = BIT(1),
    kTracking    = BIT(2),
    kReconstruct = BIT(3)
  };

protected:
  TClonesArray* fTracks;
  
  virtual Int_t ReadRunDatabase(const TDatime& date);
  virtual Int_t ReadDatabase(const TDatime& date);
  virtual Int_t DefineVariables(EMode mode=kDefine);

  TList*          fNonTrackingDetectors;  //Non-tracking detectors

  Double_t fTheta_lab;		// Central NPS angle
  
  std::string fKwPrefix;

  UInt_t          fStagesDone;            //Bitfield of completed analysis stages

private:
  Bool_t          fListInit;      //Detector lists initialized

  void            ListInit();     //Initializes lists of specialized detectors

  THcNPSApparatus();
  ClassDef(THcNPSApparatus, 0);
};


#endif  // ROOT_THcNPSApparatus
