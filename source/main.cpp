#include <nds.h>
#include <stdio.h>

static int ballX = 120;
static int ballY = 80;
static int ballVX = 2;
static int ballVY = 2;

static int paddleY = 70;
static int score = 0;
static int misses = 0;

void drawPixel(int x, int y, u16 color)
{
    if (x < 0 || x >= 256 || y < 0 || y >= 192) return;
    VRAM_A[y * 256 + x] = color;
}

void clearScreen()
{
    for (int i = 0; i < 256 * 192; i++) {
        VRAM_A[i] = RGB15(0, 0, 0);
    }
}

void drawRect(int x, int y, int w, int h, u16 color)
{
    for (int iy = 0; iy < h; iy++) {
        for (int ix = 0; ix < w; ix++) {
            drawPixel(x + ix, y + iy, color);
        }
            
    }
        
}

void waitFrames(int frames)
{ //aux: 60 frames == 1 second
    for(int i = 0; i < frames; i++)
        swiWaitForVBlank();
}

void resetBall()
{
    ballX = 120;
    ballY = 80;

    ballVX = 2;
    ballVY = 2;

    if ((score + misses) % 2 == 0) ballVX = -ballVX;
    waitFrames(40);
}

void updateScoreScreen()
{
    consoleClear();
    iprintf("\n");
    iprintf("\n");
    iprintf("\n");
    
    iprintf("NDS PONG\n");
    iprintf("===========\n\n");
    iprintf("Score  : %d\n", score);
    iprintf("Misses : %d\n\n", misses);
    iprintf("START   - Quit\n");
}



int main(void)
{
    // Bottom screen: text console
    videoSetModeSub(MODE_0_2D);
    vramSetBankC(VRAM_C_SUB_BG);
    consoleDemoInit();

    // Top screen: framebuffer game
    videoSetMode(MODE_FB0);
    vramSetBankA(VRAM_A_LCD);

    
    clearScreen();
    drawRect(5, paddleY, 5, 30, RGB15(31, 31, 31));
    drawRect(ballX, ballY, 4, 4, RGB15(31, 31, 31));
    updateScoreScreen();
    waitFrames(40);

    bool game_active = true;

    while (game_active)
    {
        scanKeys();
        int held = keysHeld();
        int pressed = keysDown();

        if (pressed & KEY_START)
            game_active = false;

        if (held & KEY_UP)   paddleY -= 3;
        if (held & KEY_DOWN) paddleY += 3;

        if (paddleY < 0)   paddleY = 0;
        if (paddleY > 162) paddleY = 162;

        ballX += ballVX;
        ballY += ballVY;

        if (ballY <= 0) {
            ballY = 0;
            ballVY = -ballVY;
        }
        if (ballY >= 188) {
            ballY = 188;
            ballVY = -ballVY;
        }

        if (ballX >= 252) {
            ballX = 252;
            ballVX = -ballVX;
        }

       
        if (ballVX < 0 &&
            ballX <= 10 &&
            ballY + 3 >= paddleY &&
            ballY <= paddleY + 30) // Paddle collision 
        {
            ballX = 10;
            ballVX = -ballVX;
            score++;
            updateScoreScreen();
        }

        // ball missed
        if (ballX < 0) {
            misses++;
            updateScoreScreen();
            resetBall();
        }

        clearScreen();

        drawRect(5, paddleY, 5, 30, RGB15(31, 31, 31));

        drawRect(ballX, ballY, 4, 4, RGB15(31, 31, 31));

        swiWaitForVBlank();
    }

    consoleClear();
    iprintf("GAVE OVER.\n\n");
    iprintf("Final score : %d\n", score);
    iprintf("Total misses: %d\n", misses);

    //while (1) swiWaitForVBlank(); //game ends too quickly... so it did some weird things
    waitFrames(120);

    return 0;
}