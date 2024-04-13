#pragma once

#include "hex/statemachine.h"
#include "hex/extensions.h"

#include "player.h"
#include "pawn.h"
#include "gameevent.h"

class ActivePawn : public hex::State_Machine
{
    struct Idle : public hex::State
    {
        virtual void entry(State_Machine* sm, bool first) override;
        virtual void tick(State_Machine* sm, float dt) override;
    };

    struct Breaking : public hex::State
    {
        virtual void entry(State_Machine* sm, bool first) override;
        virtual void tick(State_Machine* sm, float dt) override;
    };

    struct Broken : public hex::State
    {
        virtual void entry(State_Machine* sm, bool first) override;
        virtual void tick(State_Machine* sm, float dt) override;
    };

    struct Attacking : public hex::State
    {
        virtual void entry(State_Machine* sm, bool first) override;
        virtual void tick(State_Machine* sm, float dt) override;
    };

    struct AttackImpact : public hex::State
    {
        virtual void entry(State_Machine* sm, bool first) override;
        virtual void tick(State_Machine* sm, float dt) override;
    };

    struct Moving : public hex::State
    {
        virtual void entry(State_Machine* sm, bool first) override;
        virtual void tick(State_Machine* sm, float dt) override;
        glm::vec3 from;
    };

    struct Moved : public hex::State
    {
        virtual void entry(State_Machine* sm, bool first) override;
        virtual void tick(State_Machine* sm, float dt) override;
    };

public:

    ActivePawn(std::shared_ptr<Pawn> pawn, hex::Event_Queue& eventQueue);

    std::shared_ptr<Pawn> pawn()
    {
        return _pawn;
    }

    void setTargetPlayer(Player* player)
    {
        _player = player;
    }

    Player* targetPlayer()
    {
        return _player;
    }

    void setTarget(const glm::vec3& target)
    {
        _target = target;
    }

    const glm::vec3& target() const
    {
        return _target;
    }

    hex::Event_Queue& eventQueue()
    {
        return _eventQueue;
    }

    virtual ~ActivePawn() noexcept
    {

    }
    
    ActivePawn(const ActivePawn& other) = delete;
    ActivePawn& operator=(const ActivePawn& other) = delete;
    
    ActivePawn(ActivePawn&& other)
        : ActivePawn{other._pawn, other._eventQueue}
    {

    }

    ActivePawn& operator=(ActivePawn&& other) = delete;

    //States
    Idle idle;
    Breaking breaking;
    Broken broken;
    Attacking attacking;
    AttackImpact attackImpact;
    Moving moving;
    Moved moved;

private:
    std::shared_ptr<Pawn> _pawn;
    Player* _player{nullptr};
    glm::vec3 _target;
    hex::Event_Queue& _eventQueue;
};