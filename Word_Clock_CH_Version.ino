// Wörter-Uhr
//------------------------------------------------------------------------------------------------------------

// Diese Software enstand z.T. aus Qlockthree_v3.4.8 von Cristians Bastel-Laden
// und eigenen Anpassungen.
// Es wird nur eine Sprache (Schweizer-Deutsch) verwendet und
// nur die LED-Streifen können angesteuert werden. Alle anderen Varianten der LED-Treiber wurden weggelassen.
// Die IR-Fernbedinungsroutinen wurden an den NEC-Code angepasst.

// Libraries
//------------------------------------------------------------------------------------------------------------
#include <stdint.h>
#include <Wire.h>
#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>
#include "LedDriver.h"
#include "LedDriverNeoPixel.h"
#include "IRTranslator.h"
#include "IRTranslatorNEC.h"
#include "IRremote.h"
#include "RTC.h"
#include "DCF77.h"
#include "Button.h"
#include "LDR.h"
#include "DCF77Helper.h"
#include "Renderer.h"
#include "Staben.h"
#include "Alarm.h"
#include "Settings.h"
#include "Zahlen.h"
#include "Configuration.h"

#define FIRMWARE_VERSION "Ver. 1.0.a vom 15.2.2016"

// Den DEBUG-Schalter gibt es in den meisten Bibiliotheken. Wird er eingeschaltet, werden ueber den
// 'Serial-Monitor' der Arduino-Umgebung Informationen ausgegeben. Die Geschwindigkeit im
// Serial-Monitor ist 115'200 Baud.
// Default: ausgeschaltet
//------------------------------------------------------------------------------------------------------------
#undef DEBUG

#ifdef DEBUG
    #define DEBUG_PRINTF   Serial.println
#else
    #define DEBUG_PRINTF(...)
#endif

// Die Geschwindigkeit der seriellen Schnittstelle. Default: 115200. Die Geschwindigkeit brauchen wir immer,
// da auch ohne DEBUG Meldungen ausgegeben werden!
//------------------------------------------------------------------------------------------------------------
#define SERIAL_SPEED 115200

// Die persistenten (im EEPROM gespeicherten) Einstellungen.
//------------------------------------------------------------------------------------------------------------
Settings settings;

// Hier definiert man die Ab- und Anschaltzeiten fuer das Display. Die Abschaltung des
// Displays verbessert den Empfang des DCF77-Empfaengers. Und hilft, falls die Uhr im
// Schlafzimmer haengt.
// Sind alle Werte auf 0 wird das Display nie abgeschaltet. Nach einer Minute kann man das
// Display manuell wieder ein- / ausschalten.
// Achtung! Wenn sich die Uhr nachmittags abschaltet ist sie in der falschen Tageshaelfte!
//------------------------------------------------------------------------------------------------------------
// um 3 Uhr Display abschalten (Minuten, Stunden, -, -, -, -)
TimeStamp offTime(0, 3, 0, 0, 0, 0);
// um 4:30 Uhr Display wieder anschalten (Minuten, Stunden, -, -, -, -)
TimeStamp onTime(30, 4, 0, 0, 0, 0);

// Der Renderer, der die Woerter auf die Matrix ausgibt.
//------------------------------------------------------------------------------------------------------------
Renderer renderer;

// Der LED-Treiber fuer NeoPixel-Stripes.
// Data: 6
//------------------------------------------------------------------------------------------------------------
  LedDriverNeoPixel ledDriver(6);
  
  #define PIN_MODE          11                // MODE-Taster   an Pin 11
  #define PIN_H_PLUS        12                // H-PLUS-Taster an Pin 12
  #define PIN_M_PLUS        13                // M-PLUS-Taster an Pin 13
  
  #define BUTTONS_PRESSING_AGAINST LOW        // Die Taster schalten gegen GND
  
  #define PIN_IR_RECEIVER   A1                // Daten von IR-Empfäger TSOP4836 an Pin A1                     
  
  #define PIN_LDR           A0                // Der LDR an Pin A0 (Pull-Up Widerstand 10kOhm gegen 3.3V)      
  #define IS_INVERTED       true              // und gegen GND
  
  #define PIN_SQW_SIGNAL    2                 // 1s Rechtecksignal von RTC-modul an Pin 2
  #define PIN_DCF77_SIGNAL  3                 // die Daten vom DCF77-Empfänger an Pin 3
  #define PIN_DCF77_PON     4                 // PON (Power On) vom DCF77-Empfänger an Pin 4
  
  #define PIN_SQW_LED       9                 // 1s Takt-LED an Pin 9
  #define PIN_DCF77_LED    10                 // DCF-Impuls-LED an Pin 10
  
  #define PIN_SPEAKER      -1                 // Piezzo-Lautsprecher für den Alarm an Pin ?? (muss noch definiert werden)


// Der IR-Remote-Control-Receiver.
//------------------------------------------------------------------------------------------------------------
#ifdef REMOTE_NEC
  IRrecv irrecv(PIN_IR_RECEIVER);
  decode_results irDecodeResults;
  IRTranslatorNEC irTranslator;
#endif


// Die Real-Time-Clock mit der Status-LED fuer das SQW-Signal.
//------------------------------------------------------------------------------------------------------------
RTC       rtc(0x68, PIN_SQW_LED);
volatile uint8_t helperSeconds;


