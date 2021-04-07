#include<stdint.h>
#include<avr/io.h>
#include <util/delay.h>
#include "spi.h"

#define DDR_SPI    DDRB
#define PORT_SPI   PORTB
#define DD_MOSI    PB5
#define DD_SCK     PB7
#define DD_SS      PB4

#define MAX6675_PORT  PORTE
#define MAX6675_DDR   DDRE
#define MAX6675_CS    PE0

void init_spi()
{
    DDR_SPI = (1<<DD_MOSI) | (1<<DD_SCK) | (1<<DD_SS);
    PORT_SPI |= (1<<DD_SS); 
	//  master mode
	//  dord = 0, msb first 
	//  cpha = 1 sample data on falling edge 
	//  fck=fosc/4
	SPCR  = (1<<CPHA) | (1<<MSTR);
	// enable  
    SPCR |= (1<<SPE);

    //CS is output
    MAX6675_DDR |= (1<<MAX6675_CS);
    disable_max_CS();
}
void change_phase(){
    SPCR  = (0<<CPHA) | (1<<MSTR);
    SPCR |= (1<<SPE);
}
void change_speed(){

    SPCR |= (1<<SPR1) | (1<<SPR0);

}
uint8_t read_spi(uint8_t dout)
{
    SPDR = dout;
	//wait for transmission
	if(SPCR&(1<<MSTR)) {

        while(!(SPSR & (1<<SPIF)) ) {}
	
	    return SPDR;
    }
	else {
        return 0xFF;
	}
}

void enable_max_CS()
{
    MAX6675_PORT &= ~(1<<MAX6675_CS);
}
void disable_max_CS()
{
    MAX6675_PORT |= (1<<MAX6675_CS);
}
// data bits D14-D3 are the temperature
// D2 is normally low and goes high when thermocouple open 
uint16_t thermo_read(){
 
    uint16_t thermo, thermo_h, thermo_l;

    //read 16 bits
    enable_max_CS();
	_delay_ms(1);
    thermo_h = read_spi(0);
	thermo_l = read_spi(0);
	disable_max_CS(); 
         
    thermo = thermo_h<<8;
    thermo += thermo_l;
	return thermo;
		
}
uint8_t thermo_good(uint16_t thermo)
{
    if(thermo & 0x04){
	  return 0;
    } 
	return 1;
}
