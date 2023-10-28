// IRTranslator
// Abstrakte Klasse fuer die Umsetzung von Fernbedienungs-Codes.

#ifndef IRTRANSLATOR_H
#define IRTRANSLATOR_H

#include "Arduino.h"

#define REMOTE_BUTTON_UNDEFINED    0

// diese Buttons braucht man haeufig und sollten alle Fernbedienungen koennen...
#define REMOTE_BUTTON_MODE         1
#define REMOTE_BUTTON_MINUTE_PLUS  2
#define REMOTE_BUTTON_HOUR_PLUS    3
#define REMOTE_BUTTON_BRIGHTER     4
#define REMOTE_BUTTON_DARKER       5
#define REMOTE_BUTTON_EXTMODE      6
#define REMOTE_BUTTON_TOGGLEBLANK  7
#define REMOTE_BUTTON_BLANK        8
#define REMOTE_BUTTON_RESUME       9
#define REMOTE_BUTTON_SETCOLOR     10

// ... weitere koennen dann die konkreten Implementierungen hinzufuegen.
#define REMOTE_BUTTON_START_YOUR_OWN_AT  100

class IRTranslator 
  {
public:
  virtual void printSignature();
  virtual uint8_t buttonForCode(uint32_t code);
  uint8_t getRed();
  uint8_t getGreen();
  uint8_t getBlue();

protected:
  void setColor(uint8_t red, uint8_t green, uint8_t blue);

private:
  uint8_t _red, _green, _blue;
  };

#endif
