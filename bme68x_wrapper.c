#include "bme68x_wrapper.h"

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "bme68x.h"

void bme68x_delay_us(uint32_t period, void *intf_ptr) {
    sleep_us(period);
}

BME68X_INTF_RET_TYPE bme68x_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr) {
    BME68X_DEV_I2c_CTX *ctx = (BME68X_DEV_I2c_CTX*)intf_ptr;
    if (i2c_write_blocking(ctx->i2c_port, ctx->dev_addr, &reg_addr, 1, true) < 0) {
        return BME68X_E_COM_FAIL;
    }
    if (i2c_read_blocking(ctx->i2c_port, ctx->dev_addr, reg_data, len, false) < 0) {
        return BME68X_E_COM_FAIL;
    }
    return BME68X_OK;
}

BME68X_INTF_RET_TYPE bme68x_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr) {
    BME68X_DEV_I2c_CTX *ctx = (BME68X_DEV_I2c_CTX*)intf_ptr;
    uint8_t buffer[len + 1];
    buffer[0] = reg_addr;
    for (uint32_t i = 0; i < len; i++) {
        buffer[i + 1] = reg_data[i];
    }
    if (i2c_write_blocking(ctx->i2c_port, ctx->dev_addr, buffer, len + 1, false) < 0) {
        return BME68X_E_COM_FAIL;
    }
    return BME68X_OK;
}

bool setup_bme(
    struct bme68x_dev *bme_dev,
    struct bme68x_conf *bme_conf,
    struct bme68x_heatr_conf *bme_heatr_conf,
    BME68X_DEV_I2c_CTX *bme_dev_i2c_ctx,
    uint32_t *del_period) {

    bme_dev->read = bme68x_i2c_read;
    bme_dev->write = bme68x_i2c_write;
    bme_dev->delay_us = bme68x_delay_us;
    bme_dev->intf = BME68X_I2C_INTF;
    bme_dev->intf_ptr = bme_dev_i2c_ctx;
    bme_dev->amb_temp = 25;
    if (bme68x_init(bme_dev) != BME68X_OK) return false;

    bme_conf->filter = BME68X_FILTER_OFF;
    bme_conf->odr = BME68X_ODR_NONE;
    bme_conf->os_hum = BME68X_OS_16X;
    bme_conf->os_pres = BME68X_OS_1X;
    bme_conf->os_temp = BME68X_OS_2X;
    if(bme68x_set_conf(bme_conf, bme_dev) != BME68X_OK) return false;

    bme_heatr_conf->enable = BME68X_ENABLE;
    bme_heatr_conf->heatr_temp = 320;
    bme_heatr_conf->heatr_dur = 150;
    if (bme68x_set_heatr_conf(BME68X_FORCED_MODE, bme_heatr_conf, bme_dev) != BME68X_OK) return false;

    *del_period = bme68x_get_meas_dur(BME68X_FORCED_MODE, bme_conf, bme_dev) + (bme_heatr_conf->heatr_dur * 1000);

    return true;
}