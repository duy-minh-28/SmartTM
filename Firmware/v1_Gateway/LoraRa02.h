#ifndef __Ra02_H__
#define __Ra02_H__

#include <SPI.h>
#include <LoRa.h>

// #include <stdint.h>
#include <Arduino.h> 

//---------------------------------------- LoRa Pin / GPIO configuration.
#define ss 5
#define rst 4
#define dio0 2  

// Send MSG mode
typedef enum{
  SendDataMode = 1,
  SyncTimeMode = 2,
} requestMode;

// Function prototype
int LoraSetup();
void onReceive(int packetSize);

void sendMessageToSlave(String sms, byte masterAddress, byte statusCode); 
void checkVarSlave_receivedMSG(int intervalSec, int periodExpectedPkg, int periodPkgThreshold); 
String extractMsg(String data, char separator, int index); 
void loggingSetup();  
 
// void sendToSlaveLoop(int intervalSendMsg); 
#endif