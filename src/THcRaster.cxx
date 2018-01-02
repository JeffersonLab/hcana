/** \class THcRaster
    \ingroup DetSupport

  A class to 
 the fast raster signals.
  Measures the two magnet currents which are proportional to horizontal and
  vertical beam position

\author Buddhini Waidyawansa 
\author Burcu Duran - Melanie Rehfuss (2017)

*/
#include "TMath.h"

#include "THcRaster.h"
#include "THaEvData.h"
#include "THaDetMap.h"


#include "THcParmList.h"
#include "THcGlobals.h"
#include "THaGlobals.h"
#include "THaApparatus.h"
#include "THcRawAdcHit.h"
#include "THcSignalHit.h"

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
  FRXA_rawadc = 0;
  FRXB_rawadc = 0;
  FRYA_rawadc = 0;
  FRYB_rawadc = 0;
  fXA_ADC = 0;
  fYA_ADC = 0;
  fXB_ADC = 0;
  fYB_ADC = 0;
  fXA_pos = 0;
  fYA_pos = 0;
  fXB_pos = 0;
  fYB_pos = 0;
  fFrCalMom = 0;
  fFrXA_ADCperCM = 1.0;
  fFrYA_ADCperCM = 1.0;
  fFrXB_ADCperCM = 1.0;
  fFrYB_ADCperCM = 1.0;
  fFrXA_ADC_zero_offset = 0;
  fFrXB_ADC_zero_offset =0;
  fFrYA_ADC_zero_offset =0;
  fFrYB_ADC_zero_offset =0;

  frPosAdcPulseIntRaw  = NULL;

  for(Int_t i=0;i<4;i++){

	fPedADC[i] = 0;
    }

  InitArrays();
}

//_____________________________________________________________________________
THcRaster::THcRaster( ) :
  THaBeamDet("THcRaster") // no default constructor available
{

 frPosAdcPulseIntRaw  = NULL;

  InitArrays();
  
  
}
//_____________________________________________________________________________
THcRaster::~THcRaster()
{

// Destructor
  
  delete frPosAdcPulseIntRaw;  frPosAdcPulseIntRaw  = NULL;

  DeleteArrays();
}

//_____________________________________________________________________________

THaAnalysisObject::EStatus THcRaster::Init( const TDatime& date )
{
  //cout << "THcRaster::Init()" << endl;

  char EngineDID[] = "xRASTER";
  EngineDID[0] = toupper(GetApparatus()->GetName()[0]);
  
  
  // Fill detector map with RASTER type channels
  if( gHcDetectorMap->FillMap(fDetMap, EngineDID) < 0 ) {
    static const char* const here = "Init()";
     Error( Here(here), "Error filling detectormap for %s.", EngineDID);

    return kInitError;
  }

  InitHitList(fDetMap,"THcRasterRawHit",fDetMap->GetTotNumChan()+1);

  EStatus status;
  if( (status = THaBeamDet::Init( date )) )
    return fStatus=status;

  return fStatus = kOK;

}

//_____________________________________________________________________________
Int_t THcRaster::ReadDatabase( const TDatime& date )

