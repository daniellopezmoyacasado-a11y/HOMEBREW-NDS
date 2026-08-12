#include "player.hpp"
#include <cstring>
#include <nds.h>
#include "NDS-Player1.h"
#include "NDS-Player2.h"
#include "Game.hpp" 
#include "Constants.hpp"
#include "movement.hpp"


static const int PLAYER_WIDTH = 16;
static const int PLAYER_HEIGHT = 32;

static const int FRAME_COLS = 4;
static const int FRAME_ROWS = 4;

static const int FRAME_WIDTH_TILES = 2;   // 16 / 8
static const int FRAME_HEIGHT_TILES = 4;  // 32 / 8

static const int SHEET_WIDTH_TILES = FRAME_COLS * FRAME_WIDTH_TILES;   // 8
static const int SHEET_HEIGHT_TILES = FRAME_ROWS * FRAME_HEIGHT_TILES; // 16

static const int TILE_BYTES_8BPP = 64;
static const int FRAME_COUNT = 4;
static const int WALK_FRAME_SEQUENCE[FRAME_COUNT] = { 0, 1, 2, 3 };
static const int SLIDE_SPEED = 1;
static const int SLIDE_FRAME_DELAY = 1;
static const int WALK_FRAME_DELAY = 4;
static const int TURN_HOLD_DELAY = 6; // frames before starting tile step (~100ms)
static const int START_TILE_X = 8;
static const int START_TILE_Y = 6;

Player::Player()
    : x(START_TILE_X * TILE_SIZE),
      y((START_TILE_Y - 1) * TILE_SIZE),
      targetX(START_TILE_X * TILE_SIZE),
      targetY((START_TILE_Y - 1) * TILE_SIZE),
      moveTimer(0),
      direction(DIR_SOUTH),
      spriteId(0),
      characterChoice(1),
      animFrame(0),
      animTimer(0),
      turnTimer(0),
      moving(false),
      gfx(nullptr) {}


void Player::init() {
    gfx = oamAllocateGfx(&oamMain, SpriteSize_16x32, SpriteColorFormat_256Color);
    //memcpy(SPRITE_GFX, NDS_Player1Tiles, NDS_Player1TilesLen);
    uploadCurrentFrame();
}

void Player::handleInput() {
    if (moving) return;

    int held = keysHeld();


    int dx = 0;
    int dy = 0;
    Direction nextDirection = direction;

    if (held & KEY_LEFT) {
        dx = -1;
        nextDirection = DIR_WEST;
    } else if (held & KEY_RIGHT) {
        dx = 1;
        nextDirection = DIR_EAST;
    } else if (held & KEY_UP) {
        dy = -1;
        nextDirection = DIR_NORTH;
    } else if (held & KEY_DOWN) {
        dy = 1;
        nextDirection = DIR_SOUTH;
    }

    if (dx == 0 && dy == 0) {
        turnTimer = 0;
        return;
    }

    if (direction != nextDirection) {
        direction = nextDirection;
        uploadCurrentFrame();
        turnTimer = 1;
        return;
    }

    if (turnTimer > 0 && turnTimer < TURN_HOLD_DELAY) {
        turnTimer++;
        return;
    }

    int nextX = x + dx * TILE_SIZE;
    int nextY = y + dy * TILE_SIZE;

    if (nextX < 0 || nextX > 256 - PLAYER_WIDTH) return;
    if (nextY < 0 || nextY > 192 - PLAYER_HEIGHT) return;

    int footX = nextX;
    int footY = nextY + PLAYER_HEIGHT - TILE_SIZE;

    if (gRoom.check_collision(footX, footY)) return;

    targetX = nextX;
    targetY = nextY;
    moveTimer = 0;
    animFrame = 0;
    animTimer = 0;
    turnTimer = TURN_HOLD_DELAY;
    moving = true;
    uploadCurrentFrame();
}

void Player::updateMovement() {
    if (!moving) return;

    moveTimer++;
    if (moveTimer < SLIDE_FRAME_DELAY) return;

    moveTimer = 0;

    if (moveTowards(x, y, targetX, targetY, SLIDE_SPEED)) {
        moving = false;
    }
}


void Player::updateAnimation() {
    int oldFrame = animFrame;
    Direction oldDirection = direction;

    if (moving) {
        animTimer++;
        if (animTimer >= WALK_FRAME_DELAY) {
            animTimer = 0;
            animFrame = (animFrame + 1) % FRAME_COUNT;
        }
    } else {
        animFrame = 0;
        animTimer = 0;
    }

    if (animFrame != oldFrame || direction != oldDirection) {
        uploadCurrentFrame();
    }
}

void Player::setCharacterChoice(int choice) {
    characterChoice = choice;
    uploadCurrentFrame();
}

void Player::setPosition(int newX, int newY) {
    x = newX;
    y = newY;
    targetX = newX;
    targetY = newY;
    moving = false;
}

void Player::uploadCurrentFrame() {
    // Each frame is 2x4 tiles inside an 8x16 tile sheet.
    const u8* src = (characterChoice == 2)
        ? reinterpret_cast<const u8*>(NDS_Player2Tiles)
        : reinterpret_cast<const u8*>(NDS_Player1Tiles);
    u8* dst = reinterpret_cast<u8*>(gfx);


    int frameTileX = WALK_FRAME_SEQUENCE[animFrame] * FRAME_WIDTH_TILES;          
    int frameTileY = static_cast<int>(direction) * FRAME_HEIGHT_TILES; 

    int dstTileIndex = 0;

    for (int ty = 0; ty < FRAME_HEIGHT_TILES; ++ty) {
        for (int tx = 0; tx < FRAME_WIDTH_TILES; ++tx) {
            int sheetTileX = frameTileX + tx;
            int sheetTileY = frameTileY + ty;
            int sheetTileIndex = sheetTileY * SHEET_WIDTH_TILES + sheetTileX;

            memcpy(
                dst + dstTileIndex * TILE_BYTES_8BPP,
                src + sheetTileIndex * TILE_BYTES_8BPP,
                TILE_BYTES_8BPP
            );

            dstTileIndex++;
        }
    }

    DC_FlushRange(gfx, 8 * TILE_BYTES_8BPP);
}


void Player::update() {
    handleInput();
    updateMovement();
    updateAnimation();
}

int Player::getFootX() const {
    return x;
}

int Player::getFootY() const {
    return y + PLAYER_HEIGHT - TILE_SIZE;
}

int Player::getTileX() const {
    return getFootX() / TILE_SIZE;
}

int Player::getTileY() const {
    return getFootY() / TILE_SIZE;
}

void Player::draw() const {
    oamSet(
        &oamMain,
        spriteId,
        x, y,
        0,
        0,
        SpriteSize_16x32,
        SpriteColorFormat_256Color,
        gfx,
        -1,
        false,
        false,
        false,
        false,
        false
    );
}

void Player::getFacingTile(int &outTileX, int &outTileY) const {
    outTileX = getTileX();
    outTileY = getTileY();

    switch (direction) {
        case DIR_SOUTH: outTileY += 1; break;
        case DIR_EAST:  outTileX += 1; break;
        case DIR_WEST:  outTileX -= 1; break;
        case DIR_NORTH: outTileY -= 1; break;
    }
}

bool Player::checkInteractTrigger(int &outTileX, int &outTileY) {
    if (moving) return false;

    u32 down = keysDown();
    if (down & KEY_A) {
        getFacingTile(outTileX, outTileY);
        return true;
    }
    return false;
}

