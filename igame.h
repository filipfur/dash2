#pragma once

#include "glm/glm.hpp"

class IGame
{
public:
    virtual std::shared_ptr<class Pawn> pawnAt(const glm::vec3& position) = 0;
    virtual void removeFromGrid(const glm::vec3& position) = 0;
    virtual bool tryMoveOnGrid(const glm::vec3& p0, const glm::vec3& p1) = 0;
    virtual void deletePawn(std::shared_ptr<class Pawn> pawn) = 0;
};