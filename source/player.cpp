#include "player.hpp"
#include <cstring>
#include <nds.h>
#include "NDS-Player1.h" 

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

Player::Player()
    : x(120),
      y(80),
      direction(DIR_SOUTH),
      spriteId(0),
      animFrame(0),
      animTimer(0),
      moving(false),
      gfx(nullptr) {}

void Player::init() {
    memcpy(SPRITE_PALETTE, NDS_Player1Pal, NDS_Player1PalLen);
    gfx = oamAllocateGfx(&oamMain, SpriteSize_16x32, SpriteColorFormat_256Color);
    //memcpy(SPRITE_GFX, NDS_Player1Tiles, NDS_Player1TilesLen);
    uploadCurrentFrame();
}

void Player::handleInput() {
    moving = false;

    scanKeys();
    int held = keysHeld();

    if (held & KEY_LEFT) {
        x--;
        direction = DIR_WEST;
        moving = true;
        uploadCurrentFrame();
    }
    if (held & KEY_RIGHT) {
        x++;
        direction = DIR_EAST;
        moving = true;
        uploadCurrentFrame();
    }
    if (held & KEY_UP) {
        y--;
        direction = DIR_NORTH;
        moving = true;
        uploadCurrentFrame();
    }
    if (held & KEY_DOWN) {
        y++;
        direction = DIR_SOUTH;
        moving = true;
        uploadCurrentFrame();
    }

    // Keep player on-screen
    if (x < 0) x = 0;
    if (x > 256 - PLAYER_WIDTH) x = 256 - PLAYER_WIDTH;

    if (y < 0) y = 0;
    if (y > 192 - PLAYER_HEIGHT) y = 192 - PLAYER_HEIGHT;
}

void Player::updateAnimation() {
    int oldFrame = animFrame;
    Direction oldDirection = direction;

    if (moving) {
        animTimer++;
        if (animTimer >= 8) {
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

void Player::uploadCurrentFrame() {
    // Each frame is 2x4 tiles inside an 8x16 tile sheet. (tiles are 16x16 as of rn)
    const u8* src = reinterpret_cast<const u8*>(NDS_Player1Tiles);
    u8* dst = reinterpret_cast<u8*>(gfx);

    int frameTileX = animFrame * FRAME_WIDTH_TILES;          
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
    updateAnimation();
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