#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer0.h"

extern unsigned long system_time;

int timer0_init(void)
{
  // set timer interrupt
  // timer frequency is F_CPU
  // prescale set for F_CPU/64
  // when the timer overflows 256 counts later
  // for a F_CPU=16.000MHZ
  // FCPU/64 = 250,000Hz
  // Overflows in 1.024 ms
  
  
  
  TCCR0 = 0x04;        //clock divide by 64 = F_CPU/64
  TIFR  |= (1<<TOV0);   //clear overflow flag
  TIMSK |= (1<<TOIE0);  //enable timer 0 overflow interrupt
  return 1;
}

ISR(TIMER0_OVF_vect)
{
  system_time++;
}

/*
int timer1_init(void)
{
  // set timer interrupt
  // timer frequency is F_CPU
  // when the timer overflows 256 counts later
  // divides the F_CPU/64 by 256 counts
  // for a F_CPU=16.000
  // or 16us
  
  TCCR1A = 0x00;        //gewoon timer
  TCCR1B = 0x01;        //clock divide by 1 = F_CPU
  TIFR  = (1<<TOV1);    //clear overflow flag
  TIMSK = (1<<TOIE1);   //enable timer 0 overflow interrupt
  return 1;
}  
*/

