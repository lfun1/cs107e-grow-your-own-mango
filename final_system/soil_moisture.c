/* Soil Moisture Sensor Implementation
 *
 * Lisa Fung
 */

#include "gpio.h"
#include "uart.h"
#include "printf.h"
#include "mcp3008.h"

static unsigned int mcp_channel;

void soil_moisture_init(unsigned int channel) {
    mcp3008_init();
    mcp_channel = channel;    
}

// Read soil moisture AOUT through MCP3008 Channel 0
float soil_moisture_read(void) {
    // Values read when completely dry and completely in water
    static const int DRY = 860, WATER = 400;
    
    int moisture = mcp3008_read(mcp_channel);
    //printf("Soil Moisture value read: %04d\n", moisture);
    
    float moisture_percent = 0;
    if (moisture > DRY) {
        moisture_percent = (float)100;
    }
    else if (moisture > WATER) {
        moisture_percent = 100.0 * (DRY - moisture) / (float)(DRY - WATER);
    }
    //printf("Moisture: %02d%%\n", moisture_percent);
    return moisture_percent;
}
