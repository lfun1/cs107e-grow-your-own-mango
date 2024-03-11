/* 
 * BME280 Library Implementations
 * Lisa Fung, Elias Chikwanda
 * March 10, 2024 
 */

#include "bme280.h"

#include "printf.h"


/* BME280 config register */

static unsigned int config_get(struct config *configReg) {
    return (configReg->t_sb << 5) | (configReg->filter << 2) | configReg->spi3w_en;
}

static struct config _configReg = {.get = config_get,};


/* BME280 meas register */

static unsigned int meas_get(struct ctrl_meas *measReg) {
    return (measReg->osrs_t << 5) | (measReg->osrs_p << 2) | measReg->mode;    
}

static struct ctrl_meas _measReg = {.get = meas_get,};


/* BME280 hum register */

static unsigned int hum_get(struct ctrl_hum *humReg) {
    return (humReg->osrs_h);   
}

static struct ctrl_hum _humReg = {.get = hum_get,};


/* Set Sampling */
void setSampling(sensor_mode_t mode,
                 sensor_sampling_t tempSampling,
                 sensor_sampling_t pressSampling,
                 sensor_sampling_t humSampling,
                 sensor_filter_t filter,
                 standby_duration_t duration) {
    
}


/* Read and Write to BME280 Registers */

// Read `len` bytes starting from BME280 register `reg`
// Only return the value in register `reg`
uint8_t read(uint8_t reg, int len) {
    uint8_t tx[len], rx[len];
    tx[0] = (uint8_t)(reg | 0x80); // RW = '1' for read
    
    spi_transfer(tx, rx, len);
    
    return rx[1]; // Data in register `reg` is second byte
}

uint8_t read8(uint8_t reg) {
    return read(reg, 2);
}

// Write `value` to BME280 register `reg`
void write8(uint8_t reg, uint8_t value) {
    uint8_t tx[2], rx[2];
    tx[0] = (uint8_t) (reg & ~0x80); // RW = '0' for write
    tx[1] = value;
    spi_transfer(tx, rx, 2);
}
