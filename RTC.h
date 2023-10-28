// RTC
// Klasse fuer den Zugriff auf die DS3231 Echtzeituhr.

#ifndef MYRTC_H
#define MYRTC_H

#include "Arduino.h"

class RTC 
  {
public:
    RTC(int16_t address, uint8_t statusLedPin);

    void statusLed(boolean on);

    void readTime();
    void writeTime();

    void enableSQWOnDS3231();

    void set(const char* date, const char* time);
    void setSeconds(uint8_t seconds);
    void setMinutes(uint8_t minutes);
    void incMinutes();
    void setHours(uint8_t hours);
    void incHours();
    void setDayOfWeek(uint8_t dayOfWeek);
    void setDate(uint8_t date);
    void setMonth(uint8_t month);
    void setYear(uint8_t year);

    uint8_t getSeconds();
    uint8_t getMinutes();
    uint16_t getMinutesOfDay();
    uint16_t getMinutesOf12HoursDay();
    uint8_t getHours();
    uint8_t getDayOfWeek();
    uint8_t getDate();
    uint8_t getMonth();
    uint8_t getYear();

private:
    int16_t _address;
    uint8_t _statusLedPin;

    uint8_t _seconds;
    uint8_t _minutes;
    uint8_t _hours;
    uint8_t _dayOfWeek;
    uint8_t _date;
    uint8_t _month;
    uint8_t _year;

    uint8_t decToBcd(uint8_t val);
    uint8_t bcdToDec(uint8_t val);
    uint8_t conv2d(const char* p);
};

#endif
