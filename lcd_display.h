#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <LiquidCrystal_I2C.h>
extern LiquidCrystal_I2C lcd;

void initLCD();
void displayInfo(String f_message, String s_message);
void lcdScrollText(int row, String message, int delayTime, int lcdColumns);

#endif
