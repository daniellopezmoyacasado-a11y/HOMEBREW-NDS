#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <nds.h>

class Player {
public:
    enum Direction {
        DIR_SOUTH = 0,
        DIR_EAST  = 1,
        DIR_WEST  = 2,
        DIR_NORTH = 3
    };

    Player();

    void init();
    void update();
    void draw() const;

    int getX() const { return x; }
    int getY() const { return y; }
    int getFootX() const;
    int getFootY() const;
    int getTileX() const;
    int getTileY() const;
    Direction getDirection() const { return direction; }
    void setSpriteId(int id) { spriteId = id; }

    void setCharacterChoice(int choice);
    int getCharacterChoice() const { return characterChoice; }
    void setPosition(int newX, int newY);

    void getFacingTile(int &outTileX, int &outTileY) const;
    bool checkInteractTrigger(int &outTileX, int &outTileY);

private:
    void handleInput();
    void updateMovement();
    void updateAnimation();
    void uploadCurrentFrame();

    int x;
    int y;

    int targetX;
    int targetY;
    int moveTimer;

    Direction direction;
    int spriteId;
    int characterChoice;
    int animFrame;
    int animTimer;
    int turnTimer;

    bool moving;

    u16* gfx;
};


#endif
