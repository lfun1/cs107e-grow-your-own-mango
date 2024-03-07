#include "gpio.h"
#include "spi.h"

#define M 1
#define N 0b10 // FACTOR_N 4
#define PC2_FUNC 0b0010
#define PC3_FUNC 0b0010
#define PC4_FUNC 0b0010
#define PC5_FUNC 0b0010

static unsigned int *SPI0_CLK_REG = (unsigned int *)(CLOCK_CONTROLLER_UNIT_BASE_ADDRSS + SPI0_CLK_REG_OFFSET);
static unsigned int *SPI_BGR_REG = (unsigned int *)(CLOCK_CONTROLLER_UNIT_BASE_ADDRSS + SPI_BGR_REG_OFFSET);
static unsigned int *SPI_TCR_REG = (unsigned int *)(SPI0_BASE_ADDRESS + SPI_TCR_OFFSET);
static unsigned int *SPI_BCC_REG = (unsigned int *)(SPI0_BASE_ADDRESS + SPI_BCC_OFFSET);
static unsigned int *SPI_MTC_REG = (unsigned int *)(SPI0_BASE_ADDRESS + SPI_MTC_OFFSET);
static unsigned int *SPI_MBC_REG = (unsigned int *)(SPI0_BASE_ADDRESS + SPI_MBC_OFFSET);


void config_spi_clock(void) {
	// 31 Get the SPI0_CLK_GATING first
	unsigned int val = 1;
	// 30:27 empty and create space to write 26:24
	val <<= 7;
	// 26:24 Choose CLK_SRC_SEL. In this case 001: PLL_PERI(1X)
	val |= 0b001;
	// 23:10 empty and create space to write 9:8
	val <<= 16;
	// Put FACTOR_N.
	val |= N;
	// 7:4 empty and create space to write 3:0
	val <<= 8;
	// Put FACTOR_M.
	val |= M;

	// Jus be sure clear out everything in SPI0_CLK_REG.
	*SPI0_CLK_REG &= 0;
	// Write val to config the value.
	*SPI0_CLK_REG |= val;
}

void de_assert_spi_reset(void) {
	unsigned int val = 1;
	// Make sure everything is zero.
	*SPI_BGR_REG &= 0;
	// SPI0_RST at bit 16
	*SPI_BGR_REG = *SPI_BGR_REG | val << 16;
}

void config_spi_PINS(void) {
	// Config SPI0-CLK, GPIO_PC2
	gpio_set_function(GPIO_PC2, PC2_FUNC);
	// Config SPI0-CS0, GPIO_PC3
	gpio_set_function(GPIO_PC3, PC3_FUNC);
	// Config SPI0-MOSI, GPIO_PC4
	gpio_set_function(GPIO_PC4, PC4_FUNC);
	// Config SPI0-MISO, GPIO_PC5
	gpio_set_function(GPIO_PC5, PC5_FUNC);
}

void config_spi_sample_mode(void) {
	// SPI_TCR[11][13] Set it Delay since the clock has high speed.
	// Set Master Sample Data Control to delay to reduce error and leave SDM on delay
	unsigned int val = 1;
	val <<= 11;
	*SPI_TCR_REG = 0xFFFFF7FF;
	*SPI_TCR_REG |= val;
}

void config_sample_mode(void) {
	// SPI_TCR[0][1] Choose Mode 1 [1] = 0, [0] = 1;
	unsigned int val = 1;
	*SPI_TCR_REG = 0xFFFFFFFC;
	*SPI_TCR_REG |= val;
}

void config_dummy_counter(void) {
	// Default is zero.
	unsigned int val = 3;
	val <<= 24;
	val |= 0b11;
	*SPI_BCC_REG |= val;
}

void config_total_trans_len(void) {
	unsigned int val = 3;
	*SPI_MTC_REG |= val;
	*SPI_MBC_REG |= val;
}

void start_transmit(void) {
	*SPI_TCR_REG &= 0x7FFFFFFF;
	*SPI_TCR_REG |= 0x80000000; // Start transmit.
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

