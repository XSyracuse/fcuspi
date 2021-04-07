#include <string.h>
#include "hexutil.h"
#include "step/speed_cntr.h"

//  steps/accel/deccel/speed
 
//  parse_motor
//  steps  -> 4 chars in text hexadecimal - signed 
//  accel  -> 4 chars in text - unsigned 
//  deccel -> 4 chars in text - unsigned 
//  speed  -> 4 chars in text - unsigned

void parse_motor(char *line) {         
    
  char m,l;
  int16_t  steps;
  uint16_t accel;
  uint16_t deccel;
  uint16_t speed;

  if(*line=='>' && strlen(line)>16) {

    line++;

	m = *line++;
	l = *line++;
    steps = xto8(m,l) << 8;
	
	m = *line++;
    l = *line++;
	steps |= xto8(m,l);
	
	/***********/

	m = *line++;
	l = *line++;
    accel = xto8(m,l) << 8;
	
	m = *line++;
    l = *line++;
	accel |= xto8(m,l);
	
    /**********/	
	
	m = *line++;
	l = *line++;
    deccel = xto8(m,l) << 8;
	
	m = *line++;
    l = *line++;
	deccel |= xto8(m,l);
	
    /**********/	

	m = *line++;
	l = *line++;
    speed = xto8(m,l) << 8;
	
	m = *line++;
    l = *line++;
	speed |= xto8(m,l);
		
	speed_cntr_Move(steps, accel, deccel, speed);
  
  }//line == '>'

  

}
