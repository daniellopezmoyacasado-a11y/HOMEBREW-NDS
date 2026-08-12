#ifndef ROOM_HPP
#define ROOM_HPP

#include <nds.h>
#include "Constants.hpp"
#include "Interactable.hpp"

static const int MAX_INTERACTABLES = 32;

class Room {
public:
    Room();
    void load_room(int room_id);
    bool collision[MAP_HEIGHT][MAP_WIDTH];
    bool dynamic_collision[MAP_HEIGHT][MAP_WIDTH];
    bool check_collision(int x, int y);
    bool check_collision_rect(int x, int y, int width, int height);
    void update_collision(int x, int y);
    void update_collision_rect(int x, int y, int width, int height);
    void reset_collision();

    bool register_interactable(Interactable* obj);
    void unregister_interactable(Interactable* obj);
    Interactable* get_interactable_at(int tileX, int tileY) const;
    void reset_interactables();

private:
    Interactable* interactables[MAX_INTERACTABLES];
    int interactableCount;
};


#endif
