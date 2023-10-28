// IRTranslator
// Abstrakte Klasse fuer die Umsetzung von Fernbedienungs-Codes.

#include "IRTranslator.h"

uint8_t IRTranslator::getRed() 
  {
  return _red;
  }

uint8_t IRTranslator::getGreen() 
  {
  return _green;
  }

uint8_t IRTranslator::getBlue() 
  {
  return _blue;
  }

void IRTranslator::setColor(uint8_t red, uint8_t green, uint8_t blue) 
  {
  _red = red;
  _green = green;
  _blue = blue;
  }
