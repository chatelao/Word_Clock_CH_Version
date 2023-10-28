// Renderer
// Diese Klasse rendert die Woerter auf die Matrix.

#include "Renderer.h"
#include "Woerter_CH.h"

// #define DEBUG
#include "Configuration.h"

Renderer::Renderer()
  {
  }


// Ein Zufallsmuster erzeugen (zum Testen der LEDs)

void Renderer::scrambleScreenBuffer(uint16_t matrix[16]) 
  {
  for (uint8_t i = 0; i < 16; i++) 
    {
    matrix[i] = random(65536);
    }
  }


// Die Matrix loeschen (zum Stromsparen, DCF77-Empfang
// verbessern etc.)

void Renderer::clearScreenBuffer(uint16_t matrix[16]) 
  {
  for (uint8_t i = 0; i < 16; i++) 
    {
    matrix[i] = 0;
    }
  }


// Die Matrix komplett einschalten (zum Testen der LEDs)

void Renderer::setAllScreenBuffer(uint16_t matrix[16]) 
  {
  for (uint8_t i = 0; i < 16; i++) 
    {
    matrix[i] = 65535;
    }
  }


// Setzt die Wortminuten, je nach hours/minutes.

void Renderer::setMinutes(char hours, uint8_t minutes, uint16_t matrix[16]) 
  {
  while (hours < 0) 
    {
    hours += 12;
    }
  while (hours > 24) 
    {
    hours -= 12;
    }
  

 // Schweiz: Bern-Deutsch
 //------------------------------------------------------------------------------------------------------------

  CH_ESISCH;

  switch (minutes / 5) 
    {
    case 0:                          // ganze Stunde
      setHours(hours, true, matrix);
      break;
      
    case 1:                          // 5 ab
      CH_FUEF; CH_AB;
      setHours(hours, false, matrix);
      break;
    
    case 2:                            // 10 ab
      CH_ZAEAE; CH_AB;
      setHours(hours, false, matrix);
      break;
    
    case 3:                            // viertel ab
      CH_VIERTU; CH_AB;
      setHours(hours, false, matrix);
      break;
    
    case 4:                            // 20 ab
      CH_ZWAENZG; CH_AB;
      setHours(hours, false, matrix);
      break;
    
    case 5:                            // 5 vor halb
      CH_FUEF; CH_VOR; CH_HAUBI;
      setHours(hours + 1, false, matrix);
      break;
    
    case 6:                            // halb
      CH_HAUBI;
      setHours(hours + 1, false, matrix);
      break;
    
    case 7:                            // 5 ab halb
      CH_FUEF; CH_AB; CH_HAUBI;
      setHours(hours + 1, false, matrix);
      break;
    
    case 8:                            // 20 vor
      CH_ZWAENZG;  CH_VOR;
      setHours(hours + 1, false, matrix);
      break;
    
    case 9:                            // viertel vor
      CH_VIERTU; CH_VOR;
      setHours(hours + 1, false, matrix);
      break;
      
    case 10:                           // 10 vor
      CH_ZAEAE; CH_VOR;
      setHours(hours + 1, false, matrix);
      break;
    
    case 11:                           // 5 vor
      CH_FUEF; CH_VOR;
      setHours(hours + 1, false, matrix);
      break;
    }
  }


// Setzt die Stunden, je nach hours. 'glatt' bedeutet, es ist genau diese Stunde und wir muessen 'UHR'
// dazuschreiben und EIN statt EINS, falls es 1 ist. (Zumindest im Deutschen)
//------------------------------------------------------------------------------------------------------------
void Renderer::setHours(uint8_t hours, boolean glatt, uint16_t matrix[16]) 
  {
  // Schweiz: Berner-Deutsch
  switch (hours) 
    {
    case 0:
    case 12:
    case 24:
      CH_H_ZWOEUFI;
      break;
      
    case 1:
    case 13:
      CH_H_EIS;
      break;
    
    case 2:
    case 14:
      CH_H_ZWOEI;
      break;
    
    case 3:
    case 15:
      CH_H_DRUE;
      break;
    
    case 4:
    case 16:
      CH_H_VIER;
      break;
      
    case 5:
    case 17:
      CH_H_FUEFI;
      break;
    
    case 6:
    case 18:
      CH_H_SAECHSI;
      break;
      
    case 7:
    case 19:
      CH_H_SIEBNI;
      break;
      
    case 8:
    case 20:
      CH_H_ACHTI;
      break;
      
    case 9:
    case 21:
      CH_H_NUENI;
      break;
      
    case 10:
    case 22:
      CH_H_ZAENI;
      break;
      
    case 11:
    case 23:
      CH_H_EUFI;
      break;
    }
  }



// @param ccw: TRUE -> clock wise -> im Uhrzeigersinn.
//             FALSE -> counter clock wise -> gegen den Uhrzeigersinn.
// Setzt die vier Punkte, je nach minutes % 5 (Rest)
//------------------------------------------------------------------------------------------------------------

void Renderer::setCorners(uint8_t minutes, boolean cw, uint16_t matrix[16]) 
  {
  if (cw) 
    {
    // im Uhrzeigersinn
    switch (minutes % 5) 
      {
    case 4:
      matrix[3] |= 0b0000000000011111; // 4
      
    case 3:
      matrix[2] |= 0b0000000000011111; // 3
      
    case 2:
      matrix[1] |= 0b0000000000011111; // 2
      
    case 1:
      matrix[0] |= 0b0000000000011111; // 1
      CH_H_GSI;
      }
    } 
  else 
    {
    // gegen den Uhrzeigersinn
    switch (minutes % 5) 
      {
    case 4:
      matrix[0] |= 0b0000000000011111; // 1
      
    case 3:
      matrix[1] |= 0b0000000000011111; // 2
      
    case 2:
      matrix[2] |= 0b0000000000011111; // 3
      
    case 1:
      matrix[3] |= 0b0000000000011111; // 4
      CH_H_GSI;
      }
    }
  }


// Im Alarm-Einstell-Modus muessen bestimmte Woerter weg, wie z.B. "ES IST" im Deutschen.

void Renderer::cleanWordsForAlarmSettingMode(uint16_t matrix[16]) 
  {
  matrix[0] &= 0b0010000111111111; // ES ISCH weg
  }


