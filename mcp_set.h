
#ifndef __MCP_SET_H__
#define __MCP_SET_H__
#include<stdint.h>
#include<stdbool.h>


//uint32_t target_altitude = 0;

void set_altitude(bool fast,bool cw);
uint16_t get_altitude();

#endif
