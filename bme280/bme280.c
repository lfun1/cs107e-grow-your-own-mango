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

/* Return sensor ID */
uint32_t sensorID(void) { return _sensorID; }

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

/* Read Pressure */

float readPressure(void) {
    int64_t var1, var2, var3, var4;

    readTemperature(); // must be done first to get t_fine

    int32_t adc_P = read24(BME280_REGISTER_PRESSUREDATA);
    if (adc_P == 0x800000) // value in case pressure measurement was disabled
        return -1;
    adc_P >>= 4;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)_bme280_calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)_bme280_calib.dig_P5) * 131072);
    var2 = var2 + (((int64_t)_bme280_calib.dig_P4) * 34359738368);
    var1 = ((var1 * var1 * (int64_t)_bme280_calib.dig_P3) / 256) +
        ((var1 * ((int64_t)_bme280_calib.dig_P2) * 4096));
    var3 = ((int64_t)1) * 140737488355328;
    var1 = (var3 + var1) * ((int64_t)_bme280_calib.dig_P1) / 8589934592;

    if (var1 == 0) {
        return 0; // avoid exception caused by division by zero
    }

    var4 = 1048576 - adc_P;
    var4 = (((var4 * 2147483648) - var2) * 3125) / var1;
    var1 = (((int64_t)_bme280_calib.dig_P9) * (var4 / 8192) * (var4 / 8192)) / 33554432;
    var2 = (((int64_t)_bme280_calib.dig_P8) * var4) / 524288;
    var4 = ((var4 + var1 + var2) / 256) + (((int64_t)_bme280_calib.dig_P7) * 16);

    float P = var4 / 256.0;

    return P;
}

/* Read Humidity */

float readHumidity(void) {
    int32_t var1, var2, var3, var4, var5;

    readTemperature(); // must be done first to get t_fine

    int32_t adc_H = read16(BME280_REGISTER_HUMIDDATA);
    if (adc_H == 0x8000) // value in case humidity measurement was disabled
        return -1;

    var1 = t_fine - ((int32_t)76800);
    var2 = (int32_t)(adc_H * 16384);
    var3 = (int32_t)(((int32_t)_bme280_calib.dig_H4) * 1048576);
    var4 = ((int32_t)_bme280_calib.dig_H5) * var1;
    var5 = (((var2 - var3) - var4) + (int32_t)16384) / 32768;
    var2 = (var1 * ((int32_t)_bme280_calib.dig_H6)) / 1024;
    var3 = (var1 * ((int32_t)_bme280_calib.dig_H3)) / 2048;
    var4 = ((var2 * (var3 + (int32_t)32768)) / 1024) + (int32_t)2097152;
    var2 = ((var4 * ((int32_t)_bme280_calib.dig_H2)) + 8192) / 16384;
    var3 = var5 * var2;
    var4 = ((var3 / 32768) * (var3 / 32768)) / 128;
    var5 = var3 - ((var4 * ((int32_t)_bme280_calib.dig_H1)) / 16);
    var5 = (var5 < 0 ? 0 : var5);
    var5 = (var5 > 419430400 ? 419430400 : var5);
    uint32_t H = (uint32_t)(var5 / 4096);

    return (float)H / 1024.0;
}

/* Forced Measurement */
bool takeForcedMeasurement(void) {
    bool return_value = false;
    // If we are in forced mode, the BME sensor goes back to sleep after each
    // measurement and we need to set it to forced mode once at this point, so
    // it will take the next measurement and then return to sleep again.
    // In normal mode simply does new measurements periodically.
    if (_measReg.mode == MODE_FORCED) {
        return_value = true;
        // set to forced mode, i.e. "take next measurement"
        write8(BME280_REGISTER_CONTROL, _measReg.get(&_measReg));
        // Store current time to measure the timeout
        //uint32_t timeout_start = millis();
        unsigned long timeout_start = timer_get_ticks();
        
        // wait until measurement has been completed, otherwise we would read the
        // the values from the last measurement or the timeout occurred after 2 sec.
        while (read8(BME280_REGISTER_STATUS) & 0x08) {
            // In case of a timeout, stop the while loop
            if ((timer_get_ticks() - timeout_start) > 2000*1000*TICKS_PER_USEC) {
                return_value = false;
                break;
            }
            timer_delay_ms(1);
        }
    }
    return return_value;
}


/* Read Altitude */
// Removed because <cmath> is not supported for `pow` function
/* float readAltitude(float seaLevel) { */
/*     // Equation taken from BMP180 datasheet (page 16): */
/*     //  http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf */

/*     // Note that using the equation from wikipedia can give bad results */
/*     // at high altitude. See this thread for more information: */
/*     //  http://forums.adafruit.com/viewtopic.php?f=22&t=58064 */

/*     float atmospheric = readPressure() / 100.0F; */
/*     return 44330.0 * (1.0 - pow(atmospheric / seaLevel, 0.1903)); */
/* } */


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
