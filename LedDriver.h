// LedDriver
// Abstrakte Klasse fuer einen LED-Treiber. Hiervon koennen andere Implementierungen erben

#ifndef LEDDRIVER_H
#define LEDDRIVER_H

#include "Arduino.h"

class LedDriver {
public:
    virtual void init();

    virtual void printSignature();

    virtual void writeScreenBufferToMatrix(uint16_t matrix[16], boolean onChange);

    virtual void setBrightness(uint8_t brightnessInPercent);
    virtual uint8_t getBrightness();

    void setColor(uint8_t red, uint8_t green, uint8_t blue);
    uint8_t getRed();
    uint8_t getGreen();
    uint8_t getBlue();

    virtual void setLinesToWrite(uint8_t linesToWrite);

    virtual void shutDown();
    virtual void wakeUp();

    virtual void clearData();

    void setPixelInScreenBuffer(uint8_t x, uint8_t y, uint16_t matrix[16]);
    boolean getPixelFromScreenBuffer(uint8_t x, uint8_t y, uint16_t matrix[16]);

private:
    uint8_t _red, _green, _blue;
};

#endif
