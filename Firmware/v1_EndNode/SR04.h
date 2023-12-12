#ifndef __SR04_H__
#define __SR04_H__
 
#include <Arduino.h> 
 
const uint8_t echoPin = 33;
const uint8_t trigPin = 32; 
 
void initHC_SR(int trigPin, int echoPin);
float readSR04(int trigPin, int echoPin); 
  
#endif