// Der Funkempfaenger (DCF77-Signal der PTB Braunschweig).
//------------------------------------------------------------------------------------------------------------
DCF77     dcf77(PIN_DCF77_SIGNAL, PIN_DCF77_LED);
DCF77Helper dcf77Helper;


// Variablen fuer den Alarm.
//------------------------------------------------------------------------------------------------------------
Alarm alarm(PIN_SPEAKER);


// Der Helligkeitssensor
//------------------------------------------------------------------------------------------------------------
LDR       ldr(PIN_LDR, IS_INVERTED);
uint32_t  lastBrightnessCheck;

// Die Helligkeit zum Anzeigen mit den Balken.
uint8_t brightnessToDisplay;


// Die Tasten.
//------------------------------------------------------------------------------------------------------------
Button  minutesPlusButton(PIN_M_PLUS, BUTTONS_PRESSING_AGAINST);
Button  hoursPlusButton(PIN_H_PLUS, BUTTONS_PRESSING_AGAINST);
Button  extModeDoubleButton(PIN_M_PLUS, PIN_H_PLUS, BUTTONS_PRESSING_AGAINST);
Button  modeChangeButton(PIN_MODE, BUTTONS_PRESSING_AGAINST);


// Die Standard-Modi.
//------------------------------------------------------------------------------------------------------------
#define STD_MODE_NORMAL           0
#define STD_MODE_ALARM            1
#define STD_MODE_SECONDS          2
#define STD_MODE_BRIGHTNESS       3
#define STD_MODE_BLANK            4
#define STD_MODE_COUNT            4

// nicht manuell zu erreichender Modus...
//------------------------------------------------------------------------------------------------------------
#define STD_MODE_NIGHT            7

// Die erweiterten Modi.
//------------------------------------------------------------------------------------------------------------
#define EXT_MODE_START           10
#define EXT_MODE_LDR_MODE        10
#define EXT_MODE_CORNERS         11
#define EXT_MODE_ENABLE_ALARM    12
#define EXT_MODE_DCF_IS_INVERTED 13
#define EXT_MODE_TIMESET         14
#define EXT_MODE_TIME_SHIFT      15
#define EXT_MODE_TEST            16
#define EXT_MODE_DCF_DEBUG       17
#define EXT_MODE_COUNT           17

uint8_t mode = STD_MODE_NORMAL;   // Startmode...
uint8_t lastMode = mode;          // Merker fuer den Modus vor der Abschaltung...

// Ueber die Wire-Library festgelegt:
//------------------------------------------------------------------------------------------------------------
// Arduino analog input 4 = I2C SDA
// Arduino analog input 5 = I2C SCL

uint16_t matrix[16];              // Die Matrix, eine Art Bildschirmspeicher
volatile boolean needsUpdateFromRtc = true; // Hilfsvariable, da I2C und Interrupts nicht zusammenspielen
uint8_t x, y;                     // Fuer den Bildschirm-Test

// Fuer fps-Anzeige
uint16_t frames = 0;
uint32_t lastFpsCheck = 0;


// Aenderung der Anzeige als Funktion fuer den Interrupt, der ueber das SQW-Signal
// der Real-Time-Clock gesetzt wird. Da die Wire-Bibliothek benutzt wird, kann man
// den Interrupt nicht direkt benutzen, sondern muss eine Hilfsvariable setzen, die
// dann in loop() ausgewertet wird.
//------------------------------------------------------------------------------------------------------------
void updateFromRtc() 
  {
  needsUpdateFromRtc = true;
  // die Zeit verursacht ein kurzes Flackern. Wir muessen
  // sie aber nicht immer lesen, im Modus 'normal' alle 60 Sekunden,
  // im Modus 'seconds' alle Sekunde, sonst garnicht.
  helperSeconds++;
  if (helperSeconds > 59) helperSeconds = 0;
  }


// Den noch freien Specher abschaetzen.
// siehe http://playground.arduino.cc/Code/AvailableMemory
//------------------------------------------------------------------------------------------------------------
int16_t freeRam() 
  {
  extern int16_t __heap_start, *__brkval;
  int16_t v;
  return (int16_t) &v - (__brkval == 0 ? (int16_t) &__heap_start : (int16_t) __brkval);
  }


