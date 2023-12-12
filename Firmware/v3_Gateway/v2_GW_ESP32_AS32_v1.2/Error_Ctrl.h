#ifndef __Error_Control_H__
#define __Error_Control_H__

#include <Arduino.h>  
#include <stdint.h>

/* -------------------- LED -------------------- */
// Red: Not enough MSG over period| Green: Enough MSG over period| Yellow: Blink when received MSG
const uint8_t LED_CheckMSG_Red = 27;
const uint8_t LED_CheckMSG_Yellow = 26;
const uint8_t LED_CheckMSG_Green = 25;
const uint8_t LED_CheckMSG_Blue = 13;
const uint8_t buzzlePin = 32;

void blinkLedSec(int LedPin, int delaySec);
void buzzer(int delaySec);
void LEDSetup();
 
void loggingSetup();  
 
/* -------------------- Error Enum -------------------- */
enum _config_error{
  E_Success = 0,
  E_Wifi_Setup_Fail = -1,
  E_MQTT_Setup_Fail  = -2,
  E_Lora_Setup_Fail  = -3
};

/* type to provide in your API */
typedef enum _config_error SetupError;

/* use this to provide a perror style method to help consumers out */
// struct _errordesc {
//     int  code;
//     char *message;
// } errordesc[] = {
//     { E_Success, "No error" },
//     { E_Wifi_Setup_Fail, "Invalid Wifi" },
//     { E_MQTT_Setup_Fail, "Invalid MQTT" }, 
//     { E_Lora_Setup_Fail, "Invalid Lora" }
// };

#endif 