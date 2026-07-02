// sdk libs
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
// lib-dir libs
#include "lib/i2c_lcd/i2c_lcd.h"
#include "lib/bme68x/bme68x_wrapper.h"

void lcd_and_seriall_ouput(const LCD *lcd, int line, int pos, char *text) {
    printf(text);
    lcd_string_at(lcd, line, pos, text);
}

int main() {

    // loop vars
    const uint16_t MAINLOOP_DELAY_MS = 3000;
    // i2c vars
    i2c_inst_t *I2C_PORT = i2c1;
    const uint8_t SDA_PIN = 2;
    const uint8_t SCL_PIN = 3;
    // lcd vars
    const LCD lcd = {
        .i2c_port = I2C_PORT,
        .i2c_addr = 0x27
    };
    // bme68x_wrapper vars
    BME68X_WRAPPER bme68x_wrapper;

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
    lcd_init(&lcd);

    // setup bme68x_wrapper
    if (!setup_bme68x(I2C_PORT, BME68X_I2C_ADDR_HIGH, &bme68x_wrapper)) {
        lcd_and_seriall_ouput(&lcd, 0, 0, "BME68x Setup-Error");
        while(1) {
            lcd_and_seriall_ouput(&lcd, 0, 0, "BME68x Setup-Error");
            sleep_ms(100);
        }
    }
    
    // finish setups
    lcd_clear(&lcd);

    // mainloop
    while (1) {

        // set bme68x_wrapper to forced mode
        if (!set_bme68x_mode_forced(&bme68x_wrapper)){
            lcd_and_seriall_ouput(&lcd, 0, 0, "BME68x Error");
            sleep_ms(MAINLOOP_DELAY_MS);
            continue;
        }
        delay_bme68x(&bme68x_wrapper);
        // read from bme68x_wrapper
        if (!get_data_forced(&bme68x_wrapper)){
            lcd_and_seriall_ouput(&lcd, 0, 0, "Data-Read-Fail");
            sleep_ms(MAINLOOP_DELAY_MS);
            continue;
        }

        char* lcd_line;
        // line 1
        snprintf(lcd_line, sizeof(lcd_line), "Temp: %.2f *C", bme68x_wrapper.data.temperature);
        lcd_and_seriall_ouput(&lcd, 0, 0, lcd_line);
        // line 2
        snprintf(lcd_line, sizeof(lcd_line), "Humi: %.2f %%", bme68x_wrapper.data.humidity);
        lcd_and_seriall_ouput(&lcd, 0, 0, lcd_line);

        // delay next mainloop iteration
        sleep_ms(MAINLOOP_DELAY_MS);
    }

    return 0;
}