#ifndef __PINMAP_H__

#define __PINMAP_H__
//pwm control
#define EHDDR  DDRD
#define EHPORT PORTD
#define EHPIN  0 

#define LED_DDR    DDRD
#define LED_PORT   PORTD
#define LED_PIN    4

#define ASTEP_DDR  DDRD
#define ASTEP_PORT PORTD
#define ASTEP_PIN  7

#define ADIR_DDR  DDRD
#define ADIR_PORT PORTD
#define ADIR_PIN  6

#define AEN_DDR  DDRD
#define AEN_PORT PORTD
#define AEN_PIN  5

#define SDA_PORT PORTD
#define SDA_PIN 2
#define SCL_PORT PORTD
#define SCL_PIN 3


#endif
