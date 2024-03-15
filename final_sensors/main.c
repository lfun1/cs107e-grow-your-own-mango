/* Rough tests */

/* Simple SPI + MCP3008 ADC test. */

#include "uart.h"
#include "printf.h"

// Sensors
#include "bme280.h"
#include "hall.h"
#include "soil_moisture.h"

void main(void)  {
    uart_init();
    uart_putstring("Starting main in final_sensors\n");

    // Test BME280
    /* spi_init(); */
    /* uint8 read_val = read8(BME280_REGISTER_CHIPID); */

    /* printf("Read value at Chip ID: %x\n", read_val); */

    // Test Hall sensor
    hall_init(GPIO_PB3);

    float speed = 0;
    while(1) {
        speed = hall_read_speed();
        printf("Speed: %02d.%02d\n", (int)speed, (int)(100*(speed - (int)speed)));
    }
}
