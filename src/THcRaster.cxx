/** \class THcRaster
    \ingroup DetSupport

  A class to decode the fast raster signals.
  Measures the two magnet currents which are proportional to horizontal and
  vertical beam position

\author Buddhini Waidyawansa

*/
#include "TMath.h"

#include "THcRaster.h"
#include "THaEvData.h"
#include "THaDetMap.h"


#include "THcParmList.h"
#include "THcGlobals.h"
#include "THaGlobals.h"

#include "THcRawAdcHit.h"
#include "THcSignalHit.h"
#include "THaGetApparatus.h"

//#include "THcHitList.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>


using namespace std;

//_____________________________________________________________________________
THcRaster::THcRaster( const char* name, const char* description,
		      THaApparatus* apparatus ) :
  THaBeamDet(name,description,apparatus)
{

  fAnalyzePedestals = 0;
  fNPedestalEvents = 0;
  
  fRawXAADC = 0;
  fRawYAADC = 0;
  fRawXBADC = 0;
  fRawYBADC = 0;
  
  fXAADC = 0;
  fYAADC = 0;
  fXBADC = 0;
  fYBADC = 0;

  fXApos = 0;
  fYApos = 0;
  fXBpos = 0;
  fYBpos = 0;

  fFrCalMom = 0;

  fFrXAADCperCM = 1.0;
  fFrYAADCperCM = 1.0;
  fFrXBADCperCM = 1.0; 
  fFrYBADCperCM = 1.0;

  fFrXAADC_zero_offset = 0.0;
  fFrYAADC_zero_offset = 0.0;
  fFrXBADC_zero_offset = 0.0;
  fFrYBADC_zero_offset = 0.0;
  
  for(Int_t i=0;i<4;i++){
    fPedADC[i] = 0;
  }
}

//_____________________________________________________________________________
THcRaster::THcRaster():
  THaBeamDet()
{
  //Constructor
  frPosAdcPulseIntRaw = NULL;
}

//_____________________________________________________________________________
_____________________________________________________________________________
THcRaster::~THcRaster()
{
  //Destructor
  delete frPosAdcPulseIntRaw;  frPosAdcPulseIntRaw  = NULL;

  DeleteArrays();
}

//_____________________________________________________________________________
Int_t THcRaster::ReadDatabase( const TDatime& date )
{

  // Read parameters such as calibration factor, of this detector from the database.
  cout << "THcRaster::ReadDatabase()" << endl;

  char prefix[2];

  //cout << " THcRaster::ReadDatabase GetName() called " << GetName() << endl;
  //  prefix[0]=tolower(GetName()[0]);
  // bpw- The prefix is hardcoded so that we don't have to change the gbeam.param file. o/w to get the following variables, we need to change to parameter names to rfr_cal_mom, etc where "r" comes from prefix[0]=tolower(GetName()[0]).
  prefix[0]='g';
  prefix[1]='\0';

  frPosAdcPulseIntRaw  = new TClonesArray("THcSignalHit", 4);

  // string names;
  DBRequest list[]={
    {"fr_cal_mom",&fFrCalMom, kDouble},
    {"frxa_adcpercm",&fFrXAADCperCM, kDouble},
    {"frya_adcpercm",&fFrYAADCperCM, kDouble},
    {"frxb_adcpercm",&fFrXBADCperCM, kDouble},
    {"fryb_adcpercm",&fFrYBADCperCM, kDouble},
    {"frxaADC_zero_offset",&fFrXAADC_zero_offset, kDouble},
    {"fryaADC_zero_offset",&fFrYAADC_zero_offset, kDouble},
    {"frxbADC_zero_offset",&fFrXBADC_zero_offset, kDouble},
    {"frybADC_zero_offset",&fFrYBADC_zero_offset, kDouble},
    {"pbeam",&fgpbeam, kDouble},
    {"frx_dist", &fgfrx_dist, kDouble},
    {"fry_dist", &fgfry_dist, kDouble},
    {"beam_x", &fgbeam_xoff, kDouble,0,1},
    {"beam_xp", &fgbeam_xpoff, kDouble,0,1},
    {"beam_y", &fgbeam_yoff, kDouble,0,1},
    {"beam_yp", &fgbeam_ypoff, kDouble,0,1},
    {"usefr", &fgusefr, kInt,0,1},
    {0}
  };

  // Default offsets to zero
  fgbeam_xoff = 0.0;
  fgbeam_xpoff = 0.0;
  fgbeam_yoff = 0.0;
  fgbeam_ypoff = 0.0;
  fgusefr = 0;
  // get the calibration factors from gbeam.param file
  gHcParms->LoadParmValues((DBRequest*)&list,prefix);

  return kOK;

}


