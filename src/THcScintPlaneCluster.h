#ifndef ROOT_THcScintPlaneCluster
#define ROOT_THcScintPlaneCluster

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// THcScintPlaneCluster                                                             //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include <cstdio>

class THcScintPlaneCluster : public TObject {

 public:
 THcScintPlaneCluster(Int_t cluster=0, Double_t pos=0.0) :
  fClusterNumber(cluster), fClusterPosition(pos) {}
  virtual ~THcScintPlaneCluster() {}

  Int_t GetClusterNumber() {return fClusterNumber;}
  Double_t GetClusterPosition() {return fClusterPosition;}
  Double_t GetClusterSize() {return fClusterSize;}
  Double_t GetClusterFlag() {return fClusterFlag;}
  Double_t GetClusterUsedFlag() {return fClusterUsedFlag;}

  virtual void Set(Int_t cluster, Double_t pos)
  { fClusterNumber=cluster; fClusterPosition=pos; fClusterFlag=0.; fClusterUsedFlag=0.;}

  void SetSize(Double_t size) {fClusterSize=size;}
  void SetFlag(Double_t flag) {fClusterFlag=flag;}
  void SetUsedFlag(Double_t flag) {fClusterUsedFlag=flag;}

 private:
  Int_t fClusterNumber;
  Double_t fClusterPosition;
  Double_t fClusterSize;
  Double_t fClusterFlag;
  Double_t fClusterUsedFlag;

  ClassDef(THcScintPlaneCluster,0); // Single signal value and wire/counter number
};
/////////////////////////////////////////////////////////////////
#endif
