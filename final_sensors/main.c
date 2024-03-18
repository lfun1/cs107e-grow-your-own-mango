/* Tests for all sensors 
 * 3/17/24
 */

#include "uart.h"
#include "printf.h"

// Sensors
#include "bme280.h"
#include "hall.h"
#include "soil_moisture.h"

#include "gpio.h"
#include "timer.h"

const static gpio_id_t GPIO_MCP = GPIO_PD22;
const static gpio_id_t GPIO_BME = GPIO_PD21;

static void test_bme280(void) {
    bme_init();

    while (1) {
        printf("Temperature read: %d\n", (int)(readTemperature()*100));
        printf("Pressure read: %d\n", (int)(readPressure()));
        printf("Humidity read: %d\n", (int)(readHumidity()));
        timer_delay(1);
    }    
}

static void test_hall(void) {
    hall_init(GPIO_PB3);

    float speed = 0;
    while(1) {
        speed = hall_read_speed();
        printf("Speed: %02d.%02d\n", (int)speed, (int)(100*(speed - (int)speed)));
    }
}

static void test_soil_moisture(void) {
    soil_moisture_init(0);
    
    while (1) {
        gpio_write(GPIO_MCP, 1);
        printf("Soil moisture: %d%%\n", soil_moisture_read());
        gpio_write(GPIO_MCP, 0);
        timer_delay(1);
    }
}

static void test_two_spi(void) {
    gpio_init();
    gpio_set_output(GPIO_MCP);
    gpio_write(GPIO_MCP, 0);
    gpio_set_output(GPIO_BME);
    gpio_write(GPIO_BME, 0);

    bme_init();
    soil_moisture_init(0);

    while (1) {
        gpio_write(GPIO_MCP, 1);
        printf("Soil moisture: %d%%\n", soil_moisture_read());
        gpio_write(GPIO_MCP, 0);

        gpio_write(GPIO_BME, 1);
        float temperature = readTemperature();
        printf("Temperature read: %d.%02d\n", (int)(temperature), (int)(temperature*100) - (int)(temperature) * 100);
        
        printf("Pressure read: %d\n", (int)(readPressure()));
        printf("Humidity read: %d\n", (int)(readHumidity()));
        gpio_write(GPIO_BME, 0);

        timer_delay(1);
    }
}

void main(void)  {
    uart_init();
    uart_putstring("Starting main in final_sensors\n");

    //test_bme280();
    //test_soil_moisture();
    //test_two_spi();
    test_hall();
}
