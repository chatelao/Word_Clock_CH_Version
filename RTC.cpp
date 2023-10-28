// RTC
// Klasse fuer den Zugriff auf die DS1307/DS3231 Echtzeituhr.

#include <Wire.h> // Wire library fuer I2C
#include "RTC.h"
#include "Configuration.h"

// #define DEBUG

#ifdef DEBUG
    #define DEBUG_PRINTF   Serial.printf
#else
    #define DEBUG_PRINTF(...)
#endif


// Initialisierung mit der Adresse der DS1307
//------------------------------------------------------------------------------------------------------------
RTC::RTC(int16_t address, uint8_t statusLedPin) 
  {
  _address = address;
  _statusLedPin = statusLedPin;
  pinMode(_statusLedPin, OUTPUT);
  digitalWrite(_statusLedPin, LOW);
  }


// Die LED ein- oder ausschalten.
//------------------------------------------------------------------------------------------------------------
void RTC::statusLed(boolean on) 
  {
  if (on) 
    {
    digitalWrite(_statusLedPin, HIGH);
    } 
  else 
    {
    digitalWrite(_statusLedPin, LOW);
    }
  }


// Die Uhrzeit auslesen und in den Variablen ablegen
//------------------------------------------------------------------------------------------------------------
void RTC::readTime() 
  {
  uint8_t returnStatus, count, result, retries = 0;
  do 
    {
    // Reset the register pointer
    Wire.beginTransmission(_address);
    Wire.write((uint8_t) 0x00);
    
    result = Wire.endTransmission(false); // false, damit der Bus nicht freigegeben wird und eventuell andere dazwischen kommen (in Multi-MCU-Umgebungen)
    DEBUG_PRINTF(F("Wire.endTransmission(false) = %i\n"), result);
    
    count = Wire.requestFrom(_address, 7);
    DEBUG_PRINTF(F("Wire.requestFrom(_address, 7) = %i\n"), count);
    
    if (count == 7) 
      {
      // Success
      // A few of these need masks because certain bits are control bits
      _seconds = bcdToDec(Wire.read() & 0x7f);
      _minutes = bcdToDec(Wire.read());
      _hours = bcdToDec(Wire.read() & 0x3f); // Need to change this if 12 hour am/pm
      _dayOfWeek = bcdToDec(Wire.read());
      _date = bcdToDec(Wire.read());
      _month = bcdToDec(Wire.read());
      _year = bcdToDec(Wire.read());
      } 
    else 
      {
      // Fail
      // keine 7 Byte zurueck gekommen? Buffer verwerfen...
      for (int16_t i = 0; i < count; i++) 
        {
        Wire.read();
        }
      retries++;
      }
    
    result = Wire.endTransmission(true); // true, jetzt den Bus freigeben.
    DEBUG_PRINTF(F("Wire.endTransmission(true) = %i\n"), result);
    
    } while ((count != 7) && (retries < 8));

  if (retries == 8) 
    {
    // Es konnte nichts gelesen werden
    _seconds = 11;
    _minutes = 11;
    _hours = 11;
    _dayOfWeek = 1;
    _date = 1;
    _month = 1;
    _year = 2016;
    }

  DEBUG_PRINTF(F("Time: %02i:%02i:%02i\n"), getHours(), getMinutes(), getSeconds());
  }


// Die Uhrzeit aus den Variablen in die DS1307 schreiben
//------------------------------------------------------------------------------------------------------------
void RTC::writeTime() 
  {
  Wire.beginTransmission(_address);
  Wire.write((uint8_t) 0x00);       // 0 to bit 7 starts the clock
  Wire.write(decToBcd(_seconds));
  Wire.write(decToBcd(_minutes));
  Wire.write(decToBcd(_hours));     // If you want 12 hour am/pm you need to set bit 6 (also need to change readDateDs1307)
  Wire.write(decToBcd(_dayOfWeek));
  Wire.write(decToBcd(_date));
  Wire.write(decToBcd(_month));
  Wire.write(decToBcd(_year));
  // Wire.write(0b00010000);        // 1Hz Rechteck auf SQW fuer den Display-Update-Interrupt...
  Wire.endTransmission();
  }


