////////////////////////////////////////////////////////////////////
//
//   TI scalers
//     Identified by a 9001 bank
//
/////////////////////////////////////////////////////////////////////

#include "Scaler9001.h"

using namespace std;

namespace Decoder {

Module::TypeIter_t Scaler9001::fgThisType =
  DoRegister( ModuleType( "Decoder::Scaler9001" , 9001 ));

Scaler9001::Scaler9001(Int_t crate, Int_t slot) : GenScaler(crate, slot) {
  Init();
}

Scaler9001::~Scaler9001() {
}

void Scaler9001::Init() {
  fNumChan = 12;
  fWordsExpect = 12;
  GenScaler::GenInit();
}

}

ClassImp(Decoder::Scaler9001)
