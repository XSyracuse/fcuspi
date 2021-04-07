#include "hexutil.h"



bool ishexdigit(char x)
{
  bool r;
  if((x>='A' && x<='F') || (x>='0' && x<='9'))
    r=true;    
  else
    r=false;
  return r;

}

uint8_t xtonibble(char x)
{
  uint8_t r;
  if(x>='0' && x<='9')
    r = x - '0';
  
  else if(x>='A' && x<='F')
    r = x - 'A' + 10;

  else r=0;

  return r & 0x0f;
}

uint8_t xto8(char m, char l)
{
  
  m = xtonibble(m) << 4;
  m = m | xtonibble(l);
  return m;

}


//16 bit number to 5 nibble packed bcd 
uint32_t bin2bcd(uint32_t bin)
{

  bin = bin & 0xffff;
  uint32_t output=0;

  for(int a = 17; a >= 0; a--){

      if((output & 0xF) >= 5)
          output += 3;
      if(((output & 0xF0) >> 4) >= 5)
          output += (3 << 4);
      if(((output & 0xF00) >> 8) >= 5)
          output += (3 << 8);
      if(((output & 0xF000) >> 12) >= 5)
          output += (3 << 12); 
      output = (output << 1) | ((bin >> a) & 1);

  }

  return output;
}
