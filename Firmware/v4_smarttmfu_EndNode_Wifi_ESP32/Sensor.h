#ifndef __smartTM_VL53L0X_H__
#define __smartTM_VL53L0X_H__
 
#include <Arduino.h> 
#include <Wire.h>
#include <VL53L0X.h>

#define SDApin 4
#define SCLpin 5

void VL53L0X_Setup();
int readVL53L0X();

#endif