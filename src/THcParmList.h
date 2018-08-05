#ifndef ROOT_THcParmList
#define ROOT_THcParmList

//////////////////////////////////////////////////////////////////////////
//
// THcParmList
//
//////////////////////////////////////////////////////////////////////////

#include "THaVarList.h"
#include "THaTextvars.h"

#ifdef WITH_CCDB
#ifdef __CINT__
struct pthread_cond_t;
struct pthread_mutex_t;
#endif
#include <CCDB/Calibration.h>
#include <CCDB/SQLiteCalibration.h>
using namespace ccdb;
#endif

using namespace std;


class THcParmList : public THaVarList {

public:

  THcParmList();
  virtual ~THcParmList() { Clear(); delete TextList; }

  virtual void Load( const char *fname, Int_t RunNumber=0);

  virtual void PrintFull(Option_t *opt="") const;

  std::string  PrintJSON(int run_number = 0) const;

  const char* GetString(const std::string& name) const {
    return(TextList->Get(name, 0));
  }

  Int_t AddString(const std::string& name, const std::string& value) {
    return(TextList->Add(name, value));
  }
  void RemoveString(const std::string& name) {
    TextList->Remove(name);
  }

  Int_t LoadParmValues(const DBRequest* list, const char* prefix=""); // assign values to the variables in list

  Int_t GetArray(const char* attr, Int_t* array, Int_t size);
  Int_t GetArray(const char* attr, Double_t* array, Int_t size);

#ifdef WITH_CCDB
  Int_t OpenCCDB(Int_t runnum);
  Int_t OpenCCDB(Int_t runnum, const char* connection_string);
  Int_t CloseCCDB();
  Int_t LoadCCDBDirectory(const char* directory,
			  const char* prefix);
#endif

private:

  THaTextvars* TextList;  //! Dictionary of string parameters

#ifdef WITH_CCDB
  SQLiteCalibration* CCDB_obj;
#endif

  template<class T>
    Int_t ReadArray(const char* attrC, T* array, Int_t size);

protected:

  ClassDef(THcParmList,0) // List of analyzer global parameters

};


namespace hcana{
  namespace json {

    template<typename T = double>
    T FindVarValueOr(THcParmList* parms, std::string var, T value){
      auto p = parms->Find(var.c_str());
      if(!p) {
        return value;
      }
      const auto res = static_cast<const T*>(p->GetDataPointer());
      return *res;
    }

  }
}

#endif

