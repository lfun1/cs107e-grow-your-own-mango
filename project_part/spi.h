#define CLOCK_CONTROLLER_UNIT_BASE_ADDRSS 0x02001000
#define SPI0_CLK_REG_OFFSET 0x0940
#define SPI1_CLK_REG_OFFSET 0x0944
#define SPI_BGR_REG_OFFSET 0x096C // SPI Bus Gating Reset Register

#define SPI0_BASE_ADDRESS 0x04025000
#define SPI_TCR_OFFSET 0x0008    // SPI Transfer Control Register
#define SPI_BCC_OFFSET 0x0038
#define SPI_MTC_OFFSET 0x0034
#define SPI_MBC_OFFSET 0x0030
#define SPI_TXD_OFFSET 0x0200

void config_spi_clock(void);

void de_assert_spi_reset(void);

void config_spi_PINS(void);

void config_spi_sample_mode(void);

void config_sample_mode(void);

void config_dummy_counter(void);

void config_total_trans_len(void);

void start_transmit(void);