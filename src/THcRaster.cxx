// Author : Buddhini Waidyawansa
// Date : 12-09-2013

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcRaster                                                                 //
//                                                                           //
//  A class to decode the fast raster signals.                               //
//  Measures the two magnet currents which are propotioanl to horizontal and //
//  vertical beam position                                                   //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THcRaster.h"
#include "THaEvData.h"
#include "THaDetMap.h"


#include "THcParmList.h"
#include "THcGlobals.h"
#include "THaGlobals.h"


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
  fXADC = 0;
  fYADC = 0;
  fXpos = 0;
  fYpos = 0;
  fFrCalMom = 0;
  fFrXADCperCM = 0;
  fFrXADCperCM = 0;

  for(Int_t i=0;i<2;i++){
    fPedADC[i] = 0;
    fAvgPedADC[i] = 0;
  }
}


//_____________________________________________________________________________
THcRaster::~THcRaster()
{
  //  delete [] fPedADC;
  //  delete [] fAvgPedADC;

}



//____________________________________________________________________________// _
// void THcRaster::InitializeReconstruction()
// {

// }
  


//_____________________________________________________________________________
Int_t THcRaster::ReadDatabase( const TDatime& date )
{

  // Read parameters such as calibration factor, of this detector from the database.
  // static const char* const here = "THcRaster::ReadDatabase";
 
  // InitializeReconstruction();

  char prefix[2];

  //cout << " THcRaster::ReadDatabase GetName() called " << GetName() << endl;
  //  prefix[0]=tolower(GetName()[0]);
  // bpw- The prefix is hardcoded so that we don't have to change the gbeam.param file. o/w to get the following variables, we need to change to parameter names to rfr_cal_mom, etc where "r" comes from prefix[0]=tolower(GetName()[0]).
  prefix[0]='g';
  prefix[1]='\0';


  // string names;
  DBRequest list[]={
    {"fr_cal_mom",&fFrCalMom, kDouble},
    {"frx_adcpercm",&fFrXADCperCM, kDouble},
    {"fry_adcpercm",&fFrYADCperCM, kDouble},
    {"pbeam",&fgpbeam, kDouble},
    {0}
  };

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
    {"xcurrent",  "Raster X current",    "fXADC"},
    {"ycurrent",  "Raster Y current",    "fYADC"},
    {"xpos",  "Raster X position",    "fXpos"},
    {"ypos",  "Raster Y position",    "fYpos"},
    { 0 }
  };

  return DefineVarsFromList( vars, mode );
}

//_____________________________________________________________________________
THaAnalysisObject::EStatus THcRaster::Init( const TDatime& date )
{
  cout << "THcRaster::Init()" << endl;

  THcHitList::InitHitList(fDetMap,"THcRasterRawHit",4);

  EStatus status;
  if( (status = THaBeamDet::Init( date )) )
    return fStatus=status;

  // Fill detector map with RASTER type channels
  if( gHcDetectorMap->FillMap(fDetMap, "RASTER") < 0 ) {
    static const char* const here = "Init()";
    Error( Here(here), "Error filling detectormap for %s.", 
  	   "RASTER");
    return kInitError;
  }

  return fStatus = kOK;

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

  while(ihit<nrawhits) {
    THcRasterRawHit* hit = (THcRasterRawHit *) fRawHitList->At(ihit);
    if(hit->fADC_xsig>0) {
      fPedADC[0] += hit->fADC_xsig;
      //std::cout<<" raster x pedestal collect "<<fPedADC[0]<<std::endl;
    }
    if(hit->fADC_ysig>0) {
      fPedADC[1] += hit->fADC_ysig;
      //std::cout<<" raster y pedestal collect "<<fPedADC[1]<<std::endl;
    }

    ihit++;
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
  for(Int_t i=0;i<2;i++){
    fAvgPedADC[i] = fPedADC[i]/ fNPedestalEvents; 
    // std::cout<<" raster pedestal "<<fAvgPedADC[i]<<std::endl;
  }

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
  
  while(ihit < fNhits) {
    THcRasterRawHit* hit = (THcRasterRawHit *) fRawHitList->At(ihit);
    
    if(hit->fADC_xsig>0) {
      fRawADC[0] = hit->fADC_xsig;
      // std::cout<<" Raw X = "<<fRawADC[0]<<std::endl;
    }
    
    if(hit->fADC_ysig>0) {
      fRawADC[1] = hit->fADC_ysig;
      // std::cout<<" Raw Y ADC = "<<fRawADC[1]<<std::endl;
    } 
    ihit++;
  }  

  return 0;
  
}





//_____________________________________________________________________________
Int_t THcRaster::Process( ){

  Double_t eBeam = 0.001;
  /*
    calculate raster position from ADC value.
    From ENGINE/g_analyze_misc.f -
    
    gfrx_adc = gfrx_raw_adc - gfrx_adc_ped
    gfry_adc = gfry_raw_adc - gfry_adc_ped
  */

  // calculate the raster currents
  fXADC =  fRawADC[0]-fAvgPedADC[0];
  fYADC =  fRawADC[1]-fAvgPedADC[1];
  //std::cout<<" Raw X ADC = "<<fXADC<<" Raw Y ADC = "<<fYADC<<std::endl;

  /*
    calculate the raster positions

    gfrx = (gfrx_adc/gfrx_adcpercm)*(gfr_cal_mom/ebeam)
    gfry = (gfry_adc/gfry_adcpercm)*(gfr_cal_mom/ebeam)
  */
 
  if(gHcParms->Find("gpbeam")){
    eBeam=*(Double_t *)gHcParms->Find("gpbeam")->GetValuePointer();
  }
  fXpos = (fXADC/fFrXADCperCM)*(fFrCalMom/eBeam);
  fYpos = (fYADC/fFrYADCperCM)*(fFrCalMom/eBeam);

  // std::cout<<" X = "<<fXpos<<" Y = "<<fYpos<<std::endl;
  
  fDirection.SetXYZ(0.0,0.0,1.0); // Set arbitrarily to avoid run time warnings

  return 0;
}



ClassImp(THcRaster)
////////////////////////////////////////////////////////////////////////////////
