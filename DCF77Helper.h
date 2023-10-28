// DCF77Helper

// Klasse um die Qualitaet der DCF77-Samples zu verbessern. Dazu wird der Minutenabstand
// zwischen den empfangenen DCF77-Samples mit Hilfe der Echtzeituhr verglichen.

#ifndef DCF77HELPER_H
#define DCF77HELPER_H

#include "Arduino.h"
#include "RTC.h"
#include "DCF77.h"
#include "TimeStamp.h"
#include "Configuration.h"

class DCF77Helper 
  {
public:
  DCF77Helper();

  void addSample(DCF77 dcf77, RTC rtc);
  boolean samplesOk();

private:
  uint8_t _cursor;
  TimeStamp *_zeitstempelDcf77[DCF77HELPER_MAX_SAMPLES];
  TimeStamp *_zeitstempelRtc[DCF77HELPER_MAX_SAMPLES];
  };

#endif
