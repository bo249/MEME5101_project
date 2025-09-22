#include "lcd_display.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

void initLCD() {
  lcd.init();
  lcd.backlight();
}

void displayInfo(String f_message, String s_message) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(f_message);
  lcd.setCursor(0, 1);
  lcd.print(s_message);
}

void lcdScrollText(int row, String message, int delayTime, int lcdColumns) {
  for (int i = 0; i < lcdColumns; i++) {
    message = " " + message;
  }
  message = message + " ";
  for (int pos = 0; pos < message.length(); pos++) {
    lcd.setCursor(0, row);
    lcd.print(message.substring(pos, pos + lcdColumns));
    delay(delayTime);
  }
}
