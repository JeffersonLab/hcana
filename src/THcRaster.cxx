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
#include "THcHitList.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>


using namespace std;

//_____________________________________________________________________________
THcRaster::THcRaster( const char* name, const char* description,
		      THaApparatus* apparatus ) :
  THaBeamDet(name,description,apparatus)
{
  // // Initializing channels
  // fRasterXADC = new TClonesArray("THcSignalHit",16);
  // fRasterYADC = new TClonesArray("THcSignalHit",16);
}

//_____________________________________________________________________________
THcRaster::THcRaster( ) :
  THaBeamDet()
{
  // Default constructor
}

//_____________________________________________________________________________
THcRaster::~THRaster()
{
  // Distructor
}

//_____________________________________________________________________________
void THcRaster::ClearEvent(){

  // do nothing for now
}

//_____________________________________________________________________________
// ReadDatabase:  if detectors cant be added to detmap 
Int_t THcRaster::ReadDatabase( const TDatime& date )
{
  static const char* const here = "THcRaster::ReadDatabase";

  InitializeReconstruction();

  char prefix[2];

  cout << " GetName() " << GetName() << endl;

  prefix[0]=tolower(GetName()[0]);
  prefix[1]='\0';

  string channelNames;
  DBRequest channellist[]={
    {"graster_channels",&channelNames,kString},
    {0}
  };

  // get the channel list from the parameter file
  gHcParms->LoadParmValues((DBRequest*)&channellist,prefix);
  std::cout << "Raster channel list : " << channellist << std::endl;

  vector<string> channel_names = vsplit(channellist);

  Int_t NChannels = channel_names.size_of();
  for(Int_t i=0;i<NChannels;i++) {
    //  fPlaneNames[i] = new char[plane_names[i].length()];
    //  strcpy(fPlaneNames[i], plane_names[i].c_str());
    std::cout<<" channel = "<<channel_names.at(i)<<std::endl;
  }

return kOK 

}

//_____________________________________________________________________________
Int_t THcRaster::Decode( const THaEvData& evdata )
{
  // Get the Hall C style hitlist (fRawHitList) for this event
  // empty for now while I work on reading the channel map.

  return 0;

}

//_____________________________________________________________________________
Int_t THaRaster::Process( ){

  // empty for now while I work on reading the channel map.
  return o;
}



ClassImp(THcRaster)
////////////////////////////////////////////////////////////////////////////////
