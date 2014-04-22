//*-- Author : Stephen A. Wood 10.02.2012

// THcParmList
//
// A THaVarList that holds parameters read from
// the legacy ENGINE parameter file format
//

#define INCLUDESTR "#include"

#include "TObjArray.h"
#include "TObjString.h"
#include "TSystem.h"

#include "THcParmList.h"
#include "THaVar.h"
#include "THaFormula.h"

#include "TMath.h"

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
  // Read a CTP style parameter file.
  //
  // Parameter values and arrays of values are cached in a THaVarList
  // and are available for use elsewhere in the analyzer.
  // Text strings are saved in a THaTextvars list.
  // Parameter files can contain "include" statements of the form
  //   #include "filename"
  //
  // If a run number is given, ignore input until a line with a matching
  // run number or run number range is found.  All parameters following
  // the are read until a non matching run number or range is encountered.

  static const char* const whtspc = " \t";

  ifstream ifiles[100];		// Should use stack instead

  Int_t nfiles=0;
  ifiles[nfiles].open(fname);
  if(ifiles[nfiles].is_open()) {
    cout << nfiles << ": " << "Opened \"" << fname << "\"" << endl;
    nfiles++;
  }

  if(!nfiles) {
    static const char* const here   = "THcParmList::LoadFromFile";
    Error (here, "error opening parameter file %s",fname);
    return;			// Need a success argument returned
  }
  
  string line;
  char varname[100];
  Int_t InRunRange;
  Int_t currentindex = 0;

  varname[0] = '\0';

  if(RunNumber > 0) {
    InRunRange = 0;		// Wait until run number range matching RunNumber is found
    cout << "Reading Parameters for run " << RunNumber << endl;
  } else {
    InRunRange = 1;		// Interpret all lines
  }

  while(nfiles) {
    string current_comment("");
    // EJB_Note:  existing_comment is never used.
    // string existing_comment("");
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
	continue;		// Skip to next line
      }
    }

    if(!InRunRange) continue;

    // Interpret left of = as var name
    Int_t valuestartpos=0;  // Stays zero if no = found
    Int_t ttype = 0;     // Are any of the values floating point?
    if((pos=line.find_first_of("="))!=string::npos) {
      strcpy(varname, (line.substr(0,pos)).c_str());
      valuestartpos = pos+1;
      currentindex = 0;
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

    // New pseudo code
    // currentindex = where next item goes
    // nvals = number of new items on line
    // newlength = curentindex+nvals

    // if (variable already exists)   (valuestartpos is 0 or a find succeeded)
    //       get existinglegnth
    //       if (existinglength > newlength && type doesn't change) {
    //            copy nvals values directly into array
    //       else
    //            make new longer array of length max(existinglength, newlength)
    //            copy existinglength values into longer array changing type if needed
    //            delete old varname
    //            recreate varname of proper length
    // else (variable doesn't exist)
    //      make array of newlength
    //      create varname
    //  
    // There is some code duplication here.  Refactor later

    Int_t newlength = currentindex + nvals;
    THaVar* existingvar=Find(varname);
    if(existingvar) {
      string existingcomment;
      existingcomment.assign(existingvar->GetTitle());
      if(!existingcomment.empty()) {
	current_comment.assign(existingcomment);
      }
      Int_t existingtype=existingvar->GetType();
      Int_t existinglength=existingvar->GetLen();
      if(newlength > existinglength ||
	 (existingtype == kInt && ttype > 0)) { // Length or type change needed
	if(newlength < existinglength) newlength = existinglength;
	Int_t newtype=-1;
	if(ttype>0 || existingtype == kDouble) {
	  newtype = kDouble;
	  fp = new Double_t[newlength];
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
	  newtype = kInt;
	  ip = new Int_t[newlength];
	  Int_t* existingp= (Int_t*) existingvar->GetValuePointer();
	  for(Int_t i=0;i<existinglength;i++) {
	    ip[i] = existingp[i];
	  }
	} else {
	  cout << "Whoops!" << endl;
	}
	// Now copy new values in
	for(Int_t i=0;i<nvals;i++) {
	  TString valstr = ((TObjString *)vararr->At(i))->GetString();
	  if(newtype == kInt) {
	    ip[currentindex+i] = valstr.Atoi();
	  } else {
	    if(valstr.IsFloat()) {
	      fp[currentindex+i] = valstr.Atof();
	    } else {
	      THaFormula* formula = new THaFormula("temp",valstr.Data()
						   ,this, 0);
	      fp[currentindex+i] = formula->Eval();
	      delete formula;
	    }
	  }
	}
	currentindex += nvals;
	// Remove old variable and recreate
	RemoveName(varname);
	char *arrayname=new char [strlen(varname)+20];
	sprintf(arrayname,"%s[%d]",varname,newlength);
	if(newtype == kInt) {
	  Define(arrayname, current_comment.c_str(), *ip);
	} else {
	  Define(arrayname, current_comment.c_str(), *fp);
	}
	delete[] arrayname;
      } else {
	// Existing array long enough and of right type, just copy to it.
	if(ttype == 0 && existingtype == kInt) {
	  Int_t* existingp= (Int_t*) existingvar->GetValuePointer();
	  for(Int_t i=0;i<nvals;i++) {
	    TString valstr = ((TObjString *)vararr->At(i))->GetString();
	    existingp[currentindex+i] = valstr.Atoi();
	  }
	} else {
	  Double_t* existingp= (Double_t*) existingvar->GetValuePointer();
	  for(Int_t i=0;i<nvals;i++) {
	    TString valstr = ((TObjString *)vararr->At(i))->GetString();
	    if(valstr.IsFloat()) {
	      existingp[currentindex+i] = valstr.Atof();
	    } else {
	      THaFormula* formula = new THaFormula("temp",valstr.Data()
						   ,this, 0);
	      existingp[currentindex+i] = formula->Eval();
	      delete formula;
	    }
	  }
	}
	currentindex += nvals;
      }	
    } else {
      if(currentindex !=0) {
	cout << "currentindex=" << currentindex << " shouldn't be!" << endl;
      }
      if(ttype==0) {
	ip = new Int_t[nvals];
      } else {
	fp = new Double_t[nvals];
      }
      for(Int_t i=0;i<nvals;i++) {
	TString valstr = ((TObjString *)vararr->At(i))->GetString();
	if(ttype==0) {
	  ip[i] = valstr.Atoi();
	} else {
	  if(valstr.IsFloat()) {
	    fp[i] = valstr.Atof();
	  } else {
	    THaFormula* formula = new THaFormula("temp",valstr.Data()
						 ,this, 0);
	    fp[i] = formula->Eval();
	    delete formula;
	  }
	}
      }
      currentindex = nvals;
      
      char *arrayname=new char [strlen(varname)+20];
      sprintf(arrayname,"%s[%d]",varname,nvals);
      if(ttype==0) {
	Define(arrayname, current_comment.c_str(), *ip);
      } else {
	Define(arrayname, current_comment.c_str(), *fp);
      }
      delete[] arrayname;
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
  // If prefix is specified, prepend each requested parameter name with
  // the prefix.
  
  const DBRequest *ti = list;
  Int_t cnt=0;
  Int_t this_cnt=0;

  if( !prefix ) prefix = "";

  while ( ti && ti->name ) {
    string keystr(prefix); keystr.append(ti->name);
    const char* key = keystr.c_str();
    ///    cout <<"Now at "<<ti->name<<endl;
    this_cnt = 0;
    if(this->Find(key)) {
      VarType ty = this->Find(key)->GetType();
      if (ti->nelem>1) {
	// it is an array, use the appropriateinterface
	switch (ti->type) {
	case (kDouble) :
	  this_cnt = GetArray(key,static_cast<Double_t*>(ti->var),ti->nelem);
	  break;
	case (kInt) :
	  this_cnt = GetArray(key,static_cast<Int_t*>(ti->var),ti->nelem);
	  break;
	default:
	  Error("THcParmList","Invalid type to read %s",key);
	  break;
	}

      } else {
	switch (ti->type) {
	case (kDouble) :
	  if(ty == kInt) {
	    *static_cast<Double_t*>(ti->var)=*(Int_t *)this->Find(key)->GetValuePointer();	    
	  } else if (ty == kDouble) {
	    *static_cast<Double_t*>(ti->var)=*(Double_t *)this->Find(key)->GetValuePointer();
	  } else {
	    cout << "*** ERROR!!! Type Mismatch " << key << endl;
	  }
	  this_cnt=1;

	  break;
	case (kInt) :
	  if(ty == kInt) {
	    *static_cast<Int_t*>(ti->var)=*(Int_t *)this->Find(key)->GetValuePointer();
	  } else if (ty == kDouble) {
	    *static_cast<Int_t*>(ti->var)=TMath::Nint(*(Double_t *)this->Find(key)->GetValuePointer());
	    cout << "*** WARNING!!!  Rounded " << key << " to nearest integer " << endl;
	  } else {
	    cout << "*** ERROR!!! Type Mismatch " << key << endl;
	  }
	  this_cnt=1;
	  break;
	default:
	  Error("THcParmList","Invalid type to read %s",key);
	  break;
	}
      }
    } else {			// See if it is a text variable
      const char* value = GetString(key);
      if(value) {
	this_cnt = 1;
	if(ti->type == kString) {
	  *((string*)ti->var) = string(value);
	} else if (ti->type == kTString) {
	  *((TString*)ti->var) = (TString) value;
	} else {
	  Error("THcParmList","No conversion for strings: %s",key);
	}
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

//  READING AN ARRAY INTO A C-style ARRAY
//_____________________________________________________________________________
Int_t THcParmList::GetArray(const char* attr, Int_t* array, Int_t size)
{
  // Read in a set of Int_t's in to a C-style array.
  
  return ReadArray(attr,array,size);
}
//_____________________________________________________________________________
Int_t THcParmList::GetArray(const char* attr, Double_t* array, Int_t size)
{
  // Read in a set of Double_t's in to a vector.
  
  return ReadArray(attr,array,size);
}

//_____________________________________________________________________________
template<class T>
Int_t THcParmList::ReadArray(const char* attrC, T* array, Int_t size)
{
  // Copy values from parameter store to array
  // No resizing is done, so only 'size' elements may be stored.

  Int_t cnt=0;

  THaVar *var = Find(attrC);
  if(!var) return(cnt);
  VarType ty = var->GetType();
  if( ty != kInt && ty != kDouble) {
    cout << "*** ERROR: " << attrC << " has unsupported type " << ty << endl;
    return(cnt);
  }
  Int_t sz = var->GetLen();
  const void *vp = var->GetValuePointer();
  if(size != sz) {
    cout << "*** WARNING: requested " << size << " elements of " << attrC <<
      " which has length " << sz << endl;
  }
  if(size<sz) sz = size;
  Int_t donint = 0;
  if(ty == kDouble && typeid(array[0]) == typeid(Int_t)) {
    donint = 1;			// Use nint when putting doubles in nint
    cout << "*** WARNING!!!  Rounded " << attrC << " elements to nearest integer " << endl;
  }
  for(cnt=0;cnt<sz;cnt++) {
    if(ty == kInt) {
      array[cnt] = ((Int_t*)vp)[cnt];
    } else
      if(donint) {
	array[cnt] = TMath::Nint(((Double_t*)vp)[cnt]);
      } else {
	array[cnt] = ((Double_t*)vp)[cnt];
      }
  }
  return(cnt);
}

//_____________________________________________________________________________
void THcParmList::PrintFull( Option_t* option ) const
{
  // Print all the numeric parameter desciptions and values.
  // Print all the text parameters
  THaVarList::PrintFull(option);
  TextList->Print();
}
#ifdef WITH_CCDB
//_____________________________________________________________________________
Int_t THcParmList::OpenCCDB(Int_t runnum)
{
  // Use the Environment variable "CCDB_CONNECTION" as the
  // connection string
  const char* connection_string = gSystem->Getenv("CCDB_CONNECTION");

  return(OpenCCDB(runnum,connection_string));
}
Int_t THcParmList::OpenCCDB(Int_t runnum, const char* connection_string)
{
  // Connect to a CCDB database pointed to by connection_string

  std::string s (connection_string);
  CCDB_obj = new SQLiteCalibration(runnum);
  Int_t result = CCDB_obj->Connect(s);
  if(!result) return -1;	// Need some error codes
  cout << "Opened " << s << " for run " << runnum << endl;
  return 0;
}
Int_t THcParmList::CloseCCDB()
{
  delete CCDB_obj;
  return(0);
}
Int_t THcParmList::LoadCCDBDirectory(const char* directory, 
				     const char* prefix)
{
  // Load all parameters in directory
  // Prepend prefix onto the name of each

  std::string dirname (directory);

  if(dirname[dirname.length()-1]!='/') {
    dirname.append("/");
  }
  Int_t dirlen=dirname.length();

  vector<string> namepaths;
  CCDB_obj->GetListOfNamepaths(namepaths);
  for(UInt_t iname=0;iname<namepaths.size();iname++) {
    std::string varname (namepaths[iname]);
    if(varname.compare(0,dirlen,dirname) == 0) {
      varname.replace(0,dirlen,prefix);
      //      cout << namepaths[iname] << " -> " << varname << endl;

      // To what extent is there duplication here with Load() method?

      // Retrieve assignment
      Assignment* assignment = CCDB_obj->GetAssignment(namepaths[iname], true);
      ConstantsTypeColumn::ColumnTypes ccdbtype=assignment->GetValueType(0);
      Int_t ccdbncolumns=assignment->GetColumnsCount();
      Int_t ccdbnrows=assignment->GetRowsCount();
      std::string title = assignment->GetTypeTable()->GetComment();

      // Only load single column tables
      if(ccdbncolumns == 1) {

	THaVar* existingvar=Find(varname.c_str());
	// Need to append [size] to end of varname
	char sizestring[20];
	sprintf(sizestring,"[%d]",ccdbnrows);
	std::string size_str (sizestring);
	std::string varnamearray (varname);
	varnamearray.append(size_str);

	// Select data type
	if(ccdbtype==ConstantsTypeColumn::cIntColumn) {
	  vector<vector<int> > data;
	  CCDB_obj->GetCalib(data, namepaths[iname]);

	  if(existingvar) {
	    RemoveName(varname.c_str());
	  }

	  Int_t* ip = new Int_t[data.size()];
	  for(UInt_t row=0;row<data.size(); row++) {
	    ip[row] = data[row][0];
	  }
	  Define(varnamearray.c_str(), title.c_str(), *ip);

	} else if (ccdbtype==ConstantsTypeColumn::cDoubleColumn) {
	  vector<vector<double> > data;
	  CCDB_obj->GetCalib(data, namepaths[iname]);

	  if(existingvar) {
	    RemoveName(varname.c_str());
	  }

	  Double_t* fp = new Double_t[data.size()];
	  for(UInt_t row=0;row<data.size(); row++) {
	    fp[row] = data[row][0];
	  }
	  Define(varnamearray.c_str(), title.c_str(), *fp);
	} else if (ccdbtype==ConstantsTypeColumn::cStringColumn) {
	  if(ccdbnrows > 1) {
	    cout << namepaths[iname] << ": Only first element of CCDB string array loaded."  << endl;
	  }
	  vector<vector<string> > data;
	  CCDB_obj->GetCalib(data, namepaths[iname]);
	  AddString(varname, data[0][0]);
	} else {
	  cout << namepaths[iname] << ": Unsupported CCDB data type: " << ccdbtype << endl;
	}
      } else {
	cout << namepaths[iname] << ": Multicolumn CCDB variables not supported" << endl;
      }
    }	
  }
  return 0;
}

#endif
  
