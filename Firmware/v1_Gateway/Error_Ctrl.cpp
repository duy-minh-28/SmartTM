#include "Error_Ctrl.h"  

void buzzer(int delaySec){
  tone(buzzlePin, 1000); // Send 1KHz sound signal...
  delay(delaySec);        // ...for 1 sec
  noTone(buzzlePin);     // Stop sound... 
}

void blinkLedSec(int LedPin, int delaySec){  
  digitalWrite(LedPin, HIGH); 
  delay(delaySec);                        
  digitalWrite(LedPin, LOW);  
  delay(delaySec);                        
}

void LEDSetup(){  
  pinMode(LED_CheckMSG_Red, OUTPUT);
  pinMode(LED_CheckMSG_Green, OUTPUT);
  pinMode(LED_CheckMSG_Yellow, OUTPUT);
  pinMode(LED_CheckMSG_Blue, OUTPUT);
  pinMode(buzzlePin, OUTPUT); 
  /* Start Setup, blink red 5 times */
  for(int i=0; i<5; i++){
    blinkLedSec(LED_CheckMSG_Red, 500);
  }     
}


