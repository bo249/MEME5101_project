#ifndef WS2812_LED_H
#define WS2812_LED_H

#include <Adafruit_NeoPixel.h>
extern Adafruit_NeoPixel strip;

void initWS2812(uint8_t brightness);
void led_default(uint8_t r, uint8_t g, uint8_t b);
void led_off();
void blink_yellow(bool &relay_is_on, bool &yellow_blink_state, unsigned long &last_blink_time, uint8_t relay_on_r, uint8_t relay_on_g, uint8_t relay_on_b, uint8_t relay_off_r, uint8_t relay_off_g, uint8_t relay_off_b);

#endif