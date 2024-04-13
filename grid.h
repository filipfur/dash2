#pragma once

#include "glm/glm.hpp"

class Grid
{
public:
    Grid(glm::vec2 tileSize, glm::ivec2 cells);

    glm::vec3 toWorldCoordinate(const glm::ivec2& p) const;
    glm::ivec2 toCellId(const glm::vec3& p) const;

    const glm::vec2& tileSize() const;
    const glm::ivec2& cells() const;
    const glm::vec2& halfSize() const;
    glm::vec2 size() const;

private:
    glm::vec2 _tileSize;
    glm::ivec2 _cells;
    glm::vec2 _halfSize;
};