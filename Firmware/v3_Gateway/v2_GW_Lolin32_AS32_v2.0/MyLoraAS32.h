#ifndef __MyLoraAS32_H__
#define __MyLoraAS32_H__
  
#include <SoftwareSerial.h>
#include <Arduino.h>
 
#define MYPORT_TX 15
#define MYPORT_RX 13

#define MD0 25
#define MD1 26
#define dio0 2 

const byte masterAddress = 0x01; //--> destination to send to Master (ESP32).
const byte localAddress = 0x04;       //--> address of this device (Slave 1).
 
void AS32Setup();
void AS32Sleep();
void AS32Receive(); 
void AS32SendMessage(String msgContent);

void checkVarSlave_receivedMSG(int intervalSec, int periodExpectedPkg, int periodPkgThreshold);

String extractMsg(String data, char separator, int index);
String getMsgContentFromIndex(String data, char separator, int index);

#endif