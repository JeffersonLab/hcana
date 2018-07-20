/**
   \class Scaler9001
   \ingroup Decoders

   \brief Decoder module to retrieve the TI scalers.

   These scalers are identified by a bank with the tag 9001.
*/

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
