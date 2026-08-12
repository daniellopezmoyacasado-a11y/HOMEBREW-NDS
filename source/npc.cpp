#include "NPC.hpp"
#include <cstring>
#include <stdio.h>
#include <nds.h>
#include "NPC.h"
#include "Game.hpp" 
#include "Constants.hpp"
#include "player.hpp"
#include "GameState.hpp"


static const int NPC_WIDTH = 16;
static const int NPC_HEIGHT = 32;

static const int FRAME_COLS = 4;
static const int FRAME_ROWS = 4;

static const int FRAME_WIDTH_TILES = 2;   // 16 / 8
static const int FRAME_HEIGHT_TILES = 4;  // 32 / 8

static const int SHEET_WIDTH_TILES = FRAME_COLS * FRAME_WIDTH_TILES;   // 8
static const int SHEET_HEIGHT_TILES = FRAME_ROWS * FRAME_HEIGHT_TILES; // 16

static const int TILE_BYTES_8BPP = 64;
static const int FRAME_COUNT = 4;
static const int IDLE_TURN_DELAY = 60;
static const NPC::Direction IDLE_DIRECTION_SEQUENCE[FRAME_COUNT] = {
    NPC::DIR_SOUTH,
    NPC::DIR_EAST,
    NPC::DIR_NORTH,
    NPC::DIR_WEST
};

NPC::NPC(int startX, int startY, int oamSpriteId, int paletteSlot)
    : x(startX),
      y(startY),
      direction(DIR_SOUTH),
      spriteId(oamSpriteId),
      paletteId(paletteSlot),
      animFrame(0),
      animTimer(0),
      gfx(nullptr) {}

void NPC::init() {
    gfx = oamAllocateGfx(&oamMain, SpriteSize_16x32, SpriteColorFormat_256Color);
    uploadCurrentFrame();
}

void NPC::updateAnimation() {
    animTimer++;
    if (animTimer >= IDLE_TURN_DELAY) {
        animTimer = 0;
        animFrame = (animFrame + 1) % FRAME_COUNT;
        direction = IDLE_DIRECTION_SEQUENCE[animFrame];
        uploadCurrentFrame();
    }
}

void NPC::uploadCurrentFrame() {
    const u8* src = reinterpret_cast<const u8*>(NPCTiles);
    u8* dst = reinterpret_cast<u8*>(gfx);

    int frameTileX = 0;          
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


void NPC::update() {
    updateAnimation();
}

void NPC::registerCollision() const {
    int footX = x;
    int footY = y + NPC_HEIGHT - TILE_SIZE;

    gRoom.update_collision(footX, footY);
}

int NPC::getFootX() const {
    return x;
}

int NPC::getFootY() const {
    return y + NPC_HEIGHT - TILE_SIZE;
}

int NPC::getTileX() const {
    return getFootX() / TILE_SIZE;
}

int NPC::getTileY() const {
    return getFootY() / TILE_SIZE;
}

void NPC::draw() const {
    oamSet(
        &oamMain,
        spriteId,
        x, y,
        0,
        paletteId,
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

bool NPC::isAtTile(int tileX, int tileY) const {
    return getTileX() == tileX && getTileY() == tileY;
}

void NPC::onInteractDirection(int playerTileX, int playerTileY) {
    int npcTileX = getTileX();
    int npcTileY = getTileY();

    if (playerTileX < npcTileX) {
        direction = DIR_WEST;
    } else if (playerTileX > npcTileX) {
        direction = DIR_EAST;
    } else if (playerTileY < npcTileY) {
        direction = DIR_NORTH;
    } else if (playerTileY > npcTileY) {
        direction = DIR_SOUTH;
    }

    animTimer = 0;
    uploadCurrentFrame();
}

void NPC::onInteract(Player& player) {
    onInteractDirection(player.getTileX(), player.getTileY());
    gGameState.setFlag(FLAG_TALK_NPC, true);

    gUIManager.showDialogue("Village Elder", "Hello traveler! Would you like a gameplay hint?", []() {
        static const char* options[] = {
            "Yes, please!",
            "No, I'm good."
        };
        gUIManager.showChoice("Hear a hint?", options, 2, [](int choice) {
            if (choice == 0) {
                gUIManager.showDialogue("Village Elder", "Press START anytime to open the Pause & Save menu!");
            } else {
                gUIManager.showDialogue("Village Elder", "Safe travels out there, adventurer!");
            }
        });
    });
}



