// DCF77Helper

// Klasse um die Qualitaet der DCF77-Samples zu verbessern. Dazu wird der Minutenabstand
// zwischen den empfangenen DCF77-Samples mit Hilfe der Echtzeituhr verglichen.
//------------------------------------------------------------------------------------------------------------

#include "DCF77Helper.h"
#include "Configuration.h"

#undef DEBUG


#ifdef DEBUG
    #define DEBUG_PRINTF   Serial.printf
#else
    #define DEBUG_PRINTF(...)
#endif

// Initialisierung und das Array 'falsch' vorbelegen, damit der Zeitabstand
// der Samples zum Anfang nicht stimmt.
//------------------------------------------------------------------------------------------------------------
DCF77Helper::DCF77Helper() 
  {
  _cursor = 0;
  for (uint8_t i = 0; i < DCF77HELPER_MAX_SAMPLES; i++) 
    {
    _zeitstempelDcf77[i] = new TimeStamp(i, i, i, i, i, i);
    _zeitstempelRtc[i] = new TimeStamp(100, 0, 0, 0, 0, 0);
    }
  }


// Einen neuen Sample hinzufuegen.
//------------------------------------------------------------------------------------------------------------
void DCF77Helper::addSample(DCF77 dcf77, RTC rtc) 
  {
  _zeitstempelDcf77[_cursor]->setFrom(dcf77);
  DEBUG_PRINTF(F("Zeitstempel von DCF:\t%s\n"), _zeitstempelDcf77[_cursor]->asString());
  
  _zeitstempelRtc[_cursor]->setFrom(rtc);
  DEBUG_PRINTF(F("Zeitstempel von RTC:\t%s\n"), _zeitstempelRtc[_cursor]->asString());
  
  _cursor++;
  if (_cursor == DCF77HELPER_MAX_SAMPLES) 
    {
    _cursor = 0;
    }
  }


// Die Samples vergleichen
//------------------------------------------------------------------------------------------------------------
boolean DCF77Helper::samplesOk() 
  {
  boolean ret = true;
  for (uint8_t i = 0; i < DCF77HELPER_MAX_SAMPLES - 1; i++) 
    {
    // Teste den Minutenabstand zwischen den Zeitstempeln...
    if ((_zeitstempelDcf77[i]->getMinutesOfDay() - _zeitstempelDcf77[i + 1]->getMinutesOfDay()) != (_zeitstempelRtc[i]->getMinutesOfDay() - _zeitstempelRtc[i + 1]->getMinutesOfDay())) 
      {
      DEBUG_PRINTF(F("Zeitabstand %i ist falsch  (%5u"), i, _zeitstempelDcf77[i]->getMinutesOfDay() - _zeitstempelDcf77[i + 1]->getMinutesOfDay());
      DEBUG_PRINTF(F(" != %5u)\n"), _zeitstempelRtc[i]->getMinutesOfDay() - _zeitstempelRtc[i + 1]->getMinutesOfDay());
      ret = false;
      }
    else
      {
      DEBUG_PRINTF(F("Zeitabstand %i ist richtig (%5u"), i, _zeitstempelDcf77[i]->getMinutesOfDay() - _zeitstempelDcf77[i + 1]->getMinutesOfDay());
      DEBUG_PRINTF(F(" == %5u)\n"), _zeitstempelRtc[i]->getMinutesOfDay() - _zeitstempelRtc[i + 1]->getMinutesOfDay());
      }
    }

  return ret;
  }
