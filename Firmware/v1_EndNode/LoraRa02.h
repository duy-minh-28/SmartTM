#ifndef __Ra02_H__
#define __Ra02_H__

#include <SPI.h>
#include <LoRa.h>

#include <stdint.h>
#include <Arduino.h> 
//---------------------------------------- LoRa Pin / GPIO configuration.
#define ss 5
#define rst 4
#define dio0 2 

// LoRa data transmission configuration.  
const byte masterAddress = 0x01;      //--> destination to send to Master (ESP32).
extern byte localAddress;       //--> address of this device (Slave 1).

// Send MSG mode
typedef enum{
  SendDataMode = 1,
  SyncTimeMode = 2,
} requestMode;

// For 2-way communication with master
extern uint8_t sleepFlag;
extern uint8_t notifiLed;

// Function prototype
void LoraSetup();
void onReceive(int packetSize);

void sendMessage(String sms, int requestMode);
void prepareMessageToMaster(String msgContent);

String GetValue(String data, char separator, int index);
   
void getSR04Val(); 
void printSR04Val();

#endif