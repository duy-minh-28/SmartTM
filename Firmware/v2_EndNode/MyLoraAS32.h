#ifndef __MyLoraAS32_H__
#define __MyLoraAS32_H__
  
#include <SoftwareSerial.h>
#include <Arduino.h>
 
#define MYPORT_TX 17
#define MYPORT_RX 16

#define MD0 19
#define MD1 18

void AS32Setup();
void AS32Sleep();
void AS32SendMessage(byte master_Address, byte local_Address, String msgContent);


#endif