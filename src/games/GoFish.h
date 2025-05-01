#ifndef GO_FISH_H
#define GO_FISH_H

#include "../Game.h" // Include base class - use relative path

class GoFish : public Game {
  public:
    // ===== Config Functions ======
    const char* getName() const override {
        return "GO FISH";
    }

    const String* getDisplayMessages(uint8_t& count) override {
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
        // Start GoFish with 7 cards each
        setDealAmount(7);

        // Let the system know to start dealing right away
        return GamInitResult::StartDealing;
    }

    void handleButtonPress(int button) override {
        if (button == Buttons::GREEN) {
            nextTurn();

        } else if (button == Buttons::BLUE) {
            // Dispense a card
            displayFace("o  o");
            dispenseCards(1);
        }
    }
};

#endif // GO_FISH_H