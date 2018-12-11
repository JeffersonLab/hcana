//////////////////////////////////////////////////////////////////////////
//
//  The Hall A analyzer interactive interface
//
//////////////////////////////////////////////////////////////////////////

#include "THcInterface.h"
#include <iostream>
#include <cstring>
#include <string>
#include <cstdlib>

using namespace std;

int main(int argc, char **argv)
{
  // Create a ROOT-style interactive interface
  //

  if (!std::getenv("DB_DIR")) {
    std::string db_dir_env = "DBASE";
    if (setenv("DB_DIR", db_dir_env.c_str(), 1)) {
      std::cout << "Failed to set env var DB_DIR\n";
    } else {
      std::cout << "DB_DIR set to DBASE\n";
    }
  }

  // Handle convenience command line options
  bool print_version = false, no_logo = false;
  for( int i=1; i<argc; ++i ) {
    if( !strcmp(argv[i],"-l") )
      no_logo = true;
    else if( !strcmp(argv[1],"-v") || !strcmp(argv[1],"--version") ) {
      print_version = true;
      break;
    }
  }

  if( print_version ) {
    cout << THcInterface::GetVersionString() << endl;
    return 0;
  }


  TApplication *theApp = 
    new THcInterface( "The Hall C analyzer", &argc, argv, 0, 0, no_logo );
  theApp->Run(kFALSE);

  cout << endl;

  delete theApp;

  return 0;
}
