#ifndef ROOT_THcGlobals
#define ROOT_THcGlobals

// Pick up definition of R__EXTERN
#ifndef ROOT_DllImport
#include "DllImport.h"
#endif

// Global Analyzer variables. Defined in THaInterface implementation file.

R__EXTERN class THcParmList*  gHcParms;      //List of global symbolic variables

#endif
