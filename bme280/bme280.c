/* 
 * BME280 Library Implementations
 * Lisa Fung, Elias Chikwanda
 * March 10, 2024 
 */

#include "bme280.h"
#include "timer.h"
#include "printf.h"


bme280_calib_data _bme280_calib; //!< here calibration data is stored

static int32_t _sensorID; //!< ID of the BME Sensor
static int32_t t_fine; //!< temperature with high resolution, stored as an attribute
//!< as this is used for temperature compensation reading
//!< humidity and pressure

static int32_t t_fine_adjust = 0; //!< add to compensate temp readings and in turn
//!< to pressure and humidity readings

static void readCoefficients(void);
static bool isReadingCalibration(void);

/* BME280 init */
bool bme_init(void) {
    _sensorID = read8(BME280_REGISTER_CHIPID);

    if (_sensorID != 0x60)
        return false;

    // reset the device using soft-reset
    // this makes sure the IIR is off, etc.
    write8(BME280_REGISTER_SOFTRESET, 0xB6);

    // wait for chip to wake up.
    timer_delay_ms(10);

    // if chip is still reading calibration, delay
    while (isReadingCalibration())
        timer_delay_ms(10);

    readCoefficients(); // read trimming parameters, see DS 4.2.2

    //setSampling(); // use defaults
    setSampling(MODE_NORMAL,
                SAMPLING_X1,
                SAMPLING_X1,
                SAMPLING_X1,
                FILTER_OFF,
                STANDBY_MS_1000);

    timer_delay_ms(100);

    return true;
}

/* Read Calibration */
static bool isReadingCalibration(void) {
    uint8_t const rStatus = read8(BME280_REGISTER_STATUS);
    return (rStatus & (1 << 0)) != 0;
}

/* Read Coefficients */
void readCoefficients(void) {
    _bme280_calib.dig_T1 = read16_LE(BME280_REGISTER_DIG_T1);
    _bme280_calib.dig_T2 = readS16_LE(BME280_REGISTER_DIG_T2);
    _bme280_calib.dig_T3 = readS16_LE(BME280_REGISTER_DIG_T3);

    _bme280_calib.dig_P1 = read16_LE(BME280_REGISTER_DIG_P1);
    _bme280_calib.dig_P2 = readS16_LE(BME280_REGISTER_DIG_P2);
    _bme280_calib.dig_P3 = readS16_LE(BME280_REGISTER_DIG_P3);
    _bme280_calib.dig_P4 = readS16_LE(BME280_REGISTER_DIG_P4);
    _bme280_calib.dig_P5 = readS16_LE(BME280_REGISTER_DIG_P5);
    _bme280_calib.dig_P6 = readS16_LE(BME280_REGISTER_DIG_P6);
    _bme280_calib.dig_P7 = readS16_LE(BME280_REGISTER_DIG_P7);
    _bme280_calib.dig_P8 = readS16_LE(BME280_REGISTER_DIG_P8);
    _bme280_calib.dig_P9 = readS16_LE(BME280_REGISTER_DIG_P9);

    _bme280_calib.dig_H1 = read8(BME280_REGISTER_DIG_H1);
    _bme280_calib.dig_H2 = readS16_LE(BME280_REGISTER_DIG_H2);
    _bme280_calib.dig_H3 = read8(BME280_REGISTER_DIG_H3);
    _bme280_calib.dig_H4 = ((int8_t)read8(BME280_REGISTER_DIG_H4) << 4) |
        (read8(BME280_REGISTER_DIG_H4 + 1) & 0xF);
    _bme280_calib.dig_H5 = ((int8_t)read8(BME280_REGISTER_DIG_H5 + 1) << 4) |
        (read8(BME280_REGISTER_DIG_H5) >> 4);
    _bme280_calib.dig_H6 = (int8_t)read8(BME280_REGISTER_DIG_H6);
}

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

    _measReg.mode = mode;
    _measReg.osrs_t = tempSampling;
    _measReg.osrs_p = pressSampling;

    _humReg.osrs_h = humSampling;
    _configReg.filter = filter;
    _configReg.t_sb = duration;
    _configReg.spi3w_en = 0;

    // making sure sensor is in sleep mode before setting configuration
    // as it otherwise may be ignored
    write8(BME280_REGISTER_CONTROL, MODE_SLEEP);

    // you must make sure to also set REGISTER_CONTROL after setting the
    // CONTROLHUMID register, otherwise the values won't be applied (see
    // DS 5.4.3)
    write8(BME280_REGISTER_CONTROLHUMID, _humReg.get(&_humReg));
    write8(BME280_REGISTER_CONFIG, _configReg.get(&_configReg));
    write8(BME280_REGISTER_CONTROL, _measReg.get(&_measReg));
}


/* Read Temperature */
float readTemperature(void) {
    int32_t var1, var2;

    int32_t adc_T = read24(BME280_REGISTER_TEMPDATA);
    if (adc_T == 0x800000) // value in case temp measurement was disabled
        return -1;
    adc_T >>= 4;

    var1 = (int32_t)((adc_T / 8) - ((int32_t)_bme280_calib.dig_T1 * 2));
    var1 = (var1 * ((int32_t)_bme280_calib.dig_T2)) / 2048;
    var2 = (int32_t)((adc_T / 16) - ((int32_t)_bme280_calib.dig_T1));
    var2 = (((var2 * var2) / 4096) * ((int32_t)_bme280_calib.dig_T3)) / 16384;

    t_fine = var1 + var2 + t_fine_adjust;

    int32_t T = (t_fine * 5 + 128) / 256;

    return (float)T / 100;
}




/* Read and Write to BME280 Registers */

// Read `len` bytes starting from BME280 register `reg`
// Only return the value in register `reg`
#define MAX_LEN 4
static uint8_t rx[MAX_LEN];

uint8_t read(uint8_t reg, int len) {
    uint8_t tx[len];
    tx[0] = (uint8_t)(reg | 0x80); // RW = '1' for read
    
    spi_transfer(tx, rx, len);
    
    return rx[1]; // Data in register `reg` is second byte
}

uint8_t read8(uint8_t reg) {
    return read(reg, 2);
}

uint16_t read16(uint8_t reg) {
    read(reg, 3);
    return (uint16_t)rx[1] << 8 | (uint16_t)(rx[2]); // MSB first
}

uint32_t read24(uint8_t reg) {
    read(reg, 4);
    return (uint32_t)(rx[1]) << 16 | (uint32_t)(rx[2]) << 8 |
        (uint32_t)(rx[3]); // MSB first
}

uint16_t read16_LE(uint8_t reg) {
    uint16_t temp = read16(reg);
    return (temp >> 8) | (temp << 8); // swap bytes
}

int16_t readS16(uint8_t reg) { return (int16_t)read16(reg); }

int16_t readS16_LE(uint8_t reg) {
    return (int16_t)read16_LE(reg);
}

// Write `value` to BME280 register `reg`
void write8(uint8_t reg, uint8_t value) {
    uint8_t tx[2], rx[2];
    tx[0] = (uint8_t) (reg & ~0x80); // RW = '0' for write
    tx[1] = value;
    spi_transfer(tx, rx, 2);
}
