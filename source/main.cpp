#include <nds.h>
#include <stdio.h>
#include "player.hpp"
#include "TestFloor.h"
#include "NDS-Player1.h"
#include "NDS-Player2.h"
#include "NPC.h"
#include "room.hpp"
#include "Game.hpp"
#include "NPC.hpp"
#include "GameState.hpp"

Room gRoom;
GameState gGameState;
UIManager gUIManager;

enum GameMode {
    MODE_MENU,
    MODE_PLAYING
};

static void applyPlayerPalette(int choice) {
    vramSetBankF(VRAM_F_LCD);
    if (choice == 1) {
        dmaCopy(NDS_Player1Pal, &VRAM_F_EXT_SPR_PALETTE[0][0], NDS_Player1PalLen);
    } else {
        dmaCopy(NDS_Player2Pal, &VRAM_F_EXT_SPR_PALETTE[0][0], NDS_Player2PalLen);
    }
    dmaCopy(NPCPal, &VRAM_F_EXT_SPR_PALETTE[1][0], NPCPalLen);
    vramSetBankF(VRAM_F_SPRITE_EXT_PALETTE);
}

static void drawMenuUI(int currentChoice) {
    iprintf("\x1b[1;0H=================================");
    iprintf("\x1b[2;0H     NDS GAME - STARTER MENU     ");
    iprintf("\x1b[3;0H=================================");
    iprintf("\x1b[5;0H Select your Starter Character:  ");

    if (currentChoice == 1) {
        iprintf("\x1b[7;0H > [1] Hero Alpha (Player 1) <   ");
        iprintf("\x1b[8;0H   [2] Hero Beta  (Player 2)     ");
    } else {
        iprintf("\x1b[7;0H   [1] Hero Alpha (Player 1)     ");
        iprintf("\x1b[8;0H > [2] Hero Beta  (Player 2) <   ");
    }

    iprintf("\x1b[11;0H Controls:                      ");
    iprintf("\x1b[12;0H  [D-PAD UP/DOWN] Change Choice ");
    iprintf("\x1b[13;0H  [A BUTTON] Confirm Selection  ");
}

int main() {
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankB(VRAM_B_MAIN_SPRITE);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleDemoInit();
    oamInit(&oamMain, SpriteMapping_1D_32, true);

    gUIManager.init();

    GameMode mode = MODE_MENU;
    int menuChoice = 1;

    Player player;
    player.init();
    player.setCharacterChoice(menuChoice);
    player.setPosition(120, 80);

    applyPlayerPalette(menuChoice);
    drawMenuUI(menuChoice);

    NPC npc;

    while (pmMainLoop()) {
        scanKeys();
        int down = keysDown();

        if (mode == MODE_MENU) {
            if (down & KEY_UP) {
                if (menuChoice != 1) {
                    menuChoice = 1;
                    player.setCharacterChoice(menuChoice);
                    applyPlayerPalette(menuChoice);
                    drawMenuUI(menuChoice);
                }
            } else if (down & KEY_DOWN) {
                if (menuChoice != 2) {
                    menuChoice = 2;
                    player.setCharacterChoice(menuChoice);
                    applyPlayerPalette(menuChoice);
                    drawMenuUI(menuChoice);
                }
            } else if (down & KEY_A) {
                gGameState.selectedCharacter = menuChoice;
                if (menuChoice == 1) {
                    gGameState.setFlag(FLAG_PLAYER_ONE, true);
                } else {
                    gGameState.setFlag(FLAG_PLAYER_ONE, false);
                }

                mode = MODE_PLAYING;
                consoleClear();
                iprintf("\n Build - Testing NPC build\n");
                iprintf("\x1b[3;0HPress [A] to interact / [START] Menu\n");
                iprintf("\x1b[19;0H[DEBUG] FLAG_TALK_NPC: FALSE          ");

                gRoom.load_room(1);
                player.setPosition(128, 80);

                npc.init();
                npc.registerCollision();
                gRoom.register_interactable(&npc);
            }

            player.draw();

            swiWaitForVBlank();
            oamUpdate(&oamMain);
            bgUpdate();
        } else {
            if (gUIManager.isOpen()) {
                gUIManager.update();
            } else if (down & KEY_START) {
                gUIManager.showPauseMenu();
            } else {
                player.update();

                int targetTileX = 0;
                int targetTileY = 0;
                if (player.checkInteractTrigger(targetTileX, targetTileY)) {
                    iprintf("\x1b[18;0H[DEBUG] Facing Tile: (%2d, %2d)       ", targetTileX, targetTileY);
                    Interactable* target = gRoom.get_interactable_at(targetTileX, targetTileY);
                    if (target) {
                        target->onInteract(player);
                    } else {
                        gUIManager.showDialogue("Inspect", "Just an empty floor tile.");
                    }
                }
            }

            npc.update();

            if (player.getFootY() >= npc.getFootY()) {
                player.setSpriteId(0);
                npc.setSpriteId(1);
            } else {
                npc.setSpriteId(0);
                player.setSpriteId(1);
            }

            player.draw();
            npc.draw();

            iprintf(
                "\x1b[19;0H[DEBUG] FLAG_TALK_NPC: %-5s          ",
                gGameState.hasFlag(FLAG_TALK_NPC) ? "TRUE" : "FALSE"
            );

            iprintf(
                "\x1b[20;0HPlayer px:%3d,%3d tile:%2d,%2d  ",
                player.getX(), player.getY(),
                player.getTileX(), player.getTileY()
            );


            swiWaitForVBlank();
            oamUpdate(&oamMain);
            bgUpdate();
        }
    }



    return 0;
}


