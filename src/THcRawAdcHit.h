#ifndef ROOT_THcRawAdcHit
#define ROOT_THcRawAdcHit

#include "TObject.h"
#include <stdexcept>

class THcRawAdcHit : public TObject {
public:
  THcRawAdcHit();
  THcRawAdcHit& operator=( const THcRawAdcHit& right );
  virtual ~THcRawAdcHit();

  virtual void Clear( Option_t* opt = "" );

  void SetData( Int_t data );
  void SetSample( Int_t data );
  void SetSampIntTimePedestalPeak();
  void SetRefTime( Int_t refTime );
  void SetSampThreshold( Double_t thres );
  void SetSampNSAT( Int_t nsat );
  void SetRefDiffTime( Int_t refDiffTime );
  void SetDataTimePedestalPeak( Int_t data, Int_t time, Int_t pedestal,
                                Int_t peak );

  Int_t GetRawData( UInt_t iPulse = 0 ) const;
  Double_t GetF250_PeakPedestalRatio() const { return fPeakPedestalRatio; }
  Int_t GetF250_NPedestalSamples() const { return fNPedestalSamples; }
  Int_t GetF250_NSA() const { return fNSA; }
  Int_t GetF250_NSB() const { return fNSB; }

  Double_t GetAverage( UInt_t iSampleLow, UInt_t iSampleHigh ) const;
  Int_t GetIntegral( UInt_t iSampleLow, UInt_t iSampleHigh ) const;
  Double_t GetData( UInt_t iPedLow, UInt_t iPedHigh,
                    UInt_t iIntLow, UInt_t iIntHigh ) const;

  UInt_t GetNPulses() const;
  UInt_t GetNSampPulses() const;
  UInt_t GetNSamples() const;

  Bool_t HasMulti() const;
  Bool_t HasRefTime() const;

  Int_t GetPedRaw() const;
  Int_t GetSampPedRaw() const;
  Int_t GetPulseIntRaw( UInt_t iPulse = 0 ) const;
  Int_t GetPulseAmpRaw( UInt_t iPulse = 0 ) const;
  Int_t GetPulseTimeRaw( UInt_t iPulse = 0 ) const;
  Int_t GetSampleRaw( UInt_t iSample = 0 ) const;
  Int_t GetSampPulseIntRaw( UInt_t iPulse = 0 ) const;
  Int_t GetSampPulseAmpRaw( UInt_t iPulse = 0 ) const;
  Int_t GetSampPulseTimeRaw( UInt_t iPulse = 0 ) const;
  Int_t GetRefTime() const;
  Int_t GetRefDiffTime() const;

  Double_t GetPed() const;
  Double_t GetSampPed() const;
  Double_t GetSample( UInt_t iSample = 0 ) const;
  Double_t GetPulseInt( UInt_t iPulse = 0 ) const;
  Double_t GetPulseAmp( UInt_t iPulse = 0 ) const;
  Double_t GetPulseTime( UInt_t iPulse = 0 ) const;
  Double_t GetSampPulseInt( UInt_t iPulse = 0 ) const;
  Double_t GetSampPulseAmp( UInt_t iPulse = 0 ) const;
  Double_t GetSampPulseTime( UInt_t iPulse = 0 ) const;

  Int_t GetSampleIntRaw() const;
  Double_t GetSampleInt() const;
  constexpr static Double_t GetAdcTomV();
  constexpr static Double_t GetAdcTopC();
  constexpr static Double_t GetAdcTons();

  void SetF250Params( Int_t NSA, Int_t NSB, Int_t NPED );

protected:
  static constexpr UInt_t fMaxNPulses = 4;
  static constexpr UInt_t fMaxNSamples = 1024;

  // FADC conversion factors
  static constexpr Double_t fNAdcChan      = 4096.0; // Number of FADC channels in units of ADC channels
  static constexpr Double_t fAdcRange      = 1.0;    // Dynamic range of FADCs in units of V, // TO-DO: Get fAdcRange from pre-start event
  static constexpr Double_t fAdcImpedence  = 50.0;   // FADC input impedence in units of Ohms
  static constexpr Double_t fAdcTimeSample = 4000.0; // Length of FADC time sample in units of ps
  static constexpr Double_t fAdcTimeRes    = 0.0625; // FADC time resolution in units of ns

  Int_t fNPedestalSamples;  // TODO: Get this from prestart event...
  Int_t fNPeakSamples;
  Double_t fPeakPedestalRatio;
  Double_t fSubsampleToTimeFactor;
  Double_t fSampThreshold;

  Int_t fNSA;
  Int_t fNSAT;
  Int_t fNSB;

