#ifndef SHARK_BITMAPS_H
#define SHARK_BITMAPS_H
typedef const unsigned char prog_uchar;
typedef const unsigned char prog_char;
#include <avr/pgmspace.h>
#include "paratrooper_bitmaps.h"

#define NUM_SHARKS 5
#define MISSES_Y  64

extern prog_uchar shark1_bitmap[];
extern prog_uchar shark2_bitmap[];
extern prog_uchar shark3_bitmap[];

extern prog_uchar *shark_bitmaps[ NUM_SHARKS ]; 

extern prog_uchar missshark[];

extern byte misses_positions[];
extern Coord shark_positions[];

#endif SHARK_BITMAPS_H



