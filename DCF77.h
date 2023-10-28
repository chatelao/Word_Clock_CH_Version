// DCF77

// Klasse fuer die Auswertung des DCF77-Zeitsignals.
// Diese Klasse geht nicht von einem 'sauberen' Signal aus,
// braucht aber eine externe 1-Sekunden-Basis. Sie ermittelt
// die HIGH/LOWs statistisch.

#ifndef MYDCF77_H
#define MYDCF77_H

#include "Arduino.h"
#include "Configuration.h"

class DCF77 
  {
public:
  DCF77(uint8_t signalPin, uint8_t statusLedPin);
  
  void statusLed(boolean on);
  
  void poll(boolean signalIsInverted);
  boolean newSecond();
  
  uint8_t getBitAtPos(uint8_t pos);
  uint8_t getBitPointer();
  
  uint8_t getMinutes();
  uint8_t getHours();
  
  uint8_t getDate();
  uint8_t getDayOfWeek();
  uint8_t getMonth();
  uint8_t getYear();
  
  char*   asString();
  
  boolean signal(boolean signalIsInverted);

private:
  uint8_t _signalPin;
  uint8_t _statusLedPin;
  
  uint16_t _highcount;
  uint16_t _meanvalues[MYDCF77_MEANCOUNT];
  uint8_t  _meanpointer;
  
  uint8_t _minutes;
  uint8_t _hours;
  
  uint8_t _date;
  uint8_t _dayOfWeek;
  uint8_t _month;
  uint8_t _year;
  
  char    _cDateTime[20];
  
  uint8_t _bits[MYDCF77_TELEGRAMMLAENGE];
  uint8_t _bitsPointer;
  
  boolean decode();
  
  void clearBits();
  };

#endif
