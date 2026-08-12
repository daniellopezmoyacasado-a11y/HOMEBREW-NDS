#ifndef UI_MANAGER_HPP
#define UI_MANAGER_HPP

#include <nds.h>

enum UIState {
    UI_STATE_NONE = 0,
    UI_STATE_DIALOGUE,
    UI_STATE_CHOICE,
    UI_STATE_PAUSE_MENU
};

typedef void (*DialogueFinishedCallback)();
typedef void (*ChoiceCallback)(int choiceIndex);

class UIManager {
public:
    UIManager();

    void init();
    void update();
    void draw();

    void showDialogue(const char* speaker, const char* text, DialogueFinishedCallback onFinish = nullptr);
    void showChoice(const char* prompt, const char* options[], int count, ChoiceCallback onSelect);
    void showPauseMenu();
    void closeUI();

    bool isOpen() const { return currentState != UI_STATE_NONE; }
    UIState getState() const { return currentState; }

private:
    void updateDialogueInput();
    void updateChoiceInput();
    void updatePauseMenuInput();

    void drawDialogueUI();
    void drawChoiceUI();
    void drawPauseMenuUI();

    UIState currentState;

    // Dialogue State
    char dialogueSpeaker[32];
    char dialogueText[128];
    int textLength;
    int visibleChars;
    int typeTimer;
    DialogueFinishedCallback onDialogueFinish;

    // Choice State
    char choicePrompt[64];
    const char* choiceOptions[4];
    int choiceCount;
    int selectedIndex;
    ChoiceCallback onChoiceSelect;

    // Pause Menu State
    int pauseSelectedIndex;
};

#endif
