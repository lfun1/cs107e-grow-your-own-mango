#include "gpio.h"
#include "spi.h"
#include "printf.h"

#define M 0b1000
#define N 0b01
#define PC2_FUNC 0b0010
#define PC3_FUNC 0b0010
#define PC4_FUNC 0b0010
#define PC5_FUNC 0b0010

static unsigned int *SPI1_CLK_REG = (unsigned int *)(CLOCK_CONTROLLER_UNIT_BASE_ADDRSS + SPI1_CLK_REG_OFFSET);
static unsigned int *SPI_BGR_REG = (unsigned int *)(CLOCK_CONTROLLER_UNIT_BASE_ADDRSS + SPI_BGR_REG_OFFSET);
static unsigned int *SPI_TCR_REG = (unsigned int *)(SPI1_BASE_ADDRESS + SPI_TCR_OFFSET);
static unsigned int *SPI_BCC_REG = (unsigned int *)(SPI1_BASE_ADDRESS + SPI_BCC_OFFSET);
static unsigned int *SPI_MTC_REG = (unsigned int *)(SPI1_BASE_ADDRESS + SPI_MTC_OFFSET);
static unsigned int *SPI_MBC_REG = (unsigned int *)(SPI1_BASE_ADDRESS + SPI_MBC_OFFSET);
static unsigned int *SPI_GCR_REG = (unsigned int *)(SPI1_BASE_ADDRESS + SPI_GCR_OFFSET);

void enable_spi_clock(void) {
    // Turn on clock in SPI1_SCLK_GATING
    // Write 1: Clock is ON to SPI1_CLK_REG[31]
	*SPI1_CLK_REG &= ~(1 << 31);
	*SPI1_CLK_REG |= (1 << 31);
}

void config_spi_clock(void) {
	// Clock source select
    // Write 000: HOSC to SPI1_CLK_REG[26:24]
	*SPI1_CLK_REG &= ~(0b111 << 24);
    const unsigned int HOSC = 0b000;
	*SPI1_CLK_REG |= (HOSC << 24);

    // DO NOT TAMPER WITH PLL clock

	printf("Clock value at address %p is: %x\n", (void *)SPI1_CLK_REG, *SPI1_CLK_REG);
}

void de_assert_spi_reset(void) {
    // SPI1_RST at bit 17, write 1: De-assert
    *SPI_BGR_REG &= ~(1 << 17);
    *SPI_BGR_REG |= (1 << 17);

    // Gating Clock for SPI1 at bit 1, write 1: Pass
    *SPI_BGR_REG &= ~(1 << 1);
    *SPI_BGR_REG |= (1 << 1);
}

void config_spi_PINS(void) {
    // SPI0
	// Config SPI0-CLK, GPIO_PC2
    // Config SPI0-CS0, GPIO_PC3
	// Config SPI0-MOSI, GPIO_PC4
	// Config SPI0-MISO, GPIO_PC5

    // SPI1 configure pins
	gpio_set_function(GPIO_PD11, GPIO_FN_ALT4); // SPI1-CLK
	gpio_set_function(GPIO_PD10, GPIO_FN_ALT4); // SPI1-CS
	gpio_set_function(GPIO_PD12, GPIO_FN_ALT4); // SPI1-MOSI
	gpio_set_function(GPIO_PD13, GPIO_FN_ALT4); // SPI1-MISO
}

void config_spi_sample_mode(void) {
	// SPI_TCR[11][13] Set it Delay since the clock has high speed.
	// Set Master Sample Data Control to delay to reduce error and leave SDM on delay
	/* unsigned int val = 1; */
	/* val <<= 11; */
	/* *SPI_TCR_REG = 0xFFFFF7FF; */
	/* *SPI_TCR_REG |= val; */

    // Turn on SDC[11] = 0 (Normal operation), SDM[13] = 1 (Normal sample mode)
    *SPI_TCR_REG &= ~(1 << 11);
    *SPI_TCR_REG |= (1 << 13);
}

void softaware_controller(void) {
	*SPI_TCR_REG &= ~(1 << 6);
	*SPI_TCR_REG |= (1 << 6);
}

void config_clock_mode(void) {
	// SPI_TCR[0][1] Choose Mode 0 [1] = 0, [0] = 0;
    // MCP3008 only supports Mode0, Mode3

    // Set to Mode0 with polarity = 0, phase = 0
    *SPI_TCR_REG &= ~0b11;
    *SPI_TCR_REG |= 0b00; // CPOL[1] = 0, CPHA[0] = 0
}

void config_dummy_counter(void) {
	// Default is zero.
	// unsigned int val = 3;
	// val <<= 24;
	// val |= 0b11;
	*SPI_BCC_REG |= 3;
}

void config_total_trans_len(void) {
	unsigned int val = 3;
	*SPI_MTC_REG |= val;
	*SPI_MBC_REG |= val + val;

}

void config_master(void) {
    // SPI_GCR[1] = 1: Master Mode
	*SPI_GCR_REG |= (1 << 1);
}

void spi_module_enable(void) {
    // SPI_GCR[0] = 1: Enable SPI Module Control
    *SPI_GCR_REG |= 1;    
}

void start_transmit(void) {
	// *SPI_TCR_REG &= ~(1 << 7);
	*SPI_TCR_REG &= 0x7FFFFFFF;
	*SPI_TCR_REG |= 0x80000000; // Start transmit.
	// printf("Clock value at address %p is: %x\n", (void *)SPI1_CLK_REG, *SPI1_CLK_REG);
}

void stop_transmit(void) {
	*SPI_TCR_REG &= ~(1 << 31);
	*SPI_TCR_REG |= (1 << 7);
}


void print_spi_registers(void) {
    // Print contents of all SPI registers
    printf("SPI1_CLK_REG address %p contains: %x\n", (void *)SPI1_CLK_REG, *SPI1_CLK_REG);
    printf("SPI_BGR_REG address %p contains: %x\n", (void *)SPI_BGR_REG, *SPI_BGR_REG);
    printf("SPI_GCR_REG address %p contains: %x\n", (void *)SPI_GCR_REG, *SPI_GCR_REG);
    printf("SPI_TCR_REG address %p contains: %x\n", (void *)SPI_TCR_REG, *SPI_TCR_REG);
    printf("SPI_BCC_REG address %p contains: %x\n", (void *)SPI_BCC_REG, *SPI_BCC_REG);
    printf("SPI_MTC_REG address %p contains: %x\n", (void *)SPI_MTC_REG, *SPI_MTC_REG);
    printf("SPI_MBC_REG address %p contains: %x\n", (void *)SPI_MBC_REG, *SPI_MBC_REG);
}

void hexdump_spi(void) {
    printf("HEXDUMP SPI\n");
    unsigned int *spi_ptr = (unsigned int *)SPI1_BASE_ADDRESS;
    for (int i = 0; i < 20; i++) {
        printf("Address %p has: %x\n", spi_ptr, *spi_ptr);
        spi_ptr++; // add 4
    }
}



// void main(void) {
// 	// Enable SPI clock
// 	config_spi_clock();
// 	// De-assert SPI Reset
// 	de_assert_spi_reset();
// 	// Configure SPI PIN
// 	config_spi_PINS();
// 	// Configure SPI Sample mode
// 	config_spi_sample_mode();
// 	// Configure SPI mode
// 	config_sample_mode();
// 	// Enable SPI control: Already set default 0.
// 	// Configure SPI_TX number and dummy counter
// 	config_dummy_counter();
// 	// Config SPI_TX length and total number of transfers
// 	config_total_trans_len();
// 	// Start Transmit
// 	start_transmit();
// }

