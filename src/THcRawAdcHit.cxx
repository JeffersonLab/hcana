/**
\class THcRawAdcHit
\ingroup DetSupport
\brief Class representing a single raw ADC hit.

It supports rich data from flash 250 ADC modules.
*/

/**
\fn THcRawAdcHit::THcRawAdcHit()
\brief Constructor.
*/

/**
\fn THcRawAdcHit& THcRawAdcHit::operator=(const THcRawAdcHit& right)
\brief Assignment operator.
\param[in] right Raw ADC hit to be assigned.
*/

/**
\fn THcRawAdcHit::~THcRawAdcHit()
\brief Destructor.
*/

/**
\fn void THcRawAdcHit::Clear(Option_t* opt="")
\brief Clears variables before next event.
\param[in] opt Maybe used in base clas... Not sure.
*/

/**
\fn void THcRawAdcHit::SetData(Int_t data)
\brief Sets raw ADC value.
\param[in] data Raw ADC value. In channels.
\throw std::out_of_range Tried to set too many pulses.

Should be used for old style ADCs.
*/

/**
\fn void THcRawAdcHit::SetSample(Int_t data)
\brief Sets raw signal sample.
\param[in] data Raw signal sample. In channels.
\throw std::out_of_range Tried to set too many samples.
*/

/**
\fn void THcRawAdcHit::SetDataTimePedestalPeak(Int_t data, Int_t time, Int_t pedestal, Int_t peak)
\brief Sets various bits of ADC data.
\param[in] data Raw pulse integral. In channels.
\param[in] time Raw pulse time. In subsamples.
\param[in] pedestal Raw signal pedestal. In channels.
\param[in] peak Raw pulse amplitude. In channels.
\throw std::out_of_range Tried to set too many pulses.

Should be used for flash 250 modules.
*/

/**
\fn void THcRawAdcHit::SetRefTime(Int_t refTime)
\brief Sets reference time. In channels.
\param[in] refTime Reference time. In channels.
*/

/**
\fn Int_t THcRawAdcHit::GetRawData(UInt_t iPulse=0) const
\brief Gets raw pulse integral. In channels.
\param[in] iPulse Sequential number of requested pulse.
\throw std::out_of_range Tried to get nonexisting pulse.

Returns 0 if tried to access first pulse but no pulses are set.
*/

/**
\fn Double_t THcRawAdcHit::GetAverage(UInt_t iSampleLow, UInt_t iSampleHigh) const
\brief Gets average of raw samples. In channels.
\param[in] iSampleLow Sequential number of first sample to be averaged.
\param[in] iSampleHigh Sequential number of last sample to be averaged.
\throw std::out_of_range Tried to average over nonexisting sample.
*/

/**
\fn Int_t THcRawAdcHit::GetIntegral(UInt_t iSampleLow, UInt_t iSampleHigh) const
\brief Gets integral of raw samples. In channels.
\param[in] iSampleLow Sequential number of first sample to be integrated.
\param[in] iSampleHigh Sequential number of last sample to be integrated.
\throw std::out_of_range Tried to integrate over nonexisting sample.
*/

/**
\fn Double_t THcRawAdcHit::GetData(UInt_t iPedLow, UInt_t iPedHigh, UInt_t iIntLow, UInt_t iIntHigh) const
\brief Gets pedestal subtracted integral of samples. In channels.
\param[in] iPedLow Sequential number of first sample to be averaged for pedestal value.
\param[in] iPedHigh Sequential number of last sample to be averaged for pedestal value.
\param[in] iIntLow Sequential number of first sample to be integrated.
\param[in] iIntHigh Sequential number of last sample to be integrated.
*/

/**
\fn UInt_t THcRawAdcHit::GetNPulses() const
\brief Gets number of set pulses.
*/

/**
\fn UInt_t THcRawAdcHit::GetNSamples() const
\brief Gets number of set samples.
*/

/**
\fn Bool_t THcRawAdcHit::HasMulti() const
\brief Queries whether data is from flash 250 module.
*/

/**
\fn Int_t THcRawAdcHit::GetPedRaw() const
\brief Gets raw signal pedestal. In channels.

Returns 0 if no signal pedestal is set.
*/

/**
\fn Int_t THcRawAdcHit::GetPulseIntRaw(UInt_t iPulse=0) const
\brief Gets raw pulse integral. In channels.
\param[in] iPulse Sequential number of requested pulse.
\throw std::out_of_range Tried to get nonexisting pulse.
*/

