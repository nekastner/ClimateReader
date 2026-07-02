#include "./i2c_lcd.h"

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"

static void lcd_toggle_enable(const LCD *lcd, uint8_t val);

void lcd_init(const LCD *lcd) {
    lcd_send_byte(lcd, 0x03, LCD_COMMAND);
    lcd_send_byte(lcd, 0x03, LCD_COMMAND);
    lcd_send_byte(lcd, 0x03, LCD_COMMAND);
    lcd_send_byte(lcd, 0x02, LCD_COMMAND);
    lcd_send_byte(lcd, LCD_ENTRYMODESET | LCD_ENTRYLEFT, LCD_COMMAND);
    lcd_send_byte(lcd, LCD_FUNCTIONSET | LCD_2LINE, LCD_COMMAND);
    lcd_send_byte(lcd, LCD_DISPLAYCONTROL | LCD_DISPLAYON, LCD_COMMAND);
    lcd_clear    (lcd);
}

void lcd_string(const LCD *lcd, const char *s) {
    while (*s) lcd_char(lcd, *s++);
}

void lcd_send_byte(const LCD *lcd, uint8_t val, int mode) {
    uint8_t high = mode | (val & 0xF0) | LCD_BACKLIGHT;
    uint8_t low = mode | ((val << 4) & 0xF0) | LCD_BACKLIGHT;
    i2c_write_byte(lcd, high);
    lcd_toggle_enable(lcd, high);
    i2c_write_byte(lcd, low);
    lcd_toggle_enable(lcd, low);
}

static void lcd_toggle_enable(const LCD *lcd, uint8_t val) {
    const int DELAY_US = 600;
    sleep_us(DELAY_US);
    i2c_write_byte(lcd, val | LCD_ENABLE_BIT);
    sleep_us(DELAY_US);
    i2c_write_byte(lcd, val & ~LCD_ENABLE_BIT);
    sleep_us(DELAY_US);
}