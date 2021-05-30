#include "common.hxx"
#include "mem.h"

using namespace SME::Util;

void Spc::read8(TSpcInterp *interp, u32 argc) {
    SME_ASSERT(argc == 2, "Incorrect number of arguments passed to spc::read8 (%lu args passed, 2 needed)", argc);
    u8 *dst = reinterpret_cast<u8 *>(Spc::Stack::popItem(interp).mValue);
    Spc::Stack::pushItem(interp, *dst, Spc::ValueType::INT);
}

void Spc::read16(TSpcInterp *interp, u32 argc) {
    SME_ASSERT(argc == 2, "Incorrect number of arguments passed to spc::read16 (%lu args passed, 2 needed)", argc);
    u16 *dst = reinterpret_cast<u16 *>(Spc::Stack::popItem(interp).mValue);
    Spc::Stack::pushItem(interp, *dst, Spc::ValueType::INT);
}

void Spc::read32(TSpcInterp *interp, u32 argc) {
    SME_ASSERT(argc == 2, "Incorrect number of arguments passed to spc::read32 (%lu args passed, 2 needed)", argc);
    u32 *dst = reinterpret_cast<u32 *>(Spc::Stack::popItem(interp).mValue);
    Spc::Stack::pushItem(interp, *dst, Spc::ValueType::INT);
}

void Spc::write8(TSpcInterp *interp, u32 argc) {
    SME_ASSERT(argc == 2, "Incorrect number of arguments passed to spc::write8 (%lu args passed, 2 needed)", argc);
    u8 *dst = reinterpret_cast<u8 *>(Spc::Stack::popItem(interp).mValue);
    *dst = static_cast<u8>(Spc::Stack::popItem(interp).mValue);
}

void Spc::write16(TSpcInterp *interp, u32 argc) {
    SME_ASSERT(argc == 2, "Incorrect number of arguments passed to spc::write16 (%lu args passed, 2 needed)", argc);
    u16 *dst = reinterpret_cast<u16 *>(Spc::Stack::popItem(interp).mValue);
    *dst = static_cast<u16>(Spc::Stack::popItem(interp).mValue);
}

void Spc::write32(TSpcInterp *interp, u32 argc) {
    SME_ASSERT(argc == 2, "Incorrect number of arguments passed to spc::write32 (%lu args passed, 2 needed)", argc);
    u32 *dst = reinterpret_cast<u32 *>(Spc::Stack::popItem(interp).mValue);
    *dst = static_cast<u32>(Spc::Stack::popItem(interp).mValue);
}

void Spc::memcpy_(TSpcInterp *interp, u32 argc) {
    SME_ASSERT(argc == 3, "Incorrect number of arguments passed to spc::memcpy (%lu args passed, 3 needed)", argc);
    void *dst = reinterpret_cast<void *>(Spc::Stack::popItem(interp).mValue);
    void *src = reinterpret_cast<void *>(Spc::Stack::popItem(interp).mValue);
    size_t size = Spc::Stack::popItem(interp).mValue;
    memcpy(dst, src, size);
}

void Spc::memmove_(TSpcInterp *interp, u32 argc) {
    SME_ASSERT(argc == 3, "Incorrect number of arguments passed to spc::memmove (%lu args passed, 3 needed)", argc);
    void *dst = reinterpret_cast<void *>(Spc::Stack::popItem(interp).mValue);
    void *src = reinterpret_cast<void *>(Spc::Stack::popItem(interp).mValue);
    size_t size = Spc::Stack::popItem(interp).mValue;
    memmove(dst, src, size);
}

void Spc::memcmp_(TSpcInterp *interp, u32 argc) {
    SME_ASSERT(argc == 3, "Incorrect number of arguments passed to spc::memcmp (%lu args passed, 3 needed)", argc);
    void *dst = reinterpret_cast<void *>(Spc::Stack::popItem(interp).mValue);
    void *src = reinterpret_cast<void *>(Spc::Stack::popItem(interp).mValue);
    size_t size = Spc::Stack::popItem(interp).mValue;
    memcmp(dst, src, size);
}

void Spc::memset_(TSpcInterp *interp, u32 argc) {
    SME_ASSERT(argc == 3, "Incorrect number of arguments passed to spc::memset (%lu args passed, 3 needed)", argc);
    void *dst = reinterpret_cast<void *>(Spc::Stack::popItem(interp).mValue);
    u32 fill = Spc::Stack::popItem(interp).mValue;
    size_t size = Spc::Stack::popItem(interp).mValue;
    memset(dst, fill, size);
}