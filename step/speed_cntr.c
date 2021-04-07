/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief Linear speed ramp controller.
 *
 * Stepper motor driver, increment/decrement the position and outputs the
 * correct signals to stepper motor.
 *
 * - File:               speed_cntr.c
 * - Compiler:           avr-gcc
 * - Supported devices:  All devices with a 16 bit timer can be used.
 *                       The example is written for ATmega48
 * - AppNote:            AVR446 - Linear speed control of stepper motor
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support email: avr@atmel.com
 *
 * $Name: RELEASE_1_0 $
 * $Revision: 1.2 $
 * $RCSfile: speed_cntr.c,v $
 * $Date: 2006/05/08 12:25:58 $
 *****************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "global.h"
#include "speed_cntr.h"
#include "../pinmap.h"

//! Cointains data for timer interrupt.
speedRampData srd;

void speed_cntr_Init(){

  LED_DDR |= (1<<LED_PIN);
  LED_PORT &= ~(1<<LED_PIN);

  ASTEP_DDR |= (1<<ASTEP_PIN);
  ASTEP_PORT &= ~(1<<ASTEP_PIN);
  
  ADIR_DDR |= (1<<ADIR_PIN);
  ADIR_PORT &= (1<<ADIR_PIN);
  
  AEN_DDR |= (1<<AEN_PIN);
  AEN_PORT &= (1<<AEN_PIN);
  
}
/*! \brief Move the stepper motor a given number of steps.
 *
 *  Makes the stepper motor move the given number of steps.
 *  It accelerates with given acceleration up to maximum speed and decelerate
 *  with given deceleration so it stops at the given step.
 *  If accel/decel is to small and steps to move is to few, speed might not
 *  reach the max speed limit before deceleration starts.
 *
 *  \param step  Number of steps to move (pos - CW, neg - CCW).
 *  \param accel  Acceleration to use, in 0.01*rad/sec^2.
 *  \param decel  Deceleration to use, in 0.01*rad/sec^2.
 *  \param speed  Max speed, in 0.01*rad/sec.
 */
void speed_cntr_Move(signed int step, unsigned int accel, unsigned int decel, unsigned int speed)
{
  //! Number of steps before we hit max speed.
  unsigned int max_s_lim;
  //! Number of steps before we must start deceleration (if accel does not hit max speed).
  unsigned int accel_lim;

  // if not STOP, do not set up a movement
  if( srd.run_state != STOP ) return;

  // Set direction from sign on step value.
  if(step < 0){
    srd.dir = CCW;
    step = -step;
    ADIR_PORT |= (1<<ADIR_PIN);
  }
  else{
    srd.dir = CW;
    ADIR_PORT &= ~(1<<ADIR_PIN);
  }

  // If moving only 1 step.
  if(step == 1){
    // Move one step...
    srd.accel_count = -1;
    // ...in DECEL state.
    srd.run_state = DECEL;
    // Just a short delay so main() can act on 'running'.
    srd.step_delay = 1000;
    status.running = TRUE;
    
    AEN_PORT   |=  (1<<AEN_PIN);
    
    OCR1A = 10;
    // Run Timer/Counter 1 with prescaler = 8.
    TCCR1B |= ((0<<CS12)|(1<<CS11)|(0<<CS10));
  }
  // Only move if number of steps to move is not zero.
  else if(step != 0){
    // Refer to documentation for detailed information about these calculations.

    // Set max speed limit, by calc min_delay to use in timer.
    // min_delay = (alpha / tt)/ w
    srd.min_delay = A_T_x100 / speed;

    // Set accelration by calc the first (c0) step delay .
    // step_delay = 1/tt * sqrt(2*alpha/accel)
    // step_delay = ( tfreq*0.676/100 )*100 * sqrt( (2*alpha*10000000000) / (accel*100) )/10000
    srd.step_delay = (T1_FREQ_148 * long_sqrt(A_SQ / accel))/100;

    // Find out after how many steps does the speed hit the max speed limit.
    // max_s_lim = speed^2 / (2*alpha*accel)
    max_s_lim = (long)speed*speed/(long)(((long)A_x20000*accel)/100);
    // If we hit max speed limit before 0,5 step it will round to 0.
    // But in practice we need to move atleast 1 step to get any speed at all.
    if(max_s_lim == 0){
      max_s_lim = 1;
    }

    // Find out after how many steps we must start deceleration.
    // n1 = (n1+n2)decel / (accel + decel)
    accel_lim = ((long)step*decel) / (accel+decel);
    // We must accelrate at least 1 step before we can start deceleration.
    if(accel_lim == 0){
      accel_lim = 1;
    }

    // Use the limit we hit first to calc decel.
    if(accel_lim <= max_s_lim){
      srd.decel_val = accel_lim - step;
    }
    else{
      srd.decel_val = -((long)max_s_lim*accel)/decel;
    }
    // We must decelerate at least 1 step to stop.
    if(srd.decel_val == 0){
      srd.decel_val = -1;
    }

    // Find step to start decleration.
    srd.decel_start = step + srd.decel_val;

    // If the maximum speed is so low that we dont need to go via acceleration state.
    if(srd.step_delay <= srd.min_delay){
      srd.step_delay = srd.min_delay;
      srd.run_state = RUN;
    }
    else{
      srd.run_state = ACCEL;
    }

    // Reset counter.
    srd.accel_count = 0;
    status.running = TRUE;
    
    
    AEN_PORT   |=  (1<<AEN_PIN);

    OCR1A = 10;
    // Set Timer/Counter to divide clock by 8
    TCCR1B |= ((0<<CS12)|(1<<CS11)|(0<<CS10));
  }//else steps over 1
}

