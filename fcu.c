#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/pgmspace.h>

#include "getDecStr.h"
#include "serial/serial.h"
#include "timer0.h"
#include "timer2.h"
#include "avrrotary.h"
#include "relay.h"
#include "spi.h"
#include "i2cscan.h"
#include "step/speed_cntr.h"
#include "six.h"
#include "oled_parse.h"
#include "motor_parse.h"
#include "seven.h"
#include "hexutil.h"
#include "mcp_set.h"
#include "display_motor.h"

void temp_header();
void periodic_temp();

void mcp_display_header();
void periodic_mcp();
void update_mcp();

void pwm_display_header();
void update_pwm();
void periodic_pwm();

//page coordination
void page_sw_handler(bool fast,bool cw);
void page_init();
void page_button_handler(uint8_t button);
void page_cycler();

extern int8_t r0;
extern int8_t r1;
extern int8_t r2;
extern int8_t r3;
extern int8_t r4;
extern int8_t r5;

unsigned long system_time = 0;

struct GLOBAL_FLAGS status;

#define SPEED_GATE_DEFAULT 50

uint16_t speed_gate = SPEED_GATE_DEFAULT;

unsigned long time_r0 = 0;
unsigned long time_r1 = 0;
unsigned long time_r2 = 0;
unsigned long time_r3 = 0;
unsigned long time_r4 = 0;
unsigned long time_r5 = 0;

unsigned long time = 0;

unsigned char pwm2 = 0;

uint8_t six = 0;

static const char PROGMEM donemsg[] = {"done\r\n"};
#define MAXBUF 32
unsigned char lineok = 0;
unsigned char linecnt = 0;
char linebuf[MAXBUF];

char parsemode = 0;

char str[5];

#define PAGE_TEMP   0
#define PAGE_MOTOR  1
#define PAGE_MCP    2
#define PAGE_PWM    3

uint8_t page_current = PAGE_TEMP;
uint8_t page_refresh_count = 0;





