//*-- Author : Stephen A. Wood 10.02.2012

// THcParmList
//
// A THaVarList that holds parameters read from
// the legacy ENGINE parameter file format
//

#define INCLUDESTR "#include"

#include "TObjArray.h"
#include "TObjString.h"

#include "THcParmList.h"
#include "THaVar.h"
#include "THaFormula.h"


/* #incluce <algorithm> include <fstream> include <cstring> */
#include <iostream>
#include <fstream>
#include <cassert>
#include <cstdlib>

using namespace std;

ClassImp(THcParmList)

inline static bool IsComment( const string& s, string::size_type pos )
{
  return ( pos != string::npos && pos < s.length() &&
	   (s[pos] == '#' || s[pos] == ';' || s.substr(pos,2) == "//") );
}

void THcParmList::Load( const char* fname )
{

  static const char* const here   = "THcParmList::LoadFromFile";
  static const char* const whtspc = " \t";

  ifstream ifiles[100];		// Should use stack instead

  Int_t nfiles=0;
  ifiles[nfiles].open(fname);
  if(ifiles[nfiles].is_open()) {
    cout << nfiles << ": " << "Opened \"" << fname << "\"" << endl;
    nfiles++;
  }

  if(!nfiles) {
    Error (here, "error opening parameter file %s",fname);
    return;			// Need a success argument returned
  }
  
  string line;
  Int_t nlines_read = 0, nparameters_read = 0;
  char varname[100];

  varname[0] = '\0';

  while(nfiles) {
    string current_comment("");
    string existing_comment("");
    string::size_type start, pos = 0;

    if(!getline(ifiles[nfiles-1],line)) {
      ifiles[nfiles-1].close();
      nfiles--;
      cout << nfiles << ": " << "Closed" << endl;
      continue;
    }
    // Look for include statement
    if(line.compare(0,strlen(INCLUDESTR),INCLUDESTR)==0) {
      line.erase(0,strlen(INCLUDESTR));
      pos = line.find_first_not_of(whtspc);
      // Strip leading white space
      if(pos != string::npos && pos > 0 && pos < line.length()) {
	line.erase(0,pos);
      }
      char quotechar=line[0];
      if(quotechar == '"' || quotechar == '\'') {
	line.erase(0,1);
	line.erase(line.find_first_of(quotechar));
      } else {
	line.erase(line.find_first_of(whtspc));
      }
      cout << line << endl;
      ifiles[nfiles].open(line.c_str());
      if(ifiles[nfiles].is_open()) {
	cout << nfiles << ": " << "Opened \"" << line << "\"" << endl;
	nfiles++;
      }
      continue;
    }


    // Blank line or comment?
    if( line.empty()
	|| (start = line.find_first_not_of( whtspc )) == string::npos
	|| IsComment(line, start) )
      continue;

    // Get rid of trailing comments and leading and trailing whitespace
    // Need to save the comment and put it in the thVar
    
    while( (pos = line.find_first_of("#;/", pos+1)) != string::npos ) {
      if( IsComment(line, pos) ) {
	current_comment.assign(line,pos+1,line.length());
	line.erase(pos);	// Strip off comment
	// Strip leading white space from comment
	cout << "CommentA: " << current_comment << endl;
	pos = current_comment.find_first_not_of(whtspc);
	if(pos!=string::npos && pos > 0 && pos < current_comment.length()) {
	  current_comment.erase(0,pos);
	}
	cout << "CommentB: " << current_comment << endl;
	break;
      }
    }
    pos = line.find_last_not_of( whtspc );
    assert( pos != string::npos );
    if( pos != string::npos && ++pos < line.length() )
      line.erase(pos);
    pos = line.find_first_not_of(whtspc);
    // Strip leading white space
    if(pos != string::npos && pos > 0 && pos < line.length()) {
      line.erase(0,pos);
    }
    // Ignore begin and end statements
    if(line.compare(0,5,"begin")==0 ||
       line.compare(0,3,"end")==0) {
      cout << "Skipping: " << line << endl;
      continue;
    }
    // Ignore string assingments
    if(line.find_first_of("\"'")!=string::npos) {
      cout << "Skipping string assignment: " << line << endl;
      continue;
    }

    // Get rid of all white space
    while((pos=line.find_first_of(whtspc)) != string::npos) {
      line.erase(pos,1);
    }
    // Need to do something to bug out if line is empty

    // Interpret left of = as var name
    Int_t valuestartpos=0;  // Stays zero if no = found
    Int_t existinglength=0;
    Int_t ttype = 0;     // Are any of the values floating point?
    if((pos=line.find_first_of("="))!=string::npos) {
      strcpy(varname, (line.substr(0,pos)).c_str());
      valuestartpos = pos+1;
    }

    TString values((line.substr(valuestartpos)).c_str());
    TObjArray *vararr = values.Tokenize(",");
    Int_t nvals = vararr->GetLast()+1;
    
    Int_t* ip;
    Double_t* fp;
    // or expressions
    for(Int_t i=0;(ttype==0&&i<nvals);i++) {
      TString valstr = ((TObjString *)vararr->At(i))->GetString();
      if(valstr.IsFloat()) {	// Is a number
	if(valstr.Contains(".") || valstr.Contains("e",TString::kIgnoreCase)) {
	  ttype = 1;
	  break;
	}
      } else {
	ttype = 2;		// Force float if expression or Var
	break;
      }
    }

    if(valuestartpos==0) {	// We are adding to an existing array
      // Copy out the array and delete the variable to be recreated
      THaVar* existingvar=Find(varname);
      if(existingvar) {
	string existingcomment;
	existingcomment.assign(existingvar->GetTitle());
	if(!existingcomment.empty()) {
	  current_comment.assign(existingcomment);
	}
	existinglength=existingvar->GetLen();
	Int_t existingtype=existingvar->GetType();
	if(ttype>0 || existingtype == kDouble) {
	  ttype = 1;
	  fp = new Double_t[nvals+existinglength];
	  if(existingtype == kDouble) {
	    Double_t* existingp= (Double_t*) existingvar->GetValuePointer();
	    for(Int_t i=0;i<existinglength;i++) {
	      fp[i] = existingp[i];
	    }
	  } else if(existingtype == kInt) {
	    Int_t* existingp= (Int_t*) existingvar->GetValuePointer();
	    for(Int_t i=0;i<existinglength;i++) {
	      fp[i] = existingp[i];
	    }
	  } else {
	    cout << "Whoops!" << endl;
	  }
	} else if(existingtype == kInt) {	// Stays int
	  ip = new Int_t[nvals+existinglength];
	  Int_t* existingp= (Int_t*) existingvar->GetValuePointer();
	  for(Int_t i=0;i<existinglength;i++) {
	    ip[i] = existingp[i];
	  }
	} else {
	  cout << "Whoops!" << endl;
	}
	RemoveName(varname);
      }
    }
	
    if(nvals > 0) {		// It's an array
      // Type the array floating unless all are ints.  (if any variables
      // or expressions, go floating)
      // Allocate the array
      if(existinglength==0) {
	if(ttype==0) {
	  ip = new Int_t[nvals];
	} else {
	  fp = new Double_t[nvals];
	}
      }
      for(Int_t i=0;i<nvals;i++) {
	TString valstr = ((TObjString *)vararr->At(i))->GetString();
	if(ttype==0) {
	  ip[existinglength+i] = valstr.Atoi();
	} else {
	  if(valstr.IsFloat()) {
	    fp[existinglength+i] = valstr.Atof();
	  } else {
	    THaFormula* formula = new THaFormula("temp",valstr.Data()
						 ,this, 0);
	    fp[existinglength+i] = formula->Eval();
	    delete formula;
	  }
	}
	//	cout << i << " " << valstr << endl;;
      }
      char *arrayname=new char [strlen(varname)+20];
      sprintf(arrayname,"%s[%d]",varname,nvals+existinglength);
      if(ttype==0) {
	Define(arrayname, current_comment.c_str(), *ip);
      } else {
	Define(arrayname, current_comment.c_str(), *fp);
      }
      delete[] arrayname;
    } else {
      cout << "nvals is zero??  Maybe continued on next line" << endl;
    }

    //    cout << line << endl;

  }

  return;

}
//
