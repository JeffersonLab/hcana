#include <iostream>

// SHMS calorimeter hit class for calibration.

class THcPShHit {

  Double_t ADC;        // pedestal subtracted ADC signal.
  Double_t Edep;       // Energy deposition.
  UInt_t BlkNumber;    // Block number.

 public:

  THcPShHit();
  THcPShHit(Double_t adc, UInt_t blk_number);
  ~THcPShHit();

  void SetADC(Double_t sig) {ADC = sig;}

  void SetEdep(Double_t e) {Edep = e;}

  void SetBlkNumber(UInt_t n) {BlkNumber = n;}

  Double_t GetADC() {return ADC;}

  Double_t GetEdep() {return Edep;}

  UInt_t GetBlkNumber() {return BlkNumber;}

  void Print(ostream & ostrm);
};

//------------------------------------------------------------------------------

THcPShHit::THcPShHit() {
  ADC = -99999.;
  Edep = -99999.;
  BlkNumber = 99999;
};

THcPShHit::THcPShHit(Double_t adc, UInt_t blk_number) {
  ADC = adc;
  Edep = 0.;
  BlkNumber = blk_number;
};

THcPShHit::~THcPShHit() { };

//------------------------------------------------------------------------------

void THcPShHit::Print(ostream & ostrm) {

  // Output hit data through the stream ostrm.

  ostrm << ADC << " " << Edep << " " << BlkNumber << endl;
};

//------------------------------------------------------------------------------

struct pmt_hit {Double_t signal; UInt_t channel;};