int main()
{

  init_usart1();
  timer0_init();
  timer2_init();
  //init_relays();
  init_rotary();
  init_spi();
  sei();

  puts_P(PSTR("BOOT"));

  // display init
  display_fill(1024,0x00);
  display_init();

  speed_cntr_Init();
  speed_cntr_Init_Timer1();

  page_init();


  while(1) {
    cli();
	time = system_time;
	sei();
	 
    uint8_t pb = pollrotary();

    if(r0>0) {
	  if((time-time_r0)<speed_gate)
	    puts("8+");
      else 
	    puts("0+");
      
	  time_r0 = time;
      
	  page_sw_handler(false,true);
     
	}

	else if(r0<0) {
	  if((time-time_r0)<speed_gate)
	    puts("8-");
      else 
	    puts("0-");
    
      time_r0 = time;

      page_sw_handler(false,false);
	  
    }

    if(r1>0) {
	  if((time-time_r1)<speed_gate)
	    puts("9+");
      else 
	    puts("1+");

      time_r1 = time;
	}

	else if(r1<0) {
	  if((time-time_r1)<speed_gate)
	    puts("9-");
      else 
	    puts("1-");

      time_r1 = time;
    }

    if(r2>0) {
	  if((time-time_r2)<speed_gate)
	    puts("A+");
      else 
	    puts("2+");

	  time_r2 = time;
	}

	else if(r2<0) {
	  if((time-time_r2)<speed_gate)
	    puts("A-");
      else 
	    puts("2-");

      time_r2 = time; 
    }

    if(r3>0) {
	  if((time-time_r3)<speed_gate)
	    puts("B+");
      else 
	    puts("3+");

	  time_r3 = time;
	}

	else if(r3<0) {
	  if((time-time_r3)<speed_gate)
	    puts("B-");
      else 
	    puts("3-");

      time_r3 = time; 
    }

    if(r4>0) {
	  if((time-time_r4)<speed_gate)
	    puts("C+");
      else 
	    puts("4+");

	  time_r4 = time;
	}

	else if(r4<0) {
	  if((time-time_r4)<speed_gate)
	    puts("C-");
      else 
	    puts("4-");

      time_r4 = time; 
    }

    if(r5>0) {
	  if((time-time_r5)<speed_gate)
	    puts("D+");
      else 
	    puts("5+");

	  time_r5 = time;
	}

	else if(r5<0) {
	  if((time-time_r5)<speed_gate)
	    puts("D-");
      else 
	    puts("5-");

      time_r5 = time; 
    }


	if(pb & 0x01) {
	  puts("0B");
	  page_button_handler(0);
    }
	if(pb & 0x02) {
	  puts("1B");
	  page_button_handler(1);
    }
	if(pb & 0x04) puts("2B");

    if(pb & 0x08) {
	  puts("3B");
    }
	if(pb & 0x10) {
	  puts("4B");
    }
	if(pb & 0x20) {
	  puts("5B");
    }
	if(pb & 0x40) {
	  puts("6B");
    }
	char c;
    char ok = __getc(&c);
    
	//buffering to get a line
    if(linecnt<(MAXBUF-1)) {
	  if(ok==1) {
	    if(c==0x0a || c==0x0d){
	      lineok = 1;
		  linebuf[linecnt] = 0;
		  linecnt = 0;
        }
	    else {
	      linebuf[linecnt] = c;
          linecnt++;
		  lineok=0;
	    }
      }
	}
	else {
	  linecnt = 0;
	  lineok = 1;
    }

	//see if a line is buffered
	if(lineok==1) {

	  lineok=0;
      puts(linebuf);
	  if(*linebuf=='*') {
	    parse_oledwrite(linebuf);
	    oled_prog();
      }
	  else if(*linebuf=='>') {
        parse_motor(linebuf);
	  }

	}

    //switch to bracket parse mode
	if(ok==1 && c=='['){
      parsemode = 1;
    }
	if(ok==1 && c==']'){
      parsemode = 0;
	}
	if(ok==1 && parsemode==0) {
     
	  switch(c) {
        case 'I':
          speed_gate += 10;
		  break;
        case 'J':
		  speed_gate -= 10;
		  break;
        case 'H':
          speed_gate = SPEED_GATE_DEFAULT;
		  break;
        case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
        case '8':
		case '9':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
          six = six_pack(six,c);
		  update_six(six);
		  break;
        case 'L':
		  i2c_test();
		  break;
        case 'm': 
		  page_current = PAGE_MOTOR;
          page_init();
		  break;
		case 'M':
		  page_current = PAGE_MCP;
		  page_init();
          break;
        case 'N':
		  speed_cntr_Move(-1000, 50, 50, 50);
		  break;
        case 'O':
		  speed_cntr_Move(1000, 50, 50, 50);
		  break;
        case 'Q':
		  display_init();
		  puts_P(donemsg);
		  break;
        case 'Y':
		  display_fill(1024,0x00);
          puts_P(donemsg);
		  break;
        case 'R':
		  display_fill(1024,0xAA);
          puts_P(donemsg);
		  break;
        case 'U':
          
		  
          break;
        case 'V':
		  display_fill(1024,0xFF);
          puts_P(donemsg);
		  break;
        case 'Z':
		  pwm2 += 5;
		  setpwm2(pwm2);
		  break;
        case 'X':
		  pwm2 -= 5;
		  setpwm2(pwm2);
		  break;
        case 'P':
		  change_phase();
		  puts("PH");
		  break;
        case 'S':
		  change_speed();
		  puts("SP");
		  break;
        case 't':
		  page_current = PAGE_TEMP;
		  page_init();
		  break;
        case 'T':
		 
		  break;
		  
        default:
		  break;
	  }

	}

	
    

    if(page_refresh_count>250) {
      page_refresh_count = 0;
      
	  switch(page_current) {
        case PAGE_TEMP:
		  periodic_temp();
		  break;
        case PAGE_MOTOR:
          periodic_motor();
		  break;
        case PAGE_MCP:
		  periodic_mcp();
		  break;
		case PAGE_PWM:
		  periodic_pwm();
          break;	
        default:
		  break;
	  }

	}
	else {

      page_refresh_count++;
	  _delay_ms(1.9);
    }

  }//while forever

  
}

