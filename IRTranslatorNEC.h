// IRTranslatorNEC
// Remote-Klasse fuer die NEC-Remote (IR-Fernbedienung mit 21 Tasten, NEC-Codierung
// http://www.play-zone.ch/de/ultra-dunne-infrarot-fernbedienung-mit-21-tasten-nec-codierung-2790.html

#ifndef IRTRANSLATORNEC_H
#define IRTRANSLATORNEC_H

#include "Arduino.h"
#include "IRTranslator.h"

class IRTranslatorNEC : public IRTranslator 
  {
public:
  void printSignature();
  uint8_t buttonForCode(uint32_t code);
  };

#endif
