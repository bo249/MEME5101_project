#include "ws2812_led.h"

Adafruit_NeoPixel strip(8, 18, NEO_GRB + NEO_KHZ800); // 記得根據你的實際腳位/燈數

void initWS2812(uint8_t brightness) {
  strip.begin();
  strip.setBrightness(brightness);
  strip.show();
}

void led_default(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();
}

void led_off() {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0);
  }
  strip.show();
}

void blink_yellow(bool &relay_is_on, bool &yellow_blink_state, unsigned long &last_blink_time, uint8_t relay_on_r, uint8_t relay_on_g, uint8_t relay_on_b, uint8_t relay_off_r, uint8_t relay_off_g, uint8_t relay_off_b) {
  unsigned long now = millis();
  static bool last_blink_state = false;
  if (now - last_blink_time >= 300) { // 300ms
    last_blink_time = now;
    yellow_blink_state = !yellow_blink_state;
    last_blink_state = yellow_blink_state;
    if (yellow_blink_state)
      led_default(relay_on_r, relay_on_g, relay_on_b);
    else
      led_off();
  }
}
