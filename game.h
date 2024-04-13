#pragma once

#include <string>
#include <unordered_map>
#include <queue>

#include "player.h"
#include "igame.h"
#include "activeplayer.h"
#include "activepawn.h"
#include "gameevent.h"
#include "grid.h"

class Game
{
public:

    Game(Grid& grid);

    void tick(float dt);

    std::string getRandomWord();

    Player& createPlayer(std::shared_ptr<lix::Node> node);
    std::vector<Player>& players();

    Player& player(size_t i);

    std::vector<Player>::iterator currentPlayer();

    void setIGame(IGame* iGame);

    void handlePawnMovements(Player& player);

    void handlePawnAttacks(Player& player);

    void playerMovedOrTimeout(Player& player);

    ActivePlayer& makeActivePlayer(Player& player);
    ActivePlayer& activePlayer(uint32_t id);

    ActivePawn& makeActivePawn(std::shared_ptr<Pawn> pawn);
    bool isActive(std::shared_ptr<Pawn> pawn);

private:
    Grid& _grid;
    std::vector<Player> _players;
    std::vector<Player>::iterator _currentPlayer;
    std::unordered_map<uint32_t, ActivePlayer> _activePlayers;
    std::unordered_map<uint32_t, ActivePawn> _activePawns;
    hex::Event_Queue _eventQueue;
    IGame* _iGame{nullptr};
};