/* File: main.c
 * -------------
 * Sample main program.
 */
#include "printf.h"
#include "uart.h"
#include "spi.h"

void main(void) {
    uart_init();
    uart_putstring("Hello, world!\n");
    printf("I am printf, here m%c %s!\n", 'e', "ROAR");
     // Enable SPI clock
     config_spi_clock();
     // De-assert SPI Reset
     de_assert_spi_reset();
     // Configure SPI PIN
     config_spi_PINS();
     // Configure SPI Sample mode
     config_spi_sample_mode();
     // Configure SPI mode
     config_sample_mode();
     // Enable SPI control: Already set default 0.
     // Configure SPI_TX number and dummy counter
     config_dummy_counter();
     // Config SPI_TX length and total number of transfers
     config_total_trans_len();

     // THE BYTES TO BE SENT

     // First byte, includes start bit 1, 0b0000 0001
     // Second byte,  [SGLDIFF][D2][D1][D0]xxxx where x = “Don’t care bits can be 0s”,  SGLDIFF always 1.
     // So the channel i can have a second byte of 0b1000 0000
     // Third byte, "Dont care bits" 0b0000 0000

     // 

     // Chanel zero = 0b1000 000 = 0x80
     


     // Start Transmit
     start_transmit();


}
