#ifndef BME280_H
#define BME280_H

/* BME280 Library */

#include "spi.h"
#include <stdint.h>
#include <stdbool.h>

/* Init BME280 */
bool init(void);

/* Read and write to BME280 Registers */
uint8_t read(uint8_t reg, int len);
uint8_t read8(uint8_t reg);
void write8(uint8_t reg, uint8_t value);


/* Registers copied from Adafruit BME280 LIbrary */
// https://github.com/adafruit/Adafruit_BME280_Library/blob/9412f3d1a2e334a3415d79df706dad02925488aa/Adafruit_BME280.h

enum {
	BME280_REGISTER_CAL00 = 0x88, // calibration stored in 0x88 - 0xA1
	BME280_REGISTER_CHIPID = 0xD0,
	BME280_REGISTER_SOFTRESET = 0xE0,
	BME280_REGISTER_CAL26 = 0xE1, // R calibration stored in 0xE1-0xF0


	BME280_REGISTER_CONTROLHUMID = 0xF2,
	BME280_REGISTER_STATUS = 0XF3,
	BME280_REGISTER_CONTROL = 0xF4,
	BME280_REGISTER_CONFIG = 0xF5,
	BME280_REGISTER_PRESSUREDATA = 0xF7,
	BME280_REGISTER_TEMPDATA = 0xFA,
	BME280_REGISTER_HUMIDDATA = 0xFD,
};

typedef enum sensor_sampling sensor_sampling_t;

enum sensor_sampling {
    SAMPLING_NONE = 0b000,
    SAMPLING_X1 = 0b001,
    SAMPLING_X2 = 0b010,
    SAMPLING_X4 = 0b011,
    SAMPLING_X8 = 0b100,
    SAMPLING_X16 = 0b101
};

typedef enum sensor_mode sensor_mode_t;

enum sensor_mode {
    MODE_SLEEP = 0b00,
    MODE_FORCED = 0b01,
    MODE_NORMAL = 0b11
};

typedef enum sensor_filter sensor_filter_t;

enum sensor_filter {
    FILTER_OFF = 0b000,
    FILTER_X2 = 0b001,
    FILTER_X4 = 0b010,
    FILTER_X8 = 0b011,
    FILTER_X16 = 0b100
};

typedef enum standby_duration standby_duration_t;

enum standby_duration {
    STANDBY_MS_0_5 = 0b000,
    STANDBY_MS_10 = 0b110,
    STANDBY_MS_20 = 0b111,
    STANDBY_MS_62_5 = 0b001,
    STANDBY_MS_125 = 0b010,
    STANDBY_MS_250 = 0b011,
    STANDBY_MS_500 = 0b100,
    STANDBY_MS_1000 = 0b101
};

void setSampling(sensor_mode_t mode,
                 sensor_sampling_t tempSampling,
                 sensor_sampling_t pressSampling,
                 sensor_sampling_t humSampling,
                 sensor_filter_t filter,
                 standby_duration_t duration);

/* 	  bool init(); */
/* 	  void setSampling(sensor_mode mode = MODE_NORMAL, */
/*                    sensor_sampling tempSampling = SAMPLING_X16, */
/*                    sensor_sampling pressSampling = SAMPLING_X16, */
/*                    sensor_sampling humSampling = SAMPLING_X16, */
/*                    sensor_filter filter = FILTER_OFF, */
/*                    standby_duration duration = STANDBY_MS_0_5); */

/* 	  bool takeForcedMeasurement(void); */
/* 	  float readTemperature(void); */
/* 	  float readPressure(void); */
/* 	  float readHumidity(void); */

/* 	  float getTemperatureCompensation(void); */
/* 	  void setTemperatureCompensation(float); */

/* 	  Adafruit_Sensor *getTemperatureSensor(void); */
/* 	  Adafruit_Sensor *getPressureSensor(void); */
/* 	  Adafruit_Sensor *getHumiditySensor(void); */

/* 	  // Meant to be protected. */
/* 	  SPIDevice *spi_dev = NULL; ///< Pointer to SPI bus interface */
/* 	  BME280_Temp *temp_sensor = NULL; */
/* 	  BME280_Pressure *pressure_sensor = NULL; */
/* 	  BME280_Humidity *humidity_sensor = NULL; */
/* 	  int32_t _sensorID; */
/* 	  int32_t t_fine; */
/* 	  int32_t t_fine_adjust = 0; */
/* 	  bme280_calib_data _bme280_calib; //!< here calibration data is stored */

/* 	  void readCoefficients(void); */
/* 	  bool isReadingCalibration(void); */

/* 	  void write8(byte reg, byte value); */
/* 	  uint8_t read8(byte reg); */
/* 	  uint16_t read16(byte reg); */
/* 	  // uint32_t read24(byte reg); */
/* 	  int16_t readS16(byte reg); */
/* 	  uint16_t read16_LE(byte reg); // little endian */
/* 	  int16_t readS16_LE(byte reg); // little endian */

struct config {
    // inactive duration (standby time) in normal mode
    // 000 = 0.5 ms
    // 001 = 62.5 ms
    // 010 = 125 ms
    // 011 = 250 ms
    // 100 = 500 ms
    // 101 = 1000 ms
    // 110 = 10 ms
    // 111 = 20 ms
    unsigned int t_sb : 3; ///< inactive duration (standby time) in normal mode

    // filter settings
    // 000 = filter off
    // 001 = 2x filter
    // 010 = 4x filter
    // 011 = 8x filter
    // 100 and above = 16x filter
    unsigned int filter : 3; ///< filter settings

    // unused - don't set
    unsigned int none : 1;     ///< unused - don't set
    unsigned int spi3w_en : 1; ///< unused - don't set

    /// @return combined config register
    unsigned int (*get)(struct config *configReg);
    // { return (t_sb << 5) | (filter << 2) | spi3w_en; };
};

/* unsigned int config_get(struct config *configReg) { */
/*     return (configReg->t_sb << 5) | (configReg->filter << 2) | configReg->spi3w_en; */
/* } */

/* struct config _configReg; */


struct ctrl_meas {
    // temperature oversampling
    // 000 = skipped
    // 001 = x1
    // 010 = x2
    // 011 = x4
    // 100 = x8
    // 101 and above = x16
    unsigned int osrs_t : 3; ///< temperature oversampling

    // pressure oversampling
    // 000 = skipped
    // 001 = x1
    // 010 = x2
    // 011 = x4
    // 100 = x8
    // 101 and above = x16
    unsigned int osrs_p : 3; ///< pressure oversampling

    // device mode
    // 00       = sleep
    // 01 or 10 = forced
    // 11       = normal
    unsigned int mode : 2; ///< device mode

    /// @return combined ctrl register
    unsigned int (*get)(struct ctrl_meas *measReg);
    //unsigned int get() { return (osrs_t << 5) | (osrs_p << 2) | mode; }
};

// ctrl_meas _measReg;

struct ctrl_hum {
    /// unused - don't set
    unsigned int none : 5;

    // pressure oversampling
    // 000 = skipped
    // 001 = x1
    // 010 = x2
    // 011 = x4
    // 100 = x8
    // 101 and above = x16
    unsigned int osrs_h : 3; ///< pressure oversampling

    /// @return combined ctrl hum register
    unsigned int (*get)(struct ctrl_hum *humReg);
    //unsigned int get() { return (osrs_h); }
};
//ctrl_hum _humReg;

#endif
