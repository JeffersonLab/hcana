/** \class THcRaster
    \ingroup Detectors

    \brief Detector class for fast raster.

    Measures the two magnet currents which are proportional to horizontal and
    vertical beam position

    \author Buddhini Waidyawansa 
    \author Burcu Duran - Melanie Rehfuss (2017)

*/
#include "TMath.h"

#include "THcRaster.h"
#include "THaEvData.h"
#include "THaDetMap.h"
#include "THcAnalyzer.h"

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
  for (Int_t nt=0;nt<4;nt++) {
  fXbeam_prev[nt] = 0;
  fYbeam_prev[nt] = 0;
  }
  fXpbeam_prev = 0;
  fYpbeam_prev = 0;
  BPMXA_raw = 0;
  BPMYA_raw = 0;
  BPMXB_raw = 0;
  BPMYB_raw = 0;
  BPMXC_raw = 0;
  BPMYC_raw = 0;
  BPMXA_pos = 0;
  BPMYA_pos = 0;
  BPMXB_pos = 0;
  BPMYB_pos = 0;
  BPMXC_pos = 0;
  BPMYC_pos = 0;
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
  fEbeamEpics_read=0.;
  fEbeamEpics_prev=0.;
  fEbeamEpics=0.;
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
  //cout << "In THcRaster::Init()" << endl;

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

  //cout << "THcRaster::ReadDatabase()" << endl;

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
    {"pbeam",&fgpbeam, kDouble,0,1},
    {"frx_dist", &fgfrx_dist, kDouble},
    {"fry_dist", &fgfry_dist, kDouble},
    {"beam_x", &fgbeam_xoff, kDouble,0,1},
    {"beam_xp", &fgbeam_xpoff, kDouble,0,1},
    {"beam_y", &fgbeam_yoff, kDouble,0,1},
    {"beam_yp", &fgbeam_ypoff, kDouble,0,1},
    {"bpmxa_slope", &fgbpmxa_slope, kDouble,0,1},
    {"bpmxa_off", &fgbpmxa_off, kDouble,0,1},
    {"bpmxb_slope", &fgbpmxb_slope, kDouble,0,1},
    {"bpmxb_off", &fgbpmxb_off, kDouble,0,1},
    {"bpmxc_slope", &fgbpmxc_slope, kDouble,0,1},
    {"bpmxc_off", &fgbpmxc_off, kDouble,0,1},
    {"bpmya_slope", &fgbpmya_slope, kDouble,0,1},
    {"bpmya_off", &fgbpmya_off, kDouble,0,1},
    {"bpmyb_slope", &fgbpmyb_slope, kDouble,0,1},
    {"bpmyb_off", &fgbpmyb_off, kDouble,0,1},
    {"bpmyc_slope", &fgbpmyc_slope, kDouble,0,1},
    {"bpmyc_off", &fgbpmyc_off, kDouble,0,1},
    {"bpma_zpos", &fgbpma_zpos, kDouble,0,1},
    {"bpmb_zpos", &fgbpmb_zpos, kDouble,0,1},
    {"bpmc_zpos", &fgbpmc_zpos, kDouble,0,1},
    {"usefr", &fgusefr, kInt,0,1},
    {0}
  };
  fgpbeam = 0.001;
 //
  //positions of BPMs relative to target (from Fall 2018 survey)
  fgbpma_zpos = 320.17;
  fgbpmb_zpos = 224.81 ;// cm
    fgbpmc_zpos = 129.38 ;// cm
  // Default offsets to zero and slopes to +/- 1
  fgbeam_xoff = -999.;
  fgbeam_xpoff =-999.;
  fgbeam_yoff = -999.;
  fgbeam_ypoff =-999.;
  //
  fgbpmxa_off = 0.0;
  fgbpmxa_slope = -1.0;
  fgbpmxb_off = 0.0;
  fgbpmxb_slope = -1.0;
  fgbpmxc_off = 0.0;
  fgbpmxc_slope = -1.0;
  fgbpmya_off = 0.0;
  fgbpmya_slope = 1.0;
  fgbpmyb_off = 0.0;
  fgbpmyb_slope = 1.0;
  fgbpmyc_off = 0.0;
  fgbpmyc_slope = 1.0;
  fgusefr = 0;
 
  
  // get the calibration factors from gbeam.param file
  gHcParms->LoadParmValues((DBRequest*)&list,prefix);

  frPosAdcPulseIntRaw  = new TClonesArray("THcSignalHit", 4);

  THcAnalyzer *analyzer = dynamic_cast<THcAnalyzer*>(THcAnalyzer::GetInstance());
  fEpicsHandler = analyzer->GetEpicsEvtHandler();
  
  //
  fFlag_use_EPICS_bpm = kFALSE;
  if (fgbeam_xoff ==-999. && fgbeam_yoff ==-999.) {
    fFlag_use_EPICS_bpm = kTRUE;
    cout << " THcRaster is using EPICS bpm for beam position" << endl;    
  } else {
    if (fgbeam_xoff ==-999.) fgbeam_xoff = 0.;
    if (fgbeam_yoff ==-999.) fgbeam_yoff = 0.;
    if (fgbeam_xpoff ==-999.) fgbeam_xpoff = 0.;
    if (fgbeam_ypoff ==-999.) fgbeam_ypoff = 0.;
    cout << " THcRaster is using parameters for  beam position" << " x = " << fgbeam_xoff<< " y = " << fgbeam_yoff<< endl;    
    cout << " THcRaster is using parameters for  beam angle position" << " xp = " << fgbeam_xpoff<< " yp = " << fgbeam_ypoff<< endl;    
    
  }


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
    {"fr_xbpm_tar",  "X BPM at target (+X is beam right)",    "fXbpm_tar"},
    {"fr_ybpm_tar",  "Y BPM at target (+Y is up)",    "fYbpm_tar"},
    {"fr_xbpmA",  "X BPM at BPMA (+X is beam right)",    "fXbpm_A"},
    {"fr_ybpmA",  "Y BPM at BPMA (+Y is up)",    "fYbpm_A"},
    {"fr_xbpmB",  "X BPM at BPMB (+X is beam right)",    "fXbpm_B"},
    {"fr_ybpmB",  "Y BPM at BPMB (+Y is up)",    "fYbpm_B"},
    {"fr_xbpmC",  "X BPM at BPMC (+X is beam right)",    "fXbpm_C"},
    {"fr_ybpmC",  "Y BPM at BPMC (+Y is up)",    "fYbpm_C"},
    {"ebeam_epics",  "Beam energy of epics variable HALLC:p",    "fEbeamEpics"},
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
  //cout << "THcRaster::AccumulatePedestels()" << endl;

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
    //cout << "THcRaster::CalculatePedestels()" << endl;
  
    fFrYA_ADC_zero_offset = fPedADC[0]/ fNPedestalEvents;
    fFrXA_ADC_zero_offset = fPedADC[1]/ fNPedestalEvents;
    fFrYB_ADC_zero_offset = fPedADC[2]/ fNPedestalEvents;
    fFrXB_ADC_zero_offset = fPedADC[3]/ fNPedestalEvents;
    
  
}