/**
\fn Int_t THcRawAdcHit::GetPulseAmpRaw(UInt_t iPulse=0) const
\brief Gets raw pulse amplitude. In channels.
\param[in] iPulse Sequential number of requested pulse.
\throw std::out_of_range Tried to get nonexisting pulse.
*/

/**
\fn Int_t THcRawAdcHit::GetPulseTimeRaw(UInt_t iPulse=0) const
\brief Gets raw pulse time. In subsamples.
\param[in] iPulse Sequential number of requested pulse.
\throw std::out_of_range Tried to get nonexisting pulse.
*/

/**
\fn Int_t THcRawAdcHit::GetSampleRaw(UInt_t iSample=0) const
\brief Gets raw sample. In channels.
\param[in] iSample Sequential number of requested sample.
\throw std::out_of_range Tried to get nonexisting sample.
*/

/**
\fn Double_t THcRawAdcHit::GetPed() const
\brief Gets sample pedestal. In channels.
*/

/**
\fn Double_t THcRawAdcHit::GetPulseInt(UInt_t iPulse=0) const
\brief Gets pedestal subtracted pulse integral. In channels.
\param[in] iPulse Sequential number of requested pulse.
*/

/**
\fn Double_t THcRawAdcHit::GetPulseAmp(UInt_t iPulse=0) const
\brief Gets pedestal subtracted pulse amplitude. In channels.
\param[in] iPulse Sequential number of requested pulse.
*/

/**
\fn Int_t THcRawAdcHit::GetSampleIntRaw() const
\brief Gets raw integral of sTimeFacamples. In channels.
*/

/**
\fn Double_t THcRawAdcHit::GetSampleInt() const
\brief Gets pedestal subtracted integral of samples. In channels.
*/

/**
\fn void THcRawAdcHit::SetF250Params(Int_t NSA, Int_t NSB, Int_t NPED)
\brief Sets F250 parameters used for pedestal subtraction.
\param [in] NSA NSA parameter of F250 modules.
\param [in] NSB NSB parameter of F250 modules.
\param [in] NPED NPED parameter of F250 modules.
*/

// TODO: Disallow using both SetData and SetDataTimePedestalPeak.


#include "THcRawAdcHit.h"
#include "TString.h"
#include <iostream>

using namespace std;

THcRawAdcHit::THcRawAdcHit()
  : fNPedestalSamples(4)
  , fNPeakSamples(9)
  , fPeakPedestalRatio(1.0*fNPeakSamples/fNPedestalSamples)
  , fSubsampleToTimeFactor(0.0625)
  , fSampThreshold(10.)
  , fNSA(-1)
  , fNSAT(-1)
  , fNSB(-1)
  , fPed(0)
  , fSampPed(0)
  , fPulseInt()
  , fPulseAmp()
  , fPulseTime()
  , fSampPulseInt()
  , fSampPulseAmp()
  , fSampPulseTime()
  , fSample()
  , fRefTime(0)
  , fRefDiffTime(0)
  , fHasMulti(kFALSE)
  , fHasRefTime(kFALSE)
  , fNPulses(0)
  , fNSampPulses(0)
  , fNSamples(0)
{}

THcRawAdcHit& THcRawAdcHit::operator=(const THcRawAdcHit& right) {
  TObject::operator=(right);

  if (this != &right) {
    fPed = right.fPed;
    fSampPed = right.fSampPed;
    for (UInt_t i=0; i<fMaxNPulses; ++i) {
      fPulseInt[i]  = right.fPulseInt[i];
      fPulseAmp[i]  = right.fPulseAmp[i];
      fPulseTime[i] = right.fPulseTime[i];
      fSampPulseInt[i]  = right.fSampPulseInt[i];
      fSampPulseAmp[i]  = right.fSampPulseAmp[i];
      fSampPulseTime[i] = right.fSampPulseTime[i];
   }
    for (UInt_t i=0; i<fMaxNSamples; ++i) {
      fSample[i] = right.fSample[i];
    }
    fHasMulti = right.fHasMulti;
    fNPulses  = right.fNPulses;
    fNSampPulses  = right.fNSampPulses;
    fNSamples = right.fNSamples;
    fRefTime = right.fRefTime;
    fHasRefTime = right.fHasRefTime;
  }

  return *this;
}

THcRawAdcHit::~THcRawAdcHit() = default;

