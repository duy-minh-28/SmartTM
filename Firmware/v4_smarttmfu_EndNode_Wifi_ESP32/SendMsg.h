#ifndef __smartTM_SendMsg_H__
#define __smartTM_SendMsg_H__

#include <Arduino.h> 
 
/* ---------------- Sleep time ----------------*/
#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
// #define TIME_TO_SLEEP 120          /* Time ESP32 will go to sleep (in seconds) */
RTC_DATA_ATTR extern int bootCount; // MSG ID, reset after ESP is reset

void setup_EN_Wifi();
void sleep_EN_Wifi();
void sendMessage_EN_Wifi(int binID, int sensorValue, String token);

#endif