#include <Arduino.h>
#include "relay_control.h"

#define RELAY 32

void relayInit() {
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);
}

void controlRelay(bool on) {
  digitalWrite(RELAY, on ? HIGH : LOW);
}