// Initialisierung
// setup() wird einmal zu Beginn aufgerufen, wenn der Arduino Strom bekommt.
//------------------------------------------------------------------------------------------------------------
void setup() 
  {
  Serial.begin(SERIAL_SPEED);
  Serial.println(F("Word-Clock wird initialisiert...\n"));
  DEBUG_PRINTF(F("und startet im Debug-Modus...\n"));
  
  pinMode(PIN_DCF77_PON, OUTPUT);
  enableDcf(false);                     // DCF-Empfänger ausschalten
  
  if (settings.getEnableAlarm()) 
    {
    // als Wecker Display nicht abschalten...
    TimeStamp offTime(0, 0, 0, 0, 0, 0);
    TimeStamp onTime(0, 0, 0, 0, 0, 0);
    }
  
  ledDriver.init();                     // LED-Treiber initialisieren
  ledDriver.clearData();                // Inhalt des Led-Treibers loeschen...
  renderer.clearScreenBuffer(matrix);   // und Inhalt des Bildspeichers loeschen
  ledDriver.setLinesToWrite(10);        // wir brauchen nur 10 Zeilen...

  Wire.begin();                         // starte Wire-Library als I2C-Bus Master

  pinMode(PIN_SQW_SIGNAL, INPUT);       // RTC-Interrupt-Pin konfigurieren
  digitalWrite(PIN_SQW_SIGNAL, HIGH);

  // DCF77-LED drei Mal blinken und Speaker piepsen lassen, falls ENABLE_ALARM eingeschaltet ist.
  for (uint8_t i = 0; i < 3; i++) 
    {
    dcf77.statusLed(true);
    if (settings.getEnableAlarm()) 
      {
      alarm.buzz(true);
      }
    delay(100);
    dcf77.statusLed(false);
    if (settings.getEnableAlarm()) 
      {
      alarm.buzz(false);
      }
    delay(100);
    }

   Serial.println(F("Kompiliert: __TIME__ / __DATE__\n"));

// Uhrzeit in RTC nach Compile-Zeit stellen...
//    rtc.set(__DATE__, __TIME__);
//    rtc.writeTime();

  // RTC starten...
  rtc.readTime();
  if ((rtc.getSeconds() >= 60) || (rtc.getMinutes() >= 60) || (rtc.getHours() >= 24) || (rtc.getYear() < 15)) 
    {
    // Echtzeituhr enthaelt Schrott, neu mit erkennbaren Zahlen beschreiben...
    DEBUG_PRINTF(F("Resetting RTC..."));
    rtc.setHours(11);
    rtc.setMinutes(11);
    rtc.setSeconds(11);
    rtc.setYear(15);
    rtc.setMonth(1);
    rtc.setDate(1);
    rtc.setDayOfWeek(1);
    }
  rtc.enableSQWOnDS3231();

  rtc.writeTime();
  helperSeconds = rtc.getSeconds();
  // Serial.println(F("RTC-Zeit / Datum: %02u:%02u:%02u"),rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());
  // Serial.println(F("  %02u.%02u.%02u\n"), rtc.getDate(), rtc.getMonth(), rtc.getYear());  
  
  // Den Interrupt konfigurieren,
  // nicht mehr CHANGE, das sind 2 pro Sekunde,
  // RISING ist einer pro Sekunde, das reicht.
  // Auf FALLING geandert, das signalisiert
  // den Sekundenwechsel, Danke an Peter.
  attachInterrupt(0, updateFromRtc, FALLING);
  
  // Werte vom LDR einlesen und vermuellen, da die ersten nichts taugen...
  for (int16_t i = 0; i < 1000; i++) 
    {
    analogRead(PIN_LDR);
    }
  
  // rtcSQWLed-LED drei Mal als 'Hello' blinken lassen
  // und Speaker piepsen kassen, falls ENABLE_ALARM eingeschaltet ist.
  for (uint8_t i = 0; i < 3; i++) 
    {
    rtc.statusLed(true);
    if (settings.getEnableAlarm()) 
      {
      alarm.buzz(true);
      }
    delay(100);
    rtc.statusLed(false);
    if (settings.getEnableAlarm()) 
      {
      alarm.buzz(false);
      }
    delay(100);
    }
  
  // ein paar Infos ausgeben
  // Serial.println(F("Version: %s\n"), FIRMWARE_VERSION);
  Serial.println(F("LED-Treiber: ")); ledDriver.printSignature();

#ifndef REMOTE_NO_REMOTE
  Serial.println(F("Fernbedienung: ")); irTranslator.printSignature();
  irrecv.enableIRIn();
#else
  Serial.println(F("Fernbedienung: deaktiviert\n"));
#endif

  if (settings.getEnableAlarm()) 
    {
    Serial.println(F("Alarm: eingeschaltet\n"));
    }

  if (settings.getDcfSignalIsInverted()) 
    {
    Serial.println(F("DCF77-Signal: invertiert\n"));
    }
//  Serial.println(F("Freier RAM-Speicher: %u Bytes\n"), freeRam());

  enableDcf(true);                                    // DCF-Empfänger einschalten
  ledDriver.wakeUp();                                 // Display einschalten
  ledDriver.setBrightness(settings.getBrightness());  // und Helligkeit einstellen
  }


