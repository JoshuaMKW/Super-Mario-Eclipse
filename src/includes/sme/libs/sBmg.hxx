#pragma once

#include "CARD.h"
#include "Globals.hxx"
#include "printf.h"
#include "sString.hxx"
#include "sTime.hxx"
#include "sms/SMS.hxx"
#include "Globals.hxx"

namespace SME::Util {

String &formatBMG(String &msg);
char *formatBMGRaw(char *dst, const char *src, size_t len);

}