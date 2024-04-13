#include "grid.h"

#include <cmath>

Grid::Grid(glm::vec2 tileSize, glm::ivec2 cells)
    : _tileSize{tileSize}, _cells{cells},
    _halfSize{tileSize * glm::vec2(cells) * 0.5f}
{
    
}

glm::vec3 Grid::toWorldCoordinate(const glm::ivec2& p) const
{
    return glm::vec3{
            p.x * _tileSize.x
            - _halfSize.x,
        0.0f,
        p.y * _tileSize.y - _halfSize.y
    };
}

glm::ivec2 Grid::toCellId(const glm::vec3& p) const
{
    int y = glm::trunc((p.z + _halfSize.y) / (_tileSize.y));
    int x = glm::trunc((p.x + _halfSize.x) / (_tileSize.x));
    return glm::ivec2{
        glm::max(glm::min(x, _cells.x - 1), 0),
        glm::max(glm::min(y, _cells.y - 1), 0)
    };
}

const glm::vec2& Grid::tileSize() const
{
    return _tileSize;
}

const glm::ivec2& Grid::cells() const
{
    return _cells;
}

const glm::vec2& Grid::halfSize() const
{
    return _halfSize;
}

glm::vec2 Grid::size() const
{
    return _halfSize * 2.0f;
}