// LDR
// Klasse fuer den Zugriff auf einen lichtabhaengigen Widerstand.

#include "LDR.h"
#include "Configuration.h"

#undef DEBUG

#ifdef DEBUG
    #define DEBUG_PRINTF   Serial.println
#else
    #define DEBUG_PRINTF(...)
#endif


// Initialisierung mit dem Pin, an dem der LDR haengt.
// Die Maximalwerte vom LDR koennen automatisch
// eingemessen werden (LDR_AUTOSCALE).
// Ansonsten muss man diese Werte im #define-DEBUG-Mode
// ausmessen und eintragen.
//------------------------------------------------------------------------------------------------------------
LDR::LDR(uint8_t pin, boolean isInverted) 
  {
  _pin = pin;
  _isInverted = isInverted;
  _lastValue = 0;
  _outputValue = 0;
#ifdef LDR_AUTOSCALE
  _min = 1023;
  _max = 0;
  #else
  _min = LDR_MANUAL_MIN;
  _max = LDR_MANUAL_MAX;
#endif
  }


// Welchen Wert hat der LDR? In Prozent...
//------------------------------------------------------------------------------------------------------------
uint8_t LDR::value() 
  {
  int16_t rawVal, val;
  if (!_isInverted) 
    {
    rawVal = analogRead(_pin);
    } 
  else 
    {
    rawVal = (1023 - analogRead(_pin));
    }
  
  if ((rawVal != _lastValue) && ((rawVal == 0) || (rawVal == 1023) || (rawVal > (_lastValue + LDR_HYSTERESE) || (rawVal < _lastValue - LDR_HYSTERESE)))) 
    {
    val = rawVal;
    _lastValue = val;
#ifdef LDR_AUTOSCALE
    if (val < _min) 
      {
      _min = val;
      }
    if (val > _max) 
      {
      _max = val;
      }
#else
    val = constrain(val, _min, _max);
#endif
    int16_t mapVal = map(val, _min, _max, 0, 100);
    mapVal = constrain(mapVal, LDR_MIN_PERCENT, LDR_MAX_PERCENT);
    DEBUG_PRINTF(F("rawVal: %u val: %u _min: %u _max: %u mapValue: %u\n"), rawVal, val, _min, _max, mapVal);
    _outputValue = mapVal;
    }
  return _outputValue;
  }
  
