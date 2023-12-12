// Include Library
#include "WifiConfig.h"   
#include "Error_Ctrl.h" 
#include "Oled.h"   
#include "NTP_Client.h"  
#include "MyMQTT.h"   
#include "MyLoraAS32.h" 

void setup() {
  // put your setup code here, to run once: 
  Serial.begin(115200);
  
  // On-Sight Notification Setup: Oled, LED
  OledSetup();
  LEDSetup();
  delay(2000);
    
  WifiConfig(); 
  delay(4000);
  MQTTSetup();  
  delay(2000);
  AS32Setup();  
  delay(2000);
  
  // Time and Logging Setup  
  dateSetup();  
  loggingSetup(); 
  
  clearOLED();
  drawReceiveFromSlave(0, 10, "##### Finish Setup #####"); 
  drawReceiveFromSlave(0, 22, "Waiting for End Node MSG"); 
  delay(3000);
}  
 
void loop() { 
  // Waiting for msg from slave  
  AS32Receive();
  
  // Check if Slave send enough SMG over a period of time
  // checkVarSlave_receivedMSG(60, 7, 7); 

  // Refresh MQTT client connection
  if (runEvery(5000)) {
    mqttClientRefresh();
  }  
} 

boolean runEvery(unsigned long interval){
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;
    return true;
  }
  return false;
}
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 