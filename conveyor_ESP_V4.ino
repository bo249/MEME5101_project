#include "lcd_display.h"
#include "ws2812_led.h"
#include "wifi_mqtt.h"
#include "relay_control.h"
#define IR2 23

// 全域狀態宣告
bool relay_is_on = false;
bool yellow_blink_state = false;
unsigned long last_blink_time = 0;
uint8_t relay_on_r = 255, relay_on_g = 180, relay_on_b = 0; // yellow
uint8_t relay_off_r = 0, relay_off_g = 255, relay_off_b = 0; // green
uint8_t brightness = 30;

const char* ssid = "WIFI ssid";
const char* password = "WIFI password";
const char* mqtt_server = "192.168.69.162";
const int mqtt_port = 9566;

char command;
bool auto_mode = true;
bool auto_relay_on = false;

void handleManualMode(char cmd);
void handleAutoMode();

void setup() {
  Serial.begin(115200);
  initLCD();
  initWS2812(brightness);
  connectWiFi(ssid, password);
  //initMQTT(mqtt_server, mqtt_port, callback); // 你自己的回呼
  led_default(relay_off_r, relay_off_g, relay_off_b);
}

void loop() {
  //maintainMQTT();
  // 監聽指令，切換模式或下達relay動作
  if (Serial.available() > 0) {
    command = Serial.read();
    switch (command) {
      case 'A':
        auto_mode = true;
        Serial.println("auto mode");
        auto_relay_on = false;
        controlRelay(false); // 預設關閉
        break;
      case 'M':
        auto_mode = false;
        Serial.println("manual mode");
        auto_relay_on = false;
        controlRelay(false); // 預設關閉
        break;
      case 'Z':
        if (auto_mode) {
          auto_relay_on = true;
          controlRelay(true); // relay 開
          Serial.println("auto relay ON by Z");
        }
        break;
      default:
        if (!auto_mode) {
          handleManualMode(command);
        }
        break;
    }
  }

  // 自動模式持續監控 IR2
  if (auto_mode) {
    handleAutoMode();
  }

  //開啟時閃爍黃燈
  if (relay_is_on) {
    blink_yellow(relay_is_on, yellow_blink_state, last_blink_time,
                 relay_on_r, relay_on_g, relay_on_b,
                 relay_off_r, relay_off_g, relay_off_b);
  } else {
    static bool last_state = true;
    if (last_state) {
      led_default(relay_off_r, relay_off_g, relay_off_b); // 綠
      last_state = false;
    }
  }

  delay(10);
}

// --- 自動模式處理，IR2觸發時publish MQTT ---
void handleAutoMode() {
  if (auto_relay_on) {
    int val2 = digitalRead(IR2);
    if (val2 == 0) { // IR2 被觸發
      controlRelay(false);    // relay 關閉
      auto_relay_on = false;  // 必須重新 Z 才啟動
      Serial.println("auto relay OFF by IR2");
      // ------ 這裡發送 MQTT 通知 ------
      mqttPublish("conveyor/conveyor", "1");
    }
  }
}

// 手動模式處理
void handleManualMode(char cmd) {
  switch (cmd) {
    case '1':
      controlRelay(true);
      Serial.println("relay ON (manual)");
      break;
    case '0':
      controlRelay(false);
      Serial.println("relay OFF (manual)");
      break;
    default:
      break;
  }
}

