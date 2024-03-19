#ifndef HALL_H
#define HALL_H

#include "gpio.h"

void hall_init(gpio_id_t input_pin);
float hall_read_speed(void);

#endif
