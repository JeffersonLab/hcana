#ifndef ROOT_THcReactionPoint
#define ROOT_THcReactionPoint

//////////////////////////////////////////////////////////////////////////
//
// THcReactionPoint
//
//////////////////////////////////////////////////////////////////////////

#include "THaReactionPoint.h"

class THcReactionPoint : public THaReactionPoint {
  
public:
  THcReactionPoint( const char* name, const char* description,
		    const char* spectro="", const char* beam="" );
  virtual ~THcReactionPoint();
  
  virtual Int_t     Process( const THaEvData& );

protected:

  ClassDef(THcReactionPoint,0)   //Single arm track-beam vertex module
};

#endif
