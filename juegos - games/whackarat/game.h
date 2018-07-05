#ifndef GAME_H
#define GAME_H

class Game {
  public:
    void initNewGame();
    void calcndraw();
    boolean gameOver();
    void waitUntilNoKeyIsPressed();
    void waitUntilKeyIsPressed();
    int getPoints();
    
  private:
    unsigned long _show_timer;
    byte _rat_number;
    byte _hammers;
    unsigned int _points;
    boolean _gameOver;
    
    void _drawMetaInfos();
    void _hitPostProcessing();
    void _missPostProcessing();
};

#endif

