#ifndef NPC_HPP
#define NPC_HPP

#include <nds.h>
#include "Interactable.hpp"

class Player;

class NPC : public Interactable {
public:
    NPC(int startX = 160, int startY = 80, int oamSpriteId = 1, int paletteSlot = 1);

    void init();
    void update();
    void draw() const;
    void registerCollision() const;

    virtual bool isAtTile(int tileX, int tileY) const override;
    virtual void onInteract(Player& player) override;
    void onInteractDirection(int playerTileX, int playerTileY);



    int getX() const { return x; }
    int getY() const { return y; }
    int getFootX() const;
    int getFootY() const;
    int getTileX() const;
    int getTileY() const;
    void setSpriteId(int id) { spriteId = id; }

    enum Direction {
        DIR_SOUTH = 0,
        DIR_EAST  = 1,
        DIR_WEST  = 2,
        DIR_NORTH = 3
    };

private:
    void updateAnimation();
    void uploadCurrentFrame();

    int x;
    int y;

    Direction direction;
    int spriteId;
    int paletteId;
    int animFrame;
    int animTimer;

    u16* gfx;
};

#endif