  Int_t fPed;
  Int_t fSampPed;
  Int_t fPulseInt[fMaxNPulses];
  Int_t fPulseAmp[fMaxNPulses];
  Int_t fPulseTime[fMaxNPulses];
  Int_t fSampPulseInt[fMaxNPulses];
  Int_t fSampPulseAmp[fMaxNPulses];
  Int_t fSampPulseTime[fMaxNPulses];
  Int_t fSample[fMaxNSamples];
  Int_t fRefTime;
  Int_t fRefDiffTime;

  Bool_t fHasMulti;
  Bool_t fHasRefTime;
  UInt_t fNPulses;
  UInt_t fNSampPulses;
  UInt_t fNSamples;

  static void throw_bad_index( const char* loc, UInt_t i, UInt_t N );
  static void throw_bad_range( const char* loc, UInt_t lo, UInt_t hi, UInt_t N );

  ClassDef(THcRawAdcHit, 0)
};

// FADC conversion factors
// Convert pedestal and amplitude to mV
constexpr Double_t THcRawAdcHit::GetAdcTomV() {
  // 1000 mV / 4096 ADC channels
  return (fAdcRange * 1000. / fNAdcChan);
}

// Convert integral to pC
constexpr Double_t THcRawAdcHit::GetAdcTopC() {
  // (1 V / 4096 adc channels) * (4000 ps time sample / 50 ohms input resistance) = 0.020 pc/channel
  return (fAdcRange / fNAdcChan) * (fAdcTimeSample / fAdcImpedence);
}

// Convert time sub samples to ns
constexpr Double_t THcRawAdcHit::GetAdcTons() {
  return fAdcTimeRes;
}

inline
UInt_t THcRawAdcHit::GetNPulses() const {
  return fNPulses;
}

inline
UInt_t THcRawAdcHit::GetNSampPulses() const {
  return fNSampPulses;
}

inline
UInt_t THcRawAdcHit::GetNSamples() const {
  return fNSamples;
}

inline
Bool_t THcRawAdcHit::HasMulti() const {
  return fHasMulti;
}

inline
Int_t THcRawAdcHit::GetPedRaw() const {
  return fPed;
}

inline
Int_t THcRawAdcHit::GetSampPedRaw() const {
  return fSampPed;
}

inline
Double_t THcRawAdcHit::GetPed() const {
  return (static_cast<Double_t>(fPed) / static_cast<Double_t>(fNPedestalSamples)) * GetAdcTomV();
}

inline
Double_t THcRawAdcHit::GetSampPed() const {
  return (static_cast<Double_t>(fSampPed) / static_cast<Double_t>(fNPedestalSamples)) * GetAdcTomV();
}

inline
Double_t THcRawAdcHit::GetPulseInt( UInt_t iPulse ) const {
  return (static_cast<Double_t>(fPulseInt[iPulse]) - static_cast<Double_t>(fPed) * fPeakPedestalRatio) * GetAdcTopC();
}

inline
Double_t THcRawAdcHit::GetSampPulseInt( UInt_t iPulse ) const {
  return (static_cast<Double_t>(fSampPulseInt[iPulse]) - static_cast<Double_t>(fSampPed) * fPeakPedestalRatio) *
         GetAdcTopC();
}

inline
Double_t THcRawAdcHit::GetPulseAmp( UInt_t iPulse ) const {
  return (static_cast<Double_t>(fPulseAmp[iPulse]) -
          static_cast<Double_t>(fPed) / static_cast<Double_t>(fNPedestalSamples)) * GetAdcTomV();
}

inline
Double_t THcRawAdcHit::GetSampPulseAmp( UInt_t iPulse ) const {
  return (static_cast<Double_t>(fSampPulseAmp[iPulse]) -
          static_cast<Double_t>(fSampPed) / static_cast<Double_t>(fNPedestalSamples)) * GetAdcTomV();
}

inline
Double_t THcRawAdcHit::GetPulseTime( UInt_t iPulse ) const {
  Int_t rawtime = fPulseTime[iPulse];
  if( fHasRefTime ) {
    rawtime -= fRefTime;
  }
  return (static_cast<Double_t>(rawtime) * GetAdcTons());
}

inline
Double_t THcRawAdcHit::GetSampPulseTime( UInt_t iPulse ) const {
  Int_t rawtime = fSampPulseTime[iPulse];
  if( fHasRefTime ) {
    rawtime -= fRefTime;
  }
  return (static_cast<Double_t>(rawtime) * GetAdcTons());
}

inline
Int_t THcRawAdcHit::GetSampPulseTimeRaw( UInt_t iPulse ) const {
  return fSampPulseTime[iPulse];
}

