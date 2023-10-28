// Button
// Kleine Klasse zum Entprellen der Tasten.

#include "Button.h"

// #define DEBUG
#include "Configuration.h"

// Initialisierung mit dem Pin, an dem der Taster haengt.
// @param  pin: der Pin, an dem der Taster haengt
//         pressedAgainst: wogegen schaltet der Taster? (HIGH/LOW)
//------------------------------------------------------------------------------------------------------------
Button::Button(uint8_t pin, uint8_t pressedAgainst) 
  {
  _pin1 = pin;
  _lastPressTime = 0;
  _doubleMode = false;
  _pressedAgainst = pressedAgainst;
  if (_pressedAgainst == HIGH) 
    {
    pinMode(_pin1, INPUT);
    } 
  else 
    {
    pinMode(_pin1, INPUT_PULLUP);
    }
  }


// Initialisierung mit den zwei Pins, an denen die Taster haengen, die man gleichzeitig abfragen moechte.
// @param  pin1, pin2: die Pins, an dem der virtuelle Taster haengt
//         pressedAgainst: wogegen schalten die Taster? (HIGH/LOW)
//------------------------------------------------------------------------------------------------------------
Button::Button(uint8_t pin1, uint8_t pin2, uint8_t pressedAgainst) 
  {
  _pin1 = pin1;
  _pin2 = pin2;
  _lastPressTime = 0;
  _doubleMode = true;
  _pressedAgainst = pressedAgainst;
  _pressedAgainst = pressedAgainst;
  if (_pressedAgainst == HIGH) 
    {
    pinMode(_pin1, INPUT);
    pinMode(_pin2, INPUT);
    } 
  else 
    {
    pinMode(_pin1, INPUT_PULLUP);
    pinMode(_pin2, INPUT_PULLUP);
    }
  }


// Wurde der Taster gedrueckt?
//------------------------------------------------------------------------------------------------------------
boolean Button::pressed() 
  {
  boolean _retVal = false;
  
  if (millis() < _lastPressTime) 
    {
    // wir hatten einen Ueberlauf...
    _lastPressTime = millis();
    }
  
  if (!_doubleMode) 
    {
    if ((digitalRead(_pin1) == _pressedAgainst) && (_lastPressTime + BUTTON_TRESHOLD < millis())) 
      {
      _lastPressTime = millis();
      _retVal = true;
      }
    } 
  else 
    {
    if ((digitalRead(_pin1) == _pressedAgainst) && (digitalRead(_pin2) == _pressedAgainst) && (_lastPressTime + BUTTON_TRESHOLD < millis())) 
      {
      _lastPressTime = millis();
      _retVal = true;
      }
    }
  return _retVal;
  }
  
