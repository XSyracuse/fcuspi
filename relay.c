/* relay output */

#include<stdint.h>
#include<avr/io.h>

#define RELAYPORT  PORTD
#define RELAYPIN   PIND
#define RELAYDDR   DDRD

#define ACTIVELOW

#define SIX0  0
#define SIX1  1
#define SIX2  2
#define SIX3  3 
#define SIX4  4
#define SIX5  5
#define CAUTION 6
#define WARNING 7

void init_relays()
{
  
  RELAYDDR = 0xFF;
  RELAYPORT = 0xFF;
  
}

void set_relay(uint8_t b)
{

  //RELAYPORT &= ~(1<<b);

}

void clr_relay(uint8_t b)
{

  //RELAYPORT |= (1<<b);

}
// six_pack lamps
// and caution and warning
/*
void six_pack(uint8_t c)
{

  switch(c) {
      case '0':
	      RELAYPORT &= ~(1<<SIX0);
		  break;
      case '1':
          RELAYPORT &= ~(1<<SIX1);
		  break;
	  case '2':
	      RELAYPORT &= ~(1<<SIX2);
		  break;
	  case '3':
          RELAYPORT &= ~(1<<SIX3);
		  break;
	  case '4':
	      RELAYPORT &= ~(1<<SIX4);
		  break;
	  case '5':
	      RELAYPORT &= ~(1<<SIX5);
		  break;
	  case '6':
	      RELAYPORT &= ~(1<<CAUTION);
		  break;
	  case '7':
	      RELAYPORT &= ~(1<<WARNING);
		  break;
      case '8':
	      RELAYPORT |= (1<<SIX0);
		  break;
	  case '9':
	      RELAYPORT |= (1<<SIX1);
		  break;
	  case 'A':
	      RELAYPORT |= (1<<SIX2);
		  break;
	  case 'B':
	      RELAYPORT |= (1<<SIX3);
		  break;
	  case 'C':
	      RELAYPORT |= (1<<SIX4);
		  break;
      case 'D':
	      RELAYPORT |= (1<<SIX5);
		  break;
	  case 'E':
	      RELAYPORT |= (1<<CAUTION);
		  break;
      case 'F':
	      RELAYPORT |= (1<<WARNING);
		  break;
      default:
	      break;
  }

}
*/
uint8_t six_pack(uint8_t six, uint8_t c)
{

  switch(c) {

      case '0':
	      six &= ~(1<<SIX0);
		  break;
      case '1':
          six &= ~(1<<SIX1);
		  break;
	  case '2':
	      six &= ~(1<<SIX2);
		  break;
	  case '3':
          six &= ~(1<<SIX3);
		  break;
	  case '4':
	      six &= ~(1<<SIX4);
		  break;
	  case '5':
	      six &= ~(1<<SIX5);
		  break;
	  
      case '8':
	      six |= (1<<SIX0);
		  break;
	  case '9':
	      six |= (1<<SIX1);
		  break;
	  case 'A':
	      six |= (1<<SIX2);
		  break;
	  case 'B':
	      six |= (1<<SIX3);
		  break;
	  case 'C':
	      six |= (1<<SIX4);
		  break;
      case 'D':
	      six |= (1<<SIX5);
		  break;

      default:
	      break;
  }
  
  return six;

}
