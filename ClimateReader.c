// sdk libs
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
// lib-dir libs
#include "lib/i2c_lcd/i2c_lcd.h"
#include "lib/bme68x/bme68x_wrapper.h"

// loop vars
const uint16_t MAINLOOP_DELAY_MS = 3000;
// i2c vars
i2c_inst_t *I2C_PORT = i2c0;
const uint8_t SDA_PIN = 4;
const uint8_t SCL_PIN = 5;
// lcd vars
const uint8_t LCD_ADDR = 0x27;
char lcd_line_buffer[16];
// bme68x_wrapper vars
BME68X_WRAPPER bme68x_wrapper;

int main() {

    // setup standard stuff
    stdio_init_all();

    // init i2c
    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    // ?
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    // init lcd
    lcd_init(I2C_PORT, LCD_ADDR);

    // setup bme68x_wrapper
    if (!setup_bme68x(I2C_PORT, BME68X_I2C_ADDR_HIGH, &bme68x_wrapper)) {
        snprintf(lcd_line_buffer, sizeof(lcd_line_buffer), "BME68x Setup-Error");
        printf(lcd_line_buffer);
        lcd_string_at(I2C_PORT, LCD_ADDR, 0, 0, lcd_line_buffer);
        while(1) { sleep_ms(100); }
    }

    // finish setups
    lcd_clear(I2C_PORT, LCD_ADDR);

    // mainloop
    while (1) {

        // set bme68x_wrapper to forced mode
        if (!set_bme68x_mode_forced(&bme68x_wrapper)){
            // line 1
            snprintf(lcd_line_buffer, sizeof(lcd_line_buffer), "BME68x Error");
            printf(lcd_line_buffer);
            lcd_string_at(I2C_PORT, LCD_ADDR, 0, 0, lcd_line_buffer);
            // next iteration
            sleep_ms(MAINLOOP_DELAY_MS);
            continue;
        }
        delay_bme68x(&bme68x_wrapper);
        // read from bme68x_wrapper
        if (!get_data_forced(&bme68x_wrapper)){
            // line 1
            snprintf(lcd_line_buffer, sizeof(lcd_line_buffer), "Data-Read-Fail");
            printf(lcd_line_buffer);
            lcd_string_at(I2C_PORT, LCD_ADDR, 0, 0, lcd_line_buffer);
            // next iteration
            sleep_ms(MAINLOOP_DELAY_MS);
            continue;
        }

        // line 1
        snprintf(lcd_line_buffer, sizeof(lcd_line_buffer), "Temp: %.2f *C", bme68x_wrapper.data.temperature);
        printf(lcd_line_buffer);
        lcd_set_cursor(I2C_PORT, LCD_ADDR, 0, 0);
        lcd_string_at(I2C_PORT, LCD_ADDR, 0, 0, lcd_line_buffer);
        // line 2
        snprintf(lcd_line_buffer, sizeof(lcd_line_buffer), "Humi: %.2f %%", bme68x_wrapper.data.humidity);
        printf(lcd_line_buffer);
        lcd_string_at(I2C_PORT, LCD_ADDR, 1, 0, lcd_line_buffer);

        // delay next mainloop iteration
        sleep_ms(MAINLOOP_DELAY_MS);
    }

    return 0;
}