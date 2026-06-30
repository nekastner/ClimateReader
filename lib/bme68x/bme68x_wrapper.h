#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "bme68x.h"

#ifndef BME68X_WRAPPER_H
#define BME68X_WRAPPER_H

typedef struct {
    i2c_inst_t *i2c_port;
    uint8_t i2c_addr;
} BME68X_I2C_CTX_WRAPPER;

typedef struct {
    BME68X_I2C_CTX_WRAPPER i2c_ctx_wrapper;
    struct bme68x_dev dev;
    struct bme68x_conf conf;
    struct bme68x_heatr_conf heatr_conf;
    struct bme68x_data data;
    uint32_t del_period;
    uint8_t n_fields;
} BME68X_WRAPPER;

bool setup_bme68x(i2c_inst_t *i2c_port, uint8_t i2c_addr, BME68X_WRAPPER *bme68x_wrapper);

static inline bool set_bme68x_mode_forced(BME68X_WRAPPER *bme68x_wrapper) {
    return bme68x_set_op_mode(BME68X_FORCED_MODE, &bme68x_wrapper->dev) == BME68X_OK;
}

static inline void delay_bme68x(BME68X_WRAPPER *bme68x_wrapper) {
    bme68x_wrapper->dev.delay_us(bme68x_wrapper->del_period, bme68x_wrapper->dev.intf_ptr);
}

static inline bool get_data_forced(BME68X_WRAPPER *bme68x_wrapper) {
    return
    (
        bme68x_get_data(BME68X_FORCED_MODE, &bme68x_wrapper->data, &bme68x_wrapper->n_fields, &bme68x_wrapper->dev) == BME68X_OK &&
        bme68x_wrapper->n_fields > 0
    ) &&
    (bme68x_wrapper->data.status & BME68X_NEW_DATA_MSK);
}

#endif