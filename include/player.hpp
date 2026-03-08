#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <nds.h>

class Player {
public:
    Player();

    void init();
    void update();
    void draw() const;

    int getX() const { return x; }
    int getY() const { return y; }

private:
    void handleInput();
    void updateAnimation();
    void uploadCurrentFrame();
    
    enum Direction {
        DIR_SOUTH = 0,
        DIR_EAST  = 1,
        DIR_WEST  = 2,
        DIR_NORTH = 3
    };

    int x;
    int y;

    Direction direction;
    int spriteId;
    int animFrame;
    int animTimer;

    bool moving;

    u16* gfx;
};

#endif