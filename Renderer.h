// Renderer
// Diese Klasse rendert die Woerter auf die Matrix.

#ifndef RENDERER_H
#define RENDERER_H

#include "Arduino.h"

class Renderer 
  {
public:
  Renderer();

  void setMinutes(char hours, uint8_t minutes, uint16_t matrix[16]);
  void setCorners(uint8_t minutes, boolean cw, uint16_t matrix[16]);

  void cleanWordsForAlarmSettingMode(uint16_t matrix[16]);

  void scrambleScreenBuffer(uint16_t matrix[16]);
  void clearScreenBuffer(uint16_t matrix[16]);
  void setAllScreenBuffer(uint16_t matrix[16]);

private:
  void setHours(uint8_t hours, boolean glatt,uint16_t matrix[16]);

  };

#endif
