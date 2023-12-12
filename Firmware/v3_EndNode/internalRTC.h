#ifndef __internalRTC_H__
#define __internalRTC_H__

#include <ESP32Time.h>

/* ---------------- Sleep time ----------------*/
#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 5          /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR extern int bootCount; // MSG ID, reset after ESP is reset


/* ---------------- RTC ----------------*/ 

RTC_DATA_ATTR extern int secondRTC; 
RTC_DATA_ATTR extern int minuteRTC; 
RTC_DATA_ATTR extern int hourRTC; 
RTC_DATA_ATTR extern int dayRTC; 
RTC_DATA_ATTR extern int monthRTC; 
RTC_DATA_ATTR extern int yearRTC; 

void rtcSetup();
void saveTimeRTC();

String getDateRTC();

#endif