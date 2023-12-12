#ifndef __MyLoraAS32_H__
#define __MyLoraAS32_H__
  
#include <SoftwareSerial.h>
#include <Arduino.h>
 
#define MYPORT_TX 17
#define MYPORT_RX 16

#define MD0 19
#define MD1 18
#define dio0 2 

// Master address 
const byte localAddress = 0x02;        
 
void AS32Setup();
void AS32Sleep();
void AS32Receive(); 
void AS32SendMessage(String msgContent);

void checkVarSlave_receivedMSG(int intervalSec, int periodExpectedPkg, int periodPkgThreshold);

String extractMsg(String data, char separator, int index);
String getMsgContentFromIndex(String data, char separator, int index);

#endif