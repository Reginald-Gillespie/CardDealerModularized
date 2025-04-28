#ifndef GO_FISH_GAME_H
#define GO_FISH_GAME_H

#ifndef useSerial
    #define useSerial 0
#endif

#include "../Game.h" // Include base class - use relative path

class GoFish : public Game {
  public:
  
    const char *getName()  const override {
        return "GO FISH";
    }

    bool initialize() override {
        // initialRoundsToDeal = 1;
        initialRoundsToDeal = 0;
        postCardsToDeal = 127;   // Use a large number to signify 'deal until empty' logic needed later
        remainingRoundsToDeal = initialRoundsToDeal; // Second value has smth to do with rigged game handling?

        return GamInitResult::StartDealing;
    }

    void handleButtonPress(int buttonPin) override {
        if (buttonPin == BUTTON_PIN_1) { // Green Button (Pass)
            nextPlayersTurn();

        } else if (buttonPin == BUTTON_PIN_2) { // Blue Button (Fish)
            // Player asks for a card
            displayFace("o  o");
            dispenseCards(1);
            restoreFace();

            messageLine = 0;          // Reset message line for next display update
            scrollingStarted = false; // Allow text to restart
        }
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
            messageLine++;
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