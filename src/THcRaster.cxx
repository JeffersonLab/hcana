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
  fRawPos[0]=0;
  fRawPos[1]=0;

  // fRasterXADC = new TClonesArray("THcSignalHit",16);
  // fRasterYADC = new TClonesArray("THcSignalHit",16);
}


//_____________________________________________________________________________
THcRaster::~THcRaster()
{
  // Distructor
}


//_____________________________________________________________________________
void THcRaster::ClearEvent(){
  // nothing for now
}

//_____________________________________________________________________________
void THcRaster::InitializeReconstruction()
{

}
  


//_____________________________________________________________________________
Int_t THcRaster::ReadDatabase( const TDatime& date )
{

  // Read parameters such as calibration factor, of this detector from the database.
  // static const char* const here = "THcRaster::ReadDatabase";
 
  // InitializeReconstruction();

  // char prefix[2];

  // cout << " THcRaster::ReadDatabase GetName() called " << GetName() << endl;

  // prefix[0]=tolower(GetName()[0]);
  // prefix[1]='\0';


  // string names;
  // DBRequest list[]={
  //   {"raster_channel_name",&names,kString},
  //   {"raster_channel_number",&names,kString},
  //   {0}
  // };

  // // get the channel assignments from the parameter file
  // gHcParms->LoadParmValues((DBRequest*)&list,prefix);

  // std::cout <<"Raster channel no : " << std::endl;
  // std::cout <<" X signal channel = "<<fChNO[0]<<std::endl;
  // std::cout <<" Y signal channel = "<<fChNO[1]<<std::endl;


  // std::vector<string> channel_names = vsplit(names);
  // Int_t NChannels = channel_names.size();
  // for(Int_t i=0;i<NChannels;i++) {
  //   //  fPlaneNames[i] = new char[plane_names[i].length()];
  //   //  strcpy(fPlaneNames[i], plane_names[i].c_str());
  //   //std::cout<<" channel = "<<channel_names.at(i)<<std::endl;
  //   std::cout<<" Channel numbers = "<<
  // }


  // Read the detector map file

  // Based on the ungly method used in THaRaster class. I have no idea
  // how to do this more elegantly.
  // Buddhini
  // const int LEN=100;
  // char buf[LEN];
  //  FILE* fi = OpenFile( date );
  // if( !fi ) return kFileError;

  // fDetMap->Clear();
  // int first_chan, crate, dummy, slot, first, last, modulid;
  // std::cout<<"$$$$$$$$$$$$ THcRaster detmap"<<std::endl;
  // do {
  //   fgets( buf, LEN, fi);
  //   sscanf(buf,"%d %d %d %d %d %d %d",&first_chan, &crate, &dummy, &slot, &first, &last, &modulid);

  //   std::cout<<"$$$$$$$$$$$$ "<<crate<<std::endl;

  //   // if (first_chan>=0) {
  //   //   if ( fDetMap->AddModule (crate, slot, first, last, first_chan )<0) {
  //   // 	Error( Here(here), "Couldnt add Raster to DetMap!");
  //   // 	fclose(fi);
  //   // 	return kInitError;
  //   //   }
  //   // }
  // } while (first_chan>=0);




  return kOK;

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
  // Extract data from the hit list, accumulating into arrays for
  // calculating pedestals

}


//_____________________________________________________________________________
void THcRaster::CalculatePedestals( )
{
 // Use the accumulated pedestal data to calculate pedestals
}


//_____________________________________________________________________________
Int_t THcRaster::Decode( const THaEvData& evdata )
{
  cout << "THcRaster::Decode()" << endl;

  // clears the event structure
  // loops over all channels defined in the detector map
  // copies raw data into local variables
  // performs pedestal subtraction

  ClearEvent();

  // Get the Hall C style hitlist (fRawHitList) for this event
  Int_t fNhits = THcHitList::DecodeToHitList(evdata);
  std::cout<<"$$$$ Hits.."<<fNhits<<std::endl;

  // // Get the pedestals
  // if(gHaCuts->Result("Pedestal_event")) {
  //   Int_t nexthit = 0;
  //   AccumulatePedestals(fRawHitList);
    
  //   fAnalyzePedestals = 1;	// Analyze pedestals first normal events
  //   return(0);
  // }
  // if(fAnalyzePedestals) {
  //   CalculatePedestals();
  //   fAnalyzePedestals = 0;	// Don't analyze pedestals next event
  // }

  Int_t ihit = 0;
  std::cout<<"$$$$ Hits.."<<fNhits<<std::endl;

  while(ihit < fNhits) {
    //std::cout<<"Get raw hit.."<<std::endl;
    THcRasterRawHit* hit = (THcRasterRawHit *) fRawHitList->At(ihit);
    fRawADC[0] = hit->fADC_xsig;
    fRawADC[1] = hit->fADC_ysig;

    for(Int_t i=0;i<2;i++){
      fRawPos[i] = fRawADC[i]-fPedADC[i];
      // std::cout<<" Raw ADC = "<<fRawADC[i]
      // 	       <<" ped = "<<fPedADC[i]
      // 	       <<" corrected ="<<fRawPos[i]<<std::endl; 
    }
  }

  for(Int_t ihit = 0; ihit < fNhits ; ihit++) {
    THcRasterRawHit* hit = (THcRasterRawHit *) fRawHitList->At(ihit);
    cout << ihit << " : " << hit->fPlane << ":" << hit->fCounter << " : "
	 << hit->fADC_xsig << " " << hit->fADC_ysig << " "  <<  hit->fADC_xsync
	 << " " <<  hit->fADC_ysync << endl;
  }
  cout << endl;

  //Get X and Y ADC values


  return 0;

}





//_____________________________________________________________________________
Int_t THcRaster::Process( ){

  /*    From ENGINE/g_analyze_misc.f

	Fast Raster Signals:
	 ===================

	 gfrx_raw_adc = gmisc_dec_data(14,2)
	 gfry_raw_adc = gmisc_dec_data(16,2)

	 calculate raster position from ADC value.
	 
	 gfrx_adc = gfrx_raw_adc - gfrx_adc_ped
	 gfry_adc = gfry_raw_adc - gfry_adc_ped

  */

  // empty for now while I work on reading the channel map.


  return 0;
}



ClassImp(THcRaster)
////////////////////////////////////////////////////////////////////////////////
