#include "menus/OptionMenu.hxx"

TOptionPage::TOptionPage(const char *name, TOptionPage *parent) : TNameRef(name), mParent(parent) {
  mOptions = JGadget::TList<TOptionEntryValue *, JGadget::TAllocator>();
}
TOptionPage::~TOptionPage() {}

void TOptionPage::perform(u32 flags, JDrama::TGraphics *graphics) {
    // ReInitializeGX();
    // SMS_DrawInit__Fv();

    
}

void TOptionPage::move(s32 index) {}
void TOptionPage::enterPage() {}

bool TOptionPage::insertOption(TOptionEntryValue *option, s32 index) {
  s32 i = 0;
  for (auto opt : mOptions) {
    if (i == index) {
      mOptions.insert(opt, option);
      return true;
    }
    ++i;
  }
  return false;
}

void TOptionPage::addOption(TOptionEntryValue *option) {
  mOptions.insert(mOptions.end(), option);
}

void TOptionPage::removeOption(TOptionEntryValue *option) {
  for (auto opt : mOptions) {
    if (opt->mItem == option) {
      mOptions.erase(opt);
      return;
    }
  }
}

void TOptionPage::removeOption(const char *optionName) {
  for (auto opt : mOptions) {
    if (opt->mItem->getName() == optionName) {
      mOptions.erase(opt);
      return;
    }
  }
}