void THcRawAdcHit::Clear(Option_t* opt) {
  TObject::Clear(opt);

  fPed = 0;
  for (UInt_t i=0; i<fNPulses; ++i) {
    fPulseInt[i] = 0;
    fPulseAmp[i] = 0;
    fPulseTime[i] = 0;
    fSampPulseInt[i] = 0;
    fSampPulseAmp[i] = 0;
    fSampPulseTime[i] = 0;
  }
  for (UInt_t i=0; i<fNSamples; ++i) {
    fSample[i] = 0 ;
  }
  fHasMulti = kFALSE;
  fNPulses = 0;
  fNSampPulses = 0;
  fNSamples = 0;
  fRefTime = 0;
  fHasRefTime = kFALSE;
}

void THcRawAdcHit::SetData(Int_t data) {
  if (fNPulses >= fMaxNPulses) {
    throw std::out_of_range(
      "`THcRawAdcHit::SetData`: too many pulses!"
    );
  }
  fPulseInt[fNPulses] = data;
  ++fNPulses;
}

void THcRawAdcHit::SetRefTime(Int_t refTime) {
  fRefTime = refTime;
  fHasRefTime = kTRUE;
}

void THcRawAdcHit::SetRefDiffTime(Int_t refDiffTime) {
  fRefDiffTime = refDiffTime;
}

void THcRawAdcHit::SetSampThreshold(Double_t thres) {
  fSampThreshold = thres;
}


void THcRawAdcHit::SetSampNSAT(Int_t nsat) {
  fNSAT = nsat;
}


void THcRawAdcHit::SetSample(Int_t data) {
  if (fNSamples >= fMaxNSamples) {
    throw std::out_of_range(
      "`THcRawAdcHit::SetSample`: too many samples!"
    );
  }
  fSample[fNSamples] = data;
  ++fNSamples;
}


