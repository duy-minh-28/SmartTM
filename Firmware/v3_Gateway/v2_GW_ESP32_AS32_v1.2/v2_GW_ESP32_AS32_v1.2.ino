// Include Library
#include "WifiConfig.h"   
#include "Error_Ctrl.h" 
#include "NTP_Client.h"  
#include "MyMQTT.h"   
#include "MyLoraAS32.h" 

void setup() {
  // put your setup code here, to run once: 
  Serial.begin(115200); 
  // drawTextAlignmentDemo();

  // Led setup
  LEDSetup();
  delay(3000);
  // Protocol Setup
  // while(WifiSetup()!=1){
  //   blinkLedSec(LED_CheckMSG_Red, 300);
  // }
  WifiConfig();
  
  MQTTSetup(); 
  delay(100);
  AS32Setup(); 
  // while(LoraSetup()!=1){
  //   blinkLedSec(LED_CheckMSG_Red, 300);
  // } 
  // Time and Logging Setup  
  dateSetup();  
  loggingSetup(); 
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