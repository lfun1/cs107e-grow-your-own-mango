#ifndef SOIL_MOISTURE_H
#define SOIL_MOISTURE_H

#include "mcp3008.h"

void soil_moisture_init( unsigned int channel );
float soil_moisture_read(void); // Read soil moisture AOUT through MCP3008 Channel 0

#endif
