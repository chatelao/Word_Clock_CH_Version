// DCF77

// Klasse fuer die Auswertung des DCF77-Zeitsignals.
// Diese Klasse geht nicht von einem 'sauberen' Signal aus,
// braucht aber eine externe 1-Sekunden-Basis. Sie ermittelt
// die HIGH/LOWs statistisch.
//------------------------------------------------------------------------------------------------------------

#include "DCF77.h"
#include "Configuration.h"

#undef DEBUG

#ifdef DEBUG
    #define DEBUG_PRINTF   Serial.println
#else
    #define DEBUG_PRINTF(...)
#endif


// Initialisierung mit dem Pin, an dem das Signal des Empfaengers anliegt
//------------------------------------------------------------------------------------------------------------
DCF77::DCF77(uint8_t signalPin, uint8_t statusLedPin) 
  {
  _signalPin = signalPin;
  pinMode(_signalPin, INPUT);

  _statusLedPin = statusLedPin;
  pinMode(_statusLedPin, OUTPUT);
  digitalWrite(_statusLedPin, LOW);
  
  clearBits();
  
  for (uint8_t i = 0; i < MYDCF77_MEANCOUNT; i++) 
    {
    _meanvalues[i] = MYDCF77_MEANSTARTVALUE;
    }
  _meanpointer = 0;
  _highcount = 0;
  }


// Die LED ein- oder ausschalten.
//------------------------------------------------------------------------------------------------------------
void DCF77::statusLed(boolean on) 
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


// Liegt ein Signal vom Empfaenger an?
//------------------------------------------------------------------------------------------------------------
boolean DCF77::signal(boolean signalIsInverted) 
  {
  boolean val;
  if (signalIsInverted) 
    {
    val = (digitalRead(_signalPin) == LOW);
    } 
  else 
    {
    val = (digitalRead(_signalPin) == HIGH);
    }
  return val;
  }


// Aufsammeln der Zustaende des DCF77-Signals.
//------------------------------------------------------------------------------------------------------------
void DCF77::poll(boolean signalIsInverted) 
  {
  if (signal(signalIsInverted)) 
    {
    _highcount++;
    }
  }


// Eine Sekunde startet. Muss von einem externen
// Zeitgeber, z. B. einer RTC, aufgerufen werden.
// Zurueckgegeben wird ein Wahrheitswert.
// TRUE bedeutet, das Zeittelegramm wurde korrekt ausgewertet, die Zeitdaten
// koennen mit den Gettern abgerufen werden.
// FALSE bedeutet, die Auswertung laeuft oder war falsch, die Getter liefern
// alte Informationen.
//------------------------------------------------------------------------------------------------------------
boolean DCF77::newSecond() 
  {
  boolean retVal = false;
  
  if (_highcount >= 100) 
    {
    // Daten
    _meanvalues[_meanpointer] = _highcount;
    _meanpointer++;
    if (_meanpointer > MYDCF77_MEANCOUNT) 
      {
      _meanpointer = 0;
      }
      
    uint32_t average = 0;
    for (uint8_t i = 0; i < MYDCF77_MEANCOUNT; i++) 
      {
      average += _meanvalues[i];
      }
    average /= MYDCF77_MEANCOUNT;
    
    DEBUG_PRINTF(F("Sekunde = %02u,  DCF-Bit = "), _bitsPointer);
    
    if (_highcount > average) 
      {
      _bits[_bitsPointer] = 1;
      DEBUG_PRINTF(F("1"));
      } 
    else 
      {
      _bits[_bitsPointer] = 0;
      DEBUG_PRINTF(F("0"));
      }
    DEBUG_PRINTF(F(",  Durchschnitt = %4u; "), average);
    DEBUG_PRINTF(F("Impulslaenge = %4u;\n"), _highcount);
    
    _bitsPointer++;
    if (_bitsPointer > MYDCF77_TELEGRAMMLAENGE) 
      {
      _bitsPointer = 0;
      }
    } 
  else 
    {
    // Pause
    DEBUG_PRINTF(F("\nPAUSE\n------\n"));
    retVal = decode();
    _bitsPointer = 0;
    clearBits();
    }
  _highcount = 0;
  return retVal;
  }


// Ein Bit im Array zum Debuggen (Anzeigen) bekommen.
//------------------------------------------------------------------------------------------------------------
uint8_t DCF77::getBitAtPos(uint8_t pos) 
  {
  return _bits[pos];
  }


// Den Pointer zum Debuggen (Anzeigen) bekommen.
//------------------------------------------------------------------------------------------------------------
uint8_t DCF77::getBitPointer() 
  {
  return _bitsPointer;
  }