// Hauptprogramm: der loop() wird endlos durchlaufen
//------------------------------------------------------------------------------------------------------------
void loop() 
  {
  // FPS
#ifdef DEBUG_
  frames++;
  if (lastFpsCheck > millis()) 
    {
    lastFpsCheck = millis();
    }
  if (lastFpsCheck + 1000 < millis()) 
    {
    DEBUG_PRINTF(F("FPS: %u\n"), frames);
    lastFpsCheck = millis();
    frames = 0;
    }
#endif
  
  // Dimmen der Anzeige
  //----------------------------------
  if (settings.getUseLdr()) 
    {
    if (millis() < lastBrightnessCheck) 
      {
      lastBrightnessCheck = millis();      // wir hatten einen Ueberlauf...
      }
    if (lastBrightnessCheck + LDR_CHECK_RATE < millis()) // langsam nachsehen...
      { 
      uint8_t lv = ldr.value();
      
      if (ledDriver.getBrightness() > lv) 
        {
        ledDriver.setBrightness(ledDriver.getBrightness() - 1);
        } 
      else if (ledDriver.getBrightness() < lv) 
        {
        ledDriver.setBrightness(ledDriver.getBrightness() + 1);
        }
      lastBrightnessCheck = millis();
      }
    }
  
  // needsUpdateFromRtc wird via Interrupt gesetzt über die fallende Flanke des SQW-Signals der RTC.
  // Oder falls eine Tasten-Aktion eine sofortige Aktualisierung des Displays braucht.
  //----------------------------------
  if (needsUpdateFromRtc) 
    {
    needsUpdateFromRtc = false;
    if (dcf77.newSecond()) 
      {
      manageNewDCF77Data();
      }
    
    // Zeit einlesen...
    switch (mode) 
      {
      case STD_MODE_NORMAL:
      case EXT_MODE_TIMESET:
      case STD_MODE_ALARM:
        if (alarm.isActive()) 
          {
          rtc.readTime();
          }
        if (helperSeconds == 0) 
          {
          rtc.readTime();
          helperSeconds = rtc.getSeconds();
          }
        break;
        
      case STD_MODE_SECONDS:
      case STD_MODE_BLANK:
      case STD_MODE_NIGHT:
        rtc.readTime();
        helperSeconds = rtc.getSeconds();
        break;
      // andere Modi egal...
      }
      
    // Bildschirmpuffer beschreiben...
    //----------------------------------
    switch (mode) 
      {
      case STD_MODE_NORMAL:
      case EXT_MODE_TIMESET:
        renderer.clearScreenBuffer(matrix);
        renderer.setMinutes(rtc.getHours() + settings.getTimeShift(), rtc.getMinutes(), matrix);
        renderer.setCorners(rtc.getMinutes(), settings.getRenderCornersCw(), matrix);
        break;
      
      case EXT_MODE_TIME_SHIFT:
        renderer.clearScreenBuffer(matrix);
        if (settings.getTimeShift() < 0) 
          {
          for (uint8_t x = 0; x < 3; x++) 
            {
            ledDriver.setPixelInScreenBuffer(x, 1, matrix);
            }
          } 
        else if (settings.getTimeShift() > 0) 
          {
          for (uint8_t x = 0; x < 3; x++) 
            {
            ledDriver.setPixelInScreenBuffer(x, 1, matrix);
            }
          for (uint8_t y = 0; y < 3; y++) 
            {
            ledDriver.setPixelInScreenBuffer(1, y, matrix);
            }
          }
        for (uint8_t i = 0; i < 7; i++) 
          {
          matrix[3 + i] |= pgm_read_byte_near(&(ziffern[abs(settings.getTimeShift()) % 10][i])) << 5;
          if (abs(settings.getTimeShift()) > 9) 
            {
            matrix[3 + i] |= pgm_read_byte_near(&(ziffern[1][i])) << 10;
            }
          }
        break;
      
      case STD_MODE_ALARM:
        renderer.clearScreenBuffer(matrix);
        if (alarm.getShowAlarmTimeTimer() == 0) 
          {
          renderer.setMinutes(rtc.getHours() + settings.getTimeShift(), rtc.getMinutes(), matrix);
          renderer.setCorners(rtc.getMinutes(), settings.getRenderCornersCw(), matrix);
          matrix[4] |= 0b0000000000011111; // Alarm-LED
          } 
        else 
          {
          renderer.setMinutes(alarm.getAlarmTime()->getHours() + settings.getTimeShift(), alarm.getAlarmTime()->getMinutes(), matrix);
          renderer.setCorners(alarm.getAlarmTime()->getMinutes(), settings.getRenderCornersCw(), matrix);
          renderer.cleanWordsForAlarmSettingMode(matrix); // ES IST weg
          if (alarm.getShowAlarmTimeTimer() % 2 == 0) 
            {
            matrix[4] |= 0b0000000000011111; // Alarm-LED
            }
          alarm.decShowAlarmTimeTimer();
          }
        break;
      
      case STD_MODE_SECONDS:
        renderer.clearScreenBuffer(matrix);
        for (uint8_t i = 0; i < 7; i++) 
          {
          matrix[1 + i] |= pgm_read_byte_near(&(ziffern[rtc.getSeconds() / 10][i])) << 11;
          matrix[1 + i] |= pgm_read_byte_near(&(ziffern[rtc.getSeconds() % 10][i])) << 5;
          }
        break;
      
      case EXT_MODE_LDR_MODE:
        renderer.clearScreenBuffer(matrix);
        if (settings.getUseLdr()) 
          {
          for (uint8_t i = 0; i < 5; i++) 
            {
            matrix[2 + i] |= pgm_read_byte_near(&(staben['A' - 'A'][i])) << 8;
            }
          } 
        else 
          {
          for (uint8_t i = 0; i < 5; i++) 
            {
            matrix[2 + i] |= pgm_read_byte_near(&(staben['M' - 'A'][i])) << 8;
            }
          }
        break;
      
      case STD_MODE_BLANK:
      case STD_MODE_NIGHT:
        renderer.clearScreenBuffer(matrix);
        break;
        
        case STD_MODE_BRIGHTNESS:
        renderer.clearScreenBuffer(matrix);
        brightnessToDisplay = map(settings.getBrightness(), 1, 100, 0, 9);
        for (uint8_t xb = 0; xb < brightnessToDisplay; xb++) 
          {
          for (uint8_t yb = 0; yb <= xb; yb++) 
            {
            matrix[9 - yb] |= 1 << (14 - xb);
            }
          }
        break;
      
      case EXT_MODE_CORNERS:
        renderer.clearScreenBuffer(matrix);
        if (settings.getRenderCornersCw()) 
          {
          for (uint8_t i = 0; i < 5; i++) 
            {
            matrix[2 + i] |= pgm_read_byte_near(&(staben['C' - 'A'][i])) << 11;
            matrix[2 + i] |= pgm_read_byte_near(&(staben['W' - 'A'][i])) << 5;
            }
          } 
        else 
          {
          for (uint8_t i = 0; i < 5; i++) 
            {
            matrix[0 + i] |= pgm_read_byte_near(&(staben['C' - 'A'][i])) << 8;
            matrix[5 + i] |= pgm_read_byte_near(&(staben['C' - 'A'][i])) << 11;
            matrix[5 + i] |= pgm_read_byte_near(&(staben['W' - 'A'][i])) << 5;
            }
          }
        break;
      
      case EXT_MODE_ENABLE_ALARM:
        renderer.clearScreenBuffer(matrix);
        if (settings.getEnableAlarm()) 
          {
          for (uint8_t i = 0; i < 5; i++) 
            {
            matrix[0 + i] |= pgm_read_byte_near(&(staben['A' - 'A'][i])) << 11;
            matrix[0 + i] |= pgm_read_byte_near(&(staben['L' - 'A'][i])) << 5;
            matrix[5 + i] |= pgm_read_byte_near(&(staben['E' - 'A'][i])) << 11;
            matrix[5 + i] |= pgm_read_byte_near(&(staben['N' - 'A'][i])) << 5;
            }
          } 
        else 
          {
          for (uint8_t i = 0; i < 5; i++) 
            {
            matrix[0 + i] |= pgm_read_byte_near(&(staben['A' - 'A'][i])) << 11;
            matrix[0 + i] |= pgm_read_byte_near(&(staben['L' - 'A'][i])) << 5;
            matrix[5 + i] |= pgm_read_byte_near(&(staben['D' - 'A'][i])) << 11;
            matrix[5 + i] |= pgm_read_byte_near(&(staben['A' - 'A'][i])) << 5;
            }
          }
        break;
      
      case EXT_MODE_DCF_IS_INVERTED:
        renderer.clearScreenBuffer(matrix);
        if (settings.getDcfSignalIsInverted()) 
          {
          for (uint8_t i = 0; i < 5; i++) 
            {
            matrix[0 + i] |= pgm_read_byte_near(&(staben['R' - 'A'][i])) << 11;
            matrix[0 + i] |= pgm_read_byte_near(&(staben['S' - 'A'][i])) << 5;
            matrix[5 + i] |= pgm_read_byte_near(&(staben['I' - 'A'][i])) << 11;
            matrix[5 + i] |= pgm_read_byte_near(&(staben['N' - 'A'][i])) << 5;
            }
          } 
        else 
          {
          for (uint8_t i = 0; i < 5; i++) 
            {
            matrix[0 + i] |= pgm_read_byte_near(&(staben['R' - 'A'][i])) << 11;
            matrix[0 + i] |= pgm_read_byte_near(&(staben['S' - 'A'][i])) << 5;
            matrix[5 + i] |= pgm_read_byte_near(&(staben['N' - 'A'][i])) << 11;
            matrix[5 + i] |= pgm_read_byte_near(&(staben['O' - 'A'][i])) << 5;
            }
          }
        break;

        break;
      
      case EXT_MODE_TEST:
        renderer.clearScreenBuffer(matrix);
        renderer.setCorners(helperSeconds % 5, settings.getRenderCornersCw(), matrix);
        if (settings.getEnableAlarm()) 
          {
          matrix[4] |= 0b0000000000011111; // Alarm-LED
          }
        for (int16_t i = 0; i < 11; i++) 
          {
          ledDriver.setPixelInScreenBuffer(x, i, matrix);
          }
        x++;
        if (x > 10) 
          {
          x = 0;
          }
        break;
      
      case EXT_MODE_DCF_DEBUG:
        renderer.clearScreenBuffer(matrix);
        renderer.setCorners(dcf77.getBitPointer() % 5, settings.getRenderCornersCw(), matrix);
        break;
      }
      
    // Update mit onChange = true, weil sich hier (aufgrund needsUpdateFromRtc) immer was geaendert hat.
    // Entweder weil wir eine Sekunde weiter sind, oder weil eine Taste gedrueckt wurde.
    //----------------------------------
    ledDriver.writeScreenBufferToMatrix(matrix, true);
    }
  
//Tasten abfragen

  // M+ und H+ im STD_MODE_BLANK gedrueckt?
  //----------------------------------
  if ((mode == STD_MODE_BLANK) && extModeDoubleButton.pressed()) 
    {
    doubleExtModePressed();
    }
  
  // Taste Minuten++ (Brighness++) gedrueckt?
  //----------------------------------
  if (minutesPlusButton.pressed()) 
    {
    minutePlusPressed();
    }
  
  // Taste Stunden++ (Brightness--) gedrueckt?
  //----------------------------------
  if (hoursPlusButton.pressed()) 
    {
    hourPlusPressed();
    }
  
  // Taste Moduswechsel gedrueckt?
  //----------------------------------
  if (modeChangeButton.pressed()) 
    {
    modePressed();
    }
    
// Tasten der Fernbedienung abfragen...
//----------------------------------
#ifndef REMOTE_NO_REMOTE
  if (irrecv.decode(&irDecodeResults)) 
    {
    DEBUG_PRINTF(F("IR erfolgreich dekodiert als %#x\n"), irDecodeResults.value);
    needsUpdateFromRtc = true;
    
    switch (irTranslator.buttonForCode(irDecodeResults.value)) 
      {
      case REMOTE_BUTTON_MODE:
        modePressed();
        break;
      
      case REMOTE_BUTTON_MINUTE_PLUS:
        minutePlusPressed();
        break;
      
      case REMOTE_BUTTON_HOUR_PLUS:
        hourPlusPressed();
        break;
      
      case REMOTE_BUTTON_BRIGHTER:
        setDisplayBrighter();
        break;
      
      case REMOTE_BUTTON_DARKER:
        setDisplayDarker();
        break;
      
      case REMOTE_BUTTON_EXTMODE:
        doubleExtModePressed();
        break;
      
      case REMOTE_BUTTON_TOGGLEBLANK:
        setDisplayToToggle();
        break;
        
      case REMOTE_BUTTON_BLANK:
        setDisplayToBlank();
        break;
      
      case REMOTE_BUTTON_RESUME:
        setDisplayToResume();
        break;
        
      case REMOTE_BUTTON_SETCOLOR:
        ledDriver.setColor(irTranslator.getRed(), irTranslator.getGreen(), irTranslator.getBlue());
        break;
      }
    irrecv.resume();
    }
#endif
  
  // Display zeitgesteuert abschalten?
  // Das Verbessert den DCF77-Empfang bzw. ermoeglicht ein dunkles Schlafzimmer.
  //----------------------------------
  if ((offTime.getMinutesOfDay() != 0) && (onTime.getMinutesOfDay() != 0)) 
    {
    if ((mode != STD_MODE_NIGHT) && (offTime.getMinutesOfDay() == rtc.getMinutesOfDay())) 
      {
      mode = STD_MODE_NIGHT;
      ledDriver.shutDown();
      }
    if ((mode == STD_MODE_NIGHT) && (onTime.getMinutesOfDay() == rtc.getMinutesOfDay())) 
      {
      mode = lastMode;
      ledDriver.wakeUp();
      }
    }
  
  // Alarm?
  //----------------------------------
  if ((mode == STD_MODE_ALARM) && (alarm.getShowAlarmTimeTimer() == 0) && !alarm.isActive()) 
    {
    if (alarm.getAlarmTime()->getMinutesOf12HoursDay(0) == rtc.getMinutesOf12HoursDay()) 
      {
      alarm.activate();
      }
    }
    
  if (alarm.isActive()) 
    {
    // Nach 10 Minuten automatisch abschalten, falls der Wecker alleine rumsteht und die Nachbarn nervt...
    if (alarm.getAlarmTime()->getMinutesOf12HoursDay(MAX_BUZZ_TIME_IN_MINUTES) == rtc.getMinutesOf12HoursDay()) 
      {
      alarm.deactivate();
      alarm.buzz(false);
      mode = STD_MODE_NORMAL;
      }
      
    // Krach machen...
    if (rtc.getSeconds() % 2 == 0) 
      {
      alarm.buzz(true);
      } 
    else 
      {
      alarm.buzz(false);
      }
    }
  
  // Die Matrix auf die LEDs multiplexen, hier 'Refresh-Zyklen'.
  //----------------------------------
  if ((mode != STD_MODE_BLANK) && (mode != STD_MODE_NIGHT)) 
    {
    ledDriver.writeScreenBufferToMatrix(matrix, false);
    }
  
  // Status-LEDs ausgeben
  //----------------------------------
#ifdef ENABLE_DCF_LED
  dcf77.statusLed(dcf77.signal(settings.getDcfSignalIsInverted()));
#endif
  
#ifdef ENABLE_SQW_LED
  rtc.statusLed(digitalRead(PIN_SQW_SIGNAL) == HIGH);
#endif
  
  // DCF77-Empfaenger anticken...
  //----------------------------------
  dcf77.poll(settings.getDcfSignalIsInverted());
  }  // Ende von loop()