/*! \brief Init of Timer/Counter1.
 *
 *  Set up Timer/Counter1 to use mode 1 CTC and
 *  enable Output Compare A Match Interrupt.
 */
void speed_cntr_Init_Timer1(void)
{
  // Tells what part of speed ramp we are in.
  srd.run_state = STOP;
  // Timer/Counter 1 in mode 4 CTC (Not running).
  TCCR1B = (1<<WGM12);
  // Timer/Counter 1 Output Compare A Match Interrupt enable.
  TIMSK |= (1<<OCIE1A);
}

/*! \brief Timer/Counter1 Output Compare A Match Interrupt.
 *
 *  Timer/Counter1 Output Compare A Match Interrupt.
 *  Increments/decrements the position of the stepper motor
 *  exept after last position, when it stops.
 *  The \ref step_delay defines the period of this interrupt
 *  and controls the speed of the stepper motor.
 *  A new step delay is calculated to follow wanted speed profile
 *  on basis of accel/decel parameters.
 */
// vector=TIMER1_COMPA_vect
ISR(TIMER1_COMPA_vect)
{
  // Holds next delay period.
  unsigned int new_step_delay = 0;
  // Remember the last step delay used when accelrating.
  static int last_accel_delay = 0;
  // Counting steps when moving.
  static unsigned int step_count = 0;
  // Keep track of remainder from new_step-delay calculation to increase accurancy
  static unsigned int rest = 0;

  OCR1A = srd.step_delay;
  
  if(step_count%2 == 0)
    LED_PORT &= ~(1<<LED_PIN);
  else
    LED_PORT|= (1<<LED_PIN);

  switch(srd.run_state) {
    case STOP:
      step_count = 0;
      rest = 0;
      // Stop Timer/Counter 1.
      TCCR1B &= ~((1<<CS12)|(1<<CS11)|(1<<CS10));
      status.running = FALSE;
      break;

    case ACCEL:
      ASTEP_PORT |=  (1<<ASTEP_PIN);
      //ASTEP_PORT &= ~(1<<ASTEP_PIN);
      step_count++;
      srd.accel_count++;
      new_step_delay = srd.step_delay - (((2 * (long)srd.step_delay) + rest)/(4 * srd.accel_count + 1));
      rest = ((2 * (long)srd.step_delay)+rest)%(4 * srd.accel_count + 1);
      // Check if we should start decelration.
      if(step_count >= srd.decel_start) {
        srd.accel_count = srd.decel_val;
        srd.run_state = DECEL;
      }
      // Check if we reached max speed.
      else if(new_step_delay <= srd.min_delay) {
        last_accel_delay = new_step_delay;
        new_step_delay = srd.min_delay;
        rest = 0;
        srd.run_state = RUN;
      }
      break;

    case RUN:
      ASTEP_PORT |=  (1<<ASTEP_PIN);
      //ASTEP_PORT &= ~(1<<ASTEP_PIN);
      step_count++;
      new_step_delay = srd.min_delay;
      // Check if we should start decelration.
      if(step_count >= srd.decel_start) {
        srd.accel_count = srd.decel_val;
        // Start decelration with same delay as accel ended with.
        new_step_delay = last_accel_delay;
        srd.run_state = DECEL;
      }
      break;

    case DECEL:
      ASTEP_PORT |=  (1<<ASTEP_PIN);
      //ASTEP_PORT &= ~(1<<ASTEP_PIN); 
      step_count++;
      srd.accel_count++;
      new_step_delay = srd.step_delay - (((2 * (long)srd.step_delay) + rest)/(4 * srd.accel_count + 1));
      rest = ((2 * (long)srd.step_delay)+rest)%(4 * srd.accel_count + 1);
      // Check if we at last step
      if(srd.accel_count >= 0){
        srd.run_state = STOP;
      }
      break;
    default:
	  break;
  }
  srd.step_delay = new_step_delay;
  ASTEP_PORT &= ~(1<<ASTEP_PIN);
}

/*! \brief Square root routine.
 *
 * sqrt routine 'grupe', from comp.sys.ibm.pc.programmer
 * Subject: Summary: SQRT(int) algorithm (with profiling)
 *    From: warwick@cs.uq.oz.au (Warwick Allison)
 *    Date: Tue Oct 8 09:16:35 1991
 *
 *  \param x  Value to find square root of.
 *  \return  Square root of x.
 */
unsigned long long_sqrt(unsigned long x)
{
  register unsigned long xr;  // result register
  register unsigned long q2;  // scan-bit register
  register unsigned char f;   // flag (one bit)

  xr = 0;                     // clear result
  q2 = 0x40000000L;           // higest possible result bit
  do
  {
    if((xr + q2) <= x)
    {
      x -= xr + q2;
      f = 1;                  // set flag
    }
    else{
      f = 0;                  // clear flag
    }
    xr >>= 1;
    if(f){
      xr += q2;               // test flag
    }
  } while(q2 >>= 2);          // shift twice
  if(xr < x){
    return xr +1;             // add for rounding
  }
  else{
    return xr;
  }
}

/*! \brief Find minimum value.
 *
 *  Returns the smallest value.
 *
 *  \return  Min(x,y).
 */
unsigned int min(unsigned int x, unsigned int y)
{
  if(x < y){
    return x;
  }
  else{
    return y;
  }
}

unsigned char speed_get_state() 
{

  return srd.run_state;

}