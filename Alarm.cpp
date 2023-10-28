// Alarm
// Klasse fuer die Weckfunktion.

#include "Alarm.h"
#include "Configuration.h"



// #define DEBUG

#ifdef DEBUG
    #define DEBUG_PRINTF   Serial.printf
#else
    #define DEBUG_PRINTF(...)
#endif

// Konstruktor.
// @param speakerPin Der Pin, an dem der Lautsprecher oder Buzzer haengt.
//------------------------------------------------------------------------------------------------------------
Alarm::Alarm(uint8_t speakerPin) 
  {
  _speakerPin = speakerPin;
  pinMode(_speakerPin, OUTPUT);
  _alarmTime = new TimeStamp(0, 7, 0, 0, 0, 0);
  _isActive = false;
  _showAlarmTimeTimer = 0;
  }


// Den Weckton einschalten.
// @param: on - TRUE, der Ton wird eingeschaltet.
//              FALSE, der Ton wird ausgeschaltet.
//------------------------------------------------------------------------------------------------------------
void Alarm::buzz(boolean on) 
  {
#ifdef SPEAKER_IS_BUZZER
  if (on) 
    {
    digitalWrite(_speakerPin, HIGH);
    } 
  else 
    {
    digitalWrite(_speakerPin, LOW);
    }
#else
  if (on) 
    {
    tone(_speakerPin, SPEAKER_FREQUENCY);
    }
  else 
    {
    noTone(_speakerPin);
    }
#endif
  }


// Die eingestellte Weckzeit bekommen.
// @return Eine TimeStamp mit der eingstellten Weckzeit.
//------------------------------------------------------------------------------------------------------------
TimeStamp* Alarm::getAlarmTime() 
  {
  return _alarmTime;
  }


// Die verbleibende Zeit in Sekunden bekommen, fuer die die Weckzeit angezeigt werden soll.
// @return Die Zeit in Sekunden.
//------------------------------------------------------------------------------------------------------------
uint8_t Alarm::getShowAlarmTimeTimer() 
  {
  return _showAlarmTimeTimer;
  }


// Die Zeit in Sekunden setzten, fuer die die Weckzeit angezeigt werden soll.
// @param seconds Die Zeit in Sekunden.
//------------------------------------------------------------------------------------------------------------
void Alarm::setShowAlarmTimeTimer(uint8_t seconds) 
  {
  _showAlarmTimeTimer = seconds;
  }


// Die Zeit, fuer die die Weckzeit angezeigt werden soll, um eine Sekunde verringern.
//------------------------------------------------------------------------------------------------------------
void Alarm::decShowAlarmTimeTimer() 
  {
  if (_showAlarmTimeTimer > 0) 
    {
    _showAlarmTimeTimer--;
    }
  }


// Ist der Wecker aktiv?
// @return TRUE, wenn der Wecker aktiv ist.
//         FALSE, wenn der Wekcer ausgeschaltet ist.
//------------------------------------------------------------------------------------------------------------
boolean Alarm::isActive() 
  {
  return _isActive;
  }


// Den Wecker einschalten.
//------------------------------------------------------------------------------------------------------------
void Alarm::activate() 
  {
  _isActive = true;
  }


// Den Wecker ausschalten.
//------------------------------------------------------------------------------------------------------------
void Alarm::deactivate() 
  {
  _isActive = false;
  buzz(false);
  }
  
