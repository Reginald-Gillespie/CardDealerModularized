#ifndef GO_FISH_GAME_H
#define GO_FISH_GAME_H

#ifndef useSerial
    #define useSerial false
#endif

#include "../Game.h" // Include base class - use relative path

// --- Extern variables/functions GoFish needs ---
// These should already be declared in Game.h or globally available
extern uint16_t textSpeedInterval;
extern uint16_t textStartHoldTime;
extern uint16_t textEndHoldTime;
extern const unsigned long timeUntilScreensaverStart; // Example if needed
extern const unsigned long expressionDuration;        // Example if needed

// Button Pins (ensure these defines are accessible)
#define BUTTON_PIN_1 17
#define BUTTON_PIN_2 16
// #define BUTTON_PIN_3 15 // Not used by GoFish AwaitDecision
// #define BUTTON_PIN_4 14 // Back button handled by main loop

class GoFish : public Game {
  public:
    // --- Required Methods ---

    const char *getName() const override {
        return "GO FISH";
    }

    bool onSelect() override {
        initialRoundsToDeal = 1;
        postCardsToDeal = 127;   // Use a large number to signify 'deal until empty' logic needed later
        remainingRoundsToDeal = initialRoundsToDeal; // Second value has smth to do with rigged game handling?

        return true; // Go Fish starts dealing immediately after selection
    }

    void handleButtonPress(int buttonPin) override {
        // This is called ONLY when currentDealState is AWAITING_PLAYER_DECISION
        if (buttonPin == BUTTON_PIN_1) { // Green Button (Pass)
            // Player passes, advance to the next player
            stopScrollText();                   // Stop any scrolling text
            displayFace(">  >");                // Show advancing face briefly
            advanceOnePlayer = true;            // Signal core logic to advance one player
            currentDealState = ADVANCING;       // Change state to advancing
        } else if (buttonPin == BUTTON_PIN_2) { // Blue Button (Fish)
            // Player asks for a card
            stopScrollText();    // Stop any scrolling text
            displayFace("o  o"); // Show dealing face briefly
            dealSingleCard();
            cardDealt = false;        // dealSingleCard sets this true, reset for state machine logic
                                      // Stay in AWAITING_PLAYER_DECISION for the same player? Or does Go Fish logic dictate advancing?
                                      // For simplicity here, we assume the player might fish again or pass next.
                                      // If fishing always ends the turn, you'd set advanceOnePlayer = true and state = ADVANCING here too.
                                      // Let's keep it simple: stay awaiting decision from the same player after fishing.
            messageLine = 0;          // Reset message line for next display update
            scrollingStarted = false; // Allow text to restart
        }
        // Other buttons (Yellow/Red) are typically handled by the main loop (decrease/back)
        // and won't call this function when in AWAITING_PLAYER_DECISION state.
    }

    void handleAwaitDecisionDisplay() override {
        // Manage the scrolling text for Go Fish instructions
        if (!scrollingStarted) {
            // Alternate between "BLUE = FISH" and "GREEN = PASS"
            if (messageLine % 2 == 0) {
                // Add spaces for padding/timing if needed
                startScrollText("  BLUE = FISH   ", textStartHoldTime, textSpeedInterval, textEndHoldTime);
            } else {
                startScrollText("  GREEN = PASS  ", textStartHoldTime, textSpeedInterval, textEndHoldTime);
            }
            scrollingStarted = true;
        }

        // Call updateScrollText() - this will handle the actual scrolling
        updateScrollText();

        // Check if the message cycle should repeat or stop (using messageRepetitions/messageLine)
        // This simple version just flips between the two messages indefinitely until a button stops it.
        // messageLine is incremented automatically by updateScrollText when a scroll completes.
        if (!scrollingStarted && messageRepetitions > 0) { // If a scroll just finished
            messageRepetitions = 0;                        // Reset repetitions
            scrollingStarted = false;                      // Ready to start the next line's scroll
        }
    }

    // --- Optional Overrides ---

    void onMainDealEnd() override {
        // Called when initial 5 rounds are dealt.
        // Go Fish continues until cards run out or game logic dictates.
        // The default implementation handles the postCardsToDeal check.
        // Since postCardsToDeal is > 0, it defaults to awaiting player decision.
        // We don't need to do anything special here for Go Fish's basic post-deal start.
        if (useSerial) Serial.println(F("GoFish: Main deal complete. Entering post-deal phase."));
        // Reset scroll state for AwaitDecision phase
        messageLine = 0;
        scrollingStarted = false;
    }

    // GoFish doesn't require card/player selection via the UI
    // bool requiresCardSelection() const override { return false; }
    // bool requiresPlayerSelection() const override { return false; }
};

#endif // GO_FISH_GAME_H