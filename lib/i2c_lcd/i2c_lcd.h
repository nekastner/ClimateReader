/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// significantly modified!

#include <stdio.h>
#include "hardware/i2c.h"

#ifndef I2C_LCD_H
#define I2C_LCD_H

// commands
#define LCD_CLEARDISPLAY        0x01
#define LCD_RETURNHOME          0x02
#define LCD_ENTRYMODESET        0x04
#define LCD_DISPLAYCONTROL      0x08
#define LCD_CURSORSHIFT         0x10
#define LCD_FUNCTIONSET         0x20
#define LCD_SETCGRAMADDR        0x40
#define LCD_SETDDRAMADDR        0x80

// flags for display entry mode
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYLEFT           0x02

// flags for display and cursor control
#define LCD_BLINKON             0x01
#define LCD_CURSORON            0x02
#define LCD_DISPLAYON           0x04

// flags for display and cursor shift
#define LCD_MOVERIGHT           0x04
#define LCD_DISPLAYMOVE         0x08

// flags for function set
#define LCD_5x10DOTS            0x04
#define LCD_2LINE               0x08
#define LCD_8BITMODE            0x10

// flag for backlight control
#define LCD_BACKLIGHT           0x08

#define LCD_ENABLE_BIT          0x04

// Modes for lcd_send_byte
#define LCD_CHARACTER           1
#define LCD_COMMAND             0

typedef struct {
    i2c_inst_t *i2c_port;
    uint8_t i2c_addr;
} LCD;

void lcd_init     (const LCD *lcd);
void lcd_string   (const LCD *lcd, const char *s);
void lcd_send_byte(const LCD *lcd, uint8_t val, int mode);

static inline void lcd_clear(const LCD *lcd) {
    lcd_send_byte(lcd, LCD_CLEARDISPLAY, LCD_COMMAND);
}

static inline void lcd_set_cursor(const LCD *lcd, int line, int pos) {
    lcd_send_byte(lcd, (line == 0) ? 0x80 + pos : 0xC0 + pos, LCD_COMMAND);
}

static inline void lcd_string_at(const LCD *lcd, int line, int pos, const char *s){
    lcd_set_cursor(lcd, line, pos);
    lcd_string(lcd, s);
}

static inline void i2c_write_byte(const LCD *lcd, uint8_t val) {
    i2c_write_blocking(lcd->i2c_port, lcd->i2c_addr, &val, 1, false);
}

static inline void lcd_char(const LCD *lcd, char val) {
    lcd_send_byte(lcd, val, LCD_CHARACTER);
}

#endif