#ifndef __OLED_PARSE_H__

#define __OLED_PARSE_H__

#include <stdint.h>
#include "i2cscan.h"

void parse_oledwrite(char *line);
void oled_prog();

#endif
