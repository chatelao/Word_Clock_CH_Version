// Settings
// Die Einstellungen werden hier verwaltet und im EEPROM gespeichert.

#include "Settings.h"
#include <EEPROM.h>
#include "Renderer.h"

#define SETTINGS_MAGIC_NUMBER 0xAA
#define SETTINGS_VERSION 1


//  Konstruktor.
//------------------------------------------------------------------------------------------------------------
Settings::Settings() 
  {
  _renderCornersCw = true;
  _use_ldr = false;
  _brightness = 75;
  _enableAlarm = false;
  _dcfSignalIsInverted = true;
  _timeShift = 0;
  
  // Versuche alte Einstellungen zu laden...
  loadFromEEPROM();
  }

// Die Laufrichtung der Eck-LEDs.
//------------------------------------------------------------------------------------------------------------
boolean Settings::getRenderCornersCw() 
  {
  return _renderCornersCw;
  }

void Settings::setRenderCornersCw(boolean cw) 
  {
  _renderCornersCw = cw;
  }


// Der LDR-Modus.
//------------------------------------------------------------------------------------------------------------
boolean Settings::getUseLdr() 
  {
  return _use_ldr;
  }

void Settings::setUseLdr(boolean useLdr) 
  {
  _use_ldr = useLdr;
  }


// Die Helligkeit.
//------------------------------------------------------------------------------------------------------------
uint8_t Settings::getBrightness() 
  {
  return _brightness;
  }

void Settings::setBrightness(uint8_t brightness) 
  {
  _brightness = brightness;
  }


// Wecker enablen?
//------------------------------------------------------------------------------------------------------------
boolean Settings::getEnableAlarm() 
  {
  return _enableAlarm;
  }

void Settings::setEnableAlarm(boolean enableAlarm) 
  {
  _enableAlarm = enableAlarm;
  }


// Ist das DCF-Signal invertiert?
//------------------------------------------------------------------------------------------------------------
boolean Settings::getDcfSignalIsInverted() 
  {
  return _dcfSignalIsInverted;
  }

void Settings::setDcfSignalIsInverted(boolean dcfSignalIsInverted) 
  {
  _dcfSignalIsInverted = dcfSignalIsInverted;
  }


// Zeitverschiebung
//------------------------------------------------------------------------------------------------------------
char Settings::getTimeShift() 
  {
  return _timeShift;
  }

void Settings::setTimeShift(char timeShift) 
  {
  _timeShift = timeShift;
  }


// Die Einstellungen laden.
//------------------------------------------------------------------------------------------------------------
void Settings::loadFromEEPROM() 
  {
  if ((EEPROM.read(0) == SETTINGS_MAGIC_NUMBER) && (EEPROM.read(1) == SETTINGS_VERSION)) 
    {
    // es sind gueltige Einstellungen vorhanden...
    _renderCornersCw = EEPROM.read(2);
    _use_ldr = EEPROM.read(3);
    _brightness = EEPROM.read(4);
    _enableAlarm = EEPROM.read(5);
    _dcfSignalIsInverted = EEPROM.read(6);
    _timeShift = EEPROM.read(7);
    }
  }


// Die Einstellungen speichern.
//------------------------------------------------------------------------------------------------------------
void Settings::saveToEEPROM() 
  {
  if (EEPROM.read(0) != SETTINGS_MAGIC_NUMBER) 
    {
    EEPROM.write(0, SETTINGS_MAGIC_NUMBER);
    }
  if (EEPROM.read(1) != SETTINGS_VERSION) 
    {
    EEPROM.write(1, SETTINGS_VERSION);
    }
  if (EEPROM.read(2) != _renderCornersCw) 
    {
    EEPROM.write(2, _renderCornersCw);
    }
  if (EEPROM.read(3) != _use_ldr) 
    {
    EEPROM.write(3, _use_ldr);
    }
  if (EEPROM.read(4) != _brightness) 
    {
    EEPROM.write(4, _brightness);
    }
  if (EEPROM.read(5) != _enableAlarm) 
    {
    EEPROM.write(5, _enableAlarm);
    }
  if (EEPROM.read(6) != _dcfSignalIsInverted) 
    {
    EEPROM.write(6, _dcfSignalIsInverted);
    }
  if (EEPROM.read(7) != _timeShift) 
    {
    EEPROM.write(7, _timeShift);
    }
  }
