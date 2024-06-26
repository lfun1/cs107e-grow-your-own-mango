/* Simple SPI + MCP3008 ADC test. */

#include "gpio.h"
#include "uart.h"
#include "mcp3008.h"
#include "printf.h"
#include "timer.h"


static void show_pots(void) {
    while (1) {
        printf("Channel[6]: %4d Channel[7]: %4d\n", mcp3008_read(6), mcp3008_read(7));
        timer_delay_ms(250);
    }
}

static void show_joystick(void) {
    while (1) {
        // x and y will be between 0 and 1023
        printf("U/D: %4d, L/R: %4d\n", mcp3008_read(0), mcp3008_read(1));
        timer_delay_ms(250);
    }
}

static void show_on_off(void) {
    while (1) {
        printf("Value read on channel 0: %4d\n", mcp3008_read(0));
        timer_delay_ms(250);
    }
}

static void read_soil_moisture(void) {
    // Soil Moisture Sensor AOUT on MCP3008 Channel 0
    
    // values read when completely dry and completely in water
    static const int DRY = 860, WATER = 400;
    
    while (1) {
        int moisture = mcp3008_read(0);
        printf("Soil Moisture value read: %4d\n", moisture);
        printf("Moisture: %2d%%\n", 100 * (DRY - moisture) / (DRY - WATER));
        timer_delay_ms(250);
    }
}

void main(void)  {
    gpio_init();
    uart_init();
    mcp3008_init();

    //show_pots();
    //show_joystick();
    //show_on_off();
    read_soil_moisture();
}
