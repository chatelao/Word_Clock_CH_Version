// LedDriverNeoPixel
// Implementierung auf der Basis von WS2812B-Streifen wie sie die Adafruit-Neo-Pixel verwenden.

// Es lohnt sich in jedem Fall, den UeberGuide von Adafruit zu lesen:
// https://learn.adafruit.com/adafruit-neopixel-uberguide/overview

// Verkabelung: Einspeisung oben links, dann schlangenfoermig runter,
// dann Ecke unten links, oben links, oben rechts, unten rechts.

#ifndef LED_DRIVER_NEOPIXEL_H
#define LED_DRIVER_NEOPIXEL_H

#include "Arduino.h"
#include "LedDriver.h"
#include <Adafruit_NeoPixel.h>

class LedDriverNeoPixel : public LedDriver {
public:
    LedDriverNeoPixel(uint8_t dataPin);

    void init();

    void printSignature();

    void writeScreenBufferToMatrix(uint16_t matrix[16], boolean onChange);

    void setBrightness(uint8_t brightnessInPercent);
    uint8_t getBrightness();

    void setLinesToWrite(uint8_t linesToWrite);

    void shutDown();
    void wakeUp();

    void clearData();

private:
    uint8_t _brightnessInPercent;

    boolean _dirty;

    void _setPixel(uint8_t x, uint8_t y, uint32_t c);
    void _setPixel(uint8_t num, uint32_t c);

    uint32_t _wheel(uint8_t wheelPos);

    uint8_t _brightnessScaleColor(uint8_t colorPart);

    uint8_t _dataPin;
    Adafruit_NeoPixel *_strip;
};

#endif
