#include <stdio.h>
#include <avr/io.h>
#include <string.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>


#include "timer0.h"
#include "serial.h"

#define KERN_ERR "<3>"
#define DEBUG_P printf_P
#define DEBUG 1
#define DEBUG_PRINT_P(fmt, ...) \
          do { if (DEBUG) fprintf_P(stdout, PSTR(fmt), __VA_ARGS__); } while (0)

int main(void)
{
    int i=0;

	timer0_init();
    init_usart1();
    init_pwm3();
    sei();
    
    printf_P(PSTR("Reset: %x\n"), MCUCSR);
    DEBUG_PRINT_P(KERN_ERR"Major explosion: %x\n",PINA);

    while(1)
	{
      i++;
	  printf_P(PSTR("%04x\n"),i);
	}
    
    return 0; 

}
