#include "Arduino.h"
#include "Enums.h"
#include "Game.h"

// Let C know that these will be declared by this file importing this one - TODO: see what can be defined in here instead
extern displayState currentDisplayState;
extern dealState currentDealState;
extern Game* currentGamePtr;
extern uint16_t scrollDelayTime;
extern unsigned long overallTimeoutTag;
extern int scrollIndex;
extern int8_t messageLine;
extern int8_t currentToolsMenu;
extern int8_t currentGame;
extern uint8_t messageRepetitions;
extern bool scrollingStarted;
extern bool scrollingMenu;
extern bool blinkingAnimationActive;
extern bool buttonInitialization;
extern bool toolsExit;
extern bool gamesExit;
extern bool toolsMenuActive;
extern bool initialAnimationInProgress;
extern bool scrollingComplete;
extern bool initialAnimationComplete;
extern bool scrollInstructions;
extern bool numCardsLocked;

void displayFace(const char* word);
void rotateStop();
void flywheelOff();
void startPreGameAnimation();
void advanceMenu();
void increaseSetting();
void decreaseSetting();

// Returns to prior menu, or exits program.
void goBack() {
    if (verbose) {
        Serial.println(F("Going back one menu."));
    }
    scrollingMenu = false;
    switch (currentDisplayState) {
        case SELECT_PLAYERS:
            numCardsLocked = false;
            toolsMenuActive = false; // Switching to select game menu. Deactivating Tools menu.
            currentDisplayState = SELECT_CARDS;
            break;

        case DEAL_CARDS:
            currentDisplayState = SELECT_CARDS;
            break;

        case SELECT_CARDS:
            toolsMenuActive = false; // Switching to select game menu. Deactivating Tools menu.
            currentDisplayState = SELECT_GAME;
            break;

        case SELECT_TOOL:
            toolsMenuActive = false; // Switching to select game menu. Deactivating Tools menu.
            currentDisplayState = SELECT_GAME;
            break;

        case SELECT_GAME:
            currentGame = 0;
            currentToolsMenu = 0;
            toolsMenuActive = false;
            buttonInitialization = false;
            scrollingStarted = false;

            // If instructions are enabled, going back when in the "select game" menu goes to instructions.
            if (scrollInstructions) {
                currentDisplayState = SCROLL_PICK_GAME_TEXT;
                blinkingAnimationActive = false;
            } else { // Otherwise, we just play the blinking animation.
                initialAnimationComplete = false;
                currentDisplayState = INTRO_ANIM;
            }
            currentDealState = IDLE;
            break;

        case SCROLL_PICK_GAME_TEXT:
            messageRepetitions = 2; // Skip scrolling text when going back
            buttonInitialization = false;
            currentDisplayState = SCROLL_PLACE_TAGS_TEXT;
            blinkingAnimationActive = false;
            scrollingStarted = false;
            scrollingComplete = false;
            startPreGameAnimation();
            break;

        case SCROLL_PLACE_TAGS_TEXT:
            // We don't go back to the quick intro blink, we just stay put.
            buttonInitialization = false;
            break;

        case INTRO_ANIM:
            // Nothing happens when we go back, but we can make sure buttonInitialization = false so the screensaver starts.
            buttonInitialization = false;
            break;

        case SCREENSAVER:
            // If we're watching the screensaver and someone hits the back button, the system should wake up.
            advanceMenu();
            break;

        case LOOK_STRAIGHT:
            break;

        case LOOK_RIGHT:
            break;

        case LOOK_LEFT:
            break;

        case STRUGGLE:
            break;

        case FLIP:
            break;

        case ERROR:
            break;
    }
}

void exitButtonAction() {
    if (currentDealState != IDLE) {
        if (toolsMenuActive) {
            toolsExit = true;
        } else {
            gamesExit = true;
        }
        displayFace("EXIT");
        rotateStop();
        flywheelOff();
        delay(1000);
        currentDealState = RESET_DEALR;
    } else {
        goBack();
    }
}

