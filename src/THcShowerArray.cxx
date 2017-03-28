/** \class THcShowerArray
    \ingroup DetSupport

Fly's eye array of shower blocks

*/

#include "THcShowerArray.h"
#include "TClonesArray.h"
#include "THcSignalHit.h"
#include "THcGlobals.h"
#include "THcParmList.h"
#include "THcHitList.h"
#include "THcShower.h"
#include "THcRawShowerHit.h"
#include "TClass.h"
#include "math.h"
#include "THaTrack.h"
#include "THaTrackProj.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <fstream>

using namespace std;

ClassImp(THcShowerArray)

//______________________________________________________________________________
THcShowerArray::THcShowerArray( const char* name,
                                const char* description,
				const Int_t layernum,
				THaDetectorBase* parent )
  : THaSubDetector(name,description,parent)
{
  fADCHits = new TClonesArray("THcSignalHit",100);
  fLayerNum = layernum;

  frAdcPedRaw = new TClonesArray("THcSignalHit", 16);
  frAdcErrorFlag = new TClonesArray("THcSignalHit", 16);
  frAdcPulseIntRaw = new TClonesArray("THcSignalHit", 16);
  frAdcPulseAmpRaw = new TClonesArray("THcSignalHit", 16);
  frAdcPulseTimeRaw = new TClonesArray("THcSignalHit", 16);

  frAdcPed = new TClonesArray("THcSignalHit", 16);
  frAdcPulseInt = new TClonesArray("THcSignalHit", 16);
  frAdcPulseAmp = new TClonesArray("THcSignalHit", 16);

  fClusterList = new THcShowerClusterList;         // List of hit clusters
}

//______________________________________________________________________________
THcShowerArray::~THcShowerArray()
{
  // Destructor
  delete fXPos;
  delete fYPos;

  delete fADCHits;

  delete frAdcPedRaw; frAdcPedRaw = NULL;
  delete frAdcErrorFlag; frAdcErrorFlag = NULL;
  delete frAdcPulseIntRaw; frAdcPulseIntRaw = NULL;
  delete frAdcPulseAmpRaw; frAdcPulseAmpRaw = NULL;
  delete frAdcPulseTimeRaw; frAdcPulseTimeRaw = NULL;

  delete frAdcPed; frAdcPed = NULL;
  delete frAdcPulseInt; frAdcPulseInt = NULL;
  delete frAdcPulseAmp; frAdcPulseAmp = NULL;

  delete [] fA;
  delete [] fP;
  delete [] fA_p;

  delete [] fE;
}

//_____________________________________________________________________________
THaAnalysisObject::EStatus THcShowerArray::Init( const TDatime& date )
{
  // Extra initialization for shower layer: set up DataDest map

  if( IsZombie())
    return fStatus = kInitError;

  // How to get information for parent
  //  if( GetParent() )
  //    fOrigin = GetParent()->GetOrigin();

  EStatus status;
  if( (status=THaSubDetector::Init( date )) )
    return fStatus = status;

  return fStatus = kOK;

}

