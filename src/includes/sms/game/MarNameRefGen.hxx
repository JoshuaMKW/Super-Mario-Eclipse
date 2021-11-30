#pragma once

#include "JDrama/JDRNameRef.hxx"
#include "JDrama/JDRNameRefGen.hxx"

class TMarNameRefGen : public JDrama::TNameRefGen {
public:
  virtual JDrama::TNameRef *getNameRef(const char *) const override;

  JDrama::TNameRef *getNameRef_BossEnemy(const char *) const;
  JDrama::TNameRef *getNameRef_Enemy(const char *) const;
  JDrama::TNameRef *getNameRef_Map(const char *) const;
  JDrama::TNameRef *getNameRef_MapObj(const char *) const;
  JDrama::TNameRef *getNameRef_NPC(const char *) const;
};