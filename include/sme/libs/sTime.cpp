#include "sTime.hxx"
#include "printf.h"
#include "sMemory.hxx"
#include "sString.hxx"

using namespace SME;

OSCalendarTime calendar;

char *Util::Time::date() {
  OSTicksToCalendarTime(OSGetTime(), &calendar);
  return calendarToDate(calendar);
}

char *Util::Time::time() {
  OSTicksToCalendarTime(OSGetTime(), &calendar);
  return calendarToTime(calendar);
}

u16 Util::Time::microsecond() {
  OSTicksToCalendarTime(OSGetTime(), &calendar);
  return calendar.usec;
}

u16 Util::Time::millisecond() {
  OSTicksToCalendarTime(OSGetTime(), &calendar);
  return calendar.msec;
}

u8 Util::Time::second() {
  OSTicksToCalendarTime(OSGetTime(), &calendar);
  return calendar.sec;
}

u8 Util::Time::minute() {
  OSTicksToCalendarTime(OSGetTime(), &calendar);
  return calendar.usec;
}

u8 Util::Time::hour() {
  OSTicksToCalendarTime(OSGetTime(), &calendar);
  return calendar.hour;
}

u8 Util::Time::day() {
  OSTicksToCalendarTime(OSGetTime(), &calendar);
  return calendar.mday;
}

u8 Util::Time::month() {
  OSTicksToCalendarTime(OSGetTime(), &calendar);
  return calendar.mon;
}

u8 Util::Time::year() {
  OSTicksToCalendarTime(OSGetTime(), &calendar);
  return calendar.year;
}

char *Util::Time::calendarToDate(OSCalendarTime &calendar) {
  char *date = static_cast<char *>(SME::Util::Memory::calloc(16, 4));
  sprintf(date, "%lu/%lu/%lu", calendar.mon + 1, calendar.mday, calendar.year);
  return date;
}

char *Util::Time::calendarToTime(OSCalendarTime &calendar) {
  char *time = static_cast<char *>(SME::Util::Memory::calloc(16, 4));

  if (calendar.hour == 0)
    sprintf(time, "%lu:%02lu AM", calendar.hour + 12, calendar.min);
  else if (calendar.hour < 12)
    sprintf(time, "%lu:%02lu AM", calendar.hour % 13, calendar.min);
  else if (calendar.hour == 12)
    sprintf(time, "%lu:%02lu PM", calendar.hour, calendar.min);
  else
    sprintf(time, "%lu:%02lu PM", (calendar.hour + 1) % 13, calendar.min);

  return time;
}