//_____________________________________________________________________________
Int_t THcShowerArray::ReadDatabase( const TDatime& date )
{

  char prefix[2];
  prefix[0]=tolower(GetParent()->GetPrefix()[0]);
  prefix[1]='\0';

  cout << "Parent name: " << GetParent()->GetPrefix() << endl;
  fNRows=fNColumns=0;
  fXFront=fYFront=fZFront=0.;
  fXStep=fYStep=fZSize=0.;
  fUsingFADC=0;
  fPedSampLow=0;
  fPedSampHigh=9;
  fDataSampLow=23;
  fDataSampHigh=49;
  DBRequest list[]={
    {"cal_arr_nrows", &fNRows, kInt},
    {"cal_arr_ncolumns", &fNColumns, kInt},
    {"cal_arr_front_x", &fXFront, kDouble},
    {"cal_arr_front_y", &fYFront, kDouble},
    {"cal_arr_front_z", &fZFront, kDouble},
    {"cal_arr_xstep", &fXStep, kDouble},
    {"cal_arr_ystep", &fYStep, kDouble},
    {"cal_arr_zsize", &fZSize, kDouble},
    {"cal_using_fadc", &fUsingFADC, kInt, 0, 1},
    {"cal_arr_ADCMode", &fADCMode, kInt, 0, 1},
    {"cal_arr_AdcTimeWindowMin", &fAdcTimeWindowMin, kDouble, 0, 1},
    {"cal_arr_AdcTimeWindowMax", &fAdcTimeWindowMax, kDouble, 0, 1},
    {"cal_ped_sample_low", &fPedSampLow, kInt, 0, 1},
    {"cal_ped_sample_high", &fPedSampHigh, kInt, 0, 1},
    {"cal_data_sample_low", &fDataSampLow, kInt, 0, 1},
    {"cal_data_sample_high", &fDataSampHigh, kInt, 0, 1},
    {0}
  };
  gHcParms->LoadParmValues((DBRequest*)&list, prefix);
  fADCMode=kADCDynamicPedestal;
  fAdcTimeWindowMin=0;
  fAdcTimeWindowMax=10000;
  fNelem = fNRows*fNColumns;

  fXPos = new Double_t* [fNRows];
  fYPos = new Double_t* [fNRows];
  for (UInt_t i=0; i<fNRows; i++) {
    fXPos[i] = new Double_t [fNColumns];
    fYPos[i] = new Double_t [fNColumns];
  }

  //Looking to the front, the numbering goes from left to right, and from top
  //to bottom.

  for (UInt_t j=0; j<fNColumns; j++)
    for (UInt_t i=0; i<fNRows; i++) {
      fXPos[i][j] = fXFront - (fNRows-1)*fXStep/2 + fXStep*i;
      fYPos[i][j] = fYFront + (fNColumns-1)*fYStep/2 - fYStep*j;
  }

  fOrigin.SetXYZ(fXFront, fYFront, fZFront);

  // Debug output.

  THcShower* fParent;
  fParent = (THcShower*) GetParent();

  if (fParent->fdbg_init_cal) {
    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcShowerArray::ReadDatabase for "
    	 << GetParent()->GetPrefix() << ":" << endl;

    cout << "  Layer #" << fLayerNum << ", number of elements " << dec << fNelem
	 << endl;
    cout << "  Columns " << fNColumns << ", Rows " << fNRows << endl;

    cout << "Front X, Y Z: " << fXFront << ", " << fYFront << ", " << fZFront
	 << " cm" << endl;

    cout << "  Block to block X and Y distances: " << fXStep << ", " << fYStep
	 << " cm" << endl;

    cout << "  Block size along Z: " << fZSize << " cm" << endl;

    cout << "Block X coordinates:" << endl;
    for (UInt_t i=0; i<fNRows; i++) {
      for (UInt_t j=0; j<fNColumns; j++) {
	cout << fXPos[i][j] << " ";
      }
      cout << endl;
    }
    cout << endl;

    cout << "Block Y coordinates:" << endl;
    for (UInt_t i=0; i<fNRows; i++) {
      for (UInt_t j=0; j<fNColumns; j++) {
    	cout << fYPos[i][j] << " ";
      }
      cout << endl;
    }
    cout << endl;

    cout << "  Origin of Array:" << endl;
    cout << "    Xorig = " << GetOrigin().X() << endl;
    cout << "    Yorig = " << GetOrigin().Y() << endl;
    cout << "    Zorig = " << GetOrigin().Z() << endl;
    cout << endl;

    cout << "  Using FADC " << fUsingFADC << endl;
    if (fUsingFADC) {
      cout << "  FADC pedestal sample low = " << fPedSampLow << ",  high = "
	   << fPedSampHigh << endl;
      cout << "  FADC data sample low = " << fDataSampLow << ",  high = "
	   << fDataSampHigh << endl;
    }

  }

  // Here read the 2-D arrays of pedestals, gains, etc.

  // Pedestal limits per channel.
  fPedLimit = new Int_t [fNelem];

  Double_t cal_arr_cal_const[fNelem];
  Double_t cal_arr_gain_cor[fNelem];

  DBRequest list1[]={
    {"cal_arr_ped_limit", fPedLimit, kInt, static_cast<UInt_t>(fNelem),1},
    {"cal_arr_cal_const", cal_arr_cal_const, kDouble, static_cast<UInt_t>(fNelem)},
    {"cal_arr_gain_cor",  cal_arr_gain_cor,  kDouble, static_cast<UInt_t>(fNelem)},
    {0}
  };
  gHcParms->LoadParmValues((DBRequest*)&list1, prefix);

  // Debug output.
  if (fParent->fdbg_init_cal) {

    cout << "  fPedLimit:" << endl;
    Int_t el=0;
    for (UInt_t j=0; j<fNColumns; j++) {
      cout << "    ";
      for (UInt_t i=0; i<fNRows; i++) {
	cout << fPedLimit[el++] << " ";
      };
      cout <<  endl;
    };

    cout << "  cal_arr_cal_const:" << endl;
    el=0;
    for (UInt_t j=0; j<fNColumns; j++) {
      cout << "    ";
      for (UInt_t i=0; i<fNRows; i++) {
	cout << cal_arr_cal_const[el++] << " ";
      };
      cout <<  endl;
    };

    cout << "  cal_arr_gain_cor:" << endl;
    el=0;
    for (UInt_t j=0; j<fNColumns; j++) {
      cout << "    ";
      for (UInt_t i=0; i<fNRows; i++) {
	cout << cal_arr_gain_cor[el++] << " ";
      };
      cout <<  endl;
    };

  }    // end of debug output

  // Calibration constants (GeV / ADC channel).
  fGain = new Double_t [fNelem];
  for (Int_t i=0; i<fNelem; i++) {
    fGain[i] = cal_arr_cal_const[i] *  cal_arr_gain_cor[i];
  }

  // Debug output.
  if (fParent->fdbg_init_cal) {

    cout << "  fGain:" << endl;
    Int_t el=0;
    for (UInt_t j=0; j<fNColumns; j++) {
      cout << "    ";
      for (UInt_t i=0; i<fNRows; i++) {
	cout << fGain[el++] << " ";
      };
      cout <<  endl;
    };

  }

  fMinPeds = fParent->GetMinPeds();

  InitializePedestals();

  // Event by event amplitude and pedestal
  fA = new Double_t[fNelem];
  fP = new Double_t[fNelem];
  fA_p = new Double_t[fNelem];

  // Energy depositions per block.

  fE = new Double_t[fNelem];

#ifdef HITPIC
  hitpic = new char*[fNRows];
  for(Int_t row=0;row<fNRows;row++) {
    hitpic[row] = new char[NPERLINE*(fNColumns+1)+2];
  }
  piccolumn=0;
#endif

  // Debug output.

  if (fParent->fdbg_init_cal) {

    cout << "  fMinPeds = " << fMinPeds << endl;

    cout << "---------------------------------------------------------------\n";
  }

  return kOK;
}

