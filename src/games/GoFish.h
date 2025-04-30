#ifndef GO_FISH_H
#define GO_FISH_H

#ifndef useSerial
#define useSerial 0
#endif

#include "../Game.h" // Include base class - use relative path

class GoFish : public Game {
  public:
    // ===== Config Functions ======
    const char* getName() const override {
        return "GO FISH";
    }
    
    const String* getDisplayMessages(uint8_t &count) override {
        static const String messages[] = { 
            "  BLUE = FISH  ",
            "  GREEN = PASS  "
        };
        count = sizeof(messages) / sizeof(messages[0]);
        return messages;
    }

    // These can also be overriden to do different stuff
    // bool requiresCardSelection() const override { return true; }
    // bool requiresPlayerSelection() const override { return true; }

    bool initialize() override {
        initialRoundsToDeal = 0;
        postCardsToDeal = 127;                       // Use a large number to signify 'deal until empty' logic needed later
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
            resetScrollingMessages();
        }
    }

    // ===== Optional Overrides =====

    void onMainDealEnd() override {
        Serial.println(F("GoFish: Main deal complete. Entering post-deal phase."));

        // Reset scroll state for AwaitDecision phase
        resetScrollingMessages();
    }
};

#endif // GO_FISH_H