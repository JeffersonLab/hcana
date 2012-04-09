#ifndef ROOT_THcDetector
#define ROOT_THcDetector

//////////////////////////////////////////////////////////////////////////
//
// THcDetector
//
// Abstract base class for a generic Hall C detector. This class 
// describes an actual detector (not subdetector) and can be added to
// an apparatus.
//
//////////////////////////////////////////////////////////////////////////

#include "THcDetectorBase.h"
#include <TRef.h>
#include "THaApparatus.h"

//class THaApparatus;

class THcDetector : public THcDetectorBase {
  
public:
  virtual ~THcDetector();
  THaApparatus*  GetApparatus() const   {
    return static_cast<THaApparatus*>(fApparatus.GetObject());
  }
  
  virtual void   SetApparatus( THaApparatus* );

  THcDetector();  // for ROOT I/O only

protected:

  virtual void MakePrefix();

  //Only derived classes may construct me
  THcDetector( const char* name, const char* description, 
	       THaApparatus* apparatus = NULL );  

private:
  TRef  fApparatus;         // Apparatus containing this detector

  ClassDef(THcDetector,0)   //Abstract base class for a Hall C detector
};

#endif
