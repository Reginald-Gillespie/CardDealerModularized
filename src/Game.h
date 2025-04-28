#ifndef GAME_H
#define GAME_H

#include <Arduino.h>
#include "Enums.h"
#include "Definitions.h"
#include "Config.ino"

// Forward declare global objects/variables if needed, or include the main header
extern dealState currentDealState;
extern displayState currentDisplayState;
extern bool postDeal;
extern uint8_t remainingRoundsToDeal;
extern uint8_t initialRoundsToDeal;
extern int8_t postCardsToDeal;
extern bool cardDealt; // To reset after manual deal
extern bool gameOver;
extern bool advanceOnePlayer;

// Forward declare core functions games might need
void dealSingleCard(String customFace = "");
void advanceMenu(); // Although games shouldn't typically call this directly
void displayFace(const char *word);
void startScrollText(const char *text, uint16_t start, uint16_t delay, uint16_t end);
void updateScrollText();
void stopScrollText();
void moveOffActiveColor(bool rotateClockwise);
void returnToActiveColor(bool rotateClockwise);
void handleFlipCard(); // Example of a core function a game might trigger

// Base class for all games
class Game {
  public:
    virtual ~Game() {
    } // Virtual destructor

    // === Methods Subclasses MUST Implement ===

    // Returns the display name of the game (e.g., "GO FISH")
    virtual const char *getName() const = 0;

    // Called when the game is selected from the menu.
    // Set initial game parameters like rounds, post-deal cards.
    // Returns a value from the GamInitResult enum
    virtual bool initialize() = 0;

    // Handles button presses specifically when currentDealState is AWAITING_PLAYER_DECISION.
    // buttonPin: The pin number of the button pressed (e.g., BUTTON_PIN_1)
    virtual void handleButtonPress(int buttonPin) = 0;

    // Manages display updates (e.g., scrolling instructions) when currentDealState is AWAITING_PLAYER_DECISION.
    virtual void handleAwaitDecisionDisplay() = 0;

    // === Optional Methods ===

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
            gameOver = true;
        } else {
            // The main loop will likely transition to AWAITING_PLAYER_DECISION
            // if postDeal is true and game isn't over.
            // Game-specific post-deal setup might happen here if needed.
        }
    }

    // TODO: make these values instead of methods

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

  protected:
    // Variables
    bool scrollingStarted = false;
    uint8_t messageLine = 0;
    uint8_t messageRepetitions = 0; // TODO move handling of message lines into display array that is managed here...

    void dispenseCards(uint8_t amount, const String &customFace = "") {
        for (uint8_t i = 0; i < amount; ++i) {
            _dealSingleCard(customFace);
        }
    }

  private:
    // Internal wrappers
    void _dealSingleCard(const String &customFace = "") {
        dealSingleCard(customFace);
        cardDealt = false;
    }
};

#endif // GAME_H