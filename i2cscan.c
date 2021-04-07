
// Scan I2C bus for device responses


#define I2C_TIMEOUT 0
#define I2C_NOINTERRUPT 0
#define I2C_FASTMODE 1
#define I2C_PULLUP 0
#define FAC 1
#define I2C_CPUFREQ (F_CPU/FAC)

/*  include before SoftI2CMaster.h for IO defines */
#include "pinmap.h"
#include "SoftI2CMaster.h"

#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "ssd1306.h"
#include "font.h"

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
    _delay_ms(100);
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
      
    } else {
      i2c_stop();
    }
    
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

    } else {
      i2c_stop();
    }
  
    i2c_stop();
    add += 2;
  
  } while (add);
  
  if (!found) puts_P(PSTR("No I2C device found."));
  puts_P(PSTR("Done\n\n"));

  _delay_ms(1000/FAC);
}

/* display */
#define DISPLAY_I2C_ADDRESS 0x78 //0x3c shifted <<1

static void displaybus_init(uint8_t address) __attribute__ ((always_inline));
inline void displaybus_init(uint8_t address) 
{

i2c_setup();


}

static void displaybus_write(uint8_t d,uint8_t last) __attribute__ ((always_inline));
inline void displaybus_write(uint8_t d,uint8_t last) 
{
  i2c_write(d);
}

void ssd1306_command(uint8_t c)
{
  i2c_start_wait(DISPLAY_I2C_ADDRESS | I2C_WRITE);
  i2c_write(0x00);
  i2c_write(c);
  i2c_stop();
}
/*
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
*/

/**
 * Initializes the display's controller configuring the way of
 * displaying data.
 */
/*
void display_init(void) {
  uint8_t i;
  
  displaybus_init(DISPLAY_I2C_ADDRESS);
  i2c_start_wait(DISPLAY_I2C_ADDRESS | I2C_WRITE);

  for (i = 0; i < sizeof(init_sequence); i++) {
    // Send last byte with 'last_byte' set.
    displaybus_write(init_sequence[i], 0);
  }
  i2c_stop();
}
*/
void display_init(void){


    displaybus_init(DISPLAY_I2C_ADDRESS);
   
    // Init sequence for 128x64 OLED module 
    ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE 

    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5 
    ssd1306_command(0x80);                 // the suggested ratio 0x80 
    
    ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8 
    ssd1306_command(0x3F); 
    
    ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3 
    ssd1306_command(0x0);                                   // no offset 
    
    ssd1306_command(SSD1306_SETSTARTLINE);// | 0x0);        // line #0 
    
    ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D 
    ssd1306_command(0x14);  // using internal VCC 
    
    ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20 
    ssd1306_command(0x00);          // 0x00 horizontal addressing 
    
    ssd1306_command(SSD1306_SEGREMAP | 0x1); // rotate screen 180 
    
    ssd1306_command(SSD1306_COMSCANDEC); // rotate screen 180 
    
    ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA 
    ssd1306_command(0x12); 
    
    ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81 
    ssd1306_command(0xCF); 
    
    ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9 
    ssd1306_command(0xF1); 
    
    ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB 
    ssd1306_command(0x40); 
    
    ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4 
    
    ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6 

    ssd1306_command(SSD1306_DEACTIVATE_SCROLL);
  
    ssd1306_command(SSD1306_DISPLAYON);                     //switch on OLED 

}

void display_fill(uint16_t memsize, uint8_t fill){

        uint16_t i;
        
        /**
          Clear the screen. As this display supports many sophisticated
          commands, but not a simple 'clear', we have to overwrite the entire
          memory with zeros, byte by byte.
        */
        // Set horizontal adressing mode.
        i2c_start_wait(DISPLAY_I2C_ADDRESS | I2C_WRITE);
        displaybus_write(0x00, 0);
        displaybus_write(0x20, 0);
        displaybus_write(0x00, 1);
        i2c_stop();

        i2c_start_wait(DISPLAY_I2C_ADDRESS | I2C_WRITE);
        // Write 512 filler.
        displaybus_write(0x40, 0);
        for (i = 0; i < 1024; i++) {
          displaybus_write(fill, 0);
        }
        i2c_stop();

        // Return to page adressing mode.
        i2c_start_wait(DISPLAY_I2C_ADDRESS | I2C_WRITE);
        displaybus_write(0x00, 0);
        displaybus_write(0x20, 0);
        displaybus_write(0x02, 1);
		i2c_stop();

        //command on
        i2c_start_wait(DISPLAY_I2C_ADDRESS | I2C_WRITE);
        displaybus_write(0x00, 0);
        displaybus_write(0xAF, 1);
		i2c_stop();

}
void display_font_start(uint8_t page,uint8_t c)
{
    //set beginning
    i2c_start_wait(DISPLAY_I2C_ADDRESS | I2C_WRITE);
    displaybus_write(0x00, 0);
    displaybus_write(0xB0 | (page & 0x07), 0);
    displaybus_write(0x00 | (c & 0x0F), 0);
    displaybus_write(0x10 | ((c>>4) & 0x0F), 0);
    i2c_stop();
}
void display_symbol(uint8_t symbol) 
{
    uint8_t index = symbol - 0x20;
	uint8_t i;
    //command
    i2c_start_wait(DISPLAY_I2C_ADDRESS | I2C_WRITE);
    displaybus_write(0x40, 0);
    // Send the character bitmap.
    #ifdef FONT_IS_PROPORTIONAL
       for (i = 0; i < pgm_read_byte(&font[index].columns); i++) {
    #else
       for (i = 0; i < FONT_COLUMNS; i++) {
    #endif
           displaybus_write(pgm_read_byte(&font[index].data[i]), 0);
        }
        // Send space between characters.
        for (i = 0; i < FONT_SYMBOL_SPACE; i++) {
          displaybus_write(0x00, (i == FONT_SYMBOL_SPACE - 1));
        }
    i2c_stop();
}

void display_print_P(const char *s)
{

  uint8_t c = 0;
  uint8_t i = 0;
  while ((c=pgm_read_byte(s))!=0 && i<32) {
    display_symbol(c);
	i++;
	s++;
  }

}
//Data RAM print
void display_print(const char *s)
{

  uint8_t c = 0;
  uint8_t i = 0;
  while ((c=*s)!=0 && i<32) {
    display_symbol(c);
	i++;
	s++;
  }

}

void display_data_start() {
  i2c_start_wait(DISPLAY_I2C_ADDRESS | I2C_WRITE);
  displaybus_write(0x40, 0);
}

void display_data_push(uint8_t data){
  displaybus_write(data, 0);
}

void display_data_stop(){
  i2c_stop();   
}

void i2c_test(){

  i2c_setup();
  i2c_scan();
  return;
  
}
