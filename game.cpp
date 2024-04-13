#include "game.h"

#include <algorithm>
#include <set>

#include "properties.h"
#include "hex/extensions.h"
#include "hex/iterations.h"

namespace
{
    /*struct Word
    {
        size_t index;
        size_t length;
    };

    struct WordRegistry
    {
        Word words[10000];
    };*/

    constexpr bool isSpace(char c)
    {
        return c == ' ' || c == '\n' || c == '\t';
    }

    constexpr size_t countWords(const char* str) {
        size_t wordCount = 0;
        bool inWord = false;
        for (size_t i = 0; i < 10000000; ++i) {
            if(str[i] == '\0')
            {
                break;
            }
            else if (!isSpace(str[i]))
            {
                if (!inWord) {
                    inWord = true;
                    ++wordCount;
                }
            }
            else
            {
                inWord = false;
            }
        }
        return wordCount;
    }

    constexpr const char* wordText{"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum\0"};

    constexpr auto wordIndices{[]() constexpr {
        constexpr size_t size = countWords(wordText);
        std::array<size_t, size> result{};

        bool inWord{true};
        size_t wordCount{0};
        size_t start{0};
        for (size_t i = 0; i < 10000; ++i) {
            if(wordText[i] == '\0')
            {
                result[wordCount++] = start;
                break;
            }
            else if (isSpace(wordText[i]))
            {
                inWord = false;
                result[wordCount++] = start;
            }
            else
            {
                if (!inWord)
                {
                    inWord = true;
                    start = i;
                }
            }
        }
        return result;
    }()};
    
}

Game::Game(Grid& grid) : _grid{grid}
{

}

void Game::handlePawnMovements(Player& player)
{
    for(int i{0}; i < properties::NUM_DIRECTIONS; ++i)
    {
        const auto& dir = properties::DIRECTIONS[i];
        for(int d{2}; d <= 3; ++d)
        {
            glm::vec3 wp = player.node->translation() + dir * hex::as<float>(d);
            auto pawn = _iGame->pawnAt(wp);
            if(pawn)
            {
                glm::vec3 delta = player.node->translation() - pawn->trs->translation();
                glm::vec3 perp = glm::cross(glm::normalize(delta), glm::vec3{0.0f, 1.0f, 0.0f});
                glm::vec3 p;
                switch(pawn->type)
                {
                    case Pawn::PEASANT:
                        /*
                        // SIDESTEP
                        if(d == 1 && (i % 2) == 0)
                        {
                            glm::vec3 a = pawn->trs->translation() + perp;
                            glm::vec3 b = pawn->trs->translation() - perp;
                            if(_iGame->tryMoveOnGrid(pawn->trs->translation(), a))
                            {
                                auto& aPawn = makeActivePawn(pawn);
                                aPawn.setTarget(a);
                                aPawn.transition_to(aPawn.moving);
                            }
                            if(_iGame->tryMoveOnGrid(pawn->trs->translation(), b))
                            {
                                auto& aPawn = makeActivePawn(pawn);
                                aPawn.setTarget(b);
                                aPawn.transition_to(aPawn.moving);
                            }
                        }*/
                        p = pawn->trs->translation() - dir;
                        if(_iGame->tryMoveOnGrid(pawn->trs->translation(), p))
                        {
                            auto& aPawn = makeActivePawn(pawn);
                            aPawn.setTarget(p);
                            aPawn.transition_to(aPawn.moving);
                        }
                        break;
                    default:
                        break;
                }
            }
        }
    }
}

