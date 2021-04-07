#include<stdint.h>
#include<avr/io.h>

#define FILTERCOUNT 4
/*

PA0 = A0
PA1 = B0
PA2 = A1
PA3 = B1
PA4 = A2
PA5 = B2
PA6 = A3
PA7 = B3

PC4 = A4
PC5 = B4
PC6 = A5
PC7 = B5

Port   mapped to PB(pushbutton)
PC0 = PB1
PC1 = PB2
PC2 = PB3
PC3 = Recall
PE0 = CautionPush
PE1 = Warning Push

note: PB0 and PB4 not connected

Encoders (supplied) when at detent are always open (11)
11->01->00->10
11->10->00->01
Use transition from 
11->01 and 11->10

*/

uint8_t g0 = 0;
uint8_t g1 = 0;
uint8_t g2 = 0;
uint8_t g3 = 0;
uint8_t g4 = 0;
uint8_t g5 = 0;

uint8_t v0 = 0;
uint8_t v1 = 0;
uint8_t v2 = 0;
uint8_t v3 = 0;
uint8_t v4 = 0;
uint8_t v5 = 0;

int8_t r0 = 0;
int8_t r1 = 0;
int8_t r2 = 0;
int8_t r3 = 0;
int8_t r4 = 0;
int8_t r5 = 0;

uint8_t db0 = 0;
uint8_t db1 = 0;
uint8_t db2 = 0;
uint8_t db3 = 0;
uint8_t db4 = 0;
uint8_t db5 = 0;

uint8_t c0 = 0;
uint8_t c1 = 0;
uint8_t c2 = 0;
uint8_t c3 = 0;
uint8_t c4 = 0;
uint8_t c5 = 0;

uint8_t pb_sample0 = 0;
uint8_t pb_sample1 = 0;
uint8_t pb_sample2 = 0;
uint8_t pb_sample3 = 0;

uint8_t pb_sample4 = 0;
uint8_t pb_sample5 = 0;
uint8_t pb_sample6 = 0;

uint8_t pb_out_n1 = 0xff;
uint8_t pb_out    = 0xff;

static inline void readgrey();
static inline int8_t pollpush();

void init_rotary()
{

  // four rotary greycode input
  DDRA = 0x00;
  // pull up
  PORTA = 0xff;
  
  
  //pushbutton bits
  DDRC = 0x00;
  //pulled up
  PORTC = 0xff;
  
  //more pushbuttons
  DDRE = 0x00;
  //pulled up
  PORTE = 0xff;

  //init to current position
  readgrey();
}

//debounce logic
static inline void readgrey()
{

  uint8_t sample = PINA;
  uint8_t sample1 = PINC;

  uint8_t s0 = sample & 0x03;
  uint8_t s1 = (sample>>2) & 0x03;
  uint8_t s2 = (sample>>4) & 0x03;
  uint8_t s3 = (sample>>6) & 0x03;
  
  uint8_t s4 = (sample1>>4) & 0x03;
  uint8_t s5 = (sample1>>6) & 0x03;

  if(s0 != db0) {
      db0 = s0;
      c0  = 0;  
  }
  else {
      c0++;
  }
  if(c0==FILTERCOUNT) g0 = db0;


  if(s1 != db1) {
      db1 = s1;
      c1  = 0;  
  }
  else {
      c1++;
  }
  if(c1==FILTERCOUNT) g1 = db1;


  if(s2 != db2) {
      db2 = s2;
      c2  = 0;  
  }
  else {
      c2++;
  }
  if(c2==FILTERCOUNT) g2 = db2;


  if(s3 != db3) {
      db3 = s3;
      c3  = 0;  
  }
  else {
      c3++;
  }
  if(c3==FILTERCOUNT) g3 = db3;


  if(s4 != db4) {
      db4 = s4;
      c4  = 0;  
  }
  else {
      c4++;
  }
  if(c4==FILTERCOUNT) g4 = db4;


  if(s5 != db5) {
      db5 = s5;
      c5  = 0;  
  }
  else {
      c5++;
  }
  if(c5==FILTERCOUNT) g5 = db5;

}

static inline int8_t processgrey(uint8_t *g,uint8_t*v)
{

  int8_t r = 0;
  
  if(*v != *g) {
    if((*v==3 && *g==2))
      r = 1;

    else if((*v==3 && *g==1))
      r = -1;

    //update  
    *v = *g;
  }    

  return r;
  
}

static inline int8_t pollpush()
{
  
  uint8_t pb_in = PINC & 0x0f;
  
  //filter bit 0
  if( (pb_in & 0x01) != (pb_out & 0x01) ) {

      if(pb_sample0 == 16) {
	      pb_out = (pb_out & ~0x01) | (pb_in & 0x01);
	      pb_sample0 = 0;
	  }
	  else pb_sample0++;
      
  }
  else pb_sample0 = 0;

  //filter bit 1
  if( (pb_in & 0x02) != (pb_out & 0x02) ) {

      if(pb_sample1 == 16) {
	      pb_out = (pb_out & ~0x02) | (pb_in & 0x02);
	      pb_sample1 = 0;
	  }
	  else pb_sample1++;
      
  }
  else pb_sample1 = 0;

  //filter bit 2
  if( (pb_in & 0x04) != (pb_out & 0x04) ) {

      if(pb_sample2 == 16) {
	      pb_out = (pb_out & ~0x04) | (pb_in & 0x04);
	      pb_sample2 = 0;
	  }
	  else pb_sample2++;
      
  }
  else pb_sample2 = 0;

  //filter bit 3
  if( (pb_in & 0x08) != (pb_out & 0x08) ) {

      if(pb_sample3 == 16) {
	      pb_out = (pb_out & ~0x08) | (pb_in & 0x08);
	      pb_sample3 = 0;
	  }
	  else pb_sample3++;
      
  }
  else pb_sample3 = 0;
  
  /***************************/
  /* bits 4 and 5 from PINE */
  /***************************/
  pb_in = PINE << 4;

  //filter bit 4
  if( (pb_in & 0x10) != (pb_out & 0x10) ) {

      if(pb_sample4 == 16) {
	      pb_out = (pb_out & ~0x10) | (pb_in & 0x10);
	      pb_sample4 = 0;
	  }
	  else pb_sample4++;
      
  }
  else pb_sample4 = 0;

  //filter bit 5
  if( (pb_in & 0x20) != (pb_out & 0x20) ) {

      if(pb_sample5 == 16) {
	      pb_out = (pb_out & ~0x20) | (pb_in & 0x20);
	      pb_sample5 = 0;
	  }
	  else pb_sample5++;
      
  }
  else pb_sample5 = 0;
  
  //filter bit 6
  if( (pb_in & 0x40) != (pb_out & 0x40) ) {

      if(pb_sample6 == 16) {
	      pb_out = (pb_out & ~0x40) | (pb_in & 0x40);
	      pb_sample6 = 0;
	  }
	  else pb_sample6++;
      
  }
  else pb_sample6 = 0;

  uint8_t trans_hi_to_lo = ~pb_out & pb_out_n1;
  pb_out_n1 = pb_out;

  return trans_hi_to_lo;

}

int8_t pollrotary() 
{
  
  readgrey();

  r0 = processgrey(&g0,&v0);
  r1 = processgrey(&g1,&v1);
  r2 = processgrey(&g2,&v2);
  r3 = processgrey(&g3,&v3);
  r4 = processgrey(&g4,&v4);
  r5 = processgrey(&g5,&v5);

  return pollpush();

}


