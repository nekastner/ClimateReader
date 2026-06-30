// standard libs
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
// manually added (and modified) libs
#include "lib/i2c_lcd/i2c_lcd.h"
#include "bme68x.h"
// self written code
#include "bme68x_wrapper.h"

// i2c vars
i2c_inst_t *I2C_PORT = i2c0;
const uint8_t SDA_PIN = 4;
const uint8_t SCL_PIN = 5;
// lcd vars
const uint8_t LCD_ADDR = 0x27;
// loop vars
const uint16_t LOOP_DELAY = 3000;

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

    // setup bme68x
    struct bme68x_dev bme_dev;
    struct bme68x_conf bme_conf;
    struct bme68x_heatr_conf bme_heatr_conf;
    struct bme68x_data bme_data;
    uint32_t del_period;

    uint8_t n_fields;
    int8_t rslt;
    
    if (!setup_bme(I2C_PORT, BME68X_I2C_ADDR_HIGH, &bme_dev, &bme_conf, &bme_heatr_conf, &del_period)) {
        printf("Fehler bei bme68x_init: %d\n", rslt);
        lcd_string_at(I2C_PORT, LCD_ADDR, 0, 1, "Sensor error.");
        while(1) { sleep_ms(100); }
    }

    // finish setups
    lcd_clear(I2C_PORT, LCD_ADDR);
    char lcd_line_buffer[16];

    // mainloop
    while (1) {
        
        // read from bme68x
        rslt = bme68x_set_op_mode(BME68X_FORCED_MODE, &bme_dev);
        if (rslt != BME68X_OK){
            printf("Error on bme68x_set_op_mode: %d\n", rslt);
            lcd_string_at(I2C_PORT, LCD_ADDR, 0, 0, "Error");
            sleep_ms(LOOP_DELAY);
            continue;
        }
        bme_dev.delay_us(del_period, bme_dev.intf_ptr);
        rslt = bme68x_get_data(BME68X_FORCED_MODE, &bme_data, &n_fields, &bme_dev);
        if ((rslt != BME68X_OK || n_fields <= 0) || !(bme_data.status & BME68X_NEW_DATA_MSK)){
            sleep_ms(LOOP_DELAY);
            continue;
        }

        // line 1
        snprintf(lcd_line_buffer, sizeof(lcd_line_buffer), "Temp: %.2f *C", bme_data.temperature);
        printf(lcd_line_buffer);
        lcd_set_cursor(I2C_PORT, LCD_ADDR, 0, 0);
        lcd_string_at(I2C_PORT, LCD_ADDR, 0, 0, lcd_line_buffer);
        // line 2
        snprintf(lcd_line_buffer, sizeof(lcd_line_buffer), "Humi: %.2f %%", bme_data.humidity);
        printf(lcd_line_buffer);
        lcd_string_at(I2C_PORT, LCD_ADDR, 1, 0, lcd_line_buffer);

        // delay next mainloop iteration
        sleep_ms(LOOP_DELAY);
    }

    return 0;
}