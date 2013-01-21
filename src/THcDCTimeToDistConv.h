#ifndef ROOT_THcDCTimeToDistConv
#define ROOT_THcDCTimeToDistConv

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcDCTimeToDistConv                                                      //
//                                                                           //
// Base class for algorithms for converting TDC time into perpendicular      //
// drift distance                                                            //
///////////////////////////////////////////////////////////////////////////////

#include "TObject.h"

class THcDCTimeToDistConv : public TObject {

public:
  THcDCTimeToDistConv() {}
  virtual ~THcDCTimeToDistConv();

  virtual Double_t ConvertTimeToDist(Double_t time, Double_t tanTheta,
				     Double_t *ddist=0) = 0;
private:

  THcDCTimeToDistConv( const THcDCTimeToDistConv& );
  THcDCTimeToDistConv& operator=( const THcDCTimeToDistConv& );

  ClassDef(THcDCTimeToDistConv,0)             // DCTimeToDistConv class
};

////////////////////////////////////////////////////////////////////////////////

#endif
