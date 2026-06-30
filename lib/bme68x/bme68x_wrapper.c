#include "bme68x_wrapper.h"

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "bme68x.h"

static BME68X_INTF_RET_TYPE bme68x_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr) {
    BME68X_I2C_CTX_WRAPPER *ctx = (BME68X_I2C_CTX_WRAPPER*)intf_ptr;
    if (i2c_write_blocking(ctx->i2c_port, ctx->i2c_addr, &reg_addr, 1, true) < 0) {
        return BME68X_E_COM_FAIL;
    }
    if (i2c_read_blocking(ctx->i2c_port, ctx->i2c_addr, reg_data, len, false) < 0) {
        return BME68X_E_COM_FAIL;
    }
    return BME68X_OK;
}

static BME68X_INTF_RET_TYPE bme68x_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr) {
    BME68X_I2C_CTX_WRAPPER *ctx = (BME68X_I2C_CTX_WRAPPER*)intf_ptr;
    uint8_t buffer[len + 1];
    buffer[0] = reg_addr;
    for (uint32_t i = 0; i < len; i++) {
        buffer[i + 1] = reg_data[i];
    }
    if (i2c_write_blocking(ctx->i2c_port, ctx->i2c_addr, buffer, len + 1, false) < 0) {
        return BME68X_E_COM_FAIL;
    }
    return BME68X_OK;
}

static void bme68x_delay_us(uint32_t period, void *intf_ptr) {
    sleep_us(period);
}

bool setup_bme68x(i2c_inst_t *i2c_port, uint8_t i2c_addr, BME68X_WRAPPER *bme68x_wrapper) {

    bme68x_wrapper->i2c_ctx_wrapper.i2c_port = i2c_port;
    bme68x_wrapper->i2c_ctx_wrapper.i2c_addr = i2c_addr;

    bme68x_wrapper->dev.read = bme68x_i2c_read;
    bme68x_wrapper->dev.write = bme68x_i2c_write;
    bme68x_wrapper->dev.delay_us = bme68x_delay_us;
    bme68x_wrapper->dev.intf = BME68X_I2C_INTF;
    bme68x_wrapper->dev.intf_ptr = &bme68x_wrapper->i2c_ctx_wrapper;
    bme68x_wrapper->dev.amb_temp = 25;
    if (bme68x_init(&bme68x_wrapper->dev) != BME68X_OK) return false;

    bme68x_wrapper->conf.filter = BME68X_FILTER_OFF;
    bme68x_wrapper->conf.odr = BME68X_ODR_NONE;
    bme68x_wrapper->conf.os_hum = BME68X_OS_16X;
    bme68x_wrapper->conf.os_pres = BME68X_OS_1X;
    bme68x_wrapper->conf.os_temp = BME68X_OS_2X;
    if(bme68x_set_conf(&bme68x_wrapper->conf, &bme68x_wrapper->dev) != BME68X_OK) return false;

    bme68x_wrapper->heatr_conf.enable = BME68X_ENABLE;
    bme68x_wrapper->heatr_conf.heatr_temp = 320;
    bme68x_wrapper->heatr_conf.heatr_dur = 150;
    if (bme68x_set_heatr_conf(BME68X_FORCED_MODE, &bme68x_wrapper->heatr_conf, &bme68x_wrapper->dev) != BME68X_OK) return false;

    bme68x_wrapper->del_period = bme68x_get_meas_dur(BME68X_FORCED_MODE, &bme68x_wrapper->conf, &bme68x_wrapper->dev) + (bme68x_wrapper->heatr_conf.heatr_dur * 1000);

    return true;
}