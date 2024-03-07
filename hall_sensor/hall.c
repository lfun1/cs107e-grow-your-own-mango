/*
 * hall effect sensor 3144.
 *
 * http://www.allegromicro.com/~/media/Files/Datasheets/A3141-2-3-4-Datasheet.ashx?la=en
 *
 * looking down from the top of the pyramid = power, ground, output (vout).
 * 	- vout to gpio2.
 *	- enable pullup so don't need a resistor.
 *
 * http://www.raspberrypi-spy.co.uk/2015/09/how-to-use-a-hall-effect-sensor-with-the-raspberry-pi/
 *
 * The output of these devices (pin 3) switches low when the magnetic
 * field at the Hall element exceeds the operate point threshold (BOP). At
 * this point, the output voltage is VOUT(SAT). When the magnetic field
 * is reduced to below the release point threshold (BRP), the device
 * output goes high. The difference in the magnetic operate and release
 * points is called the hysteresis (Bhys) of the device. This built-in
 * hysteresis allows clean switching of the output even in the presence
 * of external mechanical vibration and electrical noise.
 *
 * Adapted from Julie Zelenski's starter code
 *
 */

#include "gpio.h"
#include "gpio_extra.h"
#include "uart.h"
#include "printf.h"
#include "timer.h"

#define TICKS_PER_MSEC (TICKS_PER_USEC * 1000)
#define PI 3.1416

void print_magnet(unsigned int val) {
    printf(val ?  "magnet out of range\n" : "magnet detected\n" );
}

unsigned long get_time_ms(void) {
    printf("Time in ms: %ld\n", timer_get_ticks() / TICKS_PER_MSEC);
    return timer_get_ticks() / TICKS_PER_MSEC;
}

// radius in inches, time_elapsed in ms
void print_speed(unsigned long time_elapsed_ms, float radius) {
    // Speed = 2*PI*radius / time_elapsed
    float speed_in_ms = 2*PI*radius / time_elapsed_ms; // inch/millisecond
    //printf("Speed in inches/ms: %d\n", (int)speed_in_ms);

    // Convert to miles/hour
    const int ms_per_hour = 3600000;
    const int inch_per_mile = 63360;
    float speed_mph = speed_in_ms * ms_per_hour / (float)inch_per_mile;
    printf("Speed in miles/hour: %d\n", (int)speed_mph);
}

void main(void) {
    // Configure hall effect sensor reading
	const gpio_id_t pin = GPIO_PB3;

    gpio_init();
    uart_init();

  	gpio_set_input(pin);
  	gpio_set_pullup(pin);

    // Radius and time to calculate wind speed
    const float radius = 10; // radius from center of tube to magnet
    unsigned long prev_time_ms = 0;
    unsigned long cur_time_ms = timer_get_ticks() / TICKS_PER_MSEC;
    
    // pin is 1 when the magnet is out of range of the sensor
    print_magnet(1);
    get_time_ms();
	while(1) {
  		while(gpio_read(pin) == 1) {} // wait for low
        // magnet detected
		print_magnet(0);
        get_time_ms();
        
        // Update speed, time
        prev_time_ms = cur_time_ms;
        cur_time_ms = timer_get_ticks() / TICKS_PER_MSEC;
        print_speed(cur_time_ms - prev_time_ms, radius);
        
  		while(gpio_read(pin) == 0) {} // wait for high
		print_magnet(1);
	}
}
