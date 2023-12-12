#include "Firmware.h" 

int dipPin1 = 12;
int dipPin2 = 18;
int dipPin3 = 19;
int dipPin4 = 13;
int endNodeAddress;

String getMyMacAddress(){ 
  // Serial.print("ESP Board MAC Address:  "); 
  String macAdress = WiFi.macAddress(); 
  return macAdress;
}

int getEndNodeAddress(){ 
  pinMode(dipPin1, INPUT_PULLUP);
  pinMode(dipPin2, INPUT_PULLUP);
  pinMode(dipPin3, INPUT_PULLUP);
  pinMode(dipPin4, INPUT_PULLUP);
  
  byte statePin1 = digitalRead(dipPin1);  
  byte statePin2 = digitalRead(dipPin2);  
  byte statePin3 = digitalRead(dipPin3);  
  byte statePin4 = digitalRead(dipPin4);  

  endNodeAddress = 8*statePin1 + 4*statePin2 + 2*statePin3 + 1*statePin4;

  Serial.println("statePin1: "+String(statePin1));
  Serial.println("statePin2: "+String(statePin2));
  Serial.println("statePin3: "+String(statePin3));
  Serial.println("statePin4: "+String(statePin4));
  Serial.println("endNodeAddress: "+String(endNodeAddress));
  
  return endNodeAddress;
}