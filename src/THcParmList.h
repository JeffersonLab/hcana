#ifndef ROOT_THcParmList
#define ROOT_THcParmList

//////////////////////////////////////////////////////////////////////////
//
// THcParmList
//
//////////////////////////////////////////////////////////////////////////

#include "THaDB.h"
#include "THaVarList.h"
#include "THaTextvars.h"

using namespace std;

class THcParmList : public THaVarList {

public:
 
  THcParmList();
  virtual ~THcParmList() { Clear(); delete TextList; }

  virtual void Load( const char *fname, Int_t RunNumber=0);

  virtual void PrintFull(Option_t *opt="") const;

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

private:

  THaTextvars* TextList;

  template<class T>
    Int_t ReadArray(const char* attrC, T* array, Int_t size);

protected:

  ClassDef(THcParmList,0) // List of analyzer global parameters

};
#endif

