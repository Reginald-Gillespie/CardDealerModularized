#ifndef GAME_H
#define GAME_H

//
//  In general, the idea behind this class is to abstract all functionality needed for games in here.
//  Individual games can then implement different handling for their game by overriding methods here.
// 
//  Ideally, any methods that have complicated handling for all games have an underscore in their name,
//    like "_method". This "_method" will both preform complex operations, and then call the "method" 
//    of the subclass to allow it to handle that specific event, without requiring any complex functionality.
// 

#include <Arduino.h>
#include "Enums.h"
#include "Definitions.h"
#include "Config.h"

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
    uint8_t turnsToAdvance = 0; // Used to advance multiple people forwards
    bool lockedFace = false; // Let the main file known we want the face locked to what we set it to

    // ===== Required Methods ===
    // You MUST implement these in your game.

    // Returns the display name of the game (e.g., "GO FISH")
    virtual const char* getName() const = 0;

    // Returns the messages to loop through
    virtual const char** getDisplayMessages(uint8_t &count) {
        static const char* messages[] = { 
            "START", // Messages here will scroll
            // "Another message"
        };
        count = sizeof(messages) / sizeof(messages[0]);
        return messages;
    }

    // Called when the game is selected from the menu.
    // Set initial game parameters like rounds, post-deal cards.
    // Returns a value from the GamInitResult enum
    virtual bool initialize() = 0;

    // Handles button presses specifically when currentDealState is AWAITING_PLAYER_DECISION.
    // button: The pin number of the button pressed (e.g., BUTTON_PIN_1)
    virtual void handleButtonPress(int button) = 0;

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
        if (lockedFace) return; // Don't run update the display if we've locked a face or a message 
        // TODO: locked face needs to support scrolling messages

        if (messageRepetitions != lastMessageRepetitions) {
            lastMessageRepetitions = messageRepetitions;
            displayMessageIndex++;

            uint8_t messageCount = 0;
            const char** messages = getDisplayMessages(messageCount);
            uint8_t nextIndex = 0;
            if (messageCount > 0) { // Catch no messages
                nextIndex = displayMessageIndex % messageCount;
                const char* thisMessage = messages[nextIndex];
                Serial.println("Updating message");
                startScrollText(thisMessage, textStartHoldTime, textSpeedInterval, textEndHoldTime);
                scrollingStarted = true;
            }
        }

        updateScrollText();
    }

    // Internal function dispatch buttons then restart scrolling text
    virtual void _handleButtonPress(int button) {
        // Call the subclass's internal method.
        handleButtonPress(button);

        // Start up scrolling messages
        resetScrollingMessages();
    }

    // Internal function to run when deal ends, which calls when it finishes running internal code
    virtual void _onMainDealEnd() {
        // Call the subclass's deal end code.
        onMainDealEnd();

        // Start up scrolling messages
        resetScrollingMessages();
    }

  protected:
    // Variables
    displayState lastDisplayState = DISPLAY_UNSET;
    bool scrollingStarted = false;
    int displayMessageIndex = 0;

    void dispenseCards(uint8_t amount) {
        for (uint8_t i = 0; i < amount; ++i) {
            _dealSingleCard();
        }
    }

    // Reset the scrolling messages to the start
    void resetScrollingMessages() {
        if (lockedFace) return;
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

    // Allow normall scrolling text
    void unlockFace() {
        lockedFace = false;
    }

    // Prevent scrolling text from taking over the display
    void lockFace() {
        lockedFace = true;
    }

    // Restore whatever state the face was in before running displayFace
    void restoreFace() {
        if (lastDisplayState != DISPLAY_UNSET) {
            currentDisplayState = lastDisplayState;
        }
        lastDisplayState = DISPLAY_UNSET;
    }

    // Player passes, advance to the next player
    // Accepts a parameter for how many people to skip while advancing.
    void nextTurn(uint8_t skipNumber=0) {
        // TODO: add support for moving multiple people forwards
        advanceOnePlayer = true;      // Signal core logic to advance one player
        currentDealState = ADVANCING; // Change state to advancing

        turnsToAdvance = skipNumber + 1;
    }

    void setDealAmount(uint8_t amount) {
        initialRoundsToDeal = amount;
        postCardsToDeal = 127;                       // Use a large number to signify 'deal until empty' logic needed later
        remainingRoundsToDeal = initialRoundsToDeal; // Second value has smth to do with rigged game handling?
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