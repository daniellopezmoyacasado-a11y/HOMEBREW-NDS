#include "UIManager.hpp"
#include <stdio.h>
#include <cstring>
#include "GameState.hpp"
#include "Game.hpp"

static const int TYPEWRITER_SPEED = 2; // advance 1 char every 2 frames

UIManager::UIManager()
    : currentState(UI_STATE_NONE),
      textLength(0),
      visibleChars(0),
      typeTimer(0),
      onDialogueFinish(nullptr),
      choiceCount(0),
      selectedIndex(0),
      onChoiceSelect(nullptr),
      pauseSelectedIndex(0) {
    dialogueSpeaker[0] = '\0';
    dialogueText[0] = '\0';
    choicePrompt[0] = '\0';
}

void UIManager::init() {
    closeUI();
}

void UIManager::closeUI() {
    currentState = UI_STATE_NONE;
    onDialogueFinish = nullptr;
    onChoiceSelect = nullptr;

    // Clear UI region on console screen (lines 4-15)
    for (int line = 4; line <= 16; ++line) {
        iprintf("\x1b[%d;0H                                ", line);
    }
}

void UIManager::showDialogue(const char* speaker, const char* text, DialogueFinishedCallback onFinish) {
    strncpy(dialogueSpeaker, speaker ? speaker : "Info", sizeof(dialogueSpeaker) - 1);
    dialogueSpeaker[sizeof(dialogueSpeaker) - 1] = '\0';

    strncpy(dialogueText, text ? text : "", sizeof(dialogueText) - 1);
    dialogueText[sizeof(dialogueText) - 1] = '\0';

    textLength = strlen(dialogueText);
    visibleChars = 0;
    typeTimer = 0;
    onDialogueFinish = onFinish;

    currentState = UI_STATE_DIALOGUE;
    drawDialogueUI();
}

void UIManager::showChoice(const char* prompt, const char* options[], int count, ChoiceCallback onSelect) {
    strncpy(choicePrompt, prompt ? prompt : "Select option:", sizeof(choicePrompt) - 1);
    choicePrompt[sizeof(choicePrompt) - 1] = '\0';

    choiceCount = (count > 4) ? 4 : count;
    for (int i = 0; i < choiceCount; ++i) {
        choiceOptions[i] = options[i];
    }

    selectedIndex = 0;
    onChoiceSelect = onSelect;

    currentState = UI_STATE_CHOICE;
    drawChoiceUI();
}

void UIManager::showPauseMenu() {
    pauseSelectedIndex = 0;
    currentState = UI_STATE_PAUSE_MENU;
    drawPauseMenuUI();
}

void UIManager::update() {
    if (currentState == UI_STATE_NONE) return;

    u32 down = keysDown();


    switch (currentState) {
        case UI_STATE_DIALOGUE: {
            if (visibleChars < textLength) {
                typeTimer++;
                if (typeTimer >= TYPEWRITER_SPEED) {
                    typeTimer = 0;
                    visibleChars++;
                    drawDialogueUI();
                }
            }

            if (down & KEY_A) {
                if (visibleChars < textLength) {
                    visibleChars = textLength;
                    drawDialogueUI();
                } else {
                    DialogueFinishedCallback cb = onDialogueFinish;
                    closeUI();
                    if (cb) cb();
                }
            }
            break;
        }

        case UI_STATE_CHOICE: {
            if (down & KEY_UP) {
                if (selectedIndex > 0) {
                    selectedIndex--;
                    drawChoiceUI();
                }
            } else if (down & KEY_DOWN) {
                if (selectedIndex < choiceCount - 1) {
                    selectedIndex++;
                    drawChoiceUI();
                }
            } else if (down & KEY_A) {
                ChoiceCallback cb = onChoiceSelect;
                int chosen = selectedIndex;
                closeUI();
                if (cb) cb(chosen);
            } else if (down & KEY_B) {
                closeUI();
            }
            break;
        }

        case UI_STATE_PAUSE_MENU: {
            if (down & KEY_UP) {
                if (pauseSelectedIndex > 0) {
                    pauseSelectedIndex--;
                    drawPauseMenuUI();
                }
            } else if (down & KEY_DOWN) {
                if (pauseSelectedIndex < 3) {
                    pauseSelectedIndex++;
                    drawPauseMenuUI();
                }
            } else if (down & (KEY_START | KEY_B)) {
                closeUI();
            } else if (down & KEY_A) {
                int action = pauseSelectedIndex;
                closeUI();
                if (action == 1) {
                    // Save Game Action
                    showDialogue("System", "Game Progress Saved successfully!");
                } else if (action == 2) {
                    // Character Info Action
                    char statusBuf[64];
                    snprintf(statusBuf, sizeof(statusBuf), "Day:%d Gold:%d Pet:%s", 
                        gGameState.dayNumber, 
                        gGameState.gold,
                        gGameState.hasFlag(FLAG_GOT_STARTER_PET) ? "Yes" : "No");
                    showDialogue("Status", statusBuf);
                }
            }
            break;
        }

        default:
            break;
    }
}