void Game::handlePawnAttacks(Player& player)
{
    for(int i{0}; i < properties::NUM_DIRECTIONS; ++i)
    {
        if((i % 2) == 0)
        {
            for(float j{1.0f}; j < 5.0f; j += 1.0f)
            {
                auto pawn = _iGame->pawnAt(player.node->translation() + properties::DIRECTIONS[i] * j);
                if(pawn)
                {
                    if(pawn->type == Pawn::TOWER)
                    {
                        ActivePawn& activePawn = makeActivePawn(pawn);
                        activePawn.transition_to(activePawn.attacking);
                        break;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
        else
        {
            auto pawn = _iGame->pawnAt(player.node->translation() + properties::DIRECTIONS[i]);
            if(pawn && pawn->type == Pawn::PEASANT)
            {
                ActivePawn& activePawn = makeActivePawn(pawn);
                activePawn.transition_to(activePawn.attacking);
            }

            auto pawn2 = _iGame->pawnAt(player.node->translation() + properties::DIRECTIONS[i] * glm::vec3{2.0f, 1.0f, 1.0f});
            auto pawn3 = _iGame->pawnAt(player.node->translation() + properties::DIRECTIONS[i] * glm::vec3{1.0f, 1.0f, 2.0f});
            for(auto p : {pawn2, pawn3})
            {
                if(p && p->type == Pawn::HORSE)
                {
                    ActivePawn& activePawn = makeActivePawn(p);
                    activePawn.transition_to(activePawn.attacking);
                }
            }
        }
    }
}

void Game::playerMovedOrTimeout(Player& player)
{
    handlePawnAttacks(player);
    handlePawnMovements(player);
}

void Game::tick(float dt)
{
    hex::iter_remove_if(_activePawns, [this, dt](auto& entry) {
        ActivePawn& activePawn = entry.second;
        activePawn.tick(dt);
        if(activePawn.current_state() == activePawn.broken)
        {
            _iGame->deletePawn(activePawn.pawn());
        }
        return activePawn.terminated();
    });

    hex::iter_remove_if(_activePlayers, [this, dt](auto& entry) {
        ActivePlayer& activePlayer = entry.second;
        activePlayer.player().node->children().front()->setRotation(glm::slerp(activePlayer.player().node->children().front()->rotation(), activePlayer.player().targetRotation, 12.0f * dt));
        activePlayer.tick(dt);
        if(activePlayer.current_state() == activePlayer.moved)
        {
            playerMovedOrTimeout(activePlayer.player());
            activePlayer.transition_to(activePlayer.idle);
        }
        return activePlayer.terminated();
    });

    //hex::iterateAndRemove(_activePlayers, [](std::pair<uint32_t, ActivePlayer>& activePlayer) -> bool {
    //    activePlayer.second.tick(dt);
    //    return activePlayer.second.terminated();
    //});

    /*for(Player& player : _players)
    {
        glm::vec3 delta = player.target - player.node->translation();
        float v = glm::length(delta);
        if(v > 0.1f)
        {
            glm::vec3 n = delta / v;
            player.velocity += n * properties::PLAYER_ACCELERATION * dt;
            player.node->applyTranslation(player.velocity * dt);
        }
        else if(v > 0)
        {
            player.node->setTranslation(player.target);
            player.velocity.x = player.velocity.y = player.velocity.z = 0.0f;
            playerMovedOrTimeout(player);
        }
        player.node->children().front()->setRotation(glm::slerp(player.node->children().front()->rotation(), player.targetRotation, 12.0f * dt));
    }*/
}

std::string Game::getRandomWord()
{
    static std::set<size_t> usedIndices;
    size_t i;
    while(usedIndices.find((i = rand() % (wordIndices.size() - 1))) != usedIndices.end())
    {
        ;
    }
    usedIndices.emplace(i);
    size_t a = wordIndices.at(i);
    size_t b = wordIndices.at(i + 1);

    std::string str{&wordText[a], b - a - 1};

    std::transform(str.begin(), str.end(), str.begin(), tolower);
    str.erase(std::remove_if(str.begin(), str.end(), [](auto x){
        return !std::isalpha(x);
    }), str.end());

    return str;
}

Player& Game::createPlayer(std::shared_ptr<lix::Node> node)
{
    Player& player = _players.emplace_back();
    _currentPlayer = _players.begin();
    static uint32_t nextId{0};
    player.id = nextId++;
    player.node = node;
    return player;
}

std::vector<Player>& Game::players()
{
    return _players;
}

Player& Game::player(size_t i)
{
    return _players.at(i);
}

std::vector<Player>::iterator Game::currentPlayer()
{
    return _currentPlayer;
}

void Game::setIGame(IGame* iGame)
{
    _iGame = iGame;
}

ActivePlayer& Game::makeActivePlayer(Player& player)
{
    ActivePlayer& activePlayer = _activePlayers.emplace(player.id, player).first->second;
    activePlayer.setTarget(player.node->translation());
    activePlayer.transition_to(activePlayer.idle);
    return activePlayer;
}

ActivePlayer& Game::activePlayer(uint32_t id)
{
    return _activePlayers.at(id);
}

ActivePawn& Game::makeActivePawn(std::shared_ptr<Pawn> pawn)
{
    ActivePawn& activePawn = _activePawns.emplace(pawn->id, std::move(ActivePawn{pawn, _eventQueue})).first->second;
    activePawn.setTargetPlayer(&(*_currentPlayer));
    return activePawn;
}

bool Game::isActive(std::shared_ptr<Pawn> pawn)
{
    return hex::contains(_activePawns, pawn->id);
}