//_____________________________________________________________________________
Int_t THcShowerArray::DefineVariables( EMode mode )
{
  // Initialize global variables

  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  // Register variables in global list
  RVarDef vars[] = {
    {"adchits", "List of ADC hits", "fADCHits.THcSignalHit.GetPaddleNumber()"},
    {"a", "Raw ADC Amplitude", "fA"},
    {"p", "Dynamic ADC Pedestal", "fP"},
    {"a_p", "Sparsified, ped-subtracted ADC Amplitudes", "fA_p"},
    { "nhits", "Number of hits", "fNhits" },
    { "nghits", "Number of good hits ( pass threshold on raw ADC)", "fNgoodhits" },
    { "nclust", "Number of clusters", "fNclust" },
    {"e", "Energy Depositions per block", "fE"},
    {"earray", "Energy Deposition in array", "fEarray"},
    { "ntracks", "Number of shower tracks", "fNtracks" },

    {"adcCounter",      "List of ADC counter numbers.",      "frAdcPulseIntRaw.THcSignalHit.GetPaddleNumber()"},

    {"adcPedRaw",       "List of raw ADC pedestals",         "frAdcPedRaw.THcSignalHit.GetData()"},
    {"adcErrorFlag",       "List of raw ADC pedestals",      "frAdcErrorFlag.THcSignalHit.GetData()"},
    {"adcPulseIntRaw",  "List of raw ADC pulse integrals.",  "frAdcPulseIntRaw.THcSignalHit.GetData()"},
    {"adcPulseAmpRaw",  "List of raw ADC pulse amplitudes.", "frAdcPulseAmpRaw.THcSignalHit.GetData()"},
    {"adcPulseTimeRaw", "List of raw ADC pulse times.",      "frAdcPulseTimeRaw.THcSignalHit.GetData()"},

    {"adcPed",          "List of ADC pedestals",             "frAdcPed.THcSignalHit.GetData()"},
    {"adcPulseInt",     "List of ADC pulse integrals.",      "frAdcPulseInt.THcSignalHit.GetData()"},
    {"adcPulseAmp",     "List of ADC pulse amplitudes.",     "frAdcPulseAmp.THcSignalHit.GetData()"},

    { 0 }
  };

  return DefineVarsFromList( vars, mode );
}

