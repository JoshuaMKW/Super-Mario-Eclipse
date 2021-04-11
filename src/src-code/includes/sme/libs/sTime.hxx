#pragma once

#include "OS.h"
#include "libs/sString.hxx"

namespace SME::Util::Time {
constexpr const char *buildDate() { return __DATE__; }
constexpr const char *buildTime() { return __TIME__; }
OSTime ostime() { return OSGetTime(); }
void calendarTime(OSCalendarTime &result) {
  return OSTicksToCalendarTime(ostime(), &result);
}
char *date();
char *time();
u16 nanosecond();
u16 microsecond();
u16 millisecond();
u8 second();
u8 minute();
u8 hour();
u8 day();
u8 month();
u8 year();
char *calendarToDate(OSCalendarTime &calendar);
char *calendarToTime(OSCalendarTime &calendar);
} // namespace SME::Util::Time