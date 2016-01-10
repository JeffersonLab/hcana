//*-- Author :

//////////////////////////////////////////////////////////////////////////
//
// THcShowerArray
//
//////////////////////////////////////////////////////////////////////////

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

  fClusterList = new THcShowerClusterList;
}

//______________________________________________________________________________
THcShowerArray::~THcShowerArray()
{
  // Destructor
  delete fXPos;
  delete fYPos;

  delete fADCHits;

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
    {"cal_ped_sample_low", &fPedSampLow, kInt, 0, 1},
    {"cal_ped_sample_high", &fPedSampHigh, kInt, 0, 1},
    {"cal_data_sample_low", &fDataSampLow, kInt, 0, 1},
    {"cal_data_sample_high", &fDataSampHigh, kInt, 0, 1},
    {0}
  };
  gHcParms->LoadParmValues((DBRequest*)&list, prefix);

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
    {"cal_arr_ped_limit", fPedLimit, kInt,    fNelem},
    {"cal_arr_cal_const", cal_arr_cal_const, kDouble, fNelem},
    {"cal_arr_gain_cor",  cal_arr_gain_cor,  kDouble, fNelem},
    //    {"cal_min_peds", &fShMinPeds, kInt},
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
  for (UInt_t i=0; i<fNelem; i++) {
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

    //    cout << "  Origin of Layer at  X = " << fOrigin.X()
    //	 << "  Y = " << fOrigin.Y() << "  Z = " << fOrigin.Z() << endl;

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
    { "nclust", "Number of clusters", "fNclust" },
    {"e", "Energy Depositions per block", "fE"},
    {"earray", "Energy Deposition in array", "fEarray"},
    { "ntracks", "Number of shower tracks", "fNtracks" },
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
  fNclust = 0;
  fNtracks = 0;

  for (THcShowerClusterListIt i=fClusterList->begin(); i!=fClusterList->end();
       ++i) {
    delete *i;
    *i = 0;
  }
  fClusterList->clear();

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

  THcShowerHitSet HitSet;

  UInt_t k=0;
  for (UInt_t i=0; i<fNRows; i++) {
    for(UInt_t j=0; j < fNColumns; j++) {

      if (fA_p[k] > 0) {    //hit

	THcShowerHit* hit =
	  new THcShowerHit(i, j, fXPos[i][j], fYPos[i][j], fE[k], 0., 0.);

	HitSet.insert(hit);
      }

      k++;
    }
  }

  fNhits = HitSet.size();

  //Debug output, print out hits before clustering.

  THcShower* fParent = (THcShower*) GetParent();

  if (fParent->fdbg_clusters_cal) {
    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcShowerArray::CoarseProcess for " << GetName()
	 << endl;

    cout << "  List of unclustered hits. Total hits:     " << fNhits << endl;
    THcShowerHitIt it = HitSet.begin();    //<set> version
    for (Int_t i=0; i!=fNhits; i++) {
      cout << "  hit " << i << ": ";
      (*(it++))->show();
    }
  }

  // Fill list of clusters.

  fParent->ClusterHits(HitSet, fClusterList);

  fNclust = (*fClusterList).size();   //number of clusters

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

  fOrigin = this->GetOrigin();

  THcShower* fParent = (THcShower*) GetParent();

  fParent->CalcTrackIntercept(Track, pathl, XTrFront, YTrFront);

  // Transform coordiantes to the spectrometer's coordinate system.

  XTrFront += GetOrigin().X();
  YTrFront += GetOrigin().Y();

  Bool_t inFidVol = true;            // In Fiducial Volume flag

  // Re-evaluate Fid. Volume Flag if fid. volume test is requested

  if (fParent->fvTest) {

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

  }

  // Match a cluster to the track.

  Int_t mclust = -1;    // The match cluster #, initialize with a bogus value.
  Double_t Delta = kBig;   // Track to cluster distance

  if (inFidVol) {

    // Since hits and clusters are in reverse order (with respect to Engine),
    // search backwards to be consistent with Engine.
    //
    for (Int_t i=fNclust-1; i>-1; i--) {

      THcShowerCluster* cluster = *(fClusterList->begin()+i);

      Double_t dx = TMath::Abs( clX(cluster) - XTrFront );
      Double_t dy = TMath::Abs( clZ(cluster) - YTrFront );
      Double_t distance = TMath::Sqrt(dx*dx+dy*dy);

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
Int_t THcShowerArray::FineProcess( TClonesArray& tracks )
{
  return 0;
}

//_____________________________________________________________________________
Int_t THcShowerArray::ProcessHits(TClonesArray* rawhits, Int_t nexthit)
{
  // Extract the data for this layer from hit list.

  THcShower* fParent;
  fParent = (THcShower*) GetParent();

  // Initialize variables.

  Int_t nADCHits=0;
  fADCHits->Clear();

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

  Int_t ngood = 0;
  Int_t threshold = 100;
  while(ihit < nrawhits) {
    THcRawShowerHit* hit = (THcRawShowerHit *) rawhits->At(ihit);

    if(hit->fPlane != fLayerNum) {
      break;
    }
    
    // Should probably check that counter # is in range
    if(fUsingFADC) {
      fA[hit->fCounter-1] = hit->GetData(0,fPedSampLow,fPedSampHigh,
					 fDataSampLow,fDataSampHigh);
      fP[hit->fCounter-1] = hit->GetPedestal(0,fPedSampLow,fPedSampHigh);
    } else {
          fA[hit->fCounter-1] = hit->GetData(0);
    }

    if(fA[hit->fCounter-1] > threshold) {
      ngood++;
    }

    // Sparsify hits, fill the hit list, compute the energy depostion.

    if(fA[hit->fCounter-1] >  fThresh[hit->fCounter -1]) {

      THcSignalHit *sighit = (THcSignalHit*)fADCHits->ConstructedAt(nADCHits++);
      sighit->Set(hit->fCounter, fA[hit->fCounter-1]);

      fUsingFADC ?
	fA_p[hit->fCounter-1] = fA[hit->fCounter-1] :
	fA_p[hit->fCounter-1] = fA[hit->fCounter-1] - fPed[hit->fCounter -1];

      fE[hit->fCounter-1] += fA_p[hit->fCounter-1] * fGain[hit->fCounter-1];
    }

    // Accumulate energies in the plane.

    fEarray += fE[hit->fCounter-1];

    ihit++;
  }
    
#if 0
  if(ngood > 0) {
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
  if(ngood > 0) {
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

  if (fParent->fdbg_decoded_cal) {

    cout << "---------------------------------------------------------------\n";
    cout << "Debug output from THcShowerArray::ProcessHits for "
    	 << fParent->GetPrefix() << ":" << endl;

    cout << "  nrawhits =  " << nrawhits << "  nexthit =  " << nexthit << endl;
    cout << "  Sparsified hits for shower array, plane #" << fLayerNum
	 << ", " << GetName() << ":" << endl;

    Int_t nspar = 0;
    for (Int_t jhit = nexthit; jhit < nrawhits; jhit++) {

      THcRawShowerHit* hit = (THcRawShowerHit *) rawhits->At(jhit);
      if(hit->fPlane != fLayerNum) {
	break;
      }

      if(fA[hit->fCounter-1] > fThresh[hit->fCounter -1]) {
	cout << "  counter =  " << hit->fCounter
	     << "  E = " << fE[hit->fCounter-1]
	     << endl;
	nspar++;
      }
    }

    if (nspar == 0) cout << "  No hits\n";

    cout << "  Earray = " << fEarray << endl;
    cout << "---------------------------------------------------------------\n";
  }

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

    Int_t adc = fUsingFADC ?
      hit->GetData(0,fPedSampLow,fPedSampHigh,fDataSampLow,fDataSampHigh)
      :
      hit->GetData(0);


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

      Int_t adc = fUsingFADC ?
	hit->GetData(0,fPedSampLow,fPedSampHigh,fDataSampLow,fDataSampHigh)
	:
	hit->GetData(0);

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
