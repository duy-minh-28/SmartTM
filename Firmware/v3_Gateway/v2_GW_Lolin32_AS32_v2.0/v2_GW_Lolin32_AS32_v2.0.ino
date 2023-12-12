// Include Library

#include "MyMQTT.h"  
#include "Error_Ctrl.h"  
#include "NTP_Client.h"  
#include "MyLoraAS32.h"  

void setup() {
  // put your setup code here, to run once: 
  Serial.begin(115200);
  // Led setup
  LEDSetup();
  delay(3000);
  // Protocol Setup
  while(WifiSetup()!=1){
    blinkLedSec(LED_CheckMSG_Red, 300);
  }
  MQTTSetup(); 


  AS32Setup(); 
  delay(30);
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

  // Millis/Timer to reset Lora Ra-02.
  //resetLoraLoop(60); // ~ Reset Lora if received no SMG for x seconds

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