// Was soll ausgefuehrt werden, wenn die H+ und M+ -Taste zusammen gedrueckt wird?
//------------------------------------------------------------------------------------------------------------
void doubleExtModePressed() 
  {
  needsUpdateFromRtc = true;
  DEBUG_PRINTF(F("Minutes plus AND hours plus pressed in STD_MODE_BLANK...\n"));
  while (minutesPlusButton.pressed());
  while (hoursPlusButton.pressed());
  mode = EXT_MODE_START;
  ledDriver.wakeUp();
  DEBUG_PRINTF(F("Entering EXT_MODEs, mode is now %u..."), mode);
  }


// Was soll ausgefuehrt werden, wenn die Mode-Taste gedrueckt wird?
//------------------------------------------------------------------------------------------------------------
void modePressed() 
  {
    needsUpdateFromRtc = true;
    if (alarm.isActive()) 
    {
    alarm.deactivate();
    mode = STD_MODE_NORMAL;
    } 
  else 
    {
    mode++;
    }
    
  // Brightness ueberspringen, wenn LDR verwendet wird.
  //----------------------------------
  if (settings.getUseLdr() && (mode == STD_MODE_BRIGHTNESS)) 
    {
    mode++;
    }
    
  // Alarm ueberspringen, wenn kein Alarm eingestellt ist.
  //----------------------------------
  if (!settings.getEnableAlarm() && (mode == STD_MODE_ALARM)) 
    {
    mode++;
    }
    
  if (mode == STD_MODE_COUNT + 1) 
    {
    mode = STD_MODE_NORMAL;
    }
    
  if (mode == EXT_MODE_COUNT + 1) 
    {
    mode = STD_MODE_NORMAL;
    }
  
  if (mode == STD_MODE_ALARM) 
    {
    // wenn auf Alarm gewechselt wurde, fuer 10 Sekunden die
    // Weckzeit anzeigen.
    //----------------------------------
    alarm.setShowAlarmTimeTimer(10);
    }
  
  DEBUG_PRINTF(F("Change mode pressed, mode is now %u...\n"), mode);
  
  // Displaytreiber ausschalten, wenn BLANK
  //----------------------------------
  if (mode == STD_MODE_BLANK) 
    {
    DEBUG_PRINTF(F("LED-Treiber: ausgeschaltet\n"));
    ledDriver.shutDown();
    }
    
  // und einschalten, wenn BLANK verlassen wurde
  //----------------------------------
  if (lastMode == STD_MODE_BLANK) 
    {
    DEBUG_PRINTF(F("LED-Treiber: eingeschaltet\n"));
    ledDriver.wakeUp();
    }
  
  // Merker, damit wir nach einer automatischen Abschaltung
  // zum richtigen Mode zurueckkommen.
  //----------------------------------
  lastMode = mode;
  
  // Werte speichern (die Funktion speichert nur bei geaenderten Werten)...
  //----------------------------------
  settings.saveToEEPROM();
  }


