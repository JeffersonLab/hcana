//////////////////////////////////////////////////////////////////////////
//
//  The Hall A analyzer interactive interface
//
//////////////////////////////////////////////////////////////////////////

#include "THcInterface.h"
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
  // Create a ROOT-style interactive interface

  TApplication *theApp = 
    new THcInterface( "The Hall C analyzer", &argc, argv );
  theApp->Run(kTRUE);

  cout << endl;

  delete theApp;

  return 0;
}
