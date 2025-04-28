#ifndef GAME_H
#define GAME_H

#include <Arduino.h>
#include "Enums.h"
#include "Definitions.h"

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
    } // Virtual destructor is good practice for base classes

    // --- Methods Subclasses MUST Implement ---

    // Returns the display name of the game (e.g., "GO FISH")
    // Should be short enough or the registry will handle scrolling.
    virtual const char *getName() const = 0;

    // Called when the game is selected from the menu.
    // Set initial game parameters like rounds, post-deal cards.
    // Return true if the game should start dealing immediately after selection.
    // Return false if further configuration (like SELECT_CARDS/PLAYERS) is needed first.
    virtual bool onSelect() = 0;

    // Handles button presses specifically when currentDealState is AWAITING_PLAYER_DECISION.
    // buttonPin: The pin number of the button pressed (e.g., BUTTON_PIN_1)
    virtual void handleButtonPress(int buttonPin) = 0;

    // Manages display updates (e.g., scrolling instructions) when currentDealState is AWAITING_PLAYER_DECISION.
    virtual void handleAwaitDecisionDisplay() = 0;

    // --- Methods Subclasses CAN Override (Optional) ---

    // Called just before the main dealing loop starts (after initialization to red, if applicable)
    virtual void onDealStart() {
        // Default: Do nothing
    }

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

    // Called when the game is over (either by onMainDealEnd or other game logic)
    virtual void onGameOver() {
        // Default: Do nothing, main loop handles reset via handleGameOver()
    }

    // Does this game require the user to select the number of cards per hand?
    virtual bool requiresCardSelection() const {
        return false; // Default: No
    }

    // Does this game require the user to select the number of players (for tagless)?
    // Note: Tagless itself isn't managed by this Game class system directly yet.
    virtual bool requiresPlayerSelection() const {
        return false; // Default: No
    }

    // Does this game involve flipping a card after the main deal?
    virtual bool requiresFlipCard() const {
        return false; // Default: No
    }

  protected:
    // Helper variables for managing scrolling text within game states
    // Games need to manage these themselves if they use scrolling text
    // in handleAwaitDecisionDisplay
    bool scrollingStarted = false;
    uint8_t messageLine = 0;
    uint8_t messageRepetitions = 0; // Track repetitions if needed

    // Games can access global functions like dealSingleCard() directly
    // or use forward declarations if needed.
};

#endif // GAME_H