//_____________________________________________________________________________
Int_t THcRaster::DefineVariables( EMode mode )
{
  // Initialize global variables for histogramming and tree

  cout << "THcRaster::DefineVariables called " << GetName() << endl;

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  // Register variables in global list

  RVarDef vars[] = {
    {"frxa_raw_adc",  "Raster Xa raw ADC",    "fRawXaADC"},
    {"frya_raw_adc",  "Raster Ya raw ADC",    "fRawYaADC"},
    {"frxb_raw_adc",  "Raster Xb raw ADC",    "fRawXbADC"},
    {"fryb_raw_adc",  "Raster Yb raw ADC",    "fRawYbADC"},
    {"frxa_adc",  "Raster Xa ADC",    "fXaADC"},
    {"frya_adc",  "Raster Ya ADC",    "fYaADC"},
    {"frxb_adc",  "Raster Xb ADC",    "fXbADC"},
    {"fryb_adc",  "Raster Yb ADC",    "fYbADC"},
    {"frxa",  "Raster Xa position",   "fXapos"},
    {"frya",  "Raster Ya position",   "fYapos"},
    {"frxb",  "Raster Xb position",   "fXbpos"},
    {"fryb",  "Raster Yb position",   "fYbpos"},
    {"posAdcCounter",   "Positive ADC counter numbers",   "frPosAdcPulseIntRaw.THcSignalHit.GetPaddleNumber()"},
    { 0 }
  };

  return DefineVarsFromList( vars, mode );
}

//_____________________________________________________________________________
THaAnalysisObject::EStatus THcRaster::Init( const TDatime& date )
{
  cout << "THcRaster::Init()" << endl;

  // Fill detector map with RASTER type channels
  char EngineDID[] = "xRASTER";
  EngineDID[0] = toupper(GetApparatus()->GetName()[0]);
  if( gHcDetectorMap->FillMap(fDetMap, EngineDID) < 0 ) {
    static const char* const here = "Init()";
    Error( Here(here), "Error filling detectormap for %s.", EngineDID);
    return kInitError;
  }

  THcHitList::InitHitList(fDetMap,"THcRasterRawHit",fDetMap->GetTotNumChan()+1);

  EStatus status;
  if( (status = THaBeamDet::Init( date )) )
    return fStatus=status;

  return fStatus = kOK;

}

//_____________________________________________________________________________
inline 
void THcRaster::Clear(Option_t* opt)
{
  fNhits = 0;
  frPosAdcPulseIntRaw->Clear();
}


