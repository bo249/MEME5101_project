#include "wifi_mqtt.h"

WiFiClient espClient;
PubSubClient client(espClient);

void connectWiFi(const char* ssid, const char* password) {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected. IP: " + WiFi.localIP().toString());
}

void initMQTT(const char* mqtt_server, int mqtt_port, void (*callback)(char*, byte*, unsigned int)) {
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void maintainMQTT() {
  if (!client.connected()) {
    mqttReconnect("ESP32Client-01");
  }
  client.loop();
}

void mqttReconnect(const char* clientID) {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect(clientID)) {
      Serial.println("connected");
      // mqttSubscribe("conveyor/parking");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void mqttPublish(const char* topic, const char* message) {
  client.publish(topic, message);
  Serial.print("publish: ");
  Serial.print(topic);
  Serial.print(" - ");
  Serial.println(message);
}

void mqttSubscribe(const char* topic) {
  client.subscribe(topic);
  Serial.print("subscribe: ");
  Serial.println(topic);
}