//_____________________________________________________________________________
void THcShowerArray::Clear( Option_t* )
{
  // Clears the hit lists
  fADCHits->Clear();

  fNhits = 0;
  fNgoodhits = 0;
  fNclust = 0;
  fNtracks = 0;

  for (THcShowerClusterListIt i=fClusterList->begin(); i!=fClusterList->end();
       ++i) {
    delete *i;
    *i = 0;
  }
  fClusterList->clear();

  frAdcPedRaw->Clear();
  frAdcErrorFlag->Clear();
  frAdcPulseIntRaw->Clear();
  frAdcPulseAmpRaw->Clear();
  frAdcPulseTimeRaw->Clear();

  frAdcPed->Clear();
  frAdcPulseInt->Clear();
  frAdcPulseAmp->Clear();

}

//_____________________________________________________________________________
Int_t THcShowerArray::Decode( const THaEvData& evdata )
{
  // Doesn't actually get called.  Use Fill method instead

  return 0;
}

//_____________________________________________________________________________
Int_t THcShowerArray::CoarseProcess( TClonesArray& tracks )
{

  // Fill set of unclustered shower array hits.
  // Reuse hit class pertained to the HMS/SOS type calorimeters.
  // Save Y coordinate of the hit in Z parameter of the class.
  // Save energy deposition in the module as hit mean energy, do not use
  // positive and negative side energies.

  THcShowerHitSet HitSet;         //set of hits

  UInt_t k=0;
  for(UInt_t j=0; j < fNColumns; j++) {
    for (UInt_t i=0; i<fNRows; i++) {

      if (fA_p[k] > 0) {    //hit

	THcShowerHit* hit =
	  new THcShowerHit(i, j, fXPos[i][j], fYPos[i][j], fE[k], 0., 0.);

	HitSet.insert(hit);
      }

      k++;
    }
  }
  //Debug output, print out hits before clustering.

  THcShower* fParent = (THcShower*) GetParent();

  if (fParent->fdbg_clusters_cal) {
    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcShowerArray::CoarseProcess for " << GetName()
	 << endl;

    cout << "  List of unclustered hits. Total hits:     " << fNhits << endl;
    THcShowerHitIt it = HitSet.begin();    //<set> version
    for (Int_t i=0; i!=fNgoodhits; i++) {
      cout << "  hit " << i << ": ";
      (*(it++))->show();
    }
  }

  // Cluster hits and fill list of clusters.

  fParent->ClusterHits(HitSet, fClusterList);

  fNclust = (*fClusterList).size();         //number of clusters

  //Debug output, print out clustered hits.

  if (fParent->fdbg_clusters_cal) {

    cout << "  Clustered hits. Number of clusters: " << fNclust << endl;

    UInt_t i = 0;
    for (THcShowerClusterListIt ppcl = (*fClusterList).begin();
	 ppcl != (*fClusterList).end(); ppcl++) {

      cout << "  Cluster #" << i++
	   <<":  E=" << clE(*ppcl)
	   << "  Epr=" << clEpr(*ppcl)
	   << "  X=" << clX(*ppcl)
	   << "  Z=" << clZ(*ppcl)
	   << "  size=" << (**ppcl).size()
	   << endl;

      Int_t j=0;
      for (THcShowerClusterIt pph=(**ppcl).begin(); pph!=(**ppcl).end();
	   pph++) {
	cout << "  hit " << j++ << ": ";
	(**pph).show();
      }

    }

    cout << "---------------------------------------------------------------\n";
  }

  return 0;
}

//-----------------------------------------------------------------------------

