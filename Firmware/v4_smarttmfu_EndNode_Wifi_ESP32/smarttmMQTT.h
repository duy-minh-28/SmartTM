#ifndef __smartTM_MQTT_H__
#define __smartTM_MQTT_H__

#include <WiFi.h> 
#include <PubSubClient.h> 
#include <Arduino.h> 
 
void MQTTSetup();  
void mqttPublish(char *topic, char *toPublish);

void callback(char* topic, byte* payload, unsigned int length); 
void reconnect(); 
void mqttClientRefresh();

#endif 