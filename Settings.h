// Settings
// Die vom Benutzer getaetigten Einstellungen werden hier verwaltet
// und im EEPROM persistiert.

#ifndef SETTINGS_H
#define SETTINGS_H

#include "Arduino.h"

class Settings 
  {
public:
  Settings();

  uint8_t getLanguage();
  void setLanguage(uint8_t language);

  boolean getRenderCornersCw();
  void setRenderCornersCw(boolean cw);

  boolean getUseLdr();
  void setUseLdr(boolean useLdr);

  uint8_t getBrightness();
  void setBrightness(uint8_t brightness);

  boolean getEnableAlarm();
  void setEnableAlarm(boolean enableAlarm);

  boolean getDcfSignalIsInverted();
  void setDcfSignalIsInverted(boolean dcfSignalIsInverted);

  char getTimeShift();
  void setTimeShift(char timeShift);

  void loadFromEEPROM();
  void saveToEEPROM();

private:
  boolean _renderCornersCw;
  boolean _use_ldr;
  uint8_t _brightness;
  boolean _enableAlarm;
  boolean _dcfSignalIsInverted;
  char _timeShift;
  };

#endif