// Was soll ausgefuehrt werden, wenn die H+-Taste gedrueckt wird?
//------------------------------------------------------------------------------------------------------------
void hourPlusPressed() 
  {
  needsUpdateFromRtc = true;
  
  DEBUG_PRINTF(F("Hours plus pressed...\n"));
  
  switch (mode) 
    {
    case EXT_MODE_TIMESET:
      rtc.incHours();
      rtc.setSeconds(0);
      rtc.writeTime();
      rtc.readTime();
      helperSeconds = 0;
      DEBUG_PRINTF(F("H ist %02u"), rtc.getHours());
      break;
      
    case EXT_MODE_TIME_SHIFT:
      if (settings.getTimeShift() > -13) 
        {
        settings.setTimeShift(settings.getTimeShift() - 1);
        }
      break;
      
    case STD_MODE_ALARM:
      alarm.getAlarmTime()->incHours();
      alarm.setShowAlarmTimeTimer(10);
      DEBUG_PRINTF(F("A ist %s\n"), alarm.getAlarmTime()->asString());
      break;
    
    case STD_MODE_BRIGHTNESS:
      setDisplayDarker();
      break;
    
    case EXT_MODE_LDR_MODE:
      settings.setUseLdr(!settings.getUseLdr());
      if (!settings.getUseLdr()) 
        {
        ledDriver.setBrightness(50);
        }
      DEBUG_PRINTF(F("LDR ist %u\n"), settings.getUseLdr());
      break;
    
    case EXT_MODE_CORNERS:
      settings.setRenderCornersCw(!settings.getRenderCornersCw());
      break;
    
    case EXT_MODE_ENABLE_ALARM:
      settings.setEnableAlarm(!settings.getEnableAlarm());
      break;
    
    case EXT_MODE_DCF_IS_INVERTED:
      settings.setDcfSignalIsInverted(!settings.getDcfSignalIsInverted());
      break;
    }
  }


