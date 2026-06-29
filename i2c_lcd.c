#include "i2c_lcd.h"

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"

void lcd_init(i2c_inst_t *port, uint8_t addr) {
    lcd_send_byte(port, addr, 0x03, LCD_COMMAND);
    lcd_send_byte(port, addr, 0x03, LCD_COMMAND);
    lcd_send_byte(port, addr, 0x03, LCD_COMMAND);
    lcd_send_byte(port, addr, 0x02, LCD_COMMAND);

    lcd_send_byte(port, addr, LCD_ENTRYMODESET | LCD_ENTRYLEFT, LCD_COMMAND);
    lcd_send_byte(port, addr, LCD_FUNCTIONSET | LCD_2LINE, LCD_COMMAND);
    lcd_send_byte(port, addr, LCD_DISPLAYCONTROL | LCD_DISPLAYON, LCD_COMMAND);
    lcd_clear(port, addr);
}

void lcd_clear(i2c_inst_t *port, uint8_t addr) {
    lcd_send_byte(port, addr, LCD_CLEARDISPLAY, LCD_COMMAND);
}

void lcd_string_at(i2c_inst_t *port, uint8_t addr, int line, int position, const char *s){
    lcd_set_cursor(port ,addr, line, position);
    lcd_string(port, addr, s);
}

void lcd_set_cursor(i2c_inst_t *port, uint8_t addr, int line, int position) {
    int val = (line == 0) ? 0x80 + position : 0xC0 + position;
    lcd_send_byte(port, addr, val, LCD_COMMAND);
}

void lcd_string(i2c_inst_t *port, uint8_t addr, const char *s) {
    while (*s) {
        lcd_char(port, addr, *s++);
    }
}

void lcd_send_byte(i2c_inst_t *port, uint8_t addr, uint8_t val, int mode) {
    uint8_t high = mode | (val & 0xF0) | LCD_BACKLIGHT;
    uint8_t low = mode | ((val << 4) & 0xF0) | LCD_BACKLIGHT;

    i2c_write_byte(port, addr, high);
    lcd_toggle_enable(port, addr, high);
    i2c_write_byte(port, addr, low);
    lcd_toggle_enable(port, addr, low);
}

void i2c_write_byte(i2c_inst_t *port, uint8_t addr, uint8_t val) {
    i2c_write_blocking(port, addr, &val, 1, false);
}

static inline void lcd_char(i2c_inst_t *port, uint8_t addr, char val) {
    lcd_send_byte(port, addr, val, LCD_CHARACTER);
}

void lcd_toggle_enable(i2c_inst_t *port, uint8_t addr, uint8_t val) {
    // Toggle enable pin on LCD display
    // We cannot do this too quickly or things don't work
#define DELAY_US 600
    sleep_us(DELAY_US);
    i2c_write_byte(port, addr, val | LCD_ENABLE_BIT);
    sleep_us(DELAY_US);
    i2c_write_byte(port, addr, val & ~LCD_ENABLE_BIT);
    sleep_us(DELAY_US);
}