void onButton1Release() {
    // This function handles what happens when we release Button 1 (green). It matters that the action happens on "release" so we can use long-click in some cases.

    // If we're playing a game and awaiting a player decision, do one of these things based on the game:
    if (currentDealState == AWAITING_PLAYER_DECISION) {
        if (currentGamePtr) {
            currentGamePtr->_handleButtonPress(BUTTON_PIN_1); // TODO: I'd like to change this from passing the button pin to passing a button enum of the color
        }
        // updateDisplay();
    } else {
        advanceMenu(); // If we're just in one of the menus, Button 1 is the "confirm and advance" button.
    }
}

void onButton2Release() {
    // This function handles what happens when we release Button 2 (blue).
    if (currentDealState == AWAITING_PLAYER_DECISION) {
        if (currentGamePtr) {
            currentGamePtr->_handleButtonPress(BUTTON_PIN_2);
        }
        // updateDisplay();
    }

    if (currentDisplayState == SCREENSAVER || currentDisplayState == SCROLL_PLACE_TAGS_TEXT || currentDisplayState == SCROLL_PICK_GAME_TEXT) {
        advanceMenu();
    } else if (currentDisplayState == SELECT_CARDS || currentDisplayState == SELECT_PLAYERS || currentDisplayState == SELECT_TOOL || currentDisplayState == SELECT_GAME) {
        increaseSetting();
    }
}

void onButton3Release() {
    // This function handles what happens when we release Button 3 (yellow).

    if (currentDealState == AWAITING_PLAYER_DECISION) {
        if (currentGamePtr) {
            currentGamePtr->_handleButtonPress(BUTTON_PIN_3); // If Yellow had a game function
        }
    } else if (currentDisplayState == SCREENSAVER || currentDisplayState == SCROLL_PLACE_TAGS_TEXT || currentDisplayState == SCROLL_PICK_GAME_TEXT) {
        advanceMenu();
    } else if (currentDisplayState == SELECT_PLAYERS || currentDisplayState == SELECT_CARDS || currentDisplayState == SELECT_GAME || currentDealState == IDLE) {
        decreaseSetting();
    }
}

void onButton4Release() {
    // Red button exits, unless we're in a game waiting for player decision, in which case we want to use this button for actions

    if (currentDealState == AWAITING_PLAYER_DECISION) {
        if (currentGamePtr) {
            currentGamePtr->_handleButtonPress(BUTTON_PIN_4); // If Yellow had a game function
        }
    } else {
        // Otherwise use it as the exit button
        exitButtonAction();
    }
}

void onButton1LongPress() {
    // Serial.println("Button 1 long-press"); //Currently there is no application of Button 1 long-press.
}

void onButton2LongPress() {
    overallTimeoutTag = millis();
    scrollDelayTime = 0;
    scrollingStarted = false;
    scrollingMenu = false;
    messageRepetitions = 0;
    scrollIndex = -1;

    toolsMenuActive = true;
    currentToolsMenu = 3;
    currentDisplayState = SELECT_TOOL;
    updateDisplay();
    buttonInitialization = true;
    blinkingAnimationActive = false;
    initialAnimationInProgress = false;
    scrollingComplete = true;
}

void onButton3LongPress() {
    overallTimeoutTag = millis();
    scrollDelayTime = 0;
    scrollingStarted = false;
    scrollingMenu = false;
    messageRepetitions = 0;
    scrollIndex = -1;
    buttonInitialization = true;

    // Serial.println("Shortcut to *5. UV SENSOR TUNER");
    toolsMenuActive = true;
    currentToolsMenu = 4;
    currentDisplayState = SELECT_TOOL;
    updateDisplay();
    buttonInitialization = true;
    blinkingAnimationActive = false;
    initialAnimationInProgress = false;
    scrollingComplete = true;
}