Int_t THcShowerArray::MatchCluster(THaTrack* Track,
				   Double_t& XTrFront, Double_t& YTrFront)
{
  // Match an Array cluster to a given track. Return the cluster number,
  // and track coordinates at the front of Array.

  XTrFront = kBig;
  YTrFront = kBig;
  Double_t pathl = kBig;

  // Track interception with face of Array. The coordinates are
  // in the Array's local system.

  fOrigin = GetOrigin();

  THcShower* fParent = (THcShower*) GetParent();

  fParent->CalcTrackIntercept(Track, pathl, XTrFront, YTrFront);

  // Transform coordiantes to the spectrometer's coordinate system.

  XTrFront += GetOrigin().X();
  YTrFront += GetOrigin().Y();

  Bool_t inFidVol = true;            // In Fiducial Volume flag

  // Re-evaluate Fid. Volume Flag if fid. volume test is requested

  if (fParent->fvTest) {

    TVector3 Origin = fOrigin;         //save fOrigin

    // Track coordinates at the back of the detector.

    // Origin at the back of counter.
    fOrigin.SetXYZ(GetOrigin().X(), GetOrigin().Y(), GetOrigin().Z() + fZSize);

    Double_t XTrBack = kBig;
    Double_t YTrBack = kBig;

    fParent->CalcTrackIntercept(Track, pathl, XTrBack, YTrBack);

    XTrBack += GetOrigin().X();   // from local coord. system
    YTrBack += GetOrigin().Y();   // to the spectrometer system

    inFidVol = (XTrFront <= fParent->fvXmax) && (XTrFront >= fParent->fvXmin) &&
               (YTrFront <= fParent->fvYmax) && (YTrFront >= fParent->fvYmin) &&
               (XTrBack <= fParent->fvXmax) && (XTrBack >= fParent->fvXmin) &&
               (YTrBack <= fParent->fvYmax) && (YTrBack >= fParent->fvYmin);

    fOrigin = Origin;         //restore fOrigin
  }

  // Match a cluster to the track. Choose closest to the track cluster.

  Int_t mclust = -1;    // The match cluster #, initialize with a bogus value.
  Double_t Delta = kBig;   // Track to cluster distance

  if (inFidVol) {

    // Since hits and clusters are in reverse order (with respect to Engine),
    // search backwards to be consistent with Engine.
    //
    // Note: cluster Z coordinate is used here as Y, for Z variable
    // of the THcShowerHit class was used to save Y coordinates of hits.

    for (Int_t i=fNclust-1; i>-1; i--) {

      THcShowerCluster* cluster = *(fClusterList->begin()+i);

      Double_t dx = TMath::Abs( clX(cluster) - XTrFront );
      Double_t dy = TMath::Abs( clZ(cluster) - YTrFront ); //cluster Z for Y.
      Double_t distance = TMath::Sqrt(dx*dx+dy*dy);        //cluster-track dist.

      //Choice of threshold on distance is not unuque. Use the simplest for now.

      if (distance <= (0.5*(fXStep + fYStep) + fParent->fSlop)) {
	fNtracks++;
	if (distance < Delta) {
	  mclust = i;
	  Delta = distance;
	}
      }
    }
  }

  //Debug output.

  if (fParent->fdbg_tracks_cal) {
    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcShowerArray::MatchCluster for " << GetName()
	 << endl;

    cout << "  Track at DC:"
	 << "  X = " << Track->GetX()
	 << "  Y = " << Track->GetY()
	 << "  Theta = " << Track->GetTheta()
	 << "  Phi = " << Track->GetPhi()
	 << endl;
    cout << "  Track at the front of Array:"
	 << "  X = " << XTrFront
	 << "  Y = " << YTrFront
	 << "  Pathl = " << pathl
	 << endl;
    if (fParent->fvTest)
      cout << "  Fiducial volume test: inFidVol = " << inFidVol << endl;

    cout << "  Matched cluster #" << mclust << ",  Delta = " << Delta << endl;
    cout << "---------------------------------------------------------------\n";
  }

  return mclust;
}

//_____________________________________________________________________________
Float_t THcShowerArray::GetShEnergy(THaTrack* Track) {

  // Get total energy deposited in the Array cluster matched to the given
  // spectrometer Track.

  // Track coordinates at the front of Array, initialize out of acceptance.
  Double_t Xtr = -100.;
  Double_t Ytr = -100.;

  // Associate a cluster to the track.

  Int_t mclust = MatchCluster(Track, Xtr, Ytr);

  // Coordinate corrected total energy deposition in the cluster.

  Float_t Etrk = 0.;
  if (mclust >= 0) {         // if there is a matched cluster

    // Get matched cluster.
    THcShowerCluster* cluster = *(fClusterList->begin()+mclust);

    // No coordinate correction for now.
    Etrk = clE(cluster);

  }   //mclust>=0

  //Debug output.

  THcShower* fParent = (THcShower*) GetParent();

  if (fParent->fdbg_tracks_cal) {
    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcShowerArray::GetShEnergy for "
	 << GetName() << endl;

    cout << "  Track at Array: X = " << Xtr << "  Y = " << Ytr;
    if (mclust >= 0)
      cout << ", matched cluster #" << mclust << "." << endl;
    else
      cout << ", no matched cluster found." << endl;

    cout << "  Track's Array energy = " << Etrk << "." << endl;
    cout << "---------------------------------------------------------------\n";
  }

  return Etrk;
}

//_____________________________________________________________________________
Int_t THcShowerArray::FineProcess( TClonesArray& tracks )
{
  return 0;
}

