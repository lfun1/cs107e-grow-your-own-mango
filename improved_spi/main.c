/* File: main.c
 * ------------
 */

#include "printf.h"
#include "uart.h"
#include "spi.h"
#include "timer.h"

void main(void) {
    uart_init();
    uart_putstring("Starting main\n");

    spi_init();

    unsigned char tx[3];
    tx[0] = 0x01;
    tx[1] = 0x80;
    tx[2] = 0x00;
    unsigned char rx[3];
    
    spi_transfer(tx, rx, 3);

    for (unsigned i = 0; i < 3; i++) {
        printf("Contents at index %d: %x\n", i, rx[i]);
    }

    unsigned int value = ((rx[1] & 0b11) << 8) + rx[2];
    printf("Value read: %d\n", value);
    
    
}
