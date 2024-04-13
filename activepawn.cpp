#include "activepawn.h"

ActivePawn::ActivePawn(std::shared_ptr<Pawn> pawn, hex::Event_Queue& eventQueue)
    : hex::State_Machine{}, _pawn{pawn}, _eventQueue{eventQueue}
{
    add_state(idle);
    add_state(breaking);
    add_state(broken);
    add_state(attacking);
    add_state(attackImpact);
    add_state(moving);
    add_state(moved);
}

void ActivePawn::Idle::entry(State_Machine* sm, bool first)
{

}

void ActivePawn::Idle::tick(State_Machine* sm, float dt)
{
    auto& activePawn = *dynamic_cast<ActivePawn*>(sm);
    glm::vec3 delta = activePawn.pawn()->trs->translation() - activePawn.targetPlayer()->node->translation();
    if(delta.x * delta.x + delta.z * delta.z < 0.1f)
    {
        activePawn.transition_to(activePawn.breaking);
    }
}

void ActivePawn::Breaking::entry(State_Machine* sm, bool first)
{
    auto& activePawn = *dynamic_cast<ActivePawn*>(sm);
    activePawn.transition_to(activePawn.broken, 1.0f);
}

void ActivePawn::Breaking::tick(State_Machine* sm, float dt)
{
    auto& activePawn = *dynamic_cast<ActivePawn*>(sm);
    float t = activePawn.time_since_entry();

    float wiggle = sinf(t * glm::pi<float>() * 8.0f) * 0.1f * (1.0f - t);
    activePawn.pawn()->trs->setScale(glm::vec3{1.0f - wiggle, 1.0f + wiggle, 1.0f - wiggle});
}

void ActivePawn::Broken::entry(State_Machine* sm, bool first)
{
    auto& activePawn = *dynamic_cast<ActivePawn*>(sm);
    activePawn.pawn()->trs->setScale(
        glm::vec3{ 1.0f }
    );
    activePawn.terminate();
}

void ActivePawn::Broken::tick(State_Machine* sm, float dt)
{
}

void ActivePawn::Attacking::entry(State_Machine* sm, bool first)
{
    auto& activePawn = *dynamic_cast<ActivePawn*>(sm);
    activePawn.transition_to(activePawn.attackImpact, 1.0f);
}

void ActivePawn::Attacking::tick(State_Machine* sm, float dt)
{
    auto& activePawn = *dynamic_cast<ActivePawn*>(sm);
    float wiggle = sinf(activePawn.time_since_entry() * glm::pi<float>() * 4.0f) * 0.1f;
    activePawn.pawn()->trs->setScale(glm::vec3{1.0f - wiggle, 1.0f + wiggle, 1.0f - wiggle});
}

void ActivePawn::AttackImpact::entry(State_Machine* sm, bool first)
{
    auto& activePawn = *dynamic_cast<ActivePawn*>(sm);
    activePawn.pawn()->trs->setScale(glm::vec3{1.0f, 1.0f, 1.0f});
    --activePawn.targetPlayer()->health;
    activePawn.terminate();
}

void ActivePawn::AttackImpact::tick(State_Machine* sm, float dt)
{

}

void ActivePawn::Moving::entry(State_Machine* sm, bool first)
{
    auto& activePawn = *dynamic_cast<ActivePawn*>(sm);
    activePawn.transition_to(activePawn.moved, 1.0f);
    from = activePawn.pawn()->trs->translation();
}

void ActivePawn::Moving::tick(State_Machine* sm, float dt)
{
    auto& activePawn = *dynamic_cast<ActivePawn*>(sm);
    activePawn.pawn()->trs->setTranslation(
        glm::mix(from, activePawn.target(), sm->time_since_entry()));
}

void ActivePawn::Moved::entry(State_Machine* sm, bool first)
{
    auto& activePawn = *dynamic_cast<ActivePawn*>(sm);
    activePawn.pawn()->trs->setTranslation(activePawn.target());
    activePawn.terminate();
}

void ActivePawn::Moved::tick(State_Machine* sm, float dt)
{

}