void THcRawAdcHit::SetSampIntTimePedestalPeak() {
  /*
  if ( fNPulses ==0) {
 std::cout << " in SetSampIntTimePedestalPeak " << fNPedestalSamples << " " << fNSamples<< std::endl;
  if (fNPulses>0) {
    for (UInt_t np=0;np<fNPulses;np++) {
      std::cout << " Npulse = " << np+1 << " " << fPulseInt[np] << " " << GetPulseInt(np) <<  " " << fPulseAmp[np] << " " << GetPulseAmp(np)  << " " << fPulseTime[np] << " " << GetPulseTime(np)<< std::endl;
    }
  } else {
    std::cout << " No pulses found " << std::endl;
  }
  }
  */
  fHasMulti = kTRUE;
  fSampPed = GetIntegral(0, fNPedestalSamples - 1);
  //
  Int_t NS = fNPedestalSamples - 1;
  fNSampPulses = 0;
  if( fSampThreshold == 0 ) { // just want to get the pedestal for determining FADC threshold
    fSampPulseInt[fNSampPulses] = GetIntegral(TMath::Max(NS - fNSB, 0),
                                              TMath::Min((NS + fNSA - 1), int(fNSamples - 1)));
    fNPeakSamples = TMath::Min((NS + fNSA - 1), int(fNSamples - 1)) - TMath::Max(NS - fNSB, 0) + 1;
    fPeakPedestalRatio = 1.0 * fNPeakSamples / fNPedestalSamples;
    fSampPulseAmp[fNSampPulses] = GetSampleRaw(NS);
    fSampPulseTime[fNSampPulses] = 64 * NS;
    fNSampPulses = 1;

  } else {
    //
    Bool_t CheckSampBelowThres = kFALSE;
     Int_t LastFourSampPed = GetIntegral(fNSamples - fNPedestalSamples, fNSamples - 1);
    if ( LastFourSampPed < fSampPed) {
           fSampPed= LastFourSampPed;
           CheckSampBelowThres = kTRUE;
    }
    while( NS < int(fNSamples) && fNSampPulses < fMaxNPulses ) {
      // GetSample(NS) Pedestal subtracted sample value (mV)
      if( CheckSampBelowThres ) { // find sample below threshold to start search
        if( GetSample(NS) < fSampThreshold ) CheckSampBelowThres = kFALSE;
      } else {
        Int_t ns_found = 0;
        for( Int_t nt = NS; nt < TMath::Min((NS + fNSAT), int(fNSamples)); nt++ ) {
          if( GetSample(nt) > fSampThreshold ) ns_found++;
        }
        if( ns_found == fNSAT ) { // NS is The TC bin
          fSampPulseInt[fNSampPulses] = GetIntegral(TMath::Max(NS - fNSB, 0),
                                                    TMath::Min((NS + fNSA - 1), int(fNSamples - 1)));
          fNPeakSamples = TMath::Min((NS + fNSA - 1), int(fNSamples - 1)) - TMath::Max(NS - fNSB, 0) + 1;
          fPeakPedestalRatio = 1.0 * fNPeakSamples / fNPedestalSamples;
          fSampPulseAmp[fNSampPulses] = 0;
          fSampPulseTime[fNSampPulses] = 0;
          Int_t PeakBin = 0;
          Double_t PeakVal = GetSample(NS);
          for( Int_t nt = NS + 1; nt < TMath::Min((NS + fNSA), int(fNSamples)); nt++ ) {
            if( GetSample(nt) < PeakVal && PeakBin == 0 ) {
              PeakBin = nt - 1;
            } else if( PeakBin == 0 ) {
              PeakVal = GetSample(nt);
            }
          }
          if( PeakBin > 0 ) {
            fSampPulseAmp[fNSampPulses] = GetSampleRaw(PeakBin);
            Int_t Time = NS * 64; // Raw time in ADC bin number * 64 = "subsamples"
            Double_t VMid = (GetSample(PeakBin)) / 2.;
            for( Int_t nt = TMath::Max(NS - fNSB, 0); nt < TMath::Min(PeakBin, int(fNSamples - 1)); nt++ ) {
              if( VMid >= GetSample(nt) && VMid < GetSample(nt + 1) ) {
                Time = 64 * nt + int(64 * (VMid - GetSample(nt)) /
                                     (GetSample(nt + 1) - GetSample(nt)));
              }
            }
            fSampPulseTime[fNSampPulses] = Time;
          } else {
            fSampPulseAmp[fNSampPulses] = GetSampleRaw(NS);
            fSampPulseTime[fNSampPulses] = 64 * NS;
          }
          //	  if (fNPulses ==0) 	  std::cout << " NsampPulse = " << fNSampPulses+1 << " " << fSampPulseInt[fNSampPulses] << " " << GetSampPulseInt(fNSampPulses) << " npeak = " <<PeakBin << " " << fSampPulseAmp[fNSampPulses] << " " << GetSampPulseAmp(fNSampPulses) << " " << fSampPulseTime[fNSampPulses]<< " " << GetSampPulseTime(fNSampPulses) << " Vmid = " << VMid  << " TC = " << NS << " TC+NSA-1 ="  << NS+fNSA << std::endl;
          fNSampPulses++;
          NS = NS + fNSA;
	  CheckSampBelowThres = kTRUE;
        }
      }
      NS++;
    }
  } // match else
  /*
  if ( fNSampPulses==0 && fNPulses==0) {
     std::cout << " No sample pulses found" << std::endl;
     for (Int_t nt=0;nt<fNSamples;nt++) {
       cout << GetSample(nt) << " " ;
       if (nt%10 ==0 && nt !=0) cout << endl;
        }
     cout << endl;
  }
  */
}
//
void THcRawAdcHit::SetDataTimePedestalPeak( Int_t data, Int_t time,
                                            Int_t pedestal, Int_t peak ) {
  if( fNPulses >= fMaxNPulses ) {
    throw std::out_of_range(
      "`THcRawAdcHit::SetDataTimePedestalPeak`: too many pulses!"
    );
  }
  fPulseInt[fNPulses] = data;
  fPulseTime[fNPulses] = time;
  fPed = pedestal;
  fPulseAmp[fNPulses] = peak;
  fHasMulti = kTRUE;
  ++fNPulses;
}

void THcRawAdcHit::SetF250Params( Int_t NSA, Int_t NSB, Int_t NPED ) {
  if( NSA < 0 || NSB < 0 || NPED < 0 ) {
    TString msg = TString::Format(
      "`THcRawAdcHit::SetF250Params`: One of the params is negative!  NSA = %d  NSB = %d  NPED = %d",
      NSA, NSB, NPED
    );
    throw std::invalid_argument(msg.Data());
  }
  fNPedestalSamples = NPED;
  fNPeakSamples = NSA + NSB;
  fPeakPedestalRatio = 1.0 * fNPeakSamples / fNPedestalSamples;
  fNSA = NSA;
  fNSB = NSB;
  fNSAT = 2;
}

void THcRawAdcHit::throw_bad_index( const char* loc, UInt_t i, UInt_t N )
{
  throw std::out_of_range(Form("%s: Bad index %u %u", loc, i, N));
}

void THcRawAdcHit::throw_bad_range( const char* loc, UInt_t lo, UInt_t hi,
                                    UInt_t N )
{
  throw std::out_of_range(Form("%s: bad range %u %u %u", loc, lo, hi, N));

}

ClassImp(THcRawAdcHit)
