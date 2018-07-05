#ifndef BOAT_BITMAPS_H
#define BOAT_BITMAPS_H
typedef const unsigned char prog_char;
typedef const unsigned char prog_uchar;
#include <avr/pgmspace.h>
#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#define NUM_BOATS 3
#define BOAT_Y 62

extern byte boatX[ NUM_BOATS ];

extern prog_uchar boat0_bitmap[];
extern prog_uchar boat1_bitmap[];
extern prog_uchar boat2_bitmap[];

extern prog_uchar *boat_bitmaps[ NUM_BOATS ];

#endif BOAT_BITMAPS_H

