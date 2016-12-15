#ifndef ROOT_THcDCWire
#define ROOT_THcDCWire

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcDCWire                                                                //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
#include "TObject.h"

class THcDCTimeToDistConv;

class THcDCWire : public TObject {

public:

  THcDCWire( Int_t num=0, Double_t pos=0.0, Double_t offset=0.0,
	     THcDCTimeToDistConv* ttd=NULL ) :
    fNum(num), fFlag(0), fPos(pos), fTOffset(offset), fTTDConv(ttd) {}
  virtual ~THcDCWire() {}

  // Get and Set Functions
  Int_t    GetNum()     const { return fNum;  }
  Int_t    GetFlag()    const { return fFlag; }
  Double_t GetPos()     const { return fPos; }
  Double_t GetTOffset() const { return fTOffset; }
  THcDCTimeToDistConv * GetTTDConv() { return fTTDConv; }

  void SetNum  (Int_t num)  {fNum = num;}
  void SetFlag (Int_t flag) {fFlag = flag;}
  void SetPos  (Double_t pos)       { fPos = pos; }
  void SetTOffset (Double_t tOffset){ fTOffset = tOffset; }
  void SetTTDConv (THcDCTimeToDistConv * ttdConv){ fTTDConv = ttdConv;}

protected:
  Int_t    fNum;                       //Wire Number
  Int_t    fFlag;                      //Flag for errors (e.g. Bad wire)
  Double_t fPos;                       //Position within the plane
  Double_t fTOffset;                      //Timing Offset
  THcDCTimeToDistConv* fTTDConv;     //!Time to Distance Converter

private:
  THcDCWire( const THcDCWire& );
  THcDCWire& operator=( const THcDCWire& );

  ClassDef(THcDCWire,0)             // Drift Chamber Wire class
};

////////////////////////////////////////////////////////////////////////////////

#endif
