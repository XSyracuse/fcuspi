//display_motor.c

#include<avr/pgmspace.h>

#include "display_motor.h"
#include "step/speed_cntr.h"
#include "i2cscan.h"
#include "getDecStr.h"

#define HEAD_ROW  0
#define WARN_ROW  1

#define STATE_ROW 2
#define STEPS_ROW 3
#define ACCEL_ROW 4
#define DECEL_ROW 5
#define SPEED_ROW 6
#define EXIT_ROW  7

#define CURSOR_COL 128-10
#define CURSOR "<-"
#define SHIFTCURSOR "<+"
//each page has a:
//1.  page header of static elements
//2.  periodic page drawing
//3.  rotary switch handler
//4.  button handler

static int16_t motor_steps = 1000;
static uint16_t accel = 80;
static uint16_t decel = 80;
static uint16_t speed = 200;

static char str[6];

static uint8_t cursor_row = STEPS_ROW;
static uint8_t o_cursor_row = STEPS_ROW;

static bool shift = false;


void motor_display_header()
{

    display_clear;
    display_font_start(HEAD_ROW,0);
    display_print_P(PSTR("STEPPER MOTOR"));

    display_font_start(STATE_ROW,0);
    display_print_P(PSTR("STATE"));

	display_font_start(STEPS_ROW,0);
	display_print_P(PSTR("STEPS"));

	display_font_start(ACCEL_ROW,0);
	display_print_P(PSTR("XCEL"));

    display_font_start(DECEL_ROW,0);
	display_print_P(PSTR("DCEL"));

    display_font_start(SPEED_ROW,0);
	display_print_P(PSTR("SPEED"));

    display_font_start(EXIT_ROW,0);
	display_print_P(PSTR("EXIT"));

	motor_update();
}

void periodic_motor()
{
  uint8_t state = speed_get_state();

  //display a moving warning
  display_font_start(WARN_ROW,64);

  if(state != STOP){
    display_print_P(PSTR("MOVING"));
  }
  else {
    display_print_P(PSTR("        ")); 
  }

  // display some states
  display_font_start(STATE_ROW,64);
  switch(state){
    case STOP:
	  display_print_P(PSTR("STOP"));	  
      break;
    case ACCEL:
	  display_print_P(PSTR("XCEL")); 
	  break;
    case DECEL:
	  display_print_P(PSTR("DCEL")); 
	  break;	  
    default:
	  display_print_P(PSTR("RUN ")); 
	  break;
  }

}


void motor_set_steps(bool fast, bool cw)
{

  int16_t increment;
   
  if(fast) increment = 1000;
  else increment = 100;

  if(cw) motor_steps += increment;
  else   motor_steps -= increment;

}

void motor_set(bool fast, bool cw, uint16_t *ptr)
{

  int16_t increment;
   
  if(fast) increment = 100;
  else increment = 10;

  if(cw) *ptr += increment;
  else   *ptr -= increment;

}

void motor_update()
{
  uint16_t abs;

  // blank out
  display_font_start(STEPS_ROW,64);
  display_print_P(PSTR("      ")); //6 spaces

  // display readout
  display_font_start(STEPS_ROW,64);

  if(motor_steps < 0) {
    abs = -motor_steps;
	display_symbol('-');
  }
  else {
    abs = motor_steps;
	display_symbol(' ');
  }

  getDecStr(str,5,abs);
  display_print(str);

  //accel
  getDecStr(str,5,accel);
  display_font_start(ACCEL_ROW,64);
  display_print(str);

  //decel
  getDecStr(str,5,decel);
  display_font_start(DECEL_ROW,64);
  display_print(str);

  //speed
  getDecStr(str,5,speed);
  display_font_start(SPEED_ROW,64);
  display_print(str);

  //blank old cursor
  display_font_start(o_cursor_row,CURSOR_COL);
  display_print_P(PSTR("  "));
  //cursor
  display_font_start(cursor_row,CURSOR_COL);
  if(shift) {
	display_print_P(PSTR(SHIFTCURSOR));
  }
  else {
    display_print_P(PSTR(CURSOR));
  }

}

int16_t motor_get_steps()
{
  return motor_steps;
}

//rotary switch
void motor_sw(bool fast,bool cw)
{
  // in shift mode change values
  // otherwise move cursor
  if(shift){
    switch(cursor_row) {
      case STEPS_ROW:
        motor_set_steps(fast,cw);
        break;
      case ACCEL_ROW:
	    motor_set(fast,cw,&accel);
		break;
      case DECEL_ROW:
	    motor_set(fast,cw,&decel);
	    break;
      case SPEED_ROW:
	    motor_set(fast,cw,&speed);
		break;
	  default:
	    break;	   
    }//switch

  }// if 
  else {
	o_cursor_row = cursor_row;
	if(cw){
	  if(cursor_row<EXIT_ROW) cursor_row++;
	}
	else { 
	  if(cursor_row>STEPS_ROW) cursor_row--;
    }
  }

  //update display
  motor_update();
}

int8_t motor_button_handler(uint8_t button)
{
  //entry button
  if(button==0){
    if(cursor_row==EXIT_ROW) return -1;
	else shift = !shift;
  }
  else if(button==1) {
    speed_cntr_Move(motor_steps, accel, decel, speed);
  }
  motor_update();
  return 1;
}