//_____________________________________________________________________________
Int_t THcRaster::Decode( const THaEvData& evdata )
{

  //cout << "THcRaster::Decode()" << endl;
  // Get the Hall C style hitlist (fRawHitList) for this event

  fNhits = DecodeToHitList(evdata);


  // Get the pedestals from the first 1000 events
  //if(fNPedestalEvents < 10)
  if(gHaCuts->Result("Pedestal_event") && (fNPedestalEvents < 1000)) {
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
    if (rawPosAdcHit.GetNPulses()==0 &&rawPosAdcHit.GetNSamples()>0 ) {
	Int_t NSA= rawPosAdcHit.GetF250_NSA();
      UInt_t LS = 0;
      UInt_t HS = NSA;
      Int_t rawdata = rawPosAdcHit.GetIntegral(LS,HS);
      ((THcSignalHit*) frPosAdcPulseIntRaw->ConstructedAt(nrPosAdcHits))->Set(nsig,rawdata );
       ++nrPosAdcHits;
    }     
    
     ihit++;
  }

   for(Int_t ielem = 0; ielem < frPosAdcPulseIntRaw->GetEntries(); ielem++) {
       Int_t    nraster        = ((THcSignalHit*) frPosAdcPulseIntRaw->ConstructedAt(ielem))->GetPaddleNumber() - 1;
        Double_t pulseIntRaw  = ((THcSignalHit*) frPosAdcPulseIntRaw->ConstructedAt(ielem))->GetData();
        if (nraster ==0) FRYA_rawadc = pulseIntRaw;
        if (nraster ==1) FRXA_rawadc = pulseIntRaw;
        if (nraster ==2) FRYB_rawadc = pulseIntRaw;
        if (nraster ==3) FRXB_rawadc = pulseIntRaw;
   }

   if (fEpicsHandler) {
           if (fEpicsHandler->IsLoaded("IPM3H07A.XRAW")){
            BPMXA_raw = atof(fEpicsHandler->GetString("IPM3H07A.XRAW"));
           }
           if (fEpicsHandler->IsLoaded("IPM3H07A.YRAW")){
            BPMYA_raw = atof(fEpicsHandler->GetString("IPM3H07A.YRAW"));
           }
           if (fEpicsHandler->IsLoaded("IPM3H07B.XRAW")){
            BPMXB_raw = atof(fEpicsHandler->GetString("IPM3H07B.XRAW"));
           }
           if (fEpicsHandler->IsLoaded("IPM3H07B.YRAW")){
            BPMYB_raw = atof(fEpicsHandler->GetString("IPM3H07B.YRAW"));
           }
           if (fEpicsHandler->IsLoaded("IPM3H07C.XRAW")){
            BPMXC_raw = atof(fEpicsHandler->GetString("IPM3H07C.XRAW"));
           }
           if (fEpicsHandler->IsLoaded("IPM3H07C.YRAW")){
            BPMYC_raw = atof(fEpicsHandler->GetString("IPM3H07C.YRAW"));
           }
           if (fEpicsHandler->IsLoaded("HALLC:p")){
            fEbeamEpics_read = atof(fEpicsHandler->GetString("HALLC:p"));
           }
   }

  return 0;

}

