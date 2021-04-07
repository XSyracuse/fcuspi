#ifndef __RELAY_H_INCLUDED
#define __RELAY_H_INCLUDED


void init_relays();
void set_relay(uint8_t b);
void clr_relay(uint8_t b);
//void six_pack(uint8_t c);
uint8_t six_pack(uint8_t six, uint8_t c);

#endif
