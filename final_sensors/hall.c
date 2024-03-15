/* Hall sensor implementation */

#include "hall.h"
#include "timer.h"
#include "gpio_extra.h"
#include "uart.h"
#include "printf.h"

#define TICKS_PER_MSEC (TICKS_PER_USEC * 1000)
#define PI 3.1416

static gpio_id_t pin;
static float diameter = 9/8; // diameter from magnet to axis of rotation in inches

void hall_init(gpio_id_t input_pin) {
    gpio_init();
    pin = input_pin;
    
    gpio_set_input(pin);
    gpio_set_pullup(pin);
}

static void print_magnet(unsigned int val) {
    printf(val ?  "magnet out of range\n" : "magnet detected\n" );
}

static unsigned long get_time_detected() {
    while(gpio_read(pin) == 1) {} // wait for low
    //print_magnet(0);
    while(gpio_read(pin) == 0) {} // wait for high
    //print_magnet(1);
    return timer_get_ticks() / TICKS_PER_MSEC;
}

float hall_read_speed(void) {
    unsigned long time_msec_start = get_time_detected();
    unsigned long time_msec_end = get_time_detected();

    unsigned long time_elapsed_ms = time_msec_end - time_msec_start;
    float speed_in_per_ms = PI * diameter / time_elapsed_ms; // inches/millisecond

    // Convert to miles/hour
    const int ms_per_hour = 3600000;
    const int inch_per_mile = 63360;
    float speed_mph = speed_in_per_ms * ms_per_hour / (float)inch_per_mile;
    return speed_mph;
}
