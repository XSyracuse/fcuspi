#include "getDecStr.h"

void getDecStr (char* str, uint8_t len, uint32_t val)
{
  uint8_t i;

  for(i=1; i<=len; i++)
  {
    str[len-i] = (uint8_t) ((val % 10UL) + '0');
    val/=10;
  }

  str[i-1] = '\0';
}
