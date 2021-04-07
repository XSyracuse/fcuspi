#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer2.h"

#include "pinmap.h"

void timer2_init(void)
{
  // set timer interrupt
  // timer frequency is F_CPU
  // prescale set for F_CPU/64
  // when the timer overflows 256 counts later
  // for a F_CPU=16.000MHZ
  // FCPU/64 = 250,000Hz
  // Overflows in 1.024 ms
  
  
  EHDDR |= (1<<EHPIN);  

  TCCR2 = 0x04;         // clock divide by 64 = F_CPU/64
  TCCR2 |= (1<<WGM21) | (1<<WGM20); // fast pwm
  TIFR  |= (1<<TOV2);   // clear overflow flag
  // enable timer 2 overflow interrupt and output compare 2
  TIMSK |= (1<<OCIE2) | (1<<TOIE2);
  return;
}

void setpwm2(uint8_t pwm)
{
  OCR2 = pwm;
}

ISR(TIMER2_OVF_vect)
{
  EHPORT |= (1<<EHPIN);
}

ISR(TIMER2_COMP_vect)
{
  EHPORT &= ~(1<<EHPIN);
}



