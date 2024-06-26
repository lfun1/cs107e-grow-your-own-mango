/* Hall sensor implementation 
 *
 * Lisa Fung
 * 
 * Based on Julie's Hall Sensor code in Sensors Lecture
 */

#include "hall.h"
#include "timer.h"
#include "gpio_extra.h"
#include "uart.h"
#include "printf.h"

#define TICKS_PER_MSEC (TICKS_PER_USEC * 1000)
#define PI 3.1416

static gpio_id_t pin;
static float radius = 3.25; // distance (in inches) from center of anemometer cup to axis of rotation

void hall_init(gpio_id_t input_pin) {
    gpio_init();
    pin = input_pin;
    
    gpio_set_input(pin);
    gpio_set_pullup(pin);
}

static unsigned long get_time_detected() {
    while(gpio_read(pin) == 1) {} // wait for low
    while(gpio_read(pin) == 0) {} // wait for high
    return timer_get_ticks() / TICKS_PER_MSEC;
}

float hall_read_speed(void) {
    unsigned long time_msec_start = get_time_detected();
    unsigned long time_msec_end = get_time_detected();

    unsigned long time_elapsed_ms = time_msec_end - time_msec_start;
    float speed_in_per_ms = 2 * PI * radius / time_elapsed_ms; // inches/millisecond

    // Convert to miles/hour
    const int ms_per_hour = 3600000;
    const int inch_per_mile = 63360;
    float speed_mph = speed_in_per_ms * ms_per_hour / (float)inch_per_mile;
    return speed_mph;
}