void UIManager::drawDialogueUI() {
    iprintf("\x1b[4;0H+--------------------------------+");
    iprintf("\x1b[5;0H| [%-26s] |", dialogueSpeaker);
    iprintf("\x1b[6;0H+--------------------------------+");

    char tempBuf[128];
    int len = (visibleChars <= textLength) ? visibleChars : textLength;
    memcpy(tempBuf, dialogueText, len);
    tempBuf[len] = '\0';

    iprintf("\x1b[7;0H| %-30s |", tempBuf);
    iprintf("\x1b[8;0H|                                |");
    
    if (visibleChars >= textLength) {
        iprintf("\x1b[9;0H|                     Press [A] >|");
    } else {
        iprintf("\x1b[9;0H|                                |");
    }
    iprintf("\x1b[10;0H+--------------------------------+");
}

void UIManager::drawChoiceUI() {
    iprintf("\x1b[4;0H+--------------------------------+");
    iprintf("\x1b[5;0H| %-30s |", choicePrompt);
    iprintf("\x1b[6;0H+--------------------------------+");

    for (int i = 0; i < 4; ++i) {
        if (i < choiceCount) {
            if (i == selectedIndex) {
                iprintf("\x1b[%d;0H| > %-28s |", 7 + i, choiceOptions[i]);
            } else {
                iprintf("\x1b[%d;0H|   %-28s |", 7 + i, choiceOptions[i]);
            }
        } else {
            iprintf("\x1b[%d;0H|                                |", 7 + i);
        }
    }
    iprintf("\x1b[11;0H+--------------------------------+");
}

void UIManager::drawPauseMenuUI() {
    iprintf("\x1b[4;0H+--------------------------------+");
    iprintf("\x1b[5;0H|         IN-GAME MENU           |");
    iprintf("\x1b[6;0H+--------------------------------+");

    const char* menuItems[4] = {
        "Resume Game",
        "Save Progress",
        "Character Status",
        "Close Menu"
    };

    for (int i = 0; i < 4; ++i) {
        if (i == pauseSelectedIndex) {
            iprintf("\x1b[%d;0H| > %-28s |", 7 + i, menuItems[i]);
        } else {
            iprintf("\x1b[%d;0H|   %-28s |", 7 + i, menuItems[i]);
        }
    }
    iprintf("\x1b[11;0H+--------------------------------+");
}

void UIManager::draw() {
    if (currentState == UI_STATE_DIALOGUE) {
        drawDialogueUI();
    } else if (currentState == UI_STATE_CHOICE) {
        drawChoiceUI();
    } else if (currentState == UI_STATE_PAUSE_MENU) {
        drawPauseMenuUI();
    }
}
