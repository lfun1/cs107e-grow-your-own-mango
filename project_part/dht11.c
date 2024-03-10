#include "gpio.h"
#include "spi.h"
#include "printf.h"
#include "dht11.h"
#include "timer.h"



// Data format: 8bit Humidity integer data + 8bit Humidity fraction data + 8bit Temperature integer data + 8bit Temperature fraction data + 8bit Check

// Start
// The microprocessor pulls the data bus (SDA) low for at least 18ms(not more than 30ms),notifying the sensor to prepare the data.


void dht_init(void) {
	// Wait 1S to overturn the unstable state during which can not send any instructions
	timer_delay(1);
	// Set the DHT_DATA as output
	gpio_set_output(DHT_DATA);
	gpio_write(DHT_DATA, HIGH);
	// Just delay for a bit.
	timer_delay_ms(400);
}

void dht_send_starter_sig(void) {
	// set DHT_DATA low, the low hold time can not be less than 18ms (maximum can not exceed 30ms)
	gpio_write(DHT_DATA, LOW);
	unsigned long start_time = timer_get_ticks();
	timer_delay_ms(20);
	// then the microprocessor's I/O is set to input state.
	// set the DHT_DATA high so that the sensor will bring it low.
	if (!gpio_read(DHT_DATA)) {
		uart_putchar('A');
	}
	gpio_write(DHT_DATA, HIGH);
	gpio_set_input(DHT_DATA);
	if (!gpio_read(DHT_DATA)) {
		uart_putchar('B');
	}
	// wait for the sensor to pull the data low.
	int count = 0;
	while (gpio_read(DHT_DATA)) {
		// if (timer_get_ticks() - start_time > 40*1000*TICKS_PER_USEC) {
		// 	break;
		// }
	}
	printf("Out of the loop\n");
}

void read_bits(void) {
	for (int i = 0; i <= DHT_TOTAL_BITS; i++) {
		if (gpio_read(DHT_DATA)) {
			uart_putchar('1');
			//uart_putchar('\n');
		}
		else {
			uart_putchar('0');
			//uart_putchar('\n');
		}
	}
	uart_putchar('\n');
}

