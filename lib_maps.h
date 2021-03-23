#ifndef LOCA_MAPS_H
#define LOCA_MAPS_H

#include <stdint.h>

typedef uint8_t byte;

typedef void map(byte*,byte*);

enum basic_map_funcs {times_two=0, times_three=1};
extern map* basic_math[];

byte* minus_two_func(byte* value,byte* res);

#endif