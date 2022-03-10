#include "printf.h"
#include "string.h"

#include "sms/mapobj/MapObjTree.hxx"

#include "SME.hxx"

#if SME_EXTRA_OBJECTS

// make tree leaf count dynamic, based on number of leaf col files
static TMapObjTree *getLeafCount(TMapObjTree *tree) {
  char cacheBuffer[128];
  char buffer[128];

  snprintf(cacheBuffer, 96, "/scene/mapObj/%sLeaf", tree->mRegisterName);
  strcat(cacheBuffer, "%02d.col");

  s32 num = 0;
  while (true && num < 255) {
    snprintf(buffer, 100, cacheBuffer, num + 1);
    if (!JKRFileLoader::getGlbResource(buffer)) {
      tree->mLeafCount = num;
      return tree;
    }
    num += 1;
  }
  tree->mLeafCount = 0;
  return tree;
}
SME_PATCH_B(SME_PORT_REGION(0x801F6AE4, 0x801EE9BC, 0, 0), getLeafCount);
SME_PATCH_B(SME_PORT_REGION(0x801F6B20, 0x801EE9F8, 0, 0), getLeafCount);
SME_PATCH_B(SME_PORT_REGION(0x801F6B5C, 0x801EEA34, 0, 0), getLeafCount);
SME_PATCH_B(SME_PORT_REGION(0x801F6B98, 0x801EEA70, 0, 0), getLeafCount);
SME_PATCH_B(SME_PORT_REGION(0x801F6BD4, 0x801EEAAC, 0, 0), getLeafCount);

#endif