#ifndef UNO_H
#define UNO_H

#include "../Game.h"

class Uno : public Game {
  public:
    const char* getName() const override {
        return "UNO";
    }

    virtual const char** getDisplayMessages(uint8_t &count) {
        static const char* messages[4];
        if (drawPot == 0) {
            // Normally, scroll these help messages
            messages[0] = " G = PASS  ";
            messages[1] = " B = DRAW  ";
            messages[2] = " Y = +2  ";
            messages[3] = " R = SKIP  ";
            count = 4;
        } else {
            // If there is a pot, show how large
            messages[0] = getPotSizeTypeString();
            count = 1;
        }
        return (const char**)messages;
    }

    // Uno requires a starting card
    bool requiresFlipCard() const override {
        return true;
    }

    bool initialize() override {
        setDealAmount(7);
        return GamInitResult::StartDealing;
    }


    void handleButtonPress(int button) override {
        switch (button) {
            case Buttons::RED:
                if (drawType != 0 || addedToPotThisTurn) break; // Don't allow doing this action while there is a pot
                // Skip next player
                potWasCreatedThisTurn = false;
                nextTurn(1);
                break;

            case Buttons::YELLOW:
                // If the drawType is not set, initialize the pot
                if (drawType == 0) {
                    drawType = 2;
                    drawPot = 2;
                    potWasCreatedThisTurn = true;
                }
                // If +2 was hit twice, it's actually a +4
                else if (drawType == 2 && potWasCreatedThisTurn) {
                    drawType = 4;
                    drawPot = 4;
                }
                // If you pressed this button, stack the pot higher and move on
                else if (!potWasCreatedThisTurn && !addedToPotThisTurn) {
                    // The pot increases by the card type
                    drawPot += drawType;

                    // Show briefly the new number
                    displayFace(getPotSizeTypeString());
                    delay(100);

                    // Auto advance
                    nextTurn();
                    break;
                }

                displayFace(getPotSizeTypeString());
                // delay(100);
                break;

            case Buttons::BLUE:
                if (drawType != 0 || addedToPotThisTurn) break; // Don't allow doing this action while there is a pot
                // draw one immediately
                displayFace("DRAW");
                dispenseCards(1);
                nextTurn();
                break;

                
            case Buttons::GREEN:
                // end turn: if there's a pot, dish it all; then advance
                if (!addedToPotThisTurn && !potWasCreatedThisTurn && drawPot > 0) {
                    displayFace(" L  ");
                    dispenseCards(drawPot);
                    resetPot();
                }
                potWasCreatedThisTurn = false;
                addedToPotThisTurn = false;
                nextTurn();
                break;
        }
    }

    // Get the string representing what is in the pot, for example +4*2 for 2 +4's
    const char* getPotSizeTypeString() {
        static char potString[8];
        snprintf(potString, sizeof(potString), "+%d*%d", drawType, drawPot/drawType);
        return potString;
    }

  private:
    uint8_t drawPot = 0;                // accumulated cards to draw
    uint8_t drawType = 0;               // either 0 (none) or the current stack increment (2 or 4)
    bool potWasCreatedThisTurn = false; // Flag to know if +2 was hit twice, it's actually a +4
    bool addedToPotThisTurn = false;    // If you added to the pot, you can't add any more or do any other actions

    void resetPot() {
        drawPot = 0;
        drawType = 0;
    }
};

#endif // UNO_H
