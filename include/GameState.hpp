#pragma once
#include <stdint.h>


enum GameFlag {
    FLAG_GOT_STARTER_PET = 0,
    FLAG_TALK_NPC,
    FLAG_SHOP_UNLOCKED,
    FLAG_PETS_UNLOCKED,
    FLAG_PLAYER_ONE,

    FLAG_COUNT
};

class GameState {
public:
    GameState();

    int gold;
    int totalGoldEarned;
    int dayNumber;

    int playerMapId;
    int playerTileX;
    int playerTileY;
    int selectedCharacter;


    bool hasFlag(GameFlag flag) const;
    void setFlag(GameFlag flag, bool value = true);
    void clearFlag(GameFlag flag);

    void addGold(int amount);
    bool spendGold(int amount);

    void checkProgressUnlocks();

    void reset();

private:
    bool flags[FLAG_COUNT];
};