//_____________________________________________________________________________
void THcRaster::AccumulatePedestals(TClonesArray* rawhits)
{
  /*
  Extract data from the hit list, accumulating into arrays for
  calculating pedestals.
  From ENGINE/g_analyze_misc.f -

  * JRA: Code to check FR pedestals.  Since the raster is a fixed frequency
  * and the pedestals come at a fixed rate, it is possible to keep getting
  * the same value for each pedestal event, and get the wrong zero value.
  * (see HCLOG #28325).  So calculate pedestal from first 1000 REAL
  * events and compare to value from pedestal events.  Error on each
  * measurement is RMS/sqrt(1000), error on diff is *sqrt(2), so 3 sigma
  * check is 3*sqrt(2)*RMS/sqrt(1000) = .13*RMS
  !
  ! Can't use RMS, since taking sum of pedestal**2 for these signals
  ! gives rollover for integer*4.  Just assume signal is +/-2000
  ! channels, gives sigma of 100 channels, so check for diff>130.
  !
  */

  Int_t nrawhits = rawhits->GetLast()+1;

  Int_t ihit=0;
  UInt_t nrPosAdcHits = 0;

  while(ihit<nrawhits) {
    THcRasterRawHit* hit = (THcRasterRawHit *) fRawHitList->At(ihit);
     THcRawAdcHit&     rawPosAdcHit = hit->GetRawAdcHitPos();
     Int_t             nsig         = hit->fCounter;

    for (UInt_t thit=0; thit<rawPosAdcHit.GetNPulses(); ++thit) {
       ((THcSignalHit*) frPosAdcPulseIntRaw->ConstructedAt(nrPosAdcHits))->Set(nsig,rawPosAdcHit.GetPulseIntRaw(thit));
       ++nrPosAdcHits;
    }
    ihit++;
  }

    for (Int_t ielem = 0; ielem < frPosAdcPulseIntRaw->GetEntries(); ielem++) {
       Int_t    nraster           = ((THcSignalHit*) frPosAdcPulseIntRaw->ConstructedAt(ielem))->GetPaddleNumber() - 1;
       Double_t pulseIntRaw       = ((THcSignalHit*) frPosAdcPulseIntRaw->ConstructedAt(ielem))->GetData();
       if (nraster ==0) fPedADC[0] = pulseIntRaw;
       if (nraster ==1) fPedADC[1] = pulseIntRaw;
       if (nraster ==2) fPedADC[2] = pulseIntRaw;
       if (nraster ==3) fPedADC[3] = pulseIntRaw;
   }


}


//_____________________________________________________________________________
void THcRaster::CalculatePedestals( )
{
  /*
  Use the accumulated pedestal data to calculate pedestals
  From ENGINE/g_analyze_misc.f -

     if (numfr.eq.1000) then
       avefrx = sumfrx / float(numfr)
       avefry = sumfry / float(numfr)
       if (abs(avefrx-gfrx_adc_ped).gt.130.) then
         write(6,*) 'FRPED: peds give <frx>=',gfrx_adc_ped,
  $          '  realevents give <frx>=',avefrx
       endif
       if (abs(avefry-gfry_adc_ped).gt.130.) then
         write(6,*) 'FRPED: peds give <fry>=',gfry_adc_ped,
  $          '  realevents give <fry>=',avefry
       endif
     endif
  */
 /* for(Int_t i=0;i<2;i++){
    fAvgPedADC[i] = fPedADC[i]/ fNPedestalEvents;
    // std::cout<<" raster pedestal "<<fAvgPedADC[i]<<std::endl;
  }
 */
  fFrXAADC_zero_offset = fPedADC[0]/fNPedestalEvents;
  fFrYAADC_zero_offset = fPedADC[1]/fNPedestalEvents;
  fFrXBADC_zero_offset = fPedADC[2]/fNPedestalEvents;
  fFrYBADC_zero_offset = fPedADC[3]/fNPedestalEvents;

}


