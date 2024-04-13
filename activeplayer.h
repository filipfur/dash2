#pragma once

#include "hex/statemachine.h"
#include "player.h"

class ActivePlayer : public hex::State_Machine
{
    struct Idle : public hex::State
    {
        virtual void entry(hex::State_Machine* sm, bool first) override;
        virtual void tick(hex::State_Machine* sm, float dt) override;
    };

    struct Moving : public hex::State
    {
        virtual void entry(hex::State_Machine* sm, bool first) override;
        virtual void tick(hex::State_Machine* sm, float dt) override;
        glm::vec3 p0;
        float travelTime;
    };

    struct Moved : public hex::State
    {
        virtual void entry(hex::State_Machine* sm, bool first) override;
        virtual void tick(hex::State_Machine* sm, float dt) override;
    };

public:
    ActivePlayer(Player& player);

    Player& player()
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

    Idle idle;
    Moving moving;
    Moved moved;


private:
    Player& _player;
    glm::vec3 _target;
};