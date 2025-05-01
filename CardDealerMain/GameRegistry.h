#ifndef GAME_REGISTRY_H
#define GAME_REGISTRY_H

#include <Arduino.h>
#include "Game.h"

#define MAX_GAMES 10

// ===> Add Game Imports <===
#include "games/GoFish.h" // Include all games here
// ==========================

class GameRegistry {
  private:
    // ===> Add Game Instances <===
    GoFish goFishGame;
    // ==========================

    Game* games[MAX_GAMES];
    uint8_t gameCount;
    char formattedNameBuffer[20];

  public:
    GameRegistry()
        : gameCount(0) {
        for (uint8_t i = 0; i < MAX_GAMES; ++i) {
            games[i] = nullptr;
        }
        registerAllGames();
    }

    void registerAllGames() {
        // ===> Register Games <===
        addGame(&goFishGame);
        // ==========================
    }

    bool addGame(Game* game) {
        if (gameCount < MAX_GAMES && game != nullptr) {
            games[gameCount++] = game;
            return true;
        }
        return false;
    }

    uint8_t getGameCount() const {
        return gameCount;
    }

    Game* getGame(uint8_t index) const {
        if (index < gameCount) {
            return games[index];
        }
        return nullptr;
    }

    const char* getFormattedName(uint8_t index) {
        Game* game = getGame(index);
        if (game) {
            snprintf(formattedNameBuffer, sizeof(formattedNameBuffer), "%d-%s", index + 1, game->getName());
            return formattedNameBuffer;
        }
        return nullptr;
    }
};

#endif // GAME_REGISTRY_H
