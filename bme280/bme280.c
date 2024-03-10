/* 
 * BME280 Library Implementations
 * Lisa Fung, Elias Chikwanda
 * March 10, 2024 
 */

#include "bme280.h"
#include "spi.h"

uint8_t read8(uint8_t reg) {
    uint8_t tx[1], rx[1];
    tx[0] = (uint8_t)(reg | 0x80); // RW = '1' for read
    spi_transfer(tx, rx, 1);
    return rx[0];
}

void write8(uint8_t reg, uint8_t value) {
    uint8_t tx[2], rx[2];
    tx[0] = (uint8_t) (reg & ~0x80); // RW = '0' for write
    tx[1] = value;
    spi_transfer(tx, rx, 2);
}
