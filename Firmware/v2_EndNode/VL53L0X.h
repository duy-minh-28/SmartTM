#ifndef __VL53L0X_H__
#define __VL53L0X_H__
 
#include <Arduino.h> 
#include <Wire.h>
#include <VL53L0X.h>

#define SDApin 21
#define SCLpin 22

void VL53L0X_Setup();
int readVL53L0X();

#endif