#ifndef __HEXUTIL_H__

#define __HEXTUTIL_H__

#include<stdbool.h>
#include<stdint.h>

bool ishexdigit(char x);
uint8_t xtonibble(char x);
uint8_t xto8(char m, char l);
uint32_t bin2bcd(uint32_t bin);

#endif