//_____________________________________________________________________________
Int_t THcRaster::Decode( const THaEvData& evdata )
{

  // loops over all channels defined in the detector map
  // copies raw data into local variables
  // performs pedestal subtraction


  // Get the Hall C style hitlist (fRawHitList) for this event
  Int_t fNhits = THcHitList::DecodeToHitList(evdata);

  // Get the pedestals from the first 1000 events
  //if(fNPedestalEvents < 10)
  if((gHaCuts->Result("Pedestal_event")) & (fNPedestalEvents < 1000)){
      AccumulatePedestals(fRawHitList);
      fAnalyzePedestals = 1;	// Analyze pedestals first normal events
      fNPedestalEvents++;

      return(0);
    }
  if(fAnalyzePedestals) {
    CalculatePedestals();
    fAnalyzePedestals = 0;	// Don't analyze pedestals next event
  }

  Int_t ihit = 0;
  UInt_t nrPosAdcHits = 0;

  while(ihit < fNhits) {
    THcRasterRawHit*  hit          = (THcRasterRawHit *) fRawHitList->At(ihit);
    THcRawAdcHit&     rawPosAdcHit = hit->GetRawAdcHitPos();
    Int_t             nsig         = hit->fCounter;

    for (UInt_t thit=0; thit<rawPosAdcHit.GetNPulses(); ++thit) {
       ((THcSignalHit*) frPosAdcPulseIntRaw->ConstructedAt(nrPosAdcHits))->Set(nsig,rawPosAdcHit.GetPulseIntRaw(thit));
       ++nrPosAdcHits;
    }
    ihit++;
  }

    for (Int_t ielem = 0; ielem < frPosAdcPulseIntRaw->GetEntries(); ielem++) {
       Int_t    nraster           = ((THcSignalHit*) frPosAdcPulseIntRaw->ConstructedAt(ielem))->GetPaddleNumber() - 1;
       Double_t pulseIntRaw       = ((THcSignalHit*) frPosAdcPulseIntRaw->ConstructedAt(ielem))->GetData();
       if (nraster ==0) fRawXAADC = pulseIntRaw;
       if (nraster ==1) fRawYAADC = pulseIntRaw;
       if (nraster ==2) fRawXBADC = pulseIntRaw;
       if (nraster ==3) fRawYBADC = pulseIntRaw;
   }
    return 0;

}

//_____________________________________________________________________________
Int_t THcRaster::Process( ){

gHcParms->LoadParmValues(list);

  /*
    calculate raster position from ADC value.
    From ENGINE/g_analyze_misc.f -

    gfrx_adc = gfrx_raw_adc - gfrx_adc_ped
    gfry_adc = gfry_raw_adc - gfry_adc_ped
  */

  // calculate the raster currents
  fXAADC =  fRawXAADC-fFrXAADC_zero_offset;
  fYAADC =  fRawYAADC-fFrYAADC_zero_offset;

  fXBADC =  fRawXBADC-fFrXBADC_zero_offset;
  fYBADC =  fRawYBADC-fFrYBADC_zero_offset;

  //std::cout<<" Raw X ADC = "<<fXADC<<" Raw Y ADC = "<<fYADC<<std::endl;

  /*
    calculate the raster positions

    gfrx = (gfrx_adc/gfrx_adcpercm)*(gfr_cal_mom/ebeam)
    gfry = (gfry_adc/gfry_adcpercm)*(gfr_cal_mom/ebeam)
  */

  fXApos = (fXAADC/fFrXAADCperCM)*(fFrCalMom/fgpbeam);
  fYApos = (fYAADC/fFrYAADCperCM)*(fFrCalMom/fgpbeam);

  fXBpos = (fXBADC/fFrXBADCperCM)*(fFrCalMom/fgpbeam);
  fYBpos = (fYBADC/fFrYBADCperCM)*(fFrCalMom/fgpbeam);


  // std::cout<<" X = "<<fXpos<<" Y = "<<fYpos<<std::endl;


  Double_t tt;
  Double_t tp;
  if(fgusefr != 0) {
    fPosition[1].SetXYZ(fXApos+fgbeam_xoff, fYApos+fgbeam_yoff, 0.0);
    tt = fXApos/fgfrx_dist+fgbeam_xpoff;
    tp = fYApos/fgfry_dist+fgbeam_ypoff;
  } else {			// Just use fixed beam position and angle
    fPosition[0].SetXYZ(fgbeam_xoff, fgbeam_yoff, 0.0);
    tt = fgbeam_xpoff;
    tp = fgbeam_ypoff;
  }
  fDirection.SetXYZ(tt, tp ,1.0); // Set arbitrarily to avoid run time warnings
  fDirection *= 1.0/TMath::Sqrt(1.0+tt*tt+tp*tp);

  return 0;
}



ClassImp(THcRaster)
////////////////////////////////////////////////////////////////////////////////