{

  // Read parameters such as calibration factor, of this detector from the database.
  //cout << "THcRaster::ReadDatabase()" << endl;

  char prefix[2];

  //cout << " THcRaster::ReadDatabase GetName() called " << GetName() << endl;
  //  prefix[0]=tolower(GetName()[0]);
  // bpw- The prefix is hardcoded so that we don't have to change the gbeam.param file. o/w to get the following variables, we need to change to parameter names to rfr_cal_mom, etc where "r" comes from prefix[0]=tolower(GetName()[0]).
  prefix[0]='g';
  prefix[1]='\0';

  

  // string names;
  DBRequest list[]={
    {"fr_cal_mom",&fFrCalMom, kDouble},
    {"frxa_adcpercm",&fFrXA_ADCperCM, kDouble},
    {"frya_adcpercm",&fFrYA_ADCperCM, kDouble},
    {"frxb_adcpercm",&fFrXB_ADCperCM, kDouble},
    {"fryb_adcpercm",&fFrYB_ADCperCM, kDouble},
    {"frxa_adc_zero_offset",&fFrXA_ADC_zero_offset,kDouble},
    {"frya_adc_zero_offset",&fFrYA_ADC_zero_offset,kDouble},
    {"frxb_adc_zero_offset",&fFrXB_ADC_zero_offset,kDouble},
    {"fryb_adc_zero_offset",&fFrYB_ADC_zero_offset,kDouble},
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

  frPosAdcPulseIntRaw  = new TClonesArray("THcSignalHit", 4);

  return kOK;

}


//_____________________________________________________________________________
Int_t THcRaster::DefineVariables( EMode mode )
{
  // Initialize global variables for histogramming and tree

  //cout << "THcRaster::DefineVariables called " << GetName() << endl;

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  // Register variables in global list

  RVarDef vars[] = {
    {"frxaRawAdc",  "Raster XA raw ADC",    "FRXA_rawadc"},
    {"fryaRawAdc",  "Raster YA raw ADC",    "FRYA_rawadc"},
    {"frxbRawAdc",  "Raster XB raw ADC",    "FRXB_rawadc"},
    {"frybRawAdc",  "Raster YB raw ADC",    "FRYB_rawadc"},
    {"frxa_adc",  "Raster XA ADC",    "fXA_ADC"},
    {"frya_adc",  "Raster YA ADC",    "fYA_ADC"},
    {"frxb_adc",  "Raster XB ADC",    "fXB_ADC"},
    {"fryb_adc",  "Raster YB ADC",    "fYB_ADC"},
    {"fr_xa",  "Raster XA position",    "fXA_pos"},
    {"fr_ya",  "Raster YA position",    "fYA_pos"},
    {"fr_xb",  "Raster XB position",    "fXB_pos"},
    {"fr_yb",  "Raster YB position",    "fYB_pos"},
    { 0 }
  };

  return DefineVarsFromList( vars, mode );
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

   Int_t ihit = 0;
   UInt_t nrPosAdcHits=0;

   while(ihit < nrawhits) {
     THcRasterRawHit* hit = (THcRasterRawHit *) fRawHitList->At(ihit);
     THcRawAdcHit&  rawPosAdcHit = hit->GetRawAdcHitPos();
     Int_t          nsig         = hit->fCounter;
      
     for (UInt_t thit=0; thit<rawPosAdcHit.GetNPulses(); ++thit) {
       ((THcSignalHit*) frPosAdcPulseIntRaw->ConstructedAt(nrPosAdcHits))->Set(nsig, rawPosAdcHit.GetPulseIntRaw(thit));
       ++nrPosAdcHits;
     }
    
     ihit++;
  }


   for(Int_t ielem = 0; ielem < frPosAdcPulseIntRaw->GetEntries(); ielem++) {
       Int_t    nraster        = ((THcSignalHit*) frPosAdcPulseIntRaw->ConstructedAt(ielem))->GetPaddleNumber() - 1;
        Double_t pulseIntRaw  = ((THcSignalHit*) frPosAdcPulseIntRaw->ConstructedAt(ielem))->GetData();
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
  
    fFrXA_ADC_zero_offset = fPedADC[0]/ fNPedestalEvents;
    fFrXB_ADC_zero_offset = fPedADC[1]/ fNPedestalEvents;
    fFrYA_ADC_zero_offset = fPedADC[2]/ fNPedestalEvents;
    fFrYB_ADC_zero_offset = fPedADC[3]/ fNPedestalEvents;
    
  
}


//_____________________________________________________________________________
Int_t THcRaster::Decode( const THaEvData& evdata )
{

  
  // Get the Hall C style hitlist (fRawHitList) for this event

  fNhits = DecodeToHitList(evdata);


  // Get the pedestals from the first 1000 events
  //if(fNPedestalEvents < 10)
  if(gHaCuts->Result("Pedestal_event") & (fNPedestalEvents < 1000)) {
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
   UInt_t nrPosAdcHits=0;

   while(ihit < fNhits) {
     THcRasterRawHit* hit = (THcRasterRawHit *) fRawHitList->At(ihit);
     THcRawAdcHit&  rawPosAdcHit = hit->GetRawAdcHitPos();
     Int_t          nsig         = hit->fCounter;
      
     for (UInt_t thit=0; thit<rawPosAdcHit.GetNPulses(); ++thit) {
       ((THcSignalHit*) frPosAdcPulseIntRaw->ConstructedAt(nrPosAdcHits))->Set(nsig, rawPosAdcHit.GetPulseIntRaw(thit));
       ++nrPosAdcHits;
     }
     
     ihit++;
  }


   for(Int_t ielem = 0; ielem < frPosAdcPulseIntRaw->GetEntries(); ielem++) {
       Int_t    nraster        = ((THcSignalHit*) frPosAdcPulseIntRaw->ConstructedAt(ielem))->GetPaddleNumber() - 1;
        Double_t pulseIntRaw  = ((THcSignalHit*) frPosAdcPulseIntRaw->ConstructedAt(ielem))->GetData();
        if (nraster ==0) FRXA_rawadc = pulseIntRaw;
        if (nraster ==1) FRXB_rawadc = pulseIntRaw;
        if (nraster ==2) FRYA_rawadc = pulseIntRaw;
        if (nraster ==3) FRYB_rawadc = pulseIntRaw;
   }




  return 0;

}





//_____________________________________________________________________________
Int_t THcRaster::Process( ){

  Double_t fgpBeam = 0.001;
  DBRequest list[] = {
    {"gpbeam", &fgpBeam, kDouble, 0, 1},
    {0}
  };
gHcParms->LoadParmValues(list);
  /*
    calculate raster position from ADC value.
    From ENGINE/g_analyze_misc.f -

    gfrx_adc = gfrx_raw_adc - gfrx_adc_ped
    gfry_adc = gfry_raw_adc - gfry_adc_ped
  */

  // calculate the raster currents
  fXA_ADC =  FRXA_rawadc-fFrXA_ADC_zero_offset;
  fYA_ADC =  FRYA_rawadc-fFrYA_ADC_zero_offset;
  fXB_ADC =  FRXB_rawadc-fFrXB_ADC_zero_offset;
  fYB_ADC =  FRYB_rawadc-fFrYB_ADC_zero_offset;
  
  //std::cout<<" Raw X ADC = "<<fXADC<<" Raw Y ADC = "<<fYADC<<std::endl;

  /*
    calculate the raster positions

    gfrx = (gfrx_adc/gfrx_adcpercm)*(gfr_cal_mom/ebeam)
    gfry = (gfry_adc/gfry_adcpercm)*(gfr_cal_mom/ebeam)
  */

  fXA_pos = (fXA_ADC/fFrXA_ADCperCM)*(fFrCalMom/fgpBeam);
  fYA_pos = (fYA_ADC/fFrYA_ADCperCM)*(fFrCalMom/fgpBeam);
  fXB_pos = (fXB_ADC/fFrXB_ADCperCM)*(fFrCalMom/fgpBeam);
  fYB_pos = (fYB_ADC/fFrYB_ADCperCM)*(fFrCalMom/fgpBeam);

  // std::cout<<" X = "<<fXpos<<" Y = "<<fYpos<<std::endl;


  Double_t tt;
  Double_t tp;
  if(fgusefr != 0) {
    fPosition[1].SetXYZ(fXA_pos+fgbeam_xoff, fYA_pos+fgbeam_yoff, 0.0);
    tt = fXA_pos/fgfrx_dist+fgbeam_xpoff;
    tp = fYA_pos/fgfry_dist+fgbeam_ypoff;
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
