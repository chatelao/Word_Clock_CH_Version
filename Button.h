// Button
// Kleine Klasse zum Entprellen der Tasten.

#ifndef BUTTON_H
#define BUTTON_H

#include "Arduino.h"

class Button 
{
public:
    Button(uint8_t pin, uint8_t pressedAgainst);
    Button(uint8_t pin1, uint8_t pin2, uint8_t pressedAgainst);

    boolean pressed();

private:
    uint8_t _pin1;
    uint8_t _pin2;
    boolean _doubleMode;
    uint8_t _pressedAgainst;
    uint32_t _lastPressTime;
  };

#endif
