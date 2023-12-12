#ifndef __MyOLED_H__
#define __MyOLED_H__

// #include <SSD1306Wire.h>        // legacy: #include "SSD1306.h"
#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
#include <Arduino.h>
#include "SSD1306.h"

#define SDA 5
#define SCL 4

 
void OledSetup();
void drawTextAlignmentDemo(); 

void clearOLED();
void displayOLED();
void drawReceiveFromSlave(int x, int y, String text);

#endif 