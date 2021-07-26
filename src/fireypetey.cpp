
#include "SME.hxx"


using namespace SME;


TBossPakkun* Patch::FireyPetey::createInstance() {
  return new TFireyPetey((char*)0x8039cb10);
}