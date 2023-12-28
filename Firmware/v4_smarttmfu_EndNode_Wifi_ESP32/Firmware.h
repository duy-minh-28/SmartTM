#ifndef __smartTM_FirmWare_H__
#define __smartTM_FirmWare_H__

#include <Arduino.h> 
#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif

// Firmware version
const String FirmWareVersion = "1.0(Al)";

String getMyMacAddress(); 
int getEndNodeAddress(); // Using DIP Switch  

#endif
