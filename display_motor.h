#ifndef __DISPLAY_MOTOR_H__
#define __DISPLAY_MOTOR_H__

#include "stdbool.h"
#include "stdint.h"

void periodic_motor();
void motor_display_header();
void motor_sw(bool fast,bool cw);
int8_t motor_button_handler(uint8_t button);
void motor_update();

#endif
