#include "GameState.hpp"

GameState::GameState() {
    reset();
}

void GameState::reset() {
    gold = 0;
    totalGoldEarned = 0;
    dayNumber = 1;

    playerMapId = 0;
    playerTileX = 0;
    playerTileY = 0;
    selectedCharacter = 1;

    for (int i = 0; i < FLAG_COUNT; i++) {

        flags[i] = false;
    }
}

bool GameState::hasFlag(GameFlag flag) const {
    return flags[(int)flag];
}

void GameState::setFlag(GameFlag flag, bool value) {
    flags[(int)flag] = value;
}

void GameState::clearFlag(GameFlag flag) {
    flags[(int)flag] = false;
}

void GameState::addGold(int amount) {
    if (amount <= 0) return;

    gold += amount;
    totalGoldEarned += amount;

    checkProgressUnlocks();
}

bool GameState::spendGold(int amount) {
    if (amount <= 0) return true;
    if (gold < amount) return false;

    gold -= amount;
    return true;
}

void GameState::checkProgressUnlocks() {
    // Progression flag checks can be added here
}