void temp_header()
{
    display_clear;

    display_font_start(0,0);
	display_print_P(PSTR("TEMPERATURE   Celsius"));

}
void periodic_temp(){

  uint32_t thermo = thermo_read();
  unsigned int t = thermo>>5;
		  
  getDecStr(str,4,t);
  puts(str);
          
  if(!thermo_good(thermo)) {

     puts_P(PSTR("OPN"));

     clear_pos(DPOS4);
     clear_pos(DPOS3);
     clear_pos(DPOS2);
     clear_pos(DPOS1);
     clear_pos(DPOS0);

     seven_disp(DPOS4,255);
     seven_disp(DPOS3,255);
     seven_disp(DPOS2,255);
     seven_disp(DPOS1,255);
     seven_disp(DPOS0,255);

  }
  else {
		    
    uint32_t packed = bin2bcd(t);
    
	clear_pos(DPOS1);
	seven_disp(POS1,packed);

    clear_pos(DPOS2);
	seven_disp(POS2,packed>>4);

    clear_pos(DPOS3);
	seven_disp(POS3,packed>>8);

    clear_pos(DPOS4);
	seven_disp(POS4,packed>>12);

    //big degree sign
    seven_disp(POS0,10);
            
  }//else

}



void periodic_mcp()
{

  return;

}
void mcp_display_header()
{

  display_clear;
  display_font_start(0,0);
  display_print_P(PSTR("MCP     ALTITUDE"));
  update_mcp();

}
void update_mcp()
{
  
    uint32_t packed = get_altitude();

    packed = bin2bcd(packed);
   
    clear_pos(DPOS0);
    seven_disp(POS0,packed);

    clear_pos(DPOS1);
    seven_disp(POS1,packed>>4);

    clear_pos(DPOS2);
    seven_disp(POS2,packed>>8);

    clear_pos(DPOS3);
    seven_disp(POS3,packed>>12);

    clear_pos(DPOS4);
    seven_disp(POS4,packed>>16);
  
}

void pwm_display_header()
{
    display_clear();
    display_font_start(0,0);
    display_print_P(PSTR("PWM"));
    update_pwm();
}
void update_pwm()
{
    display_font_start(1,0);
    getDecStr(str,4,pwm2);
    display_print(str);
}
void periodic_pwm()
{
}
void sw_pwm(bool fast,bool cw)
{
    if(cw) pwm2++;
	else pwm2--;
	update_pwm();
}
void page_sw_handler(bool fast,bool cw)
{

  switch(page_current) {

    case PAGE_MCP:
      set_altitude(fast,cw);
	  update_mcp();
	  break;
    case PAGE_MOTOR:
	  motor_sw(fast,cw);
	  break;
	case PAGE_PWM:
	  sw_pwm(fast,cw);
	  break;
    default:
	  break;

  }

}

void page_init()
{

  switch(page_current){

    case PAGE_TEMP:
	  temp_header();
	  break;
    case PAGE_MOTOR:
	  motor_display_header();
	  break;
    case PAGE_MCP:
	  mcp_display_header();
	  break;
    case PAGE_PWM:
	  pwm_display_header();	  
	  break; 
    default:
	  break;
  }

}

void page_button_handler(uint8_t button)
{
  switch(page_current) {
    case PAGE_TEMP:
        page_cycler();
	break;
    case PAGE_MOTOR:

        if(motor_button_handler(button)<0) page_cycler(); 
        break;
		  
    case PAGE_MCP:
	    page_cycler();
	    break;
		  
    case PAGE_PWM:
        page_cycler();
		break;
		  
    default:
	break;
  }
}

void page_cycler()
{
  switch(page_current) {
    case PAGE_TEMP:
	  page_current = PAGE_MCP;
	  break;
    case PAGE_MCP:
	  page_current = PAGE_PWM;
	  break;
    case PAGE_PWM:
      page_current = PAGE_MOTOR;
	  break;
    case PAGE_MOTOR:
	  page_current = PAGE_TEMP;
	  break;
    default:
	  page_current = PAGE_TEMP;
	  break;
  }
  page_init();
}
