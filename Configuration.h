// Configuration
// Die Compiler-Einstellungen der Firmware an einer zentralen Stelle.

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

// Alarmfunktion einschalten?
// Dazu muss ein Lautsprecher an D13 und GND und eine weitere 'Eck-LED' an die 5te Reihe.
// Dann gibt es einen neuen Modus, direkt nach der normalen Zeitanzeige. Die neue LED
// blinkt nach dem Moduswechsel und der Alarm ist eingeschaltet. Drueckt man jetzt M+ oder H+ stellt man
// die Alarmzeit ein, angedeutet durch die blinkende Alarm-LED. Druckt man 10 Sekunden
// keine Taste, hoert das Blinken auf und die normale Zeit wird wieder angezeigt.
// Bei erreichen des Alarms piept der Lautpsrecher auf D13. Zum Ausschalten muss der Modus-Taster gedrueckt werden.
// Weiter unten kommen drei weitere DEFINEs:
// - SPEAKER ist der Pin, an dem der Lautsprecher haengt.
// - SPEAKER_FREQUENCY ist die Tonhoehe, wenn der Speaker ein Lautpsrecher ist.
// - SPEAKER_IS_BUZZER: wenn einkommentiert wird davon ausgegangen, dass am Pin SPEAKER ein Buzzer haengt (Reichelt: SUMMER TDB 05).
//------------------------------------------------------------------------------------------------------------
#define SPEAKER_FREQUENCY 200000
#define MAX_BUZZ_TIME_IN_MINUTES 10
#define SPEAKER_IS_BUZZER


// Die Status-LEDs koennen hier durch auskommentieren ausgeschaltet werden.
// Default: eingeschaltet
//------------------------------------------------------------------------------------------------------------
#define ENABLE_DCF_LED
#define ENABLE_SQW_LED


// Welche Fernbedienung soll benutzt werden?
//------------------------------------------------------------------------------------------------------------
// #define REMOTE_NO_REMOTE
 #define REMOTE_NEC


// Seltener zu aendernde Einstellungen...
//------------------------------------------------------------------------------------------------------------
// Die Zeit, die mit LDR::MAX_BRIGHTNESS multipliziert auf eine Zeile aufgeteilt wird in Mikrosekunden.
// (default = 8)
#define PWM_DURATION 8


// ------------------ Tasten ---------------------
// Die Zeit in Millisekunden, innerhalb derer Prellungen der Taster nicht als Druecken zaehlen.
// (Und damit auch die Tastaturwiederholrate)
// Default: 300
#define BUTTON_TRESHOLD 300


// ------------------ DCF77-Empfaenger ---------------------
// Fuer wieviele DCF77-Samples muessen die Zeitabstaende stimmen, damit das DCF77-Telegramm als gueltig zaehlt?
// Default: 3 (min. 3 Minuten Synchronisationszeit).
#define DCF77HELPER_MAX_SAMPLES 2

// Die Telegrammlaenge.
// Default: 59
#define MYDCF77_TELEGRAMMLAENGE 59

// Anzahl der Glaettungspunkte fuer das DCF77-Signal.
#define MYDCF77_MEANCOUNT 59

// Startwerte fuer den Mittelwert.
#define MYDCF77_MEANSTARTVALUE 1450


// ------------------ Lichtabhaengiger Widerstand ---------------------
// Sollen die Grenzwerte vom LDR automatisch angepasst werden? Bei einem Neustart kann
// das Display flackern, dann muss man einmal ueber den LDR 'wischen', damit er verschiedene
// Messwerte bekommt. Ohne AUTOSCALE werden die manuellen Min-/Max-Werte genommen.
// Default: eingeschaltet.
#define LDR_AUTOSCALE
#define LDR_MANUAL_MIN 0
#define LDR_MANUAL_MAX 1023

// Der Hysterese-Trashold.
// Default: 50
#define LDR_HYSTERESE 50

// Die LDR-Werte werden auf Prozent gemappt.
// Hier koennen diese Werte beschnitten werden, falls man eine minimale oder maximale Helligkeit vorgeben moechte.
#define LDR_MIN_PERCENT 2
#define LDR_MAX_PERCENT 100

// LDR-Check-Raten. Dieser Wert beeinflusst, wie schnell sich die Displayhelligkeit an neue LDR-Werte anpasst
// und ist vom LedDriver abhaengig. Der Wert ist der Trashold in Millisekunden, bevor eine neue Anpassung stattfindet.
#define LDR_CHECK_RATE 10

#endif
