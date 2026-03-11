#include <nds.h>
#include <stdio.h>
#include "player.hpp"
#include "TestFloor.h"

int main() {
 
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankB(VRAM_B_MAIN_SPRITE);

    vramSetBankC(VRAM_C_SUB_BG);

    consoleDemoInit();
    iprintf("\n Build - Testing background\n");

    //vramSetBankB(VRAM_B_MAIN_BG);

    int bg = bgInit(0, BgType_Text8bpp, BgSize_T_256x256, 31, 0);

    dmaCopy(TestFloorTiles, bgGetGfxPtr(bg), TestFloorTilesLen);
    dmaCopy(TestFloorMap, bgGetMapPtr(bg), TestFloorMapLen);
    dmaCopy(TestFloorPal, BG_PALETTE, TestFloorPalLen);

    bgUpdate();

    oamInit(&oamMain, SpriteMapping_1D_32, false);

    Player player;
    player.init();

    while (pmMainLoop()) {
        player.update();
        player.draw();

        swiWaitForVBlank();
        oamUpdate(&oamMain);
        bgUpdate();
    }

    return 0;
}