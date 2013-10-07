//*-- Author :    Stephen Wood  13-March-2012

//////////////////////////////////////////////////////////////////////////
//
// THcAnalyzer
//
// THcAnalyzer is the base class for a "Hall C analyzer" class.
// An analyzer defines the basic actions to perform during analysis.
// THcAnalyzer is the default analyzer that is used if no user class is
// defined.  It performs a standard analysis consisting of
//
//   1. Decoding/Calibrating
//   2. Track Reconstruction
//   3. Physics variable processing
//
// At the end of each step, testing and histogramming are done for
// the appropriate block defined in the global test/histogram lists.
//
// Hall C has their own analyzer class because some things are bound to
// be different.
//
//////////////////////////////////////////////////////////////////////////

#include "THcAnalyzer.h"
#include "THaBenchmark.h"
#include "TList.h"
#include "THaGlobals.h"


#include <fstream>
#include <algorithm>
#include <iomanip>
#include <cstring>

using namespace std;


// Pointer to single instance of this object
//THcAnalyzer* THcAnalyzer::fgAnalyzer = 0;

//FIXME: 
// do we need to "close" scalers/EPICS analysis if we reach the event limit?

//_____________________________________________________________________________
THcAnalyzer::THcAnalyzer()
{

}

//_____________________________________________________________________________
THcAnalyzer::~THcAnalyzer()
{
  // Destructor. 

}

//_____________________________________________________________________________

ClassImp(THcAnalyzer)

