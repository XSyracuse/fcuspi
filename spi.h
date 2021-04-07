
#ifndef __SPI_H_INCLUDED
#define __SPI_H_INCLUDED


#include<stdint.h>

void init_spi();
uint8_t read_spi(uint8_t dout);
void enable_max_CS();
void disable_max_CS();
void change_phase();
void change_speed();
uint16_t thermo_read();
uint8_t thermo_good(uint16_t thermo);

#endif
