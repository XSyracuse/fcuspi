#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include "i2cscan.h"


void update_six(uint8_t six)
{

  display_fill(1024,0x00);
  
  if(six & 0x01) {
    display_font_start(1,0);
    display_symbol('F');
    display_symbol('L');
    display_symbol('T');
    display_symbol(' ');
    display_symbol('C');
    display_symbol('O');
    display_symbol('N');
    display_symbol('T');
  }

  if(six & 0x02) {
    display_font_start(4,16);
    display_symbol('I');
    display_symbol('R');
    display_symbol('S');
  }

  if(six & 0x04) {
    display_font_start(7,16);
    display_print_P(PSTR("FUEL"));
  }

  // second col
  if(six & 0x08) {
    display_font_start(1,64);
    display_print_P(PSTR("ELEC"));
  } 

  if(six & 0x10) {
    display_font_start(4,64);
    display_print_P(PSTR("APU"));
  } 

  if(six & 0x20) {
    display_font_start(7,64);
    display_print_P(PSTR("OVHT/DET"));
  } 

}
