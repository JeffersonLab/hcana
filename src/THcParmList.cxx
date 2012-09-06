//*-- Author : Stephen A. Wood 10.02.2012

// THcParmList
//
// A THaVarList that holds parameters read from
// the legacy ENGINE parameter file format
//

#define INCLUDESTR "#include"

#include "THaDB.h"
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
Int_t  fDebug   = 1;  // Keep this at one while we're working on the code    

ClassImp(THcParmList)

THcParmList::THcParmList() : THaVarList()
{
  TextList = new THaTextvars;
}

inline static bool IsComment( const string& s, string::size_type pos )
{
  return ( pos != string::npos && pos < s.length() &&
	   (s[pos] == '#' || s[pos] == ';' || s.substr(pos,2) == "//") );
}

void THcParmList::Load( const char* fname, Int_t RunNumber )
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
  Int_t InRunRange;

  varname[0] = '\0';

  if(RunNumber > 0) {
    InRunRange = 0;		// Wait until run number range matching RunNumber is found
    cout << "Reading Parameters for run " << RunNumber << endl;
  } else {
    InRunRange = 1;		// Interpret all lines
  }

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

    // Get rid of all white space not in quotes
    // Step through one char at a time 
    pos = 0;
    int inquote=0;
    char quotechar=' ';
    // cout << "Unstripped line: |" << line << "|" << endl;
    while(pos<line.length()) {
      if(inquote) {
	if(line[pos++] == quotechar) { // Possibly end of quoted string
	  if(line[pos] == quotechar) { // Protected quote
	    pos++;		// Skip the protected quote
	  } else {		// End of quoted string
	    inquote = 0;
	    quotechar = ' ';
	    pos++;
	  }
	}
      } else {
	if(line[pos] == ' ' || line[pos] == '\t') {
	  line.erase(pos,1);
	} else if(line[pos] == '"' || line[pos] == '\'') {
	  quotechar = line[pos++];\
	  inquote = 1;
	} else {
	  pos++;
	}
      }
    }
    // cout << "Stripped line: |" << line << "|" << endl;

    // Need to do something to bug out if line is empty

    // If in Engine database mode, check if line is a number range AAAA-BBBB
    if(RunNumber>0) {
      if(line.find_first_not_of("0123456789-")==string::npos) { // Interpret as runnum range
	if( (pos=line.find_first_of("-")) != string::npos) {
	  Int_t RangeStart=atoi(line.substr(0,pos).c_str());
	  Int_t RangeEnd=atoi(line.substr(pos+1,string::npos).c_str());
	  if(RunNumber >= RangeStart && RunNumber <= RangeEnd) {
	    InRunRange = 1;
	  } else {
	    InRunRange = 0;
	  }
	} else {		// A single number.  Run 
	  if(atoi(line.c_str()) == RunNumber) {
	    InRunRange = 1;
	  } else {
	    InRunRange = 0;
	  }
	}
      }
    }

    if(!InRunRange) continue;

    // Interpret left of = as var name
    Int_t valuestartpos=0;  // Stays zero if no = found
    Int_t existinglength=0;
    Int_t ttype = 0;     // Are any of the values floating point?
    if((pos=line.find_first_of("="))!=string::npos) {
      strcpy(varname, (line.substr(0,pos)).c_str());
      valuestartpos = pos+1;
    }

    // If first char after = is a quote, then this is a string assignment
    if(line[valuestartpos] == '"' || line[valuestartpos] == '\'') {
      quotechar = line[valuestartpos++];
      // Scan until end of line or terminating quote
      //      valuestartpos++;
      pos = valuestartpos;
      while(pos<line.length()) {
	if(line[pos++] == quotechar) { // Possibly end of quoted string
	  if(line[pos] == quotechar) { // Protected quote
	    pos++;
	  } else {
	    pos--;
	    break;
	  }
	}
      }
      if(TextList) {
	// Should check that a numerical assignment doesn't exist, but for
	// now, the same variable name can be used for strings and numbers
	string varnames(varname);
	AddString(varnames, line.substr(valuestartpos,pos-valuestartpos));
      }
      continue;
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
//_____________________________________________________________________________
Int_t THcParmList::LoadParmValues(const DBRequest* list, const char* prefix)
{
  // Load a number of entries from the database.
  // For array entries, the number of elements to be read in
  // must be given, and the memory already allocated
  // NOTE: initial code taken wholesale from THaDBFile. 
  // GN 2012
  
  const DBRequest *ti = list;
  Int_t cnt=0;
  Int_t this_cnt=0;

  if( !prefix ) prefix = "";

  while ( ti && ti->name ) {
    string keystr(prefix); keystr.append(ti->name);
    const char* key = keystr.c_str();
    ///    cout <<"Now at "<<ti->name<<endl;
    if (ti->nelem>1) {
      // it is an array, use the appropriateinterface
      switch (ti->type) {
      case (kDouble) :
	//	this_cnt = GetArray(system,ti->name,static_cast<Double_t*>(ti->var),
	//		    ti->expected,date);
	break;
      case (kInt) :
	//	this_cnt = GetArray(system,ti->name,static_cast<Int_t*>(ti->var),
	//ti->expected,date);
      break;
    default:
	Error("THcParmList","Invalid type to read %s",key);
	break;
      }

    } else {
      switch (ti->type) {
      case (kDouble) :
	if (this->Find(key)) {
	  *static_cast<Double_t*>(ti->var)=*(Double_t *)this->Find(key)->GetValuePointer();
	} else {
	  cout << "*** ERROR!!! Could not find " << key << " in the list of variables! ***" << endl;
	}
	this_cnt=1;

	break;
      case (kInt) :
	if (this->Find(key)) {
	  *static_cast<Int_t*>(ti->var)=*(Int_t *)this->Find(key)->GetValuePointer();
	} else {
	  cout << "*** ERROR!!! Could not find " << key << " in the list of variables! ***" << endl;
	}
	this_cnt=1;
	break;
      default:
	Error("THcParmList","Invalid type to read %s",key);
	break;
      }
    }
    if (this_cnt<=0) {
      if ( !ti->optional ) {
	Fatal("THcParmList","Could not find %s in database!",key);
      }
    }
    cnt += this_cnt;
    ti++;
  }
  return cnt;
}

//_____________________________________________________________________________
void THcParmList::PrintFull( Option_t* option ) const
{
  THaVarList::PrintFull(option);
  TextList->Print();
}
