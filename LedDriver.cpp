// LedDriver

// Abstrakte Klasse fuer einen LED-Treiber. Hiervon koennen andere Implementierungen erben 

#include "LedDriver.h"

void LedDriver::setColor(uint8_t red, uint8_t green, uint8_t blue) 
  {
  _red = red;
  _green = green;
  _blue = blue;
  }

uint8_t LedDriver::getRed() 
  {
  return _red;
  }

uint8_t LedDriver::getGreen() 
  {
  return _green;
  }

uint8_t LedDriver::getBlue() 
  {
  return _blue;
  }

void LedDriver::setPixelInScreenBuffer(uint8_t x, uint8_t y, uint16_t matrix[16]) 
  {
  matrix[y] |= 0b1000000000000000 >> x;
  }

boolean LedDriver::getPixelFromScreenBuffer(uint8_t x, uint8_t y, uint16_t matrix[16]) 
  {
  return (matrix[y] & (0b1000000000000000 >> x)) == (0b1000000000000000 >> x);
  }
  