//_____________________________________________________________________________
Int_t THcRaster::Process(){

  //cout << "In THcRaster::Process()" << endl;

  /*
    calculate raster position from ADC value.
    From ENGINE/g_analyze_misc.f -

    gfrx_adc = gfrx_raw_adc - gfrx_adc_ped
    gfry_adc = gfry_raw_adc - gfry_adc_ped
  */

  //std::cout << "Beam energy = " << fgpbeam << std::endl;
  //std::cout << "Raster Calibration Momentum = " << fFrCalMom << std::endl;
  //std::cout << "Raster X calibration per cm = " << fFrXA_ADCperCM << std::endl;
  //std::cout << "Raster Y calibration per cm = " << fFrYA_ADCperCM << std::endl;
  //std::cout << "Raster Offsets: " << "XA = " << fFrXA_ADC_zero_offset << " YA = " << fFrYA_ADC_zero_offset<< std::endl;
  //std::cout << "Raster Offsets: " << "XB = " << fFrXB_ADC_zero_offset << " YB = " << fFrYB_ADC_zero_offset<< std::endl;
  // calculate the raster currents
  fXA_ADC =  FRXA_rawadc-fFrXA_ADC_zero_offset;
  fYA_ADC =  FRYA_rawadc-fFrYA_ADC_zero_offset;
  fXB_ADC =  FRXB_rawadc-fFrXB_ADC_zero_offset;
  fYB_ADC =  FRYB_rawadc-fFrYB_ADC_zero_offset;
  

  /*
    calculate the raster positions

    gfrx = (gfrx_adc/gfrx_adcpercm)*(gfr_cal_mom/ebeam)
    gfry = (gfry_adc/gfry_adcpercm)*(gfr_cal_mom/ebeam)
  */

  fXA_pos = (fXA_ADC/fFrXA_ADCperCM)*(fFrCalMom/fgpbeam);
  fYA_pos = (-1.)*(fYA_ADC/fFrYA_ADCperCM)*(fFrCalMom/fgpbeam);
  fXB_pos = (fXB_ADC/fFrXB_ADCperCM)*(fFrCalMom/fgpbeam);
  fYB_pos = (-1.)*(fYB_ADC/fFrYB_ADCperCM)*(fFrCalMom/fgpbeam);


  Bool_t checkBPM = (BPMXA_raw == 0) && (BPMYA_raw == 0) && (BPMXB_raw == 0) &&
	  		(BPMYB_raw == 0) && (BPMXC_raw ==0) && (BPMYC_raw == 0);
  //cout << "BPM's working? " << checkBPM << endl;

  // Factor of 0.1 is to convert from mm to cm
  // BPM's are typically calibrated in mm, whereas the standard distances are in cm
  // in the analyzer.
  //
  BPMXA_pos = 0.1*(fgbpmxa_slope*BPMXA_raw + fgbpmxa_off);
  BPMXB_pos = 0.1*(fgbpmxb_slope*BPMXB_raw + fgbpmxb_off);
  BPMXC_pos = 0.1*(fgbpmxc_slope*BPMXC_raw + fgbpmxc_off);
  BPMYA_pos = 0.1*(fgbpmya_slope*BPMYA_raw + fgbpmya_off);
  BPMYB_pos = 0.1*(fgbpmyb_slope*BPMYB_raw + fgbpmyb_off);
  BPMYC_pos = 0.1*(fgbpmyc_slope*BPMYC_raw + fgbpmyc_off);
  
  //cout << "BPMXA_pos = " << BPMXA_pos << endl;
  //cout << "BPMYA_pos = " << BPMYA_pos << endl;
  //cout << "BPMXB_pos = " << BPMXB_pos << endl;
  //cout << "BPMYB_pos = " << BPMYB_pos << endl;
  //cout << "BPMXC_pos = " << BPMXC_pos << endl;
  //cout << "BPMYC_pos = " << BPMYC_pos << endl;

  // Calculate position and direction at target from BPM values
  // Use the A and C BPM information, as these are located downstream of the raster
  // magnets.  If there is no BPM information available, assume zero offsets.
  //
  Double_t xbeam;
  Double_t ybeam;
  Double_t xpbeam;
  Double_t ypbeam;
  if (!checkBPM){
  	xbeam = BPMXA_pos - (BPMXA_pos - BPMXC_pos)/(fgbpma_zpos - fgbpmc_zpos)*fgbpma_zpos;
  	ybeam = BPMYA_pos - (BPMYA_pos - BPMYC_pos)/(fgbpma_zpos - fgbpmc_zpos)*fgbpma_zpos;
	xpbeam = (xbeam-BPMXA_pos)/fgbpma_zpos;
	ypbeam = (ybeam-BPMYA_pos)/fgbpma_zpos;
        fXbeam_prev[0]=xbeam;
        fYbeam_prev[0]=ybeam;
        fXpbeam_prev=xpbeam;
        fYpbeam_prev=ypbeam;
        fXbeam_prev[1]=BPMXA_pos;
        fYbeam_prev[1]=BPMYA_pos;
        fXbeam_prev[2]=BPMXB_pos;
        fYbeam_prev[2]=BPMYB_pos;
        fXbeam_prev[3]=BPMXC_pos;
        fYbeam_prev[3]=BPMYC_pos;
        fEbeamEpics = fEbeamEpics_read;
	fEbeamEpics_prev=fEbeamEpics_read;
 }else{
	  xbeam = fXbeam_prev[0];
	  ybeam = fYbeam_prev[0];
	  xpbeam = fXpbeam_prev;
	  ypbeam = fYpbeam_prev;
        BPMXA_pos=fXbeam_prev[1];
        BPMYA_pos=fYbeam_prev[1];
        BPMXB_pos=fXbeam_prev[2];
        BPMYB_pos=fYbeam_prev[2];
        BPMXC_pos=fXbeam_prev[3];
        BPMYC_pos=fYbeam_prev[3];
         fEbeamEpics = fEbeamEpics_prev;
 }


  if (fFlag_use_EPICS_bpm) {
    fXbpm_tar= -xbeam; // assumes the BPM calibration gives BPM with +X beam left (HARP coordinate system)
    fYbpm_tar= ybeam;
    fXpbpm_tar= -xpbeam; // assumes the BPM calibration gives BPM with +X beam left (HARP coordinate system)
    fYpbpm_tar= ypbeam;
    
  } else {
    fXbpm_tar= fgbeam_xoff; //  +X beam right (EPICS coordinate system)
    fYbpm_tar= fgbeam_yoff;
    fXpbpm_tar= fgbeam_xpoff; //  +X beam right (EPICS coordinate system)
    fYpbpm_tar= fgbeam_ypoff;
  }


  fXbpm_A= -fXbeam_prev[1]; 
  fYbpm_A= fYbeam_prev[1];
  fXbpm_B= -fXbeam_prev[2]; 
  fYbpm_B= fYbeam_prev[2];
  fXbpm_C= -fXbeam_prev[3]; 
  fYbpm_C= fYbeam_prev[3];
  //std::cout<<" XA = "<<fXA_pos<<" YA = "<<fYA_pos<<std::endl;
  //std::cout<<" XB = "<<fXB_pos<<" YB = "<<fYB_pos<<std::endl;

  //std::cout << "Use FR? " << fgusefr << std::endl;

  //std::cout << "BPM X at Z=0 -> " << fgbeam_xoff << std::endl;
  //std::cout << "BPM Y at Z=0 -> " << fgbeam_yoff << std::endl;
  //std::cout << "BPM XP at Z=0 -> " << fgbeam_xpoff << std::endl;
  //std::cout << "BPM YP at Z=0 -> " << fgbeam_ypoff << std::endl;

  //std::cout << "Raster X distance = " << fgfrx_dist << std::endl;
  //std::cout << "Raster Y distance = " << fgfry_dist << std::endl;

  Double_t tt = fXpbpm_tar;
  Double_t tp = fYpbpm_tar;

  if(fgusefr != 0) {
    fPosition[0].SetXYZ(-fXbpm_tar, fYbpm_tar, 0.0);
    fPosition[1].SetXYZ((-1.)*(fXA_pos)-fXbpm_tar, fYA_pos+fYbpm_tar, 0.0);
    fPosition[2].SetXYZ((-1.)*(fXB_pos)-fXbpm_tar, fYB_pos+fYbpm_tar, 0.0);
    tt = (-1.)*fXA_pos/fgfrx_dist-fXpbpm_tar;
    tp = fYA_pos/fgfry_dist+fYpbpm_tar;
    fDirection.SetXYZ(tt, tp ,1.0); // Set arbitrarily to avoid run time warnings
    fDirection *= 1.0/TMath::Sqrt(1.0+tt*tt+tp*tp);
  } else {			// Just use fixed beam position and angle
    fPosition[0].SetXYZ(-fXbpm_tar, fYbpm_tar, 0.0);
    fPosition[1].SetXYZ(-fXbpm_tar, fYbpm_tar, 0.0);
    fPosition[2].SetXYZ(-fXbpm_tar, fYbpm_tar, 0.0);
    fDirection.SetXYZ(tt, tp ,1.0); // Set arbitrarily to avoid run time warnings
    fDirection *= 1.0/TMath::Sqrt(1.0+tt*tt+tp*tp);
  }
  /*  
  fXA_pos = fPosition[1](0);
  fYA_pos = fPosition[1](1);
  fXB_pos = fPosition[2](0);
  fYB_pos = fPosition[2](1);
  */
  //std::cout<<" Setting fPosition and fDirection ... " << std::endl;
  //std::cout<< fPosition[0](0) << " " << fPosition[0](1) << " " << fPosition[0](2) << std::endl;
  //std::cout<< fPosition[1](0) << " " << fPosition[1](1) << " " << fPosition[1](2) << std::endl;
  //std::cout<< fPosition[2](0) << " " << fPosition[2](1) << " " << fPosition[2](2) << std::endl;
  //std::cout<< fDirection(0) << " " << fDirection(1) << " " << fDirection(2) << std::endl;

  //std::cout<<" FRXA = "<<fXA_pos<<" FRYA = "<<fYA_pos<<std::endl;
  //std::cout<<" FRXB = "<<fXB_pos<<" FRYB = "<<fYB_pos<<std::endl;
  
  return 0;
}



ClassImp(THcRaster)
////////////////////////////////////////////////////////////////////////////////
