#ifndef __SEVEN_H__
#define __SEVEN_H__

#define DPOS4 0
#define DPOS3 25
#define DPOS2 50
#define DPOS1 75
#define DPOS0 100
#define DPOSN 125

#define DOFFSET 4

#define POS4 DPOS4 + DOFFSET
#define POS3 DPOS3 + DOFFSET
#define POS2 DPOS2 + DOFFSET
#define POS1 DPOS1 + DOFFSET
#define POS0 DPOS0 + DOFFSET

#define BLANK_SWATH  25

void seven_disp(uint8_t pos_ord,uint8_t v);
void clear_pos(uint8_t pos);

#endif