//_____________________________________________________________________________
Int_t THcShowerArray::CoarseProcessHits()
{
  THcShower* fParent;
  fParent = (THcShower*) GetParent();
    Int_t ADCMode=fParent->GetADCMode();
    if(ADCMode == kADCDynamicPedestal) {
      FillADC_DynamicPedestal();
    } else if (ADCMode == kADCSampleIntegral) {
      FillADC_SampleIntegral();
    } else if (ADCMode == kADCSampIntDynPed) {
      FillADC_SampIntDynPed();
        } else {
      FillADC_Standard();
    }
    //
  if (fParent->fdbg_decoded_cal) {

    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcShowerArray::ProcessHits for "
    	 << fParent->GetPrefix() << ":" << endl;

    cout << "  Sparsified hits for shower array, plane #" << fLayerNum
	 << ", " << GetName() << ":" << endl;

    Int_t nspar = 0;
    Int_t k=0;
    for(UInt_t j=0; j < fNColumns; j++) {
    for (UInt_t i=0; i<fNRows; i++) {
     if(fA[k] > fThresh[k]) {
	cout << "  counter =  " << k
	     << "  E = " << fE[k]
	     << endl;
	nspar++;
      }
       k++;
    }
    }

    if (nspar == 0) cout << "  No hits\n";

    cout << "  Earray = " << fEarray << endl;
    cout << "---------------------------------------------------------------\n";
  }
  //
  return 1;
}
//_____________________________________________________________________________
void THcShowerArray::FillADC_SampIntDynPed()
{
  //    adc_pos = hit->GetRawAdcHitPos().GetSampleInt();
  //    adc_neg = hit->GetRawAdcHitNeg().GetSampleInt();
  //   adc_pos_pedsub = hit->GetRawAdcHitPos().GetSampleIntRaw();
  //   adc_neg_pedsub = hit->GetRawAdcHitNeg().GetSampleIntRaw();
  // Need to create
}
//_____________________________________________________________________________
void THcShowerArray::FillADC_SampleIntegral()
{
  ///			adc_pos_pedsub = hit->GetRawAdcHitPos().GetSampleIntRaw() - fPosPed[hit->fCounter -1];
  //			adc_neg_pedsub = hit->GetRawAdcHitNeg().GetSampleIntRaw() - fNegPed[hit->fCounter -1];
  //			adc_pos = hit->GetRawAdcHitPos().GetSampleIntRaw();
  //			adc_neg = hit->GetRawAdcHitNeg().GetSampleIntRaw();
  // need to create
}
//_____________________________________________________________________________
void THcShowerArray::FillADC_Standard()
{
}
//_____________________________________________________________________________
void THcShowerArray::FillADC_DynamicPedestal()
{
  for (Int_t ielem=0;ielem<frAdcPulseInt->GetEntries();ielem++) {
    Int_t npad = ((THcSignalHit*) frAdcPulseInt->ConstructedAt(ielem))->GetPaddleNumber() - 1;
    Double_t pulseInt = ((THcSignalHit*) frAdcPulseInt->ConstructedAt(ielem))->GetData();
    Double_t pulseIntRaw = ((THcSignalHit*) frAdcPulseIntRaw->ConstructedAt(ielem))->GetData();
    Double_t pulseTime = ((THcSignalHit*) frAdcPulseTimeRaw->ConstructedAt(ielem))->GetData();
    Double_t errorflag = ((THcSignalHit*) frAdcErrorFlag->ConstructedAt(ielem))->GetData();
    Bool_t pulseTimeCut = (pulseTime > fAdcTimeWindowMin) &&  (pulseTime < fAdcTimeWindowMax);
    if (errorflag==0 && pulseTimeCut) {
      fA[npad] =pulseIntRaw;
      if(fA[npad] >  fThresh[npad]) {
       fA_p[npad] =pulseInt ;
       fE[npad] = fA_p[npad]*fGain[npad];
       fEarray += fE[npad];
      }
     }        
   }
  //
}
//_____________________________________________________________________________
Int_t THcShowerArray::ProcessHits(TClonesArray* rawhits, Int_t nexthit)
{
  // Extract the data for this layer from hit list.

  THcShower* fParent;
  fParent = (THcShower*) GetParent();

  // Initialize variables.

  fADCHits->Clear();

  frAdcPedRaw->Clear();
  frAdcErrorFlag->Clear();
  frAdcPulseIntRaw->Clear();
  frAdcPulseAmpRaw->Clear();
  frAdcPulseTimeRaw->Clear();

  frAdcPed->Clear();
  frAdcPulseInt->Clear();
  frAdcPulseAmp->Clear();

  for(Int_t i=0;i<fNelem;i++) {
    fA[i] = 0;
    fA_p[i] = 0;
    fE[i] = 0;
  }

  fEarray = 0;

  // Process raw hits. Get ADC hits for the plane, assign variables for each
  // channel.

  Int_t nrawhits = rawhits->GetLast()+1;

  Int_t ihit = nexthit;

  UInt_t nrAdcHits = 0;

  while(ihit < nrawhits) {
    THcRawShowerHit* hit = (THcRawShowerHit *) rawhits->At(ihit);

    if(hit->fPlane != fLayerNum) {
      break;
    }

    Int_t padnum = hit->fCounter;
    THcRawAdcHit& rawAdcHit = hit->GetRawAdcHitPos();
    //
    for (UInt_t thit=0; thit<rawAdcHit.GetNPulses(); ++thit) {
      ((THcSignalHit*) frAdcPedRaw->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPedRaw());
        fThresh[padnum]=rawAdcHit.GetPedRaw()*rawAdcHit.GetF250_PeakPedestalRatio()+250.;
     ((THcSignalHit*) frAdcPed->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPed());

      ((THcSignalHit*) frAdcPulseIntRaw->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPulseIntRaw(thit));
      ((THcSignalHit*) frAdcPulseInt->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPulseInt(thit));

      ((THcSignalHit*) frAdcPulseAmpRaw->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPulseAmpRaw(thit));
      ((THcSignalHit*) frAdcPulseAmp->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPulseAmp(thit));

      ((THcSignalHit*) frAdcPulseTimeRaw->ConstructedAt(nrAdcHits))->Set(padnum, rawAdcHit.GetPulseTimeRaw(thit));
      if (rawAdcHit.GetPulseAmp(thit)>0&&rawAdcHit.GetPulseIntRaw(thit)>0) {
	((THcSignalHit*) frAdcErrorFlag->ConstructedAt(nrAdcHits))->Set(padnum,0);
      } else {
	((THcSignalHit*) frAdcErrorFlag->ConstructedAt(nrAdcHits))->Set(padnum,1);
      }
      ++nrAdcHits;
    }
    ihit++;
  }

#if 0
  if(fNgoodhits > 0) {
    cout << "+";
    for(Int_t column=0;column<fNColumns;column++) {
      cout << "-";
    }
    cout << "+" << endl;
    for(Int_t row=0;row<fNRows;row++) {
      cout << "|";
      for(Int_t column=0;column<fNColumns;column++) {
	Int_t counter = column*fNRows + row;
	if(fA[counter]>threshold) {
	  cout << "X";
	} else {
	  cout << " ";
	}
      }
      cout << "|" << endl;
    }
  }
#endif
#ifdef HITPIC
  if(fNgoodhits > 0) {
    for(Int_t row=0;row<fNRows;row++) {
      if(piccolumn==0) {
	hitpic[row][0] = '|';
      }
      for(Int_t column=0;column<fNColumns;column++) {
	Int_t counter = column*fNRows+row;
	if(fA[counter] > threshold) {
	  hitpic[row][piccolumn*(fNColumns+1)+column+1] = 'X';
	} else {
	  hitpic[row][piccolumn*(fNColumns+1)+column+1] = ' ';
	}
	hitpic[row][(piccolumn+1)*(fNColumns+1)] = '|';
      }
    }
    piccolumn++;
    if(piccolumn==NPERLINE) {
      cout << "+";
      for(Int_t pc=0;pc<NPERLINE;pc++) {
	for(Int_t column=0;column<fNColumns;column++) {
	  cout << "-";
	}
	cout << "+";
      }
      cout << endl;
      for(Int_t row=0;row<fNRows;row++) {
	hitpic[row][(piccolumn+1)*(fNColumns+1)+1] = '\0';
	cout << hitpic[row] << endl;
      }
      piccolumn = 0;
    }
  }
#endif

  //Debug output.


  return(ihit);
}
//_____________________________________________________________________________
Int_t THcShowerArray::AccumulatePedestals(TClonesArray* rawhits, Int_t nexthit)
{
  // Extract data for this plane from hit list and accumulate in
  // arrays for subsequent pedestal calculations.

  Int_t nrawhits = rawhits->GetLast()+1;

  Int_t ihit = nexthit;

  while(ihit < nrawhits) {

    THcRawShowerHit* hit = (THcRawShowerHit *) rawhits->At(ihit);

    if(hit->fPlane != fLayerNum) {
      break;
    }

    Int_t element = hit->fCounter - 1; // Should check if in range

		// Should check that counter # is in range
		Int_t adc = 0;
		if (fUsingFADC) {
			adc = hit->GetRawAdcHitPos().GetData(
				fPedSampLow, fPedSampHigh, fDataSampLow, fDataSampHigh
			);
		}
		else {
			adc = hit->GetData(0);
		}

    if(adc <= fPedLimit[element]) {
      fPedSum[element] += adc;
      fPedSum2[element] += adc*adc;
      fPedCount[element]++;
      if(fPedCount[element] == fMinPeds/5) {
	fPedLimit[element] = 100 + fPedSum[element]/fPedCount[element];
      }
    }
    ihit++;
  }
  fNPedestalEvents++;

  // Debug output.

  if ( ((THcShower*) GetParent())->fdbg_raw_cal ) {

    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcShowerArray::AcculatePedestals for "
    	 << GetParent()->GetPrefix() << ":" << endl;

    cout << "Processed hit list for plane " << GetName() << ":\n";

    for (Int_t ih=nexthit; ih<nrawhits; ih++) {

      THcRawShowerHit* hit = (THcRawShowerHit *) rawhits->At(ih);

      if(hit->fPlane != fLayerNum) {
	break;
      }

			Int_t adc = 0;
			if (fUsingFADC) {
				adc = hit->GetRawAdcHitPos().GetData(
					fPedSampLow, fPedSampHigh, fDataSampLow, fDataSampHigh
				);
			}
			else {
				adc = hit->GetData(0);
			}

      cout << "  hit " << ih << ":"
	   << "  plane =  " << hit->fPlane
	   << "  counter = " << hit->fCounter
	   << "  ADC = " << adc
	   << endl;
    }

    cout << "---------------------------------------------------------------\n";

  }

  return(ihit);
}

