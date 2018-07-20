/**
   \class Scaler9250
   \ingroup Decoders

   \brief Decoder module to read the FADC250 scalers.

   These scalers are identified by a bank with the tag 9250.
*/

#include "Scaler9250.h"

using namespace std;

namespace Decoder {

Module::TypeIter_t Scaler9250::fgThisType =
  DoRegister( ModuleType( "Decoder::Scaler9250" , 9250 ));

Scaler9250::Scaler9250(Int_t crate, Int_t slot) : GenScaler(crate, slot) {
  Init();
}

Scaler9250::~Scaler9250() {
}

void Scaler9250::Init() {
  fNumChan = 16;
  fWordsExpect = 16;
  GenScaler::GenInit();
}

}

ClassImp(Decoder::Scaler9250)
