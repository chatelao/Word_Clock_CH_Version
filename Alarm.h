// Alarm
// Klasse fuer die Weckfunktion.

#ifndef ALARM_H
#define ALARM_H

#include "Arduino.h"
#include "TimeStamp.h"

class Alarm 
  {
public:
    Alarm(uint8_t speakerPin);

    TimeStamp* getAlarmTime();

    void activate();
    void deactivate();
    void buzz(boolean on);

    uint8_t getShowAlarmTimeTimer();
    void setShowAlarmTimeTimer(uint8_t seconds);
    void decShowAlarmTimeTimer();

    boolean isActive();

private:
    TimeStamp *_alarmTime;
    boolean _isActive;
    uint8_t _showAlarmTimeTimer;
    uint8_t _speakerPin;
  };

#endif
