#ifndef ROOT_THcRawHodoHit
#define ROOT_THcRawHodoHit

#include "THcRawHit.h"

class THcRawHodoHit : public THcRawHit {

 public:
  friend class THcScintillatorPlane;
  friend class THcHodoscope;
  friend class THcHodoHit;

  THcRawHodoHit(Int_t plane=0, Int_t counter=0) : THcRawHit(plane, counter) {
    for(Int_t i=0;i<4;i++) {
      fHasRef[i] = kFALSE;
      fHasMulti[i] = kFALSE;
      fNRawHits[i] = 0;
    }
    fNRawSamplesPos = 0;
    fNRawSamplesNeg = 0;
  }
  THcRawHodoHit& operator=( const THcRawHodoHit& );
  virtual ~THcRawHodoHit() {}

  virtual void Clear( Option_t* opt="" ) {
    for(Int_t i=0;i<4;i++) {
      fHasRef[i] = kFALSE;
      fHasMulti[i] = kFALSE;
      fNRawHits[i] = 0;
    }
    fNRawSamplesPos = 0;
    fNRawSamplesNeg = 0;
  }
  void SetData(Int_t signal, Int_t data);
  void SetDataTimePedestalPeak(Int_t signal, Int_t data, Int_t time,
			       Int_t pedestal, Int_t peak);
  void SetSample(Int_t signal, Int_t data);
  void SetReference(Int_t signal, Int_t reference);
  Int_t GetData(Int_t signal);
  Int_t GetData(Int_t signal, UInt_t ihit);
  Int_t GetRawData(Int_t signal);
  Int_t GetRawData(Int_t signal, UInt_t ihit);
  Int_t GetReference(Int_t signal);
  Bool_t HasReference(Int_t signal);
  Bool_t HasMulti(Int_t signal);

  //  virtual Bool_t  IsSortable () const {return kTRUE; }
  //  virtual Int_t   Compare(const TObject* obj) const;
  UInt_t GetMaxNSamplesADC() {return fMaxNSamplesADC;}
  Int_t GetADCPos() {return GetData(0, 0);}
  Int_t GetADCNeg() {return GetData(1, 0);}
  Int_t GetTDCPos() {return GetData(2, 0);}
  Int_t GetTDCNeg() {return GetData(3, 0);}

  UInt_t GetMaxNSamplesTDC() {return fMaxNHitsTDC;}
  Int_t GetTDCPos(UInt_t ihit) {return GetData(2, ihit);}
  Int_t GetTDCNeg(UInt_t ihit) {return GetData(3, ihit);}

  Int_t GetIntegralPos();
  Int_t GetIntegralNeg();
  Int_t GetPedestalPos();
  Int_t GetPedestalNeg();

  Int_t GetNSignals() { return 4;}
  ESignalType GetSignalType(Int_t signal) {
    if(signal==0||signal==1) {return kADC;}
    else {return kTDC;}
  }

 protected:
  static const UInt_t fMaxNSamplesADC = 511;
  static const UInt_t fMaxNPulsesADC = 4;
  static const UInt_t fMaxNHitsTDC = 16;
  Int_t fADC_pos[fMaxNPulsesADC];
  Int_t fADC_neg[fMaxNPulsesADC];
  Int_t fTDC_pos[fMaxNHitsTDC];
  Int_t fTDC_neg[fMaxNHitsTDC];
  Int_t fADC_Time_pos[fMaxNPulsesADC];
  Int_t fADC_Time_neg[fMaxNPulsesADC];
  Int_t fADC_Pedestal_pos[fMaxNPulsesADC];
  Int_t fADC_Pedestal_neg[fMaxNPulsesADC];
  Int_t fADC_Peak_pos[fMaxNPulsesADC];
  Int_t fADC_Peak_neg[fMaxNPulsesADC];
  Int_t fADC_Samples_pos[fMaxNSamplesADC];
  Int_t fADC_Samples_neg[fMaxNSamplesADC];
  Int_t fReferenceTime[4];
  Bool_t fHasRef[4];
  Bool_t fHasMulti[4];
  UInt_t fNRawHits[4];
  UInt_t fNRawSamplesPos;
  UInt_t fNRawSamplesNeg;

 private:

  ClassDef(THcRawHodoHit, 0);	// Raw Hodoscope hit
};

#endif