// Was soll ausgefuehrt werden, wenn die M+-Taste gedrueckt wird?
//------------------------------------------------------------------------------------------------------------
void minutePlusPressed() 
  {
  needsUpdateFromRtc = true;
  
  DEBUG_PRINTF(F("Minutes plus pressed...\n"));
  
  switch (mode) 
    {
    case EXT_MODE_TIMESET:
      rtc.incMinutes();
      rtc.setSeconds(0);
      rtc.writeTime();
      rtc.readTime();
      helperSeconds = 0;
      DEBUG_PRINTF(F("M ist nun %02u\n"), rtc.getMinutes());
      break;
    
    case EXT_MODE_TIME_SHIFT:
      if (settings.getTimeShift() < 13) 
        {
        settings.setTimeShift(settings.getTimeShift() + 1);
        }
      break;
    
    case STD_MODE_ALARM:
      alarm.getAlarmTime()->incMinutes();
      alarm.setShowAlarmTimeTimer(10);
      DEBUG_PRINTF(F("A ist %s\n"), alarm.getAlarmTime()->asString());
      break;
    
    case STD_MODE_BRIGHTNESS:
      setDisplayBrighter();
      break;
      
    case EXT_MODE_LDR_MODE:
      settings.setUseLdr(!settings.getUseLdr());
      if (!settings.getUseLdr()) 
        {
        ledDriver.setBrightness(50);
        }
      DEBUG_PRINTF(F("LDR ist %u\n"), settings.getUseLdr());
      break;
    
    case EXT_MODE_CORNERS:
      settings.setRenderCornersCw(!settings.getRenderCornersCw());
      break;
    
    case EXT_MODE_ENABLE_ALARM:
      settings.setEnableAlarm(!settings.getEnableAlarm());
      break;
    
    case EXT_MODE_DCF_IS_INVERTED:
      settings.setDcfSignalIsInverted(!settings.getDcfSignalIsInverted());
      break;
    }
  }


