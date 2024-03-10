/* File: main.c
 * -------------
 * Sample main program.
 */
#include "printf.h"
#include "uart.h"
#include "spi.h"
#include "timer.h"
#include "dht11.h"

// Changed to unsigned char * to write/read 1 byte at a time to FIFO
static unsigned char *SPI_TXD_REG = (unsigned char *)(SPI1_BASE_ADDRESS + SPI_TXD_OFFSET);
static unsigned char *SPI_RXD_REG = (unsigned char *)(SPI1_BASE_ADDRESS + SPI_RXD_OFFSET);

static void spi_setup2(void) {
    /* Turn on SPI Peripheral */
    // Configure SPI Clock
    config_spi_clock();
    // Enable SPI clock
    enable_spi_clock();
    // De-assert SPI Reset
    de_assert_spi_reset();
    
    // Configure SPI PIN
    config_spi_PINS();

    /* Set SPI as master */
    config_master();

    // Configure SPI Sample mode
    config_spi_sample_mode();
    
    // Configure SPI mode
    config_clock_mode();
    
    /* Enable SPI Peripheral */
    // After all configuration is done
    spi_module_enable();
}

static void spi_tx_rx_test(void) {
    // Configure SPI data bus mode
    // SPI_BCC[28] is DRM, keep = 0 for RX single-bit mode

    // Configure SPI_TX number and dummy counter
    config_dummy_counter();
    
    // Config SPI_TX length and total number of transfers
    config_total_trans_len();

    // Put contents into TXFIFO
    spi_fifo_status();
    
    *SPI_TXD_REG = 0x1;
    *SPI_TXD_REG = 0x80;
    *SPI_TXD_REG = 0x0; // If 

    spi_fifo_status();

    // Start transmit
    // Turn on XCH
    start_transmit();
}


static void spi_setup1(void) {

    config_master();
    
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
    config_clock_mode();
    // Enable SPI control: Already set default 0.
    // Configure SPI data bus mode
    
    // Configure SPI_TX number and dummy counter
    config_dummy_counter();
    // Config SPI_TX length and total number of transfers
    config_total_trans_len();
    // GCR
    

    // software_controller();
    
    // THE BYTES TO BE SENT

    // First byte, includes start bit 1, 0b0000 0001
    // Second byte, [SGLDIFF][D2][D1][D0]xxxx where x = “Don’t care bits can be 0s”, SGLDIFF always 1.
    // So the channel i can have a second byte of 0b1000 0000
    // Third byte, "Dont care bits" 0b0000 0000

    while(1) {
        // Trying to combine everything. write 0000 0001 | 1000 0000 | 0000 0000
        // Check: MSB vs LSB, how to set TXFIFO
        //*SPI_TXD_REG = 0x0180000;
        *SPI_TXD_REG = 0x01;
        printf("Value before send: %x\n", *SPI_TXD_REG);
        // Start Transmit
        start_transmit();

        // Try to delay it after starting.
        // timer_delay_ms(250);

        // Trying the value from the receiver.
        printf("Data after: %x\n", *SPI_TXD_REG);
    }
}

void main(void) {
    uart_init();
    uart_putstring("Hello, world!\n");
    printf("I am printf, here m%c %s!\n", 'e', "ROAR");

    printf("\nInitial register values\n");
    print_spi_registers();

    dht_init();
    dht_send_starter_sig();
    read_bits();
    
    // //spi_setup1();
    // spi_setup2();

    // // Confirmed default register values correct after SPI setup
    // //hexdump_spi();

    // printf("\nRegister values after SPI config\n");
    // print_spi_registers();


    // // Transmit data
    // spi_tx_rx_test();

    // printf("\nRegister values after Transmit test\n");
    // print_spi_registers();
}