// Decodierung des Telegramms...
//------------------------------------------------------------------------------------------------------------
boolean DCF77::decode() 
  {
  int16_t c = 0; // bitcount for checkbit
  boolean ok = true;
  
  DEBUG_PRINTF(F("Telegramm dekodieren...\n"));
  
  if (_bits[20] != 1) 
    {
    ok = false;
    DEBUG_PRINTF(F("Pruefbit S Fehler\n"));
    }
  
  if (_bits[17] == _bits[18]) 
    {
    ok = false;
    DEBUG_PRINTF(F("Pruefbit Z1 != Z2 Fehler\n"));
    }
  
  // minutes
  _minutes = 0;
  c = 0;
  if (_bits[21] == 1) 
    {
    c++;
    _minutes += _bits[21] * 1;
    }
  if (_bits[22] == 1) 
    {
    c++;
    _minutes += _bits[22] * 2;
    }
  if (_bits[23] == 1) 
    {
    c++;
    _minutes += _bits[23] * 4;
    }
  if (_bits[24] == 1) 
    {
    c++;
    _minutes += _bits[24] * 8;
    }
  if (_bits[25] == 1) 
    {
    c++;
    _minutes += _bits[25] * 10;
    }
  if (_bits[26]) 
    {
    c++;
    _minutes += _bits[26] * 20;
    }
  if (_bits[27]) 
    {
    c++;
    _minutes += _bits[27] * 40;
    }
  DEBUG_PRINTF(F("Minuten: \t%02u\n"), _minutes);

  if ((c + _bits[28]) % 2 != 0) 
    {
    ok = false;
    DEBUG_PRINTF(F("Paritibit P1: Fehler\n"));
    }
  
  // hour
  _hours = 0;
  c = 0;
  if (_bits[29] == 1) 
    {
    c++;
    _hours += _bits[29] * 1;
    }
  if (_bits[30] == 1) 
    {
    c++;
    _hours += _bits[30] * 2;
    }
  if (_bits[31] == 1) 
    {
    c++;
    _hours += _bits[31] * 4;
    }
  if (_bits[32] == 1) 
    {
    c++;
    _hours += _bits[32] * 8;
    }
  if (_bits[33] == 1) 
    {
    c++;
    _hours += _bits[33] * 10;
    }
  if (_bits[34] == 1) 
    {
    c++;
    _hours += _bits[34] * 20;
    }
  DEBUG_PRINTF(F("Stunden:\t%02u\n"), _hours);

  if ((c + _bits[35]) % 2 != 0) 
    {
    ok = false;
    DEBUG_PRINTF(F("Paritibit P2: Fehler\n"));
    }

  // date
  _date = 0;
  c = 0;
  if (_bits[36] == 1) 
    {
    c++;
    _date += _bits[36] * 1;
    }
  if (_bits[37] == 1) 
    {
    c++;
    _date += _bits[37] * 2;
    }
  if (_bits[38] == 1) 
    {
    c++;
    _date += _bits[38] * 4;
    }
  if (_bits[39] == 1) 
    {
    c++;
    _date += _bits[39] * 8;
    }
  if (_bits[40] == 1) 
    {
    c++;
    _date += _bits[40] * 10;
    }
  if (_bits[41] == 1) 
    {
    c++;
    _date += _bits[41] * 20;
    }
  DEBUG_PRINTF(F("Tag:\t\t%02u\n"), _date);
  
  // day of week
  _dayOfWeek = 0;
  if (_bits[42] == 1) 
    {
    c++;
    _dayOfWeek += _bits[42] * 1;
    }
  if (_bits[43] == 1) 
    {
    c++;
    _dayOfWeek += _bits[43] * 2;
    }
  if (_bits[44] == 1)
    {
    c++;
    _dayOfWeek += _bits[44] * 4;
    }
  DEBUG_PRINTF(F("Wochentag:\t %u\n"), _dayOfWeek);
  
  // month
  _month = 0;
  if (_bits[45] == 1) 
    {
    c++;
    _month += _bits[45] * 1;
    }
  if (_bits[46] == 1) 
    {
    c++;
    _month += _bits[46] * 2;
    }
  if (_bits[47] == 1) 
    {
    c++;
    _month += _bits[47] * 4;
    }
  if (_bits[48] == 1) 
    {
    c++;
    _month += _bits[48] * 8;
    }
  if (_bits[49] == 1) 
    {
    c++;
    _month += _bits[49] * 10;
    }
  DEBUG_PRINTF(F("Monat:\t\t%02u\n"), _month);
  
  // year
  _year = 0;
  if (_bits[50] == 1) 
    {
    c++;
    _year += _bits[50] * 1;
    }
  if (_bits[51] == 1) 
    {
    c++;
    _year += _bits[51] * 2;
    }
  if (_bits[52] == 1) 
    {
    c++;
    _year += _bits[52] * 4;
    }
  if (_bits[53] == 1) 
    {
    c++;
    _year += _bits[53] * 8;
    }
  if (_bits[54] == 1) 
    {
    c++;
    _year += _bits[54] * 10;
    }
  if (_bits[55] == 1) 
    {
    c++;
    _year += _bits[55] * 20;
    }
  if (_bits[56] == 1) 
    {
    c++;
    _year += _bits[56] * 40;
    }
  if (_bits[57] == 1) 
    {
    c++;
    _year += _bits[57] * 80;
    }
  DEBUG_PRINTF(F("Jahr:\t\t%02u\n"), _year);

  if ((c + _bits[58]) % 2 != 0) 
    {
    ok = false;
    DEBUG_PRINTF(F("Paritibit P3: Fehler\n"));
    }
    
  if (!ok) 
    {
    // discard date...
    _minutes = 0;
    _hours = 0;
    _date = 0;
    _dayOfWeek = 0;
    _month = 0;
    _year = 0;
    }
  
  return ok;
  }


// Das Zeittelegramm als String bekommen
char* DCF77::asString() 
  {
  _cDateTime[0] = 0;

  sprintf(_cDateTime, "%02d:%02d  %02d.%02d.%02d", _hours, _minutes, _date, _month, _year);
  return _cDateTime;
  }

/**
// Das Bits-Array loeschen.
 */
void DCF77::clearBits() 
  {
  for (uint8_t i = 0; i < MYDCF77_TELEGRAMMLAENGE; i++) 
    {
    _bits[i] = 0;
    }
  _bitsPointer = 0;
  }

//
// Getter
//

uint8_t DCF77::getMinutes() 
  {
  return _minutes;
  }

uint8_t DCF77::getHours() 
  {
  return _hours;
  }

uint8_t DCF77::getDate() 
  {
  return _date;
  }

uint8_t DCF77::getDayOfWeek() 
  {
  return _dayOfWeek;
  }

uint8_t DCF77::getMonth() 
  {
  return _month;
  }

uint8_t DCF77::getYear() 
  {
  return _year;
  }
  
