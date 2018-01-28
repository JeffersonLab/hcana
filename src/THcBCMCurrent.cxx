#include "THcParmList.h"
#include "THcGlobals.h"
#include "THcHitList.h"

#include "THcBCMCurrent.h"

using namespace std;

THcBCMCurrent::THcBCMCurrent(const char* name,
			     const char* description) :
  THaPhysicsModule(name, description), fThreshold(0), fPrevious(kFALSE)
{

  fBCM1flag = 0;
  fBCM2flag = 0;

}

//__________________________________________________

THcBCMCurrent::~THcBCMCurrent()
{

  DefineVariables (kDelete);

  delete [] fiBCM1;  fiBCM1 = NULL;
  delete [] fiBCM2;  fiBCM2 = NULL;
  delete [] fEvtNum; fEvtNum = NULL;

}

//__________________________________________________

THaAnalysisObject::EStatus THcBCMCurrent::Init( const TDatime& date )
{


  if( THaPhysicsModule::Init( date ) != kOK )
    return fStatus;

  //  ReadDatabase();

  /*
  InitHitList(fDetMap,"THcHodoRawHit",fDetMap->GetTotNumChan()+1);

  EStatus status;
  if( (status = THaNonTrackingDetector::Init( date )) )
    return fStatus=status;

  return fStatus =  kOK;
  */

  return fStatus =  kOK;
}

//__________________________________________________

Int_t THcBCMCurrent::ReadDatabase( const TDatime& date )
{
  
  DBRequest list1[] = {
    {"num_scal_reads", &fNscaler, kInt},
    {0}
  };
  gHcParms->LoadParmValues((DBRequest*)&list1);
  
  if(fThreshold < 1.e-5)
    {
      DBRequest list2[] = {
	{"gBCM_Current_threshold", &fThreshold, kDouble}, 
	{0}
      };
      gHcParms->LoadParmValues((DBRequest*)&list2);  
    }  
  
  fiBCM1    = new Double_t[fNscaler];
  fiBCM2    = new Double_t[fNscaler];
  fEvtNum   = new Int_t[fNscaler];

  DBRequest list3[] = {
    {"scal_read_bcm1_current", fiBCM1,    kDouble, (UInt_t) fNscaler},
    {"scal_read_bcm1_current", fiBCM2,    kDouble, (UInt_t) fNscaler},
    {"scal_read_event",        fEvtNum,   kInt,    (UInt_t) fNscaler},
    {0}
  };

  gHcParms->LoadParmValues((DBRequest*)&list3);

  BCMInfo binfo;
  for(int i=0; i<fNscaler; i++)
    {
      binfo.bcm1_current = fiBCM1[i];
      binfo.bcm2_current = fiBCM2[i];
      BCMInfoMap.insert( std::make_pair(fEvtNum[i], binfo) );
    }

  return kOK;

}

//__________________________________________________

Int_t THcBCMCurrent::DefineVariables( EMode mode )
{

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  RVarDef vars[] = {
    {"bcm1_currentCut", "BCM1 current flag for good event", "fBCM1flag"},
    {"bcm2_currentCut", "BCM2 current flag for good event", "fBCM2flag"},
    { 0 }
  };

  return DefineVarsFromList(vars, mode);

}

//__________________________________________________

Int_t THcBCMCurrent::Process( const THaEvData& evdata )
{
  
  if( !IsOK() ) return -1;
  
  // this returns # of raw hits
  //  fNhits = DecodeToHitList(evdata);
  
  int fEventNum = evdata.GetEvNum();
  
  Bool_t is_good_evt = kFALSE;
  Int_t fBeamStatus = CheckGoodEvent( fEventNum );

  switch (fBeamStatus)
    {
    case kBeamOff:
      is_good_evt = kFALSE;
      fPrevious   = kFALSE;
      break;
    case kBeamOn:
      is_good_evt = kTRUE;
      fPrevious   = kTRUE;
      break;
    case kCarryOn:
      if(!fPrevious)
	is_good_evt = kFALSE;
      else
	is_good_evt = kTRUE;
      break;
    }

  if(!is_good_evt)
    {
      fBCM1flag = 0;
      fBCM2flag = 0;
    }
  else
    {
      fBCM1flag = 1;    
      fBCM2flag = 1;    
    }

  return kOK;

}

//__________________________________________________    

Int_t THcBCMCurrent::CheckGoodEvent( Int_t fevn )
{

  map<int, BCMInfo>::const_iterator it;
  it = BCMInfoMap.find(fevn);
  if( it != BCMInfoMap.end() )
    {
      if( it->second.bcm1_current < fThreshold )
	return kBeamOff;
      else
	return kBeamOn;
    }
  else
    return kCarryOn;

}

//__________________________________________________    

ClassImp(THcBCMCurrent)
