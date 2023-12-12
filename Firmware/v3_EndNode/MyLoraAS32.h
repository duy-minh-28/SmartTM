#ifndef __MyLoraAS32_H__
#define __MyLoraAS32_H__
  
#include <SoftwareSerial.h>
#include <Arduino.h>
 
#define MYPORT_TX 0
#define MYPORT_RX 1

#define MD0 3
#define MD1 2
 
void AS32Setup();
void AS32Sleep();
void AS32SendMessage(byte master_Address, byte local_Address, String msgContent);

#endif