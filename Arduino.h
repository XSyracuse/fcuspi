
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

typedef uint8_t byte;

extern unsigned long system_time;

unsigned long millis(){
  return system_time;
}
