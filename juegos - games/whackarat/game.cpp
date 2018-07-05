#include <Arduino.h>
#include <TVout.h>
#include <fontALL.h>
#include <Controllers.h>
#include "misc_bitmaps.h"
#include "game.h"
#include "bitmap_funcs.h"

extern TVout tv;
byte _lochX[] = {7, 64-50+7, 128-50+7-14};
byte _lochY[] = {61, 84, 61};
byte _ratX[] = {7, 64-50+7, 128-50+7-14};
byte _ratY[] = {61-38+12, 84-38+12, 61-38+12};
byte _hitX[] = {7-2, 64-50+7-2, 128-50+7-14-2};
byte _hitY[] = {61-61+12, 84-61+12, 61-61+12};

void Game::initNewGame() {
  tv.clear_screen();
  for(int i=0; i<3; i++) {
    tv.bitmap(_lochX[i], _lochY[i], loch_bitmap);
  }
  _show_timer = 0;
  _rat_number = 0;
  _hammers = 3;
  _points = 0;
  _gameOver = false;
  _drawMetaInfos();
};

void Game::calcndraw() {
  if(_show_timer > 0) {
    _show_timer--;
    if(_show_timer == 0) {
      tv.tone(440, 50);
      tv.clear_screen();
      for(int i=0; i<3; i++) {
        tv.bitmap(_lochX[i], _lochY[i], loch_bitmap);
      }
      _drawMetaInfos();
    }
  } else {
    if(random(2500) == 0) {
      _show_timer = 5000 + random(5000);
      _rat_number = random(3);
      overlaybitmap(_ratX[_rat_number], _ratY[_rat_number], ratte_bitmap);
      tv.tone(880, 50);
    }
  }
  
  if(Controller.leftPressed()) {
    if((_show_timer > 0) && (_rat_number == 0)) {
      tv.bitmap(_hitX[0], _hitY[0], treffer_bitmap);
      _hitPostProcessing();
    } else {
      overlaybitmap(_hitX[0], _hitY[0], hammer_bitmap);
      _missPostProcessing();
      erasebitmap(_hitX[0], _hitY[0], hammer_bitmap);
    }
  } else if(Controller.downPressed()) {
    if((_show_timer > 0) && (_rat_number == 1)) {
      tv.bitmap(_hitX[1], _hitY[1], treffer_bitmap);
      _hitPostProcessing();
    } else {
      overlaybitmap(_hitX[1], _hitY[1], hammer_bitmap);
      _missPostProcessing();
      erasebitmap(_hitX[1], _hitY[1], hammer_bitmap);
    }
  } else if(Controller.rightPressed()) {
    if((_show_timer > 0) && (_rat_number == 2)) {
      tv.bitmap(_hitX[2], _hitY[2], treffer_bitmap);
      _hitPostProcessing();
    } else {
      overlaybitmap(_hitX[2], _hitY[2], hammer_bitmap);
      _missPostProcessing();
      erasebitmap(_hitX[2], _hitY[2], hammer_bitmap);
    }
  } else if(Controller.upPressed()) {
    _missPostProcessing();
  }
}

void Game::_drawMetaInfos() {
  // Punkte...
  tv.print(0, 0, "POINTS: ");
  tv.print(60, 0, _points);
  // verbleibende Hammer...
  for(int i=0; i<3; i++) {
    erasebitmap(128-((i+1)*11), 0, hammer_klein_bitmap);
  }
  for(int i=0; i<_hammers; i++) {
    tv.bitmap(128-((i+1)*11), 0, hammer_klein_bitmap);
  }
}

void Game::waitUntilNoKeyIsPressed() {
  while (Controller.leftPressed() || Controller.rightPressed() || Controller.upPressed() || Controller.downPressed() || Controller.firePressed()) {}
}

void Game::waitUntilKeyIsPressed() {
  while (!Controller.leftPressed() && !Controller.rightPressed() && !Controller.upPressed() && !Controller.downPressed() && !Controller.firePressed()) {}
}

void Game::_hitPostProcessing() {
  _drawMetaInfos();
  _points++;
  _show_timer = 1;
  tv.tone(440, 40);
  tv.delay(40);
  tv.tone(660, 40);
  tv.delay(40);
  tv.tone(880, 40);
  tv.delay(40);
  waitUntilNoKeyIsPressed();
}

void Game::_missPostProcessing() {
  _hammers--;
  _drawMetaInfos();
  tv.tone(220, 250);
  tv.delay(250);
  waitUntilNoKeyIsPressed();
  
  if(_hammers == 0) {
    tv.print(20, 50, "GAME OVER!!!");
    _gameOver = true;
    waitUntilKeyIsPressed();
    waitUntilNoKeyIsPressed();
  }
}

boolean Game::gameOver() {
  return _gameOver;
}

int Game::getPoints() {
  return _points;
}

