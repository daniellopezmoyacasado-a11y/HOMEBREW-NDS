#ifndef INTERACTABLE_HPP
#define INTERACTABLE_HPP

class Player;

class Interactable {
public:
    virtual ~Interactable() {}
    virtual bool isAtTile(int tileX, int tileY) const = 0;
    virtual void onInteract(Player& player) = 0;
};

#endif
