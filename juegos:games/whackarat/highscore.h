#ifndef HIGHSCORE_H
#define HIGHSCORE_H

class Highscore {
  public:
    Highscore();
    void draw();
    void add(int points);
    boolean ready();
    
  private:
    int _points;
};

#endif