//_____________________________________________________________________________
void THcShowerArray::CalculatePedestals( )
{
  // Use the accumulated pedestal data to calculate pedestals.

  for(Int_t i=0; i<fNelem;i++) {

    fPed[i] = ((Float_t) fPedSum[i]) / TMath::Max(1, fPedCount[i]);
    fSig[i] = sqrt(((Float_t)fPedSum2[i])/TMath::Max(1, fPedCount[i])
		   - fPed[i]*fPed[i]);
    fThresh[i] = fPed[i] + TMath::Min(50., TMath::Max(10., 3.*fSig[i]));

  }

  // Debug output.

  if ( ((THcShower*) GetParent())->fdbg_raw_cal ) {

    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcShowerArray::CalculatePedestals for "
    	 << GetParent()->GetPrefix() << ":" << endl;

    cout << "  ADC pedestals and thresholds for calorimeter plane "
	 << GetName() << endl;
    for(Int_t i=0; i<fNelem;i++) {
      cout << "  element " << i << ": "
	   << "  Pedestal = " << fPed[i]
	   << "  threshold = " << fThresh[i]
	   << endl;
    }
    cout << "---------------------------------------------------------------\n";

  }

}
//_____________________________________________________________________________
void THcShowerArray::InitializePedestals( )
{
  fNPedestalEvents = 0;

  fPedSum = new Int_t [fNelem];
  fPedSum2 = new Int_t [fNelem];
  fPedCount = new Int_t [fNelem];

  fSig = new Float_t [fNelem];
  fPed = new Float_t [fNelem];
  fThresh = new Float_t [fNelem];

  for(Int_t i=0;i<fNelem;i++) {
    fPedSum[i] = 0;
    fPedSum2[i] = 0;
    fPedCount[i] = 0;
  }
}

//------------------------------------------------------------------------------

// Fiducial volume limits.

Double_t THcShowerArray::fvXmin() {
  THcShower* fParent;
  fParent = (THcShower*) GetParent();
  return fXPos[0][0] - fXStep/2 + fParent->fvDelta;
}

Double_t THcShowerArray::fvYmax() {
  THcShower* fParent;
  fParent = (THcShower*) GetParent();
  return fYPos[0][0] + fYStep/2 - fParent->fvDelta;
}

Double_t THcShowerArray::fvXmax() {
  THcShower* fParent;
  fParent = (THcShower*) GetParent();
  return fXPos[fNRows-1][fNColumns-1] + fXStep/2 - fParent->fvDelta;
}

Double_t THcShowerArray::fvYmin() {
  THcShower* fParent;
  fParent = (THcShower*) GetParent();
  return fYPos[fNRows-1][fNColumns-1] - fYStep/2 + fParent->fvDelta;
}
