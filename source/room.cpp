#include "player.hpp"
#include <cstring>
#include <nds.h>
#include "TestFloor.h"
#include "FloorCollision.hpp"
#include "room.hpp"


Room::Room() {
    reset_collision();
    reset_interactables();
}

void Room::load_room(int id) {
    reset_interactables();
    if (id == 1) { //TestFloor or Test Room
        int bg = bgInit(0, BgType_Text8bpp, BgSize_T_256x256, 31, 0);

        dmaCopy(TestFloorTiles, bgGetGfxPtr(bg), TestFloorTilesLen);
        dmaCopy(TestFloorMap, bgGetMapPtr(bg), TestFloorMapLen);
        dmaCopy(TestFloorPal, BG_PALETTE, TestFloorPalLen);

        bgUpdate();
        
        for (int y = 0; y < 12; y++) {
            for (int x = 0; x < 16; x++) {
                collision[y][x] = testRoomCollision[(y * MAP_WIDTH) + x];
            }
        }
    }
}



bool Room::check_collision(int x, int y) {
    int tileX = x / TILE_SIZE;
    int tileY = y / TILE_SIZE;

    if (tileX < 0 || tileX >= MAP_WIDTH || tileY < 0 || tileY >= MAP_HEIGHT) {
        return true;
    }    
    
   return (collision[tileY][tileX] || dynamic_collision[tileY][tileX]);

}

bool Room::check_collision_rect(int x, int y, int width, int height) {
    if (width <= 0 || height <= 0) return false;

    int right = x + width - 1;
    int bottom = y + height - 1;

    if (x < 0 || y < 0 || right >= MAP_WIDTH * TILE_SIZE || bottom >= MAP_HEIGHT * TILE_SIZE) {
        return true;
    }

    int leftTile = x / TILE_SIZE;
    int rightTile = right / TILE_SIZE;
    int topTile = y / TILE_SIZE;
    int bottomTile = bottom / TILE_SIZE;

    for (int tileY = topTile; tileY <= bottomTile; tileY++) {
        for (int tileX = leftTile; tileX <= rightTile; tileX++) {
            if (collision[tileY][tileX] || dynamic_collision[tileY][tileX]) {
                return true;
            }
        }
    }

    return false;
}

void Room::update_collision(int x, int y) {
    int tileX = x / TILE_SIZE;
    int tileY = y / TILE_SIZE;

    if (tileX < 0 || tileX >= MAP_WIDTH || tileY < 0 || tileY >= MAP_HEIGHT) {
        return;
    }    
    
   dynamic_collision[tileY][tileX] = true;

}

void Room::update_collision_rect(int x, int y, int width, int height) {
    if (width <= 0 || height <= 0) return;

    int leftTile = x / TILE_SIZE;
    int rightTile = (x + width - 1) / TILE_SIZE;
    int topTile = y / TILE_SIZE;
    int bottomTile = (y + height - 1) / TILE_SIZE;

    for (int tileY = topTile; tileY <= bottomTile; tileY++) {
        for (int tileX = leftTile; tileX <= rightTile; tileX++) {
            if (tileX >= 0 && tileX < MAP_WIDTH && tileY >= 0 && tileY < MAP_HEIGHT) {
                dynamic_collision[tileY][tileX] = true;
            }
        }
    }
}

void Room::reset_collision() {
    for (int y = 0; y < 12; y++) {
        for (int x = 0; x < 16; x++) {
            collision[y][x] = false;
            dynamic_collision[y][x] = false;
        }
    }
}

bool Room::register_interactable(Interactable* obj) {
    if (!obj || interactableCount >= MAX_INTERACTABLES) return false;
    for (int i = 0; i < interactableCount; ++i) {
        if (interactables[i] == obj) return true;
    }
    interactables[interactableCount++] = obj;
    return true;
}

void Room::unregister_interactable(Interactable* obj) {
    for (int i = 0; i < interactableCount; ++i) {
        if (interactables[i] == obj) {
            interactables[i] = interactables[interactableCount - 1];
            interactables[--interactableCount] = nullptr;
            return;
        }
    }
}

Interactable* Room::get_interactable_at(int tileX, int tileY) const {
    for (int i = 0; i < interactableCount; ++i) {
        if (interactables[i] && interactables[i]->isAtTile(tileX, tileY)) {
            return interactables[i];
        }
    }
    return nullptr;
}

void Room::reset_interactables() {
    for (int i = 0; i < MAX_INTERACTABLES; ++i) {
        interactables[i] = nullptr;
    }
    interactableCount = 0;
}

