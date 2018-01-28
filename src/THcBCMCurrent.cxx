/*
  
  This physics module does:
  - Read average BCM beam current values from scaler parameter file.
  - Write the values into bcm#.AvgCurrentCut for each event  
  - Compare the current values with the threshold and 
  set event flags (BCM1 and BCM2 only)

  You can set the threshold using SetCurrentCut
  instead of gBCM_Current_threshold

 */

#include "THcParmList.h"
#include "THcGlobals.h"
#include "THcHitList.h"

#include "THcBCMCurrent.h"

using namespace std;

THcBCMCurrent::THcBCMCurrent(const char* name,
			     const char* description) :
  THaPhysicsModule(name, description), fThreshold(0)
{

  fBCM1flag = 0;
  fBCM2flag = 0;

  fBCM1avg  = 0;
  fBCM2avg  = 0;
  fBCM4aavg = 0;
  fBCM4bavg = 0;
  fBCM17avg = 0;
}

//__________________________________________________

THcBCMCurrent::~THcBCMCurrent()
{

  DefineVariables (kDelete);

  delete [] fiBCM1;  fiBCM1 = NULL;
  delete [] fiBCM2;  fiBCM2 = NULL;
  delete [] fiBCM4a; fiBCM4a = NULL;
  delete [] fiBCM4b; fiBCM4b = NULL;
  delete [] fiBCM17; fiBCM17 = NULL;
  delete [] fEvtNum; fEvtNum = NULL;

}

//__________________________________________________

THaAnalysisObject::EStatus THcBCMCurrent::Init( const TDatime& date )
{


  if( THaPhysicsModule::Init( date ) != kOK )
    return fStatus;

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
  
  fiBCM1     = new Double_t[fNscaler];
  fiBCM2     = new Double_t[fNscaler];
  fiBCM4a    = new Double_t[fNscaler];
  fiBCM4b    = new Double_t[fNscaler];
  fiBCM17    = new Double_t[fNscaler];

  fEvtNum    = new Int_t[fNscaler];

  DBRequest list3[] = {
    {"scal_read_bcm1_current",  fiBCM1,    kDouble, (UInt_t) fNscaler},
    {"scal_read_bcm2_current",  fiBCM2,    kDouble, (UInt_t) fNscaler},
    {"scal_read_bcm4a_current", fiBCM2,    kDouble, (UInt_t) fNscaler},
    {"scal_read_bcm4b_current", fiBCM2,    kDouble, (UInt_t) fNscaler},
    {"scal_read_bcm17_current", fiBCM2,    kDouble, (UInt_t) fNscaler},
    {"scal_read_event",         fEvtNum,   kInt,    (UInt_t) fNscaler},
    {0}
  };

  gHcParms->LoadParmValues((DBRequest*)&list3);

  BCMInfo binfo;
  for(int i=0; i<fNscaler; i++)
    {
      binfo.bcm1_current  = fiBCM1[i];
      binfo.bcm2_current  = fiBCM2[i];
      binfo.bcm4a_current = fiBCM4a[i];
      binfo.bcm4b_current = fiBCM4b[i];
      binfo.bcm17_current = fiBCM17[i];

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
    {"bcm1.currentflag",     "BCM1 current flag for good event", "fBCM1flag"},
    {"bcm2.currentflag",     "BCM2 current flag for good event", "fBCM2flag"},
    {"bcm1.AvgCurrentCut",  "BCM1  average beam current", "fBCM1avg"},
    {"bcm2.AvgCurrentCut",  "BCM2  average beam current", "fBCM2avg"},
    {"bcm4a.AvgCurrentCut", "BCM4a average beam current", "fBCM4aavg"},
    {"bcm4b.AvgCurrentCut", "BCM4b average beam current", "fBCM4bavg"},
    {"bcm17.AvgCurrentCut", "BCM17 average beam current", "fBCM17avg"},
    { 0 }
  };

  return DefineVarsFromList(vars, mode);

}

//__________________________________________________

Int_t THcBCMCurrent::Process( const THaEvData& evdata )
{
  
  if( !IsOK() ) return -1;
  
  int fEventNum = evdata.GetEvNum();
  
  BCMInfo binfo;
  Int_t fGetScaler = GetAvgCurrent( fEventNum, binfo );

  if(fGetScaler != kOK)
    {
      fBCM1avg  = 0;
      fBCM2avg  = 0;
      fBCM4aavg = 0;
      fBCM4bavg = 0;
      fBCM17avg = 0;
    }
  else
    {
      fBCM1avg  = binfo.bcm1_current;
      fBCM2avg  = binfo.bcm2_current;
      fBCM4aavg = binfo.bcm4a_current;
      fBCM4bavg = binfo.bcm4b_current;
      fBCM17avg = binfo.bcm17_current;
    }

  if(fBCM1avg < fThreshold)
    fBCM1flag = 0;
  else
    fBCM1flag = 1;

  if(fBCM2avg < fThreshold)
    fBCM2flag = 0;
  else
    fBCM2flag = 1;

  return kOK;

}

//__________________________________________________    

Int_t THcBCMCurrent::GetAvgCurrent( Int_t fevn, BCMInfo &bcminfo )
{

  map<int, BCMInfo>::iterator it, next;
  it = BCMInfoMap.find(fevn);
  if( it != BCMInfoMap.end() )
    {
      bcminfo.bcm1_current  = it->second.bcm1_current;
      bcminfo.bcm2_current  = it->second.bcm2_current;
      bcminfo.bcm4a_current = it->second.bcm4a_current;
      bcminfo.bcm4b_current = it->second.bcm4b_current;
      bcminfo.bcm17_current = it->second.bcm17_current;

      return kOK;
    }
  else
    {
      next = BCMInfoMap.upper_bound(fevn);
      if( next != BCMInfoMap.end() )
	{
	  bcminfo.bcm1_current  = next->second.bcm1_current;
	  bcminfo.bcm2_current  = next->second.bcm2_current;
	  bcminfo.bcm4a_current = next->second.bcm4a_current;
	  bcminfo.bcm4b_current = next->second.bcm4b_current;
	  bcminfo.bcm17_current = next->second.bcm17_current;
	  return kOK;
	}
      return kOK+1;
    }

  return kOK+1;

}

//__________________________________________________    

ClassImp(THcBCMCurrent)