inline
Int_t THcRawAdcHit::GetSampPulseIntRaw( UInt_t iPulse ) const {
  return fSampPulseInt[iPulse];
}

inline
Int_t THcRawAdcHit::GetSampPulseAmpRaw( UInt_t iPulse ) const {
  return fSampPulseAmp[iPulse];
}


inline
Int_t THcRawAdcHit::GetSampleIntRaw() const {
  Int_t integral = 0;

  for( UInt_t iSample = 0; iSample < fNSamples; ++iSample ) {
    integral += fSample[iSample];
  }

  return integral;
}

inline
Double_t THcRawAdcHit::GetSampleInt() const {
  return static_cast<Double_t>(GetSampleIntRaw()) - GetPed() * static_cast<Double_t>(fNSamples);
}

inline
Int_t THcRawAdcHit::GetRawData( UInt_t iPulse ) const {
  if( fNPulses == 0 && iPulse == 0 )
    return 0;
  if( iPulse >= fNPulses )
    throw_bad_index(__FUNCTION__ , iPulse, fNPulses);

  return fPulseInt[iPulse];
}

inline
Double_t THcRawAdcHit::GetAverage( UInt_t iSampleLow, UInt_t iSampleHigh ) const {
  if( iSampleHigh >= fNSamples ||  iSampleLow > iSampleHigh )
    throw_bad_range(__FUNCTION__, iSampleLow, iSampleHigh, fNSamples);

  Double_t average = 0.0;
  for( UInt_t i = iSampleLow; i <= iSampleHigh; ++i ) {
    average += fSample[i];
  }
  return average / (iSampleHigh - iSampleLow + 1);
}


inline
Int_t THcRawAdcHit::GetIntegral( UInt_t iSampleLow, UInt_t iSampleHigh ) const {
  if( iSampleHigh >= fNSamples || iSampleLow > iSampleHigh )
    throw_bad_range(__FUNCTION__, iSampleLow, iSampleHigh, fNSamples);

  Int_t integral = 0;
  for( UInt_t i = iSampleLow; i <= iSampleHigh; ++i ) {
    integral += fSample[i];
  }
  return integral;
}

inline
Double_t THcRawAdcHit::GetData(
  UInt_t iPedLow, UInt_t iPedHigh, UInt_t iIntLow, UInt_t iIntHigh) const {
  return
    GetIntegral(iIntLow, iIntHigh)
    - GetAverage(iPedHigh, iPedLow) * (iIntHigh - iIntLow + 1);
}

inline
Int_t THcRawAdcHit::GetPulseIntRaw( UInt_t iPulse ) const {
  if( fNPulses == 0 && iPulse == 0 )
    return 0;
  if( iPulse >= fNPulses )
    throw_bad_index(__FUNCTION__ , iPulse, fNPulses);

  return fPulseInt[iPulse];
}

inline
Int_t THcRawAdcHit::GetPulseAmpRaw( UInt_t iPulse ) const {
  if( fNPulses == 0 && iPulse == 0 )
    return 0;
  if( iPulse >= fNPulses )
    throw_bad_index(__FUNCTION__ , iPulse, fNPulses);

  return fPulseAmp[iPulse];
}

inline
Int_t THcRawAdcHit::GetPulseTimeRaw( UInt_t iPulse ) const {
  if( fNPulses == 0 && iPulse == 0 )
    return 0;
  if( iPulse >= fNPulses )
    throw_bad_index(__FUNCTION__ , iPulse, fNPulses);

  return fPulseTime[iPulse];
}

inline
Int_t THcRawAdcHit::GetSampleRaw( UInt_t iSample ) const {
  if( iSample >= fNSamples )
    throw_bad_index(__FUNCTION__ , iSample, fNSamples);

  return fSample[iSample];
}

inline
Double_t THcRawAdcHit::GetSample( UInt_t iSample ) const {
  if( iSample >= fNSamples )
    throw_bad_index(__FUNCTION__ , iSample, fNSamples);

  return fSample[iSample] * GetAdcTomV() - GetSampPed();
}

inline
Int_t THcRawAdcHit::GetRefTime() const {
  if( !fHasRefTime )
    throw std::runtime_error("THcRawAdcHit::GetRefTime: Reference time not available!");

  return fRefTime;
}

inline
Int_t THcRawAdcHit::GetRefDiffTime() const {
  if( !fHasRefTime )
    throw std::runtime_error("THcRawAdcHit::GetRefDiffTime: Reference time not available!");

  return fRefDiffTime;
}

inline
Bool_t THcRawAdcHit::HasRefTime() const {
  return fHasRefTime;
}


#endif  // ROOT_THcRawAdcHit
