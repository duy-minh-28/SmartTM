#include "Error_Ctrl.h"  
#include "NTP_Client.h"  

void buzzer(int delaySec){
  tone(buzzlePin, 1000); // Send 1KHz sound signal...
  delay(delaySec);        // ...for 1 sec
  noTone(buzzlePin);     // Stop sound... 
}

void blinkLedSec(int LedPin, int delaySec){  
  digitalWrite(LedPin, HIGH); 
  delay(delaySec);                        
  digitalWrite(LedPin, LOW);  
}

void LEDSetup(){  
  pinMode(LED_CheckMSG_Red, OUTPUT);
  pinMode(LED_CheckMSG_Green, OUTPUT);
  pinMode(LED_CheckMSG_Yellow, OUTPUT);
  pinMode(LED_CheckMSG_Blue, OUTPUT);
  pinMode(buzzlePin, OUTPUT);
  digitalWrite(LED_CheckMSG_Red, HIGH);   
}


// -------------------- Other setup for Logging and Warning -------------------- 
void loggingSetup(){
  // getdate();
  Serial.println("########################################################################################################");    
  Serial.println("######################### STARTING NEW SESSION/LOG | DATE: "+getdate()+" #########################");    
  Serial.println("########################################################################################################");   
}