// Den DCF77-Empfaenger ein-/ausschalten.
//------------------------------------------------------------------------------------------------------------
void enableDcf(boolean enable) 
  {
  if (enable) 
    {
    DEBUG_PRINTF(F("DCF77-Empfaenger eingeschaltet\n"));
    digitalWrite(PIN_DCF77_PON, LOW);
    } 
  else 
    {
    DEBUG_PRINTF(F("DCF77-Empfaenger ausgeschaltet\n"));
    digitalWrite(PIN_DCF77_PON, HIGH);
    }
  }

// Korrekte Daten (auf Basis der Pruefbits) vom DCF-Empfaenger
// bekommen. Sicherheitshalber gegen Zeitabstaende der RTC pruefen.
//------------------------------------------------------------------------------------------------------------
void manageNewDCF77Data() 
  {
  DEBUG_PRINTF(F("Empfangene DCF-Zeit: %s\n"), dcf77.asString());
  
  rtc.readTime();
  dcf77Helper.addSample(dcf77, rtc);
  
  // Stimmen die Abstaende im Array? Pruefung ohne Datum, nur Zeit!
  //----------------------------------
  if (dcf77Helper.samplesOk()) 
    {
    rtc.setSeconds(0);
    rtc.setMinutes(dcf77.getMinutes());
    rtc.setHours(dcf77.getHours());
    
    // Wir setzen auch das Datum, dann kann man, wenn man moechte,
    // auf das Datum eingehen (spezielle Nachrichten an speziellen
    // Tagen). Allerdings ist das Datum bisher nicht ueber
    // den Abstand der TimeStamps geprueft, sondern nur ueber das
    // Checkbit des DCF77-Telegramms, was unzureichend ist!
    //----------------------------------
    rtc.setDate(dcf77.getDate());
    rtc.setDayOfWeek(dcf77.getDayOfWeek());
    rtc.setMonth(dcf77.getMonth());
    rtc.setYear(dcf77.getYear());
    
    rtc.writeTime();
    DEBUG_PRINTF(F("DCF77-Zeit in RTC geschrieben\n"));
    // falls im manuellen Dunkel-Modus, Display wieder einschalten... (Hilft bei der Erkennung, ob der DCF-Empfang geklappt hat).
    if (mode == STD_MODE_BLANK) 
      {
      mode = STD_MODE_NORMAL;
      ledDriver.wakeUp();
      }
    }
  else 
    {
    DEBUG_PRINTF(F("DCF77-Zeit verworfen weil die Abstände zwischen den Zeitstempel falsch sind\n"));
    }
  }


// Das Display toggeln (aus-/einschalten).
//------------------------------------------------------------------------------------------------------------
void setDisplayToToggle() 
  {
  if (mode != STD_MODE_BLANK) 
    {
    setDisplayToBlank();
    } 
  else 
    {
    setDisplayToResume();
    }
  }

// Das Display ausschalten.
//------------------------------------------------------------------------------------------------------------
void setDisplayToBlank() 
  {
  if (mode != STD_MODE_BLANK) 
    {
    lastMode = mode;
    mode = STD_MODE_BLANK;
    DEBUG_PRINTF(F("LED-Treiber: abgeschaltet\n"));
    ledDriver.shutDown();
    }
  }

// Das Display einschalten.
//------------------------------------------------------------------------------------------------------------
void setDisplayToResume() 
  {
  if (mode == STD_MODE_BLANK) 
    {
    mode = lastMode;
    DEBUG_PRINTF(F("LED-Treibe: eingeschaltet\n"));
    ledDriver.wakeUp();
    }
  }

// Das Display manuell heller machen.
//------------------------------------------------------------------------------------------------------------
void setDisplayBrighter() 
  {
  if ((!settings.getUseLdr()) && (settings.getBrightness() < 100)) 
    {
    uint8_t b = settings.getBrightness() + 10;
    if (b > 100) 
      {
      b = 100;
      }
    settings.setBrightness(b);
    settings.saveToEEPROM();
    ledDriver.setBrightness(b);
    }
  }

// Das Display dunkler machen.
//------------------------------------------------------------------------------------------------------------
void setDisplayDarker() 
  {
  if ((!settings.getUseLdr()) && (settings.getBrightness() > 1)) 
    {
    int16_t i = settings.getBrightness() - 10;
    if (i < 2) 
      {
      i = 1;
      }
    settings.setBrightness(i);
    settings.saveToEEPROM();
    ledDriver.setBrightness(i);
    }
  }
  
