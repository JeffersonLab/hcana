#include <iostream>

// HMS calorimeter hit class for calibration.

class THcShHit {

  Double_t ADCpos, ADCneg;   // pedestal subtracted ADC signals.
  Double_t Epos, Eneg;       // Energy depositions seen from pos. & neg. sides
  UInt_t BlkNumber;

 public:

  THcShHit();
  THcShHit(Double_t adc_pos, Double_t adc_neg,
	   UInt_t blk_number);
  ~THcShHit();

  void SetADCpos(Double_t sig) {ADCpos = sig;}

  void SetADCneg(Double_t sig) {ADCneg = sig;}

  void SetEpos(Double_t e) {Epos = e;}

  void SetEneg(Double_t e) {Eneg = e;}

  void SetBlkNumber(UInt_t n) {BlkNumber = n;}

  Double_t GetADCpos() {return ADCpos;}

  Double_t GetADCneg() {return ADCneg;}

  Double_t GetEpos() {return Epos;}

  Double_t GetEneg() {return Eneg;}
  
  UInt_t GetBlkNumber() {return BlkNumber;}

  void Print(ostream & ostrm);
};

//------------------------------------------------------------------------------

THcShHit::THcShHit() {
  ADCpos = -99999.;
  ADCneg = -99999.;
  Epos = -99999.;
  Eneg = -99999.;
  BlkNumber = 99999;
};

THcShHit::THcShHit(Double_t adc_pos, Double_t adc_neg,
		   UInt_t blk_number) {
  ADCpos = adc_pos;
  ADCneg = adc_neg;
  Epos = 0.;
  Eneg = 0.;
  BlkNumber = blk_number;
};

THcShHit::~THcShHit() { };

//------------------------------------------------------------------------------

void THcShHit::Print(ostream & ostrm) {

  // Output hit data through the stream ostrm.

  ostrm << ADCpos << " " << ADCneg << " " << Epos << " " << Eneg << " "
	<< BlkNumber << endl;
};

struct pmt_hit {Double_t signal; UInt_t channel;};
