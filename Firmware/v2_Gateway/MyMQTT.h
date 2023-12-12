#ifndef __MyMQTT_H__
#define __MyMQTT_H__

#include <PubSubClient.h> 
#include <Arduino.h> 

void MQTTSetup(); 

void callback(char* topic, byte* payload, unsigned int length);

void mqttPublish(const char* topic, const char* toPublish);
void reconnect();

void mqttClientRefresh();

#endif 