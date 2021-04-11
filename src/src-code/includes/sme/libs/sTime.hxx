#pragma once

#include "OS.h"
#include "sString.hxx"

namespace SME::Util::Time {
inline constexpr const char *buildDate() { return __DATE__; }
inline constexpr const char *buildTime() { return __TIME__; }
inline OSTime ostime() { return OSGetTime(); }
inline void calendarTime(OSCalendarTime &result) {
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