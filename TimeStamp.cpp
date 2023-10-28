// TimeStamp
// Klasse fuer die Kapselung eines Zeitstempels. Dadurch lassen sich
// Zeiten aus der Echtzeituhr und von dem DCF77-Empfaenger
// leichter verarbeiten und vergleichen.

#include "TimeStamp.h"

// #define DEBUG
#include "Configuration.h"

TimeStamp::TimeStamp(uint8_t minutes, uint8_t hours, uint8_t date, uint8_t dayOfWeek, uint8_t month, uint8_t year) 
  {
  set(minutes, hours, date, dayOfWeek, month, year);
  }

TimeStamp::TimeStamp(DCF77 dcf77) 
  {
  setFrom(dcf77);
  }

TimeStamp::TimeStamp(RTC rtc) 
  {
  setFrom(rtc);
  }

uint8_t TimeStamp::getMinutes() 
  {
  return _minutes;
  }

uint16_t TimeStamp::getMinutesOfDay() 
  {
  return _minutes + 60 * _hours;
  }


// Minuten des Tages ohne die Beruecksichtigung von 12/24 Stunden
// (fuer den Wecker)...
uint16_t TimeStamp::getMinutesOf12HoursDay(int16_t minutesDiff) 
  {
  uint16_t ret = _minutes + 60 * _hours + minutesDiff;
  
  while (ret > 12 * 60) 
    {
    ret -= 12 * 60;
    }
  return ret;
  }

uint8_t TimeStamp::getHours() 
  {
  return _hours;
  }

uint8_t TimeStamp::getDate() 
  {
  return _date;
  }

uint8_t TimeStamp::getDayOfWeek() 
  {
  return _dayOfWeek;
  }

uint8_t TimeStamp::getMonth() 
  {
  return _month;
  }

uint8_t TimeStamp::getYear() 
    {
    return _year;
    }

void TimeStamp::setFrom(DCF77 dcf77) 
  {
  _minutes = dcf77.getMinutes();
  _hours = dcf77.getHours();
  _date = dcf77.getDate();
  _dayOfWeek = dcf77.getDayOfWeek();
  _month = dcf77.getMonth();
  _year = dcf77.getYear();
  }

void TimeStamp::setFrom(RTC rtc) 
  {
  _minutes = rtc.getMinutes();
  _hours = rtc.getHours();
  _date = rtc.getDate();
  _dayOfWeek = rtc.getDayOfWeek();
  _month = rtc.getMonth();
  _year = rtc.getYear();
  }

void TimeStamp::set(uint8_t minutes, uint8_t hours, uint8_t date, uint8_t dayOfWeek, uint8_t month, uint8_t year) 
  {
  _minutes = minutes;
  _hours = hours;
  _date = date;
  _dayOfWeek = dayOfWeek;
  _month = month;
  _year = year;
  }


// Die Minuten erhoehen.
void TimeStamp::incMinutes() 
  {
  _minutes++;
  if (_minutes > 59) 
    {
    _minutes = 0;
    }
  }


// Die Stunden erhoehen.
void TimeStamp::incHours() 
  {
  _hours++;
  if (_hours > 23) 
    {
    _hours = 0;
    }
  }


// Die Zeit als String bekommen
char* TimeStamp::asString() 
  {
  _cDateTime[0] = 0;
  char temp[5];
  
  // build the string...
  if (_hours < 10) 
    {
    sprintf(temp, "0%d:", _hours);
    strncat(_cDateTime, temp, strlen(temp));
    } 
  else 
    {
    sprintf(temp, "%d:", _hours);
    strncat(_cDateTime, temp, strlen(temp));
    }
  
  if (_minutes < 10) 
    {
    sprintf(temp, "0%d ", _minutes);
    strncat(_cDateTime, temp, strlen(temp));
    } 
  else 
    {
    sprintf(temp, "%d ", _minutes);
    strncat(_cDateTime, temp, strlen(temp));
    }
  
  if (_date < 10) 
    {
    sprintf(temp, "0%d.", _date);
    strncat(_cDateTime, temp, strlen(temp));
    } 
  else 
    {
    sprintf(temp, "%d.", _date);
    strncat(_cDateTime, temp, strlen(temp));
    }
  
  if (_month < 10) 
    {
    sprintf(temp, "0%d.", _month);
    strncat(_cDateTime, temp, strlen(temp));
    } 
  else 
    {
    sprintf(temp, "%d.", _month);
    strncat(_cDateTime, temp, strlen(temp));
    }
  
  sprintf(temp, "%d", _year);
  strncat(_cDateTime, temp, strlen(temp));
  
  return _cDateTime;
  }
