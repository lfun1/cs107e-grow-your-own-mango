/* File: demo_wind.c
 * -------------
 * Demo program of wind speed readings from anemometer.
 * Lisa Fung
 */

#include "printf.h"
#include "uart.h"

// Sensors
#include "bme280.h"
#include "hall.h"
#include "soil_moisture.h"

#include "gpio.h"
#include "timer.h"

#include "dashboard.h"

const static gpio_id_t GPIO_MCP = GPIO_PD22;
const static gpio_id_t GPIO_VALVE = GPIO_PD17;

void main(void) {
    uart_init();
    uart_putstring("Starting demo!");

    // Show dashboard
    dashboard_init(2,3,GL_SILVER, GL_MOSS);
    data_graph_init();
    dashboard_draw_outline();
    dashboard_show(70, 70, 50, 2.5, 101325);

    // Init all sensors
    bme_init();
    soil_moisture_init(0);
    hall_init(GPIO_PB3);

    // Init transistor switch for soil moisture chip select
    gpio_init();
    gpio_set_output(GPIO_MCP);
    gpio_write(GPIO_MCP, 0);
    
    float d_temp, d_hum, d_soil_mois, d_wind_speed, d_pressure;

    // Config GPIO_VALVE
    gpio_set_output(GPIO_VALVE);
    gpio_write(GPIO_VALVE, 0); // keep valve closed

    while (1) {
        d_temp = readTemperature() * 1.8 + 32;
        d_hum = readHumidity();
        d_pressure = readPressure();
        gpio_write(GPIO_MCP, 1);
        d_soil_mois = soil_moisture_read();
        gpio_write(GPIO_MCP, 0);
        d_wind_speed = hall_read_speed(); // only progresses if anemometer is spinning
        dashboard_draw_outline();
        dashboard_show(d_temp, d_hum, d_soil_mois, d_wind_speed, d_pressure);

        // Print all values
        printf("Temperature: %d\n", (int)d_temp);
        printf("Humidity: %d\n", (int)d_hum);
        printf("Pressure: %d\n", (int)d_pressure);
        printf("Soil moisture: %d%%\n", (int)d_soil_mois);
        printf("Wind Speed: %02d.%02d\n", (int)d_wind_speed, (int)(100*(d_wind_speed - (int)d_wind_speed)));
        
        timer_delay(1);
    }
}
