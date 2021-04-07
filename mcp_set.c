#include<stdint.h>
#include<stdbool.h>
#include<stdio.h>

uint16_t target_altitude = 0;


void set_altitude(bool fast,bool cw)
{
  
  if(!fast){

    if(cw) target_altitude += 100;
    else   target_altitude -= 100;

  }
  else{

    if(cw) target_altitude += 1000;
    else   target_altitude -= 1000;

  }
  //will roll over for unsigned
  if(target_altitude>50000) {
    target_altitude = 0;
  }
}

uint16_t get_altitude()
{

  return target_altitude;

}


