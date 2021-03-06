#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif
#include "shark_bitmaps.h"
typedef const unsigned char prog_uchar;
typedef const unsigned char prog_char;

byte misses_positions[ 3 ] = { 88, 99, 110 };
Coord shark_positions[ NUM_SHARKS ] = { { 75, 79 }, { 45, 79 }, { 28, 89 }, { 62, 89 }, { 83, 81 } }; // 0 is top right and then moves anti-clockwise

// left facing shark
PROGMEM prog_uchar shark1_bitmap[] = {
  6, 4,
  0x0C, 0x38, 0x78, 0xFC
};

// right facing shark
PROGMEM prog_uchar shark2_bitmap[] = {
  7, 5, 0xC0, 0x70, 0x78, 0x7C, 0xFE
};

// shark rising out of water
PROGMEM prog_uchar shark3_bitmap[] = {
  31, 13,
  0x00,0x00,0x00,0x7C,
  0x00,0x00,0x0F,0xFE,
  0x00,0x00,0x7F,0xFE,
  0x00,0x01,0xFF,0xFE,
  0x00,0x07,0xFF,0xFE,
  0x00,0x0D,0xFF,0xFC,
  0x00,0x3F,0xFF,0xFC,
  0x00,0xFF,0xF5,0xF8,
  0x1F,0xDF,0xC0,0x70,
  0xFF,0x5F,0x15,0x80,
  0x7F,0x6E,0x7F,0xC0,
  0x0D,0xBF,0xFF,0x80,
  0x1F,0xFF,0xFF,0x00
};

prog_uchar *shark_bitmaps[ NUM_SHARKS ] = { shark1_bitmap, shark1_bitmap, shark2_bitmap, shark2_bitmap, shark3_bitmap };

PROGMEM prog_uchar missshark[] = {
  10, 10,
  0x00,0x00,
  0x78,0x00,
  0xFC,0x00,
  0xF6,0x00,
  0xE7,0x00,
  0xFF,0x00,
  0xFF,0xC0,
  0x57,0x80,
  0x6B,0x00,
  0x7F,0x00
};
