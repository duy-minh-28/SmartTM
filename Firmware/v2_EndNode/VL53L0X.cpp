#include "VL53L0X.h" 

// -- Defines variables -- 
VL53L0X sensor; 
float distance; 

void VL53L0X_Setup(){
  // -- Init --   
  Serial.print("VL53L0X_Setup");
  Wire.begin(SDApin,SCLpin);
  sensor.init();
  sensor.setTimeout(500); 
  sensor.startContinuous();
}

// ---------------------------- Read SR04 Sensor Value and Calculate distance ----------------------------
int readVL53L0X(){ 
  int distance = sensor.readRangeSingleMillimeters();
  int distance_cm = distance/10;
  if (sensor.timeoutOccurred()) { Serial.print("VL53L0X TIMEOUT"); } 
  return distance_cm; 
} 


 
 
