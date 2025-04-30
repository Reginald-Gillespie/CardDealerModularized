#ifndef GAME_H
#define GAME_H

#include <Arduino.h>
#include "Enums.h"
#include "Definitions.h"
#include "Config.ino"

// Forward declare globals
extern dealState currentDealState;
extern displayState currentDisplayState;
extern bool postDeal;
extern uint8_t remainingRoundsToDeal;
extern uint8_t initialRoundsToDeal;
extern int8_t postCardsToDeal;
extern bool cardDealt; // To reset after manual deal
extern bool gameOver;
extern bool advanceOnePlayer;
extern const char* customFace;
extern uint8_t messageRepetitions;

// Forward declare core functions games might need
void dealSingleCard();
void advanceMenu(); // Although games shouldn't typically call this directly
void displayFace(const char* word);
void startScrollText(const char* text, uint16_t start, uint16_t delay, uint16_t end);
void updateScrollText();
void updateDisplay();
void stopScrollText();
void moveOffActiveColor(bool rotateClockwise);
void returnToActiveColor(bool rotateClockwise);
void handleFlipCard(); // Example of a core function a game might trigger

// Base class for all games
class Game {
  public:
    virtual ~Game() {}

    // ===== Required Methods ===
    // You MUST implement these== in your game.

    // Returns the display name of the game (e.g., "GO FISH")
    virtual const char* getName() const = 0;

    // Returns the messages to loop through
    virtual const String* getDisplayMessages(uint8_t &count) {
        static const String messages[] = { 
            F("Message 1"),
            F("Another message")
        };
        count = sizeof(messages) / sizeof(messages[0]);
        return messages;
    }

    // Called when the game is selected from the menu.
    // Set initial game parameters like rounds, post-deal cards.
    // Returns a value from the GamInitResult enum
    virtual bool initialize() = 0;

    // Handles button presses specifically when currentDealState is AWAITING_PLAYER_DECISION.
    // buttonPin: The pin number of the button pressed (e.g., BUTTON_PIN_1)
    virtual void handleButtonPress(int buttonPin) = 0;

    // ===== Optional Methods =====
    // You can change these, it is not required

    // Called just before the main dealing loop starts (after initialization to red, if applicable)
    virtual void onDealStart() {}

    // Called when the game is over (either by onMainDealEnd or other game logic)
    virtual void onGameOver() {}

    // Called when the main deal completes (remainingRoundsToDeal reaches 0)
    // Use this to transition to post-deal logic or declare game over.
    virtual void onMainDealEnd() {
        // Default behavior: if postCardsToDeal > 0, assume standard post-deal by awaiting decision.
        // If postCardsToDeal is 0 or less, set gameOver.
        if (postCardsToDeal <= 0) {
            gameOver = true; // TODO: this handling seems weird, look into - is it saying the game is over if it is out of cards to dish out to people?
        }
    }

    // Does this game require the user to select the number of cards per hand?
    virtual bool requiresCardSelection() const {
        return false; // Default: No
    }

    // Does this game require the user to select the number of players (for tagless)?
    virtual bool requiresPlayerSelection() const {
        return false; // Default: No
    }

    // Does this game involve flipping a card after the main deal?
    virtual bool requiresFlipCard() const {
        return false; // Default: No
    }


    // ===== Overridable Internals =====
    // These methods take care of complicated backend stuff
    //  If you really want to override them, go ahead.

    // Called every loop to manage display updates - by default it will cycle through the messages array
    virtual void handleAwaitDecisionDisplay() {
        // Serial.println("handleAwaitDecisionDisplay called"); // Debugging
        // Serial.print("messageRepetitions: "); Serial.println(messageRepetitions); // Debugging
        // Serial.print("lastMessageRepetitions: "); Serial.println(lastMessageRepetitions); // Debugging
        // Serial.print("displayMessageIndex: "); Serial.println(displayMessageIndex); // Debugging

        if (messageRepetitions != lastMessageRepetitions) {
            lastMessageRepetitions = messageRepetitions;
            displayMessageIndex++;

            uint8_t messageCount = 0;
            const String* messages = getDisplayMessages(messageCount);
            uint8_t nextIndex = 0;
            if (messageCount > 0) { // Catch no messages
                nextIndex = displayMessageIndex % messageCount;
                const String thisMessage = messages[nextIndex];
                Serial.println("Updating message");
                startScrollText(thisMessage.c_str(), textStartHoldTime, textSpeedInterval, textEndHoldTime);
                scrollingStarted = true;
            }
        }

        updateScrollText();
    }

  protected:
    // Variables
    displayState lastDisplayState = DISPLAY_UNSET;
    bool scrollingStarted = false;
    int displayMessageIndex = 0;

    // uint8_t messageRepetitions = 0; // TODO move handling of message lines into display array that is managed here...

    void dispenseCards(uint8_t amount) {
        for (uint8_t i = 0; i < amount; ++i) {
            _dealSingleCard();
        }
    }

    void resetScrollingMessages() {
        displayMessageIndex = 0;
        scrollingStarted = true;
        lastMessageRepetitions = -2;
    }

    // Display face wrapper to handle custom faces
    void displayFace(const char* face) {
        // TODO: handle scrolling face
        lastDisplayState = currentDisplayState;
        customFace = face;
        currentDisplayState = displayState::CUSTOM_FACE;
        updateDisplay();
    }

    void restoreFace() {
        if (lastDisplayState != DISPLAY_UNSET) {
            currentDisplayState = lastDisplayState;
        }
        lastDisplayState = DISPLAY_UNSET;
    }

    // Player passes, advance to the next player
    void nextPlayersTurn() {
        // TODO: add support for moving multiple people forwards
        advanceOnePlayer = true;      // Signal core logic to advance one player
        currentDealState = ADVANCING; // Change state to advancing
    }

  private:
    // Internal tracking

    // Known when to switch shown messages
    int lastMessageRepetitions = -2;

    // Internal util functions

    void _dealSingleCard() {
        dealSingleCard();
        cardDealt = false;
    }
};

#endif // GAME_H