#include <Arduino.h>
//#include <EEPROM.h>
#include <TVout.h>
#include "misc_bitmaps.h"
#include "highscore.h"

extern TVout tv;

Highscore::Highscore() {
  _points = 0;
  // _points = EEPROM.read(0);
}

void Highscore::draw() {
  tv.print(0, 0, "HIGHSCORE:");
  tv.print(60, 30, _points);
};

void Highscore::add(int points) {
  tv.clear_screen();
  if(points > _points) {
    _points = points;
    // EEPROM.write(0, _points);
  }
};

boolean Highscore::ready() {
  return true;
};

