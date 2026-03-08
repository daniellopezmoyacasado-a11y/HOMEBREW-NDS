#include <nds.h>
#include <stdio.h>
#include "player.hpp"

int main() {
    // Main engine in 2D mode
    videoSetMode(MODE_0_2D);

    // Put main sprite graphics in VRAM A
    vramSetBankA(VRAM_A_MAIN_SPRITE);

    // Init OAM
    oamInit(&oamMain, SpriteMapping_1D_32, false);

    Player player;
    player.init();

    while (pmMainLoop()) {
        player.update();
        player.draw();

        swiWaitForVBlank();
        oamUpdate(&oamMain);
    }

    return 0;
}