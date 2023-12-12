#ifndef __MyFirmWare_H__
#define __MyFirmWare_H__

#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif

#include <Arduino.h> 

// Firmware version
const String FirmWareVersion = "v1";

String getMyMacAddress(); 
int getEndNodeAddress(); // Using DIP Switch  

#endif
