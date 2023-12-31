// LedDriverNeoPixel
// Implementierung auf der Basis von WS2812B-Streifen wie sie die Adafruit-Neo-Pixel verwenden.

// Es lohnt sich in jedem Fall, den UeberGuide von Adafruit zu lesen:
// https://learn.adafruit.com/adafruit-neopixel-uberguide/overview

// Verkabelung: Einspeisung oben links, dann schlangenfoermig runter,
// dann Ecke unten links, oben links, oben rechts, unten rechts.

#include "LedDriverNeoPixel.h"

// #define DEBUG
#include "Configuration.h"

#define NUM_PIXEL 114


// Initialisierung.
// @param data Pin, an dem die Data-Line haengt.
//------------------------------------------------------------------------------------------------------------
LedDriverNeoPixel::LedDriverNeoPixel(uint8_t dataPin) 
  {
  _dataPin = dataPin;
  _strip = new Adafruit_NeoPixel(NUM_PIXEL, _dataPin, NEO_GRB + NEO_KHZ800);
  _strip->begin();
  setColor(250, 255, 200);
  }


// init() wird im Hauptprogramm in init() aufgerufen.
// Hier sollten die LED-Treiber in eine definierten
// Ausgangszustand gebracht werden.
//------------------------------------------------------------------------------------------------------------
void LedDriverNeoPixel::init() 
  {
  setBrightness(50);
  clearData();
  wakeUp();
  }

void LedDriverNeoPixel::printSignature() 
  {
  Serial.println(F("NeoPixel - WS2812B\n"));
  }


// Den Bildschirm-Puffer auf die LED-Matrix schreiben.
// @param onChange: TRUE, wenn es Aenderungen in dem Bildschirm-Puffer gab,
//                  FALSE, wenn es ein Refresh-Aufruf war.
//------------------------------------------------------------------------------------------------------------
void LedDriverNeoPixel::writeScreenBufferToMatrix(uint16_t matrix[16], boolean onChange) 
  {
  if (onChange || _dirty) 
    {
    _dirty = false;
    _strip->clear();
    
    uint32_t color = _strip->Color(_brightnessScaleColor(getRed()), _brightnessScaleColor(getGreen()), _brightnessScaleColor(getBlue()));
    
    for (uint8_t y = 0; y < 10; y++) 
      {
      for (uint8_t x = 5; x < 16; x++) 
        {
        uint16_t t = 1 << x;
        if ((matrix[y] & t) == t) 
          {
          _setPixel(15 - x, y, color);
          }
        }
      }
    
    // wir muessen die Eck-LEDs umsetzten...
    if ((matrix[1] & 0b0000000000011111) == 0b0000000000011111) 
      {
      _setPixel(110, color); // 1
      }
    if ((matrix[0] & 0b0000000000011111) == 0b0000000000011111) 
      {
      _setPixel(111, color); // 2
      }
    if ((matrix[3] & 0b0000000000011111) == 0b0000000000011111) 
      {
      _setPixel(112, color); // 3
      }
    if ((matrix[2] & 0b0000000000011111) == 0b0000000000011111) 
      {
      _setPixel(113, color); // 4
      }
    
    _strip->show();
    }
  }


// Die Helligkeit des Displays anpassen.
// @param brightnessInPercent Die Helligkeit.
//------------------------------------------------------------------------------------------------------------
void LedDriverNeoPixel::setBrightness(uint8_t brightnessInPercent) 
  {
  if (brightnessInPercent != _brightnessInPercent) 
    {
    _brightnessInPercent = brightnessInPercent;
    _dirty = true;
    }
  }


// Die aktuelle Helligkeit bekommen.
//------------------------------------------------------------------------------------------------------------
uint8_t LedDriverNeoPixel::getBrightness() 
  {
  return _brightnessInPercent;
  }


// Anpassung der Groesse des Bildspeichers.
// @param linesToWrite Wieviel Zeilen aus dem Bildspeicher sollen
//                     geschrieben werden?
//------------------------------------------------------------------------------------------------------------
void LedDriverNeoPixel::setLinesToWrite(uint8_t linesToWrite) 
  {
  }


// Das Display ausschalten.
//------------------------------------------------------------------------------------------------------------
void LedDriverNeoPixel::shutDown() 
  {
  _strip->clear();
  _strip->show();
  }


// Das Display einschalten.
//------------------------------------------------------------------------------------------------------------
void LedDriverNeoPixel::wakeUp() 
  {
  }


// Den Dateninhalt des LED-Treibers loeschen.
//------------------------------------------------------------------------------------------------------------
void LedDriverNeoPixel::clearData() 
  {
  _strip->clear();
  _strip->show();
  }


// Einen X/Y-koordinierten Pixel in der Matrix setzen.
//------------------------------------------------------------------------------------------------------------
void LedDriverNeoPixel::_setPixel(uint8_t x, uint8_t y, uint32_t c) 
  {
  _setPixel(x + (y * 11), c);
  }


// Einen Pixel im Streifen setzten (die Eck-LEDs sind am Ende).
//------------------------------------------------------------------------------------------------------------
void LedDriverNeoPixel::_setPixel(uint8_t num, uint32_t c) 
  {
  if (num < 110) 
    {
    if ((num / 11) % 2 == 0) 
      {
      _strip->setPixelColor(num, c);
      } 
    else 
      {
      _strip->setPixelColor(((num / 11) * 11) + 10 - (num % 11), c);
      }
    } 
  else 
    {
    switch (num) 
      {
      case 110:
        _strip->setPixelColor(111, c);
        break;
        
      case 111:
        _strip->setPixelColor(112, c);
        break;
        
      case 112:
        _strip->setPixelColor(113, c);
        break;
        
      case 113:
        _strip->setPixelColor(110, c);
        break;
      }
    }
  }
  

// Funktion fuer saubere 'Regenbogen'-Farben.
// Kopiert aus den Adafruit-Beispielen (strand).
//------------------------------------------------------------------------------------------------------------
uint32_t LedDriverNeoPixel::_wheel(uint8_t wheelPos) 
  {
  if (wheelPos < 85) 
    {
    return _strip->Color(wheelPos * 3, 255 - wheelPos * 3, 0);
    } 
  else if (wheelPos < 170) 
    {
    wheelPos -= 85;
    return _strip->Color(255 - wheelPos * 3, 0, wheelPos * 3);
    } 
  else 
    {
    wheelPos -= 170;
    return _strip->Color(0, wheelPos * 3, 255 - wheelPos * 3);
    }
  }


// Hilfsfunktion fuer das Skalieren der Farben.
//------------------------------------------------------------------------------------------------------------
uint8_t LedDriverNeoPixel::_brightnessScaleColor(uint8_t colorPart) 
  {
  return map(_brightnessInPercent, 0, 100, 0, colorPart);
  }
  