void onButton4LongPress() {
    if (currentDealState == AWAITING_PLAYER_DECISION) {
        // Preform exit action with long-press on APD, this allows us to use a single red button press for an action
        exitButtonAction();

    } else {
        overallTimeoutTag = millis();
        scrollDelayTime = 0;
        scrollingStarted = false;
        scrollingMenu = false;
        messageRepetitions = 0;
        scrollIndex = -1;

        // Serial.println("Shortcut to *1. DEAL SINGLE CARD");
        toolsMenuActive = true;
        currentToolsMenu = 0;
        currentDisplayState = SELECT_TOOL;
        updateDisplay();
        buttonInitialization = true;
        blinkingAnimationActive = false;
        initialAnimationInProgress = false;
        scrollingComplete = true;
    }
}

void resetTagsOnButtonPress() {
    overallTimeoutTag = millis();    // Reset tag for overall timeout every time button is pressed.
    scrollDelayTime = 0;             // Force any scrolling text to start scrolling immediately.
    scrollingStarted = false;        // Reset scrollingStarted tag.
    scrollingMenu = false;           // Reset scrolling menu tag each time button is pressed.
    messageLine = 0;                 // Reset line that's scrolling in cases where several lines scroll.
    messageRepetitions = 0;          // Every time a button is pressed, reset messageRepetitions tag.
    scrollIndex = -1;                // Reset the scroll index.
    blinkingAnimationActive = false; // Reset the screensaver animation, since a button press indicates a user is present
}

// This demanding function handles everything related to button-pushing in DEALR.
void checkButton(int buttonPin, unsigned long& lastPress, int& lastButtonState, unsigned long& pressTime, bool& longPressFlag, uint16_t longPressDuration, void (*onRelease)(), void (*onLongPress)()) {
    int currentButtonState = digitalRead(buttonPin); // Read the current button state.

    if (currentButtonState == LOW) // If the button has been pressed...

    {
        if (lastButtonState == HIGH) // ... and if the button wasn't already being pressed...
        {
            pressTime = millis(); // Record press time.
        }

        if (!longPressFlag && millis() - pressTime >= longPressDuration) // Check for long press.
        {
            longPressFlag = true;
            if (onLongPress != nullptr) {
                resetTagsOnButtonPress();
                onLongPress(); // Trigger the long press action.
            }
        }
    }

    if (lastButtonState == LOW && currentButtonState == HIGH) // Handle button release.
    {
        lastPress = millis();

        if (!longPressFlag && onRelease != nullptr) // If not a long press, trigger the normal release action.
        {
            resetTagsOnButtonPress();
            onRelease(); // Trigger the release action.
            if (currentDisplayState != SCROLL_PLACE_TAGS_TEXT) {
                buttonInitialization = true; // A button has been pressed, so we know not to start the screensaver blinking animation, except when we've hit "back" all the way to the beginning.
            }
        }
        longPressFlag = false; // Reset the long-press flag after release
    }

    lastButtonState = currentButtonState; // Update the last button state
}

void checkButtons() {
    static unsigned long lastPress1 = millis(), lastPress2 = millis(),
                         lastPress3 = millis(), lastPress4 = millis();
    static int lastButtonState1 = HIGH, lastButtonState2 = HIGH,
               lastButtonState3 = HIGH, lastButtonState4 = HIGH;
    static unsigned long pressTime1 = 0, pressTime2 = 0,
                         pressTime3 = 0, pressTime4 = 0;
    static bool longPress1 = false, longPress2 = false,
                longPress3 = false, longPress4 = false;

    // Call the checkButton function for each button
    checkButton(BUTTON_PIN_1, lastPress1, lastButtonState1, pressTime1, longPress1, 3000, onButton1Release, onButton1LongPress);
    checkButton(BUTTON_PIN_2, lastPress2, lastButtonState2, pressTime2, longPress2, 3000, onButton2Release, onButton2LongPress);
    checkButton(BUTTON_PIN_3, lastPress3, lastButtonState3, pressTime3, longPress3, 3000, onButton3Release, onButton3LongPress);
    checkButton(BUTTON_PIN_4, lastPress4, lastButtonState4, pressTime4, longPress4, 3000, onButton4Release, onButton4LongPress);
}