#pragma once

#include "glm/glm.hpp"

namespace properties
{
    static constexpr int SCREEN_WIDTH{1080};
    static constexpr int SCREEN_HEIGHT{720};
    extern const glm::vec2 SCREEN_RESOLUTION;
    extern const float ASPECT_RATIO;
    static constexpr int NUM_DIRECTIONS{8};
    extern const glm::vec3 DIRECTIONS[NUM_DIRECTIONS];

    extern const float PLAYER_VELOCITY;

    static constexpr const glm::ivec2 MAP_CELLS{20, 20};
}