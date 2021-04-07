#include <avr/pgmspace.h>
#include "seven.h"
#include "i2cscan.h"



//page/offset/data
static const PROGMEM uint8_t a1[] = {0x02,0x03,0x02,0x07,0x07,
                   0x07,0x07,0x07,0x07,0x07,
                   0x07,0x07,0x07,0x07,0x02,0x00};
//dummy a2
static const PROGMEM uint8_t a2[] = {0x00,0x00,0x00};
                   
static const PROGMEM uint8_t g1[] = {0x03,0x03,0x00,0x80,0x80,
                   0x80,0x80,0x80,0x80,0x80,
                   0x80,0x80,0x80,0x80,0x00};


static const PROGMEM uint8_t g2[] = {0x04,0x03,0x01,0x03,0x03,
                    0x03,0x03,0x03,0x03,0x03,
                    0x03,0x03,0x03,0x03,0x01,0x00};
                    
static const PROGMEM uint8_t d1[] = {0x05,0x03,0x00,0x80,0x80,
                   0x80,0x80,0x80,0x80,0x80,
                   0x80,0x80,0x80,0x80,0x00};
                   
static const PROGMEM uint8_t d2[] = {0x06,0x03,0x01,0x03,0x03,
                    0x03,0x03,0x03,0x03,0x03,
                    0x03,0x03,0x03,0x03,0x01,0x00};
                    
static const PROGMEM uint8_t b1[] = {0x02,0x10,0xFC,0xFE,0xFC,0x00};
static const PROGMEM uint8_t b2[] = {0x03,0x10,0x7F,0xFF,0x7F,0x00};

static const PROGMEM uint8_t c1[] = {0x04,0x10,0xFC,0xFE,0xFC,0x00};
static const PROGMEM uint8_t c2[] = {0x05,0x10,0x7F,0xFF,0x7F,0x00};

static const PROGMEM uint8_t e1[] = {0x04,0x00,0xFC,0xFE,0xFC,0x00};
static const PROGMEM uint8_t e2[] = {0x05,0x00,0x7F,0xFF,0x7F,0x00};

static const PROGMEM uint8_t f1[] = {0x02,0x00,0xFC,0xFE,0xFC,0x00};
static const PROGMEM uint8_t f2[] = {0x03,0x00,0x7F,0xFF,0x7F,0x00};

static const PROGMEM uint8_t dec1[] = {0x05,0x00,0x80,0x80,0x80,0x00};
static const PROGMEM uint8_t dec2[] = {0x06,0x00,0x03,0x03,0x03,0x00};
/*               
pos from constants above 
*/
void seven_snd_seg(const uint8_t *seg, uint8_t pos_off);

void seven_snd_seg(const uint8_t *seg, uint8_t pos_off)
{

  uint8_t page = pgm_read_byte(seg++);
  uint8_t c    = pgm_read_byte(seg++) + pos_off;
  uint8_t data;
    
  display_font_start(page,c);
  display_data_start();
  
  while((data=pgm_read_byte(seg++))!=0){
    display_data_push(data);
  }

  display_data_stop();

}

