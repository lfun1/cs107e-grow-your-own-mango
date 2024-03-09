#define CLOCK_CONTROLLER_UNIT_BASE_ADDRSS 0x02001000
#define SPI0_CLK_REG_OFFSET 0x0940
#define SPI1_CLK_REG_OFFSET 0x0944
#define SPI_BGR_REG_OFFSET 0x096C // SPI Bus Gating Reset Register

#define SPI0_BASE_ADDRESS 0x04025000
#define SPI1_BASE_ADDRESS 0x04026000
#define SPI_TCR_OFFSET 0x0008    // SPI Transfer Control Register
#define SPI_BCC_OFFSET 0x0038    // SPI Master Burst Control Register
#define SPI_MTC_OFFSET 0x0034    // SPI Master Transmit Counter Register
#define SPI_MBC_OFFSET 0x0030    // SPI Master Burst Counter Register
#define SPI_TXD_OFFSET 0x0200    // SPI TX Data register
#define SPI_RXD_OFFSET 0x0300    // SPI RX Data register
#define SPI_GCR_OFFSET 0x0004

void enable_spi_clock(void);

void config_spi_clock(void);

void de_assert_spi_reset(void);

void config_spi_PINS(void);

void config_spi_sample_mode(void);

void config_sample_mode(void);

void config_dummy_counter(void);

void config_total_trans_len(void);

void start_transmit(void);

void print_spi_registers(void);
