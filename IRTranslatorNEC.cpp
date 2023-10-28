// IRTranslatorNEC
// Remote-Klasse fuer die NEC-Remote (IR-Fernbedienung mit 21 Tasten, NEC-Codierung
// http://www.play-zone.ch/de/ultra-dunne-infrarot-fernbedienung-mit-21-tasten-nec-codierung-2790.html

#include "IRTranslatorNEC.h"

// #define DEBUG
#include "Configuration.h"

// Codes, mit #debug im Hautprogrammn ausgelesen...
#define NEC_POWER  0x00FFA25D
#define NEC_MENU   0x00FF629D
#define NEC_MUTE   0x00FFE21D

#define NEC_MODE   0x00FF22DD
#define NEC_PLUS   0x00FF02FD
#define NEC_BACK   0x00FFC23D

#define NEC_LEFT   0x00FFE01F
#define NEC_SELECT 0x00FFA857
#define NEC_RIGHT  0x00FF906F
#define NEC_MINUS  0x00FF9867

#define NEC_0      0x00FF6897
#define NEC_1      0x00FF30CF
#define NEC_2      0x00FF18E7
#define NEC_3      0x00FF7A85
#define NEC_4      0x00FF10EF
#define NEC_5      0x00FF38C7
#define NEC_6      0x00FF5AA5
#define NEC_7      0x00FF42BD
#define NEC_8      0x00FF4AB5
#define NEC_9      0x00FF52AD

void IRTranslatorNEC::printSignature() 
  {
  Serial.println(F("IR-Fernbedienung mit 21 Tasten, NEC-Codierung\n"));
  }

uint8_t IRTranslatorNEC::buttonForCode(uint32_t code) 
  {
  switch (code) 
    {
    case NEC_POWER:
      return REMOTE_BUTTON_TOGGLEBLANK;
      
    case NEC_MODE:
      return REMOTE_BUTTON_MODE;
    
    case NEC_LEFT:
      return REMOTE_BUTTON_HOUR_PLUS;
      
    case NEC_RIGHT:
      return REMOTE_BUTTON_MINUTE_PLUS;
    
    case NEC_PLUS:                      // Heller
      return REMOTE_BUTTON_BRIGHTER;
    
    case NEC_MINUS:                     // Dunkler
      return REMOTE_BUTTON_DARKER;
    
    case NEC_SELECT:                    // Mode EXT
      return REMOTE_BUTTON_EXTMODE;

    case NEC_0:                         // Farbe Weiss
      setColor(255, 255, 225);
      return REMOTE_BUTTON_SETCOLOR;
    
    case NEC_1:                         // Farbe Rot
      setColor(255, 0, 0);
      return REMOTE_BUTTON_SETCOLOR;
    
    case NEC_2:                         // Farbe Grün
      setColor(0, 255, 0);
      return REMOTE_BUTTON_SETCOLOR;
    
    case NEC_3:                         // Farbe Blau
      setColor(0, 0, 255);
      return REMOTE_BUTTON_SETCOLOR;
    
    default:
      return REMOTE_BUTTON_UNDEFINED;
    }
  }
