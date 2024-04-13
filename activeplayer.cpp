#include "activeplayer.h"

#include "properties.h"

ActivePlayer::ActivePlayer(Player& player)
    : _player{player}
{
    add_state(idle);
    add_state(moving);
    add_state(moved);
}


void ActivePlayer::Idle::entry(hex::State_Machine* sm, bool first)
{
    
}

void ActivePlayer::Idle::tick(hex::State_Machine* sm, float dt)
{

}

void ActivePlayer::Moving::entry(hex::State_Machine* sm, bool first)
{
    auto activePlayer = dynamic_cast<ActivePlayer*>(sm);
    p0 = activePlayer->player().node->translation();
    travelTime = glm::distance(p0, activePlayer->target()) / properties::PLAYER_VELOCITY;
    activePlayer->transition_to(activePlayer->moved, travelTime);
}

void ActivePlayer::Moving::tick(hex::State_Machine* sm, float dt)
{
    auto activePlayer = dynamic_cast<ActivePlayer*>(sm);
    activePlayer->player().node->setTranslation(
        glm::mix(p0, activePlayer->target(), activePlayer->time_since_entry() / travelTime));
}

void ActivePlayer::Moved::entry(hex::State_Machine* sm, bool first)
{
    auto activePlayer = dynamic_cast<ActivePlayer*>(sm);
    activePlayer->player().node->setTranslation(activePlayer->target());
}

void ActivePlayer::Moved::tick(hex::State_Machine* sm, float dt)
{

}