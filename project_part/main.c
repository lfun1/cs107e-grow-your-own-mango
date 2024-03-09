/* File: main.c
 * -------------
 * Sample main program.
 */
#include "printf.h"
#include "uart.h"
#include "spi.h"
#include "timer.h"

static unsigned int *SPI_TXD_REG = (unsigned int *)(SPI1_BASE_ADDRESS + SPI_TXD_OFFSET);
static unsigned int *SPI_RXD_REG = (unsigned int *)(SPI1_BASE_ADDRESS + SPI_RXD_OFFSET);

void main(void) {
    uart_init();
    uart_putstring("Hello, world!\n");
    printf("I am printf, here m%c %s!\n", 'e', "ROAR");

    printf("Initial register values\n");
    print_spi_registers();
    
    // Enable SPI clock
    enable_spi_clock();
    // De-assert SPI Reset
    de_assert_spi_reset();
    // Configure SPI PIN
    config_spi_PINS();
    // Configure SPI clock
    config_spi_clock();
    // Configure SPI Sample mode
    config_spi_sample_mode();
    // Configure SPI mode
    config_sample_mode();
    // Enable SPI control: Already set default 0.
    // Configure SPI data bus mode
    
    // Configure SPI_TX number and dummy counter
    config_dummy_counter();
    // Config SPI_TX length and total number of transfers
    config_total_trans_len();

    printf("Register values after SPI config\n");
    print_spi_registers();
    
    // THE BYTES TO BE SENT

    // First byte, includes start bit 1, 0b0000 0001
    // Second byte, [SGLDIFF][D2][D1][D0]xxxx where x = “Don’t care bits can be 0s”, SGLDIFF always 1.
    // So the channel i can have a second byte of 0b1000 0000
    // Third byte, "Dont care bits" 0b0000 0000

    while(1) {
        // Trying to combine everything. write 0000 0001 | 1000 0000 | 0000 0000
        // Check: MSB vs LSB, how to set TXFIFO
        *SPI_TXD_REG = 0x0180000;
        *SPI_RXD_REG = 1024;
        printf("Value before transimit: %d\n", *SPI_RXD_REG);
        // Start Transmit
        start_transmit();

        // Try to delay it after starting.
        timer_delay_ms(250);

        // Trying the value from the receiver.
        printf("This is the data read: %d\n", *SPI_RXD_REG);
    }
}
