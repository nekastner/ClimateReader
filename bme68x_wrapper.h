#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "bme68x.h"

void bme68x_delay_us(uint32_t period, void *intf_ptr);
BME68X_INTF_RET_TYPE bme68x_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);
BME68X_INTF_RET_TYPE bme68x_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);

typedef struct {
    i2c_inst_t *i2c_port;
    uint8_t i2c_addr;
} I2c_CTX_WRAPPER;

bool setup_bme(
    i2c_inst_t *i2c_port,
    uint8_t i2c_addr,
    struct bme68x_dev *bme_dev,
    struct bme68x_conf *bme_conf,
    struct bme68x_heatr_conf *bme_heatr_conf,
    uint32_t *del_period);