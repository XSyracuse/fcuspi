#ifndef __I2C_SCAN_H__
#define __I2C_SCAN_H__

void i2c_test();
void display_init(void);
void display_fill(uint16_t memsize, uint8_t fill);
void display_font_start(uint8_t page,uint8_t c);
void display_symbol(uint8_t symbol);
void display_print_P(const char *s);
void display_print(const char *s);
void display_data_start();
void display_data_push();
void display_data_stop();

#define display_clear display_fill(1024,0x00)

#endif
