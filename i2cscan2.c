
// Scan I2C bus for device responses


#define I2C_TIMEOUT 0
#define I2C_NOINTERRUPT 0
#define I2C_FASTMODE 0
#define FAC 1
#define I2C_CPUFREQ (F_CPU/FAC)

/* Corresponds to A4/A5 - the hardware I2C pins on Arduinos */
/* Adjust to your own liking */
#define SDA_PORT PORTC
#define SDA_PIN 4
#define SCL_PORT PORTC
#define SCL_PIN 5
#define I2C_FASTMODE 0


#include "SoftI2CMaster.h"
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

//------------------------------------------------------------------------------
void CPUSlowDown(int fac) {
  // slow down processor by a fac
    CLKPR = _BV(CLKPCE);
    CLKPR = _BV(CLKPS1) | _BV(CLKPS0);
}
  


void i2c_setup(void) {
#if FAC != 1
  CPUSlowDown(FAC);
#endif


  if (!i2c_init()) 
    puts_P(PSTR("Initialization error. SDA or SCL are low"));
  else
    puts_P(PSTR("...done"));
}


void i2c_scan(void)
{
  uint8_t add = 0;
  char buffer[7];

  int found = false;
  puts_P(PSTR("Scanning ..."));

  puts_P(PSTR("       8-bit 7-bit addr"));
  // try read
  do {
    delay(100);
    if (i2c_start(add | I2C_READ)) {
      found = true;
      i2c_read(true);
      i2c_stop();
  
      puts_P(PSTR("Read:   0x"));
      if(add<0x0F) puts_P(PSTR("0"));
      itoa(add+I2C_READ, buffer, 16);
      puts(buffer);

      puts_P(PSTR("  0x"));
      if (add>>1 < 0x0F) puts_P(PSTR("0"));
      itoa(add>>1, buffer, 16);
      puts(buffer);
      
    } else i2c_stop();
    add += 2;
  } while (add);

  // try write
  add = 0;
  do {
    if (i2c_start(add | I2C_WRITE)) {
      found = true;
      i2c_stop();

      puts_P(PSTR("Write:  0x"));    
      if (add < 0x0F) puts_P(PSTR("0"));  
      itoa(add+I2C_WRITE, buffer,16);
      puts(buffer);

      puts_P(PSTR("  0x"));
      if (add>>1 < 0x0F) puts_P(PSTR("0"));
      itoa(add>>1, buffer, 16);
      puts(buffer);

  } else i2c_stop();
    i2c_stop();
    add += 2;
  } while (add);
  if (!found) puts_P(PSTR("No I2C device found."));
  puts_P(PSTR("Done\n\n"));

  //delay(1000/FAC);
}

#define DISPLAY_I2C_ADDRESS 0x78 //0x3c shifted <<1

static void displaybus_init(uint8_t address) __attribute__ ((always_inline));
inline void displaybus_init(uint8_t add) 
{

i2c_start(add | I2C_WRITE);

}

static uint8_t displaybus_busy(void) __attribute__ ((always_inline));
inline void displaybus_write(uint8_t d,uint8_t last) 
{
  
  i2c_write(d);
  if (last) i2c_stop();

}

static const uint8_t PROGMEM init_sequence[] = {
  0x00,             // Command marker.
  0xAE,             // Display off.
  0xD5, 0x80,       // Display clock divider (reset).
  0xA8, 0x1F,       // 1/32 duty.
  0x40 | 0x00,      // Start line (reset).
  0x20, 0x02,       // Page addressing mode (reset).
  0x22, 0x00, 0x03, // Start and end page in horiz./vert. addressing mode[1].
  0x21, 0x00, 0x7F, // Start and end column in horiz./vert. addressing mode.
  0xA0 | 0x00,      // No segment remap (reset).
  0xC0 | 0x00,      // Normal com pins mapping (reset).
  0xDA, 0x02,       // Sequental without remap com pins.
  0x81, 0x7F,       // Contrast (reset).
  0xDB, 0x20,       // Vcomh (reset).
  0xD9, 0xF1,       // Precharge period.
  0x8D, 0x14,       // Charge pump.
  0xA6,             // Positive display.
  0xA4,             // Resume display.
  0xAF              // Display on.
};

/**
 * Initializes the display's controller configuring the way of
 * displaying data.
 */
void display_init(void) {
  uint8_t i;

  displaybus_init(DISPLAY_I2C_ADDRESS);

  for (i = 0; i < sizeof(init_sequence); i++) {
    // Send last byte with 'last_byte' set.
    displaybus_write(init_sequence[i], (i == sizeof(init_sequence) - 1));
  }
}

void display_fill(uint16_t memsize, uint8_t fill){

        uint16_t i;
        /**
          Clear the screen. As this display supports many sophisticated
          commands, but not a simple 'clear', we have to overwrite the entire
          memory with zeros, byte by byte.
        */
        // Set horizontal adressing mode.
        displaybus_write(0x00, 0);
        displaybus_write(0x20, 0);
        displaybus_write(0x00, 1);

        // Write 512 filler.
        displaybus_write(0x40, fill);
        for (i = 0; i < memsize; i++) {
          displaybus_write(0x00, (i == memsize-1));
        }

        // Return to page adressing mode.
        displaybus_write(0x00, 0);
        displaybus_write(0x20, 0);
        displaybus_write(0x02, 1);

}


int main(){

  i2c_setup();
  i2c_scan();
  return 1;

}
