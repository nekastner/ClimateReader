/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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

void lcd_init              (i2c_inst_t *port, uint8_t addr);
void lcd_string            (i2c_inst_t *port, uint8_t addr, const char *s);
void lcd_send_byte         (i2c_inst_t *port, uint8_t addr, uint8_t val, int mode);

static inline void lcd_clear(i2c_inst_t *port, uint8_t addr) {
    lcd_send_byte(port, addr, LCD_CLEARDISPLAY, LCD_COMMAND);
}

static inline void lcd_set_cursor(i2c_inst_t *port, uint8_t addr, int line, int position) {
    lcd_send_byte(port, addr, (line == 0) ? 0x80 + position : 0xC0 + position, LCD_COMMAND);
}

static inline void lcd_string_at(i2c_inst_t *port, uint8_t addr, int line, int position, const char *s){
    lcd_set_cursor(port ,addr, line, position);
    lcd_string(port, addr, s);
}

static inline void i2c_write_byte(i2c_inst_t *port, uint8_t addr, uint8_t val) {
    i2c_write_blocking(port, addr, &val, 1, false);
}

static inline void lcd_char(i2c_inst_t *port, uint8_t addr, char val) {
    lcd_send_byte(port, addr, val, LCD_CHARACTER);
}

#endif