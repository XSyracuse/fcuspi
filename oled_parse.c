#include "oled_parse.h"
#include "hexutil.h"

// Ringbuffer logic for buffer 'oledbuf'.
#define BUFSIZE 16

volatile uint8_t oledhead = 0;
volatile uint8_t oledtail = 0;
volatile uint8_t oledbuf[BUFSIZE];

#include "ringbuffer.h"

//parse this 
//* = oledwrite command in [0]
//xx = page
//xx = col
//xx = data
//xx = data
//xx = data
//... max 32

void parse_oledwrite(char *line)
{
  char m = 0;
  char l = 0;
  uint8_t data;

  if(*line =='*') {
  
  line++; 
  m = *line++;
  l = *line++;
  while (m != 0 && l !=0) {

    data = xto8(m,l);
		
	if(buf_canwrite(oled))
      buf_push(oled,data);

    m = *line++;
	l = *line++;

  }

  }//if '*'
}

void oled_prog(){

  uint8_t data;
  uint8_t page = 0;
  uint8_t col = 0;
  uint8_t err = 0;

  if(buf_canread(oled))
    buf_pop(oled,page);
  else
    err++;

  if(buf_canread(oled))
    buf_pop(oled,col);
  else
    err++;

  if(!err) {

    if(buf_canread(oled)){
	  display_font_start(page,col);
	  display_data_start();
	  
	}   
    else
	  err++;
	    
	while(!err){
	   buf_pop(oled,data);
	   display_data_push(data);
	   if(!buf_canread(oled))
	     err++;
	}
	
	display_data_stop();
	  
	  
  }

}