void seven_disp(uint8_t pos_ord,uint8_t v)
{
  v &= 0x0f;
  switch(v) {
    case 0:
	  seven_snd_seg(a1,pos_ord);
	  seven_snd_seg(b1,pos_ord);
      seven_snd_seg(b2,pos_ord);
	  seven_snd_seg(c1,pos_ord);
      seven_snd_seg(c2,pos_ord); 
      seven_snd_seg(d1,pos_ord);
      seven_snd_seg(d2,pos_ord);
	  seven_snd_seg(e1,pos_ord);
      seven_snd_seg(e2,pos_ord);
      seven_snd_seg(f1,pos_ord);
      seven_snd_seg(f2,pos_ord);
	  break;
    case 1:
	  seven_snd_seg(b1,pos_ord);
      seven_snd_seg(b2,pos_ord);
	  seven_snd_seg(c1,pos_ord);
      seven_snd_seg(c2,pos_ord); 
	  break;
    case 2:
	  seven_snd_seg(a1,pos_ord);
	  seven_snd_seg(b1,pos_ord);
      seven_snd_seg(b2,pos_ord);
	  seven_snd_seg(g1,pos_ord);
      seven_snd_seg(g2,pos_ord); 
      seven_snd_seg(e1,pos_ord);
      seven_snd_seg(e2,pos_ord);
	  seven_snd_seg(d1,pos_ord);
      seven_snd_seg(d2,pos_ord);
      break;
    case 3:
	  seven_snd_seg(a1,pos_ord);
	  seven_snd_seg(b1,pos_ord);
      seven_snd_seg(b2,pos_ord);
	  seven_snd_seg(g1,pos_ord);
      seven_snd_seg(g2,pos_ord);
	  seven_snd_seg(c1,pos_ord);
      seven_snd_seg(c2,pos_ord); 
      seven_snd_seg(d1,pos_ord);
      seven_snd_seg(d2,pos_ord);
      break;
    case 4:
	  seven_snd_seg(b1,pos_ord);
      seven_snd_seg(b2,pos_ord);
	  seven_snd_seg(f1,pos_ord);
      seven_snd_seg(f2,pos_ord);
	  seven_snd_seg(g1,pos_ord);
      seven_snd_seg(g2,pos_ord);
	  seven_snd_seg(c1,pos_ord);
      seven_snd_seg(c2,pos_ord); 
      break;
    case 5:
	  seven_snd_seg(a1,pos_ord);
	  seven_snd_seg(f1,pos_ord);
      seven_snd_seg(f2,pos_ord);
	  seven_snd_seg(g1,pos_ord);
      seven_snd_seg(g2,pos_ord);
	  seven_snd_seg(c1,pos_ord);
      seven_snd_seg(c2,pos_ord);
      seven_snd_seg(d1,pos_ord);
      seven_snd_seg(d2,pos_ord);
      break;
    case 6:
	  seven_snd_seg(a1,pos_ord);
	  seven_snd_seg(f1,pos_ord);
      seven_snd_seg(f2,pos_ord);
	  seven_snd_seg(g1,pos_ord);
      seven_snd_seg(g2,pos_ord);
	  seven_snd_seg(c1,pos_ord);
      seven_snd_seg(c2,pos_ord); 
      seven_snd_seg(d1,pos_ord);
      seven_snd_seg(d2,pos_ord);
	  seven_snd_seg(e1,pos_ord);
      seven_snd_seg(e2,pos_ord);
      break;
    case 7:
	  seven_snd_seg(a1,pos_ord);
	  seven_snd_seg(b1,pos_ord);
      seven_snd_seg(b2,pos_ord);
	  seven_snd_seg(c1,pos_ord);
      seven_snd_seg(c2,pos_ord); 
      break;
    case 8:
	  seven_snd_seg(a1,pos_ord);
      seven_snd_seg(f1,pos_ord);
      seven_snd_seg(f2,pos_ord);
	  seven_snd_seg(b1,pos_ord);
      seven_snd_seg(b2,pos_ord);
      seven_snd_seg(g1,pos_ord);
      seven_snd_seg(g2,pos_ord);
	  seven_snd_seg(c1,pos_ord);
      seven_snd_seg(c2,pos_ord);
	  seven_snd_seg(e1,pos_ord);
      seven_snd_seg(e2,pos_ord);
      seven_snd_seg(d1,pos_ord);
      seven_snd_seg(d2,pos_ord); 
	  break;
    case 9:
	  seven_snd_seg(a1,pos_ord);
      seven_snd_seg(f1,pos_ord);
      seven_snd_seg(f2,pos_ord);
	  seven_snd_seg(b1,pos_ord);
      seven_snd_seg(b2,pos_ord);
      seven_snd_seg(g1,pos_ord);
      seven_snd_seg(g2,pos_ord);
	  seven_snd_seg(c1,pos_ord);
      seven_snd_seg(c2,pos_ord); 
	  break;
    case 15:
	  seven_snd_seg(g1,pos_ord);
      seven_snd_seg(g2,pos_ord);
	  break;
	default:
	  seven_snd_seg(a1,pos_ord);
      seven_snd_seg(f1,pos_ord);
      seven_snd_seg(f2,pos_ord);
	  seven_snd_seg(b1,pos_ord);
      seven_snd_seg(b2,pos_ord);
      seven_snd_seg(g1,pos_ord);
      seven_snd_seg(g2,pos_ord);
	  break;
  }

}

void clear_pos(uint8_t pos)
{

  for(uint8_t page=2;page<7;page++) {

    display_font_start(page,pos);
    display_data_start();

    for(uint8_t i=0;i<BLANK_SWATH;i++) {
      display_data_push(0x00);
    }

    display_data_stop();
  }

}