// SQW fuer DS3231.
//------------------------------------------------------------------------------------------------------------
void RTC::enableSQWOnDS3231() 
  {
  Wire.beginTransmission(_address);
  Wire.write(0x0E);                 // Datenregister
  Wire.write(0b0000000);            // enable 1HZ square wave output
  Wire.endTransmission();
  }


// Konvertierung Dezimal zu "Binary Coded Decimal"
//------------------------------------------------------------------------------------------------------------
uint8_t RTC::decToBcd(uint8_t val) 
  {
  return ((val / 10 * 16) + (val % 10));
  }


// Konvertierung "Binary Coded Decimal" zu Dezimal
//------------------------------------------------------------------------------------------------------------
uint8_t RTC::bcdToDec(uint8_t val) 
  {
  return ((val / 16 * 10) + (val % 16));
  }


// Aus einem String zwei Stellen als Zahl bekommen.
//------------------------------------------------------------------------------------------------------------
uint8_t RTC::conv2d(const char* p) 
  {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
  }


// A convenient constructor for using "the compiler's time":
// DateTime now (__DATE__, __TIME__);
// NOTE: using PSTR would further reduce the RAM footprint
//------------------------------------------------------------------------------------------------------------
void RTC::set(const char* date, const char* time) 
  {
  // sample input: date = "Feb 15 2016", time = "12:34:56"
  _year = conv2d(date + 9);
  // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
  switch (date[0]) 
    {
    case 'J':
      if (date[1] == 'a') 
        {
        _month = 1;
        } 
      else if (date[2] == 'n') 
        {
        _month = 6;
        } 
      else 
        {
        _month = 7;
        }
      break;
    
    case 'F':
      _month = 2;
      break;
    
    case 'A':
      _month = date[2] == 'r' ? 4 : 8;
      break;
    
    case 'M':
      _month = date[2] == 'r' ? 3 : 5;
      break;
      
    case 'S':
      _month = 9;
      break;
    
    case 'O':
      _month = 10;
      break;
    
    case 'N':
      _month = 11;
      break;
    
    case 'D':
      _month = 12;
      break;
    }
  _date = conv2d(date + 4);
  _hours = conv2d(time);
  _minutes = conv2d(time + 3);
  _seconds = conv2d(time + 6);
  }


// Setter / Getter
//------------------------------------------------------------------------------------------------------------
void RTC::setSeconds(uint8_t seconds) 
  {
  _seconds = seconds;
  }

void RTC::setMinutes(uint8_t minutes) 
  {
  _minutes = minutes;
  }

void RTC::incMinutes() 
  {
  _minutes++;
  if (_minutes > 59) 
    {
    _minutes = 0;
    }
  }

void RTC::setHours(uint8_t hours) 
  {
  _hours = hours;
  }

void RTC::incHours() 
  {
  _hours++;
  if (_hours > 23) 
    {
    _hours = 0;
    }
  }

void RTC::setDayOfWeek(uint8_t dayOfWeek) 
  {
  _dayOfWeek = dayOfWeek;
  }

void RTC::setDate(uint8_t date) 
  {
  _date = date;
  }

void RTC::setMonth(uint8_t month) 
  {
  _month = month;
  }

void RTC::setYear(uint8_t year) 
  {
  _year = year;
  }

uint8_t RTC::getSeconds() 
  {
  return _seconds;
  }

uint8_t RTC::getMinutes() 
  {
  return _minutes;
  }

uint16_t RTC::getMinutesOfDay() 
  {
  return _minutes + 60 * _hours;
  }

// Minuten des Tages ohne die Beruecksichtigung von 12/24 Stunden
// (fuer den Wecker)...

uint16_t RTC::getMinutesOf12HoursDay() 
  {
  int16_t h = _hours;
  while (h > 12) 
    {
    h -= 12;
    }
  return _minutes + 60 * h;
  }

uint8_t RTC::getHours() 
  {
  return _hours;
  }

uint8_t RTC::getDayOfWeek() 
  {
  return _dayOfWeek;
  }

uint8_t RTC::getDate() 
  {
  return _date;
  }

uint8_t RTC::getMonth() 
  {
  return _month;
  }

uint8_t RTC::getYear() 
  {
  return _year;
  }
