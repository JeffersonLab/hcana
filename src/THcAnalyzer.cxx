/** \class THcAnalyzer
    \ingroup Base

\brief Hall C analyzer class.

Adds the following to the Hall A analyzer base class.

1.  PrintReport method to make text reports from template files

2.  Retrieve run number and startind and ending event from parameter DB

\author S. A. Wood,  13-March-2012

*/
#include "THcAnalyzer.h"
#include "THaRunBase.h"
#include "THaBenchmark.h"
#include "TList.h"
#include "THcParmList.h"
#include "THcFormula.h"
#include "THcGlobals.h"
#include "THaGlobals.h"
#include "TMath.h"

#include <fstream>
#include <algorithm>
#include <iomanip>
#include <cstring>
#include <iostream>

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
void THcAnalyzer::PrintReport(const char* templatefile, const char* ofile)
{
  /// Generate "reports" such as end of run scaler/efficiency sheets
  /// Reads a template file, copying that file to the output, replacing
  /// variables and expressions inside of braces ({}) with evaluated values.
  /// Similar but not identical to ENGINE/CTP report templates.
  ifstream ifile;
  ifile.open(templatefile);

  if(!ifile.is_open()) {
    cout << "Error opening template file " << templatefile << endl;
    return;
  }

  ofstream ostr(ofile);

  if(!ostr.is_open()) {
      cout << "Error opening report output file " << ofile << endl;
      return;
  }

  LoadInfo();			// Load some run information into gHcParms

  // In principle, we should allow braces to be escaped.  But for
  // now we won't.  Existing template files don't seem to output
  // any braces
  for(string line; getline(ifile, line);) {
    //    cout << line << endl;
    string::size_type start;
    while((start = line.find('{',0)) != string::npos) {
      //      cout << start << endl;
      string::size_type end = line.find('}',start);
      if(end==string::npos) break; // No more expressions on the line
      string expression=line.substr(start+1,end-start-1);
      string::size_type formatpos = expression.find(':',0);
      string format;
      if(formatpos != string::npos) {
	format=expression.substr(formatpos+1);
	expression=expression.substr(0,formatpos);
	//	cout << "|" << expression << "|" << format << "|" << endl;
      }
      // Should we first check if the expression can be simply a variable
      // or index into variable?
      // For now, first check if it is a string.
      // If not, then evaluate as an expression.
      string replacement;
      if(const char *textstring=gHcParms->GetString(expression)) {
	//	cout << expression << " is a string with value " << textstring << endl;
	if(format.empty()) format = "%s";
	replacement=Form(format.c_str(),textstring);
      } else {
	THcFormula* formula = new THcFormula("temp",expression.c_str(),gHcParms,gHaVars,gHaCuts);
	Double_t value=formula->Eval();
	delete formula; formula = 0;
	// If the value is close to integer and no format is defined
	// use "%.0f" to print out integer
	if(format.empty()) {
	  if(TMath::Abs(value-TMath::Nint(value)) < 0.0000001) {
	    format = "%.0f";
	  } else {
	    format = "%f";
	  }
	}
	if(format[format.length()-1] == 'd') {
	  replacement=Form(format.c_str(),TMath::Nint(value));
	} else {
	  replacement=Form(format.c_str(),value);
	}
      }
      //      cout << "Replacement:" << replacement << endl;
      line.replace(start,end-start+1,replacement);
    }
    ostr << line << endl;
  }
  ostr.close();
  ifile.close();

  return;
}

//_____________________________________________________________________________
void THcAnalyzer::LoadInfo()
{
  /// Create several THcParms variables in gHcParms containing
  /// run information such as
  // run number, first event analyzed, number of events, etc.
  ///    `gen_run_number` - Current run
  ///    `gen_run_starting_event` - Id of first event analyzed
  ///    `gen_event_id_number` - Id of last event analyzed
  Int_t* runnum;
  Int_t* firstevent;
  Int_t* lastevent;

  THaVar* varptr;
  varptr = gHcParms->Find("gen_run_number");
  if(varptr) {
    runnum = (Int_t*) varptr->GetValuePointer(); // Assume correct type
  } else {
    runnum = new Int_t[1];
    gHcParms->Define("gen_run_number","Run Number", *runnum);
  }
  *runnum = fRun->GetNumber();

  varptr = gHcParms->Find("gen_run_starting_event");
  if(varptr) {
    firstevent = (Int_t*) varptr->GetValuePointer(); // Assume correct type
  } else {
    firstevent = new Int_t[1];
    gHcParms->Define("gen_run_starting_event","First event analyzed", *firstevent);
  }
  // May not agree with engine event definintions
  *firstevent = fRun->GetFirstEvent();

  varptr = gHcParms->Find("gen_event_id_number");
  if(varptr) {
    lastevent = (Int_t*)varptr->GetValuePointer(); // Assume correct type
  } else {
    lastevent = new Int_t[1];
    gHcParms->Define("gen_event_id_number","Last event analyzed", *lastevent);
  }
  // Not accurate
  *lastevent = fRun->GetFirstEvent()+fRun->GetNumAnalyzed();
}

//_____________________________________________________________________________

ClassImp(THcAnalyzer)

