// TimeStamp
// Klasse fuer die Kapselung eines Zeitstempels. Dadurch lassen sich
// Zeiten aus der Echtzeituhr und von dem DCF77-Empfaenger
// leichter verarbeiten und vergleichen.

#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include "Arduino.h"
#include "DCF77.h"
#include "RTC.h"

class TimeStamp 
  {
public:
  TimeStamp(uint8_t minutes, uint8_t hours, uint8_t date, uint8_t dayOfWeek, uint8_t month, uint8_t year);
  TimeStamp(DCF77 dcf77);
  TimeStamp(RTC rtc);

  void incMinutes();
  void incHours();

  uint8_t getMinutes();
  uint16_t getMinutesOfDay();
  uint16_t getMinutesOf12HoursDay(int16_t minutesDiff);
  uint8_t getHours();

  uint8_t getDate();
  uint8_t getDayOfWeek();
  uint8_t getMonth();
  uint8_t getYear();

  void setFrom(DCF77 dcf77);
  void setFrom(RTC rtc);
  void set(uint8_t minutes, uint8_t hours, uint8_t date, uint8_t dayOfWeek, uint8_t month, uint8_t year);

  char* asString();

private:
  uint8_t _minutes;
  uint8_t _hours;

  uint8_t _date;
  uint8_t _dayOfWeek;
  uint8_t _month;
  uint8_t _year;

  char _cDateTime[18];
  };

#endif
