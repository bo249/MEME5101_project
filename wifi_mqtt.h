#ifndef WIFI_MQTT_H
#define WIFI_MQTT_H

#include <WiFi.h>
#include <PubSubClient.h>

extern WiFiClient espClient;
extern PubSubClient client;

void connectWiFi(const char* ssid, const char* password);
void initMQTT(const char* mqtt_server, int mqtt_port, void (*callback)(char*, byte*, unsigned int));
void maintainMQTT();
void mqttReconnect(const char* clientID);
void mqttPublish(const char* topic, const char* message);
void mqttSubscribe(const char* topic);

#endif
