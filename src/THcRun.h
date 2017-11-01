#ifndef ROOT_THcRun
#define ROOT_THcRun

//////////////////////////////////////////////////////////////////////////
//
// THaRun
//
//////////////////////////////////////////////////////////////////////////

#include "THaRun.h"
#include "THcParmList.h"

class THcRun : public THaRun {

 public:
  THcRun( const char* filename="", const char* description="" );
  //  THaRun( const THaRun& run );
  //  THaRun( const std::vector<TString>& pathList, const char* filename,
  //	  const char* description="" );
  //  virtual THaRun& operator=( const THaRunBase& rhs );
  virtual ~THcRun();
  virtual void         Print( Option_t* opt="" ) const;
  THcParmList* GetHCParms() const { return fHcParms; }

 private:
  THcParmList* fHcParms;	/* gHcParms object */
  
  ClassDef(THcRun,0);
};
#endif
