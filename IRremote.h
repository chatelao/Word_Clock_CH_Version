// IRremote

// For details, see http://arcfn.com/2009/08/multi-protocol-infrared-remote-library.htm http://arcfn.com
// Edited by Mitra to add new controller SANYO

#ifndef IRremote_h
#define IRremote_h

#include <stdint.h>

// The following are compile-time library options.
// If you change them, recompile the library.
// If DEBUG is defined, a lot of debugging output will be printed during decoding.
// TEST must be defined for the IRtest unittests to work.  It will make some
// methods virtual, which will be slightly slower, which is why it is optional.
// #define DEBUG
 #define TEST

// Results returned from the decoder
class decode_results 
  {
public:
  int16_t decode_type; // NEC, SONY, RC5, UNKNOWN
  union 
    { // This is used for decoding Panasonic and Sharp data
    uint16_t panasonicAddress;
    uint16_t sharpAddress;
    };
  uint32_t value; // Decoded value
  int16_t bits; // Number of bits in decoded value
  volatile uint16_t *rawbuf; // Raw intervals in .5 us ticks
  int16_t rawlen; // Number of records in rawbuf.
  };

// Values for decode_type
#define NEC 1
#define SONY 2
#define RC5 3
#define RC6 4
#define DISH 5
#define SHARP 6
#define PANASONIC 7
#define JVC 8
#define SANYO 9
#define MITSUBISHI 10
#define SAMSUNG 11
#define LG 12
#define UNKNOWN -1

// Decoded value for NEC when a repeat code is received
#define REPEAT 0xffffffff

// main class for receiving IR
class IRrecv
{
public:
  IRrecv(int16_t recvpin);
  void blink13(int16_t blinkflag);
  int16_t decode(decode_results *results);
  void enableIRIn();
  void resume();
private:
  // These are called by decode
  int16_t getRClevel(decode_results *results, int16_t *offset, int16_t *used, int16_t t1);
  int32_t decodeNEC(decode_results *results);
  int32_t decodeSony(decode_results *results);
  int32_t decodeSanyo(decode_results *results);
  int32_t decodeMitsubishi(decode_results *results);
  int32_t decodeRC5(decode_results *results);
  int32_t decodeRC6(decode_results *results);
  int32_t decodePanasonic(decode_results *results);
  int32_t decodeLG(decode_results *results);
  int32_t decodeJVC(decode_results *results);
  int32_t decodeSAMSUNG(decode_results *results);
  int32_t decodeHash(decode_results *results);
  int16_t compare(uint16_t oldval, uint16_t newval);
  };


// Some useful constants

#define USECPERTICK 50  // microseconds per clock interrupt tick
#define RAWBUF 100 // Length of raw duration buffer

// Marks tend to be 100us too long, and spaces 100us too short
// when received due to sensor lag.
#define MARK_EXCESS 100

#endif
