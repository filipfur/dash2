#include "properties.h"

const glm::vec2 properties::SCREEN_RESOLUTION{
    static_cast<float>(properties::SCREEN_WIDTH),
    static_cast<float>(properties::SCREEN_HEIGHT)
};

const float properties::ASPECT_RATIO{properties::SCREEN_WIDTH / static_cast<float>(properties::SCREEN_HEIGHT)};

const glm::vec3 properties::DIRECTIONS[8] = {
    glm::vec3{1.0f, 0.0f, 0.0f},
    glm::vec3{1.0f, 0.0f, 1.0f},
    glm::vec3{0.0f, 0.0f, 1.0f},
    glm::vec3{-1.0f, 0.0f, 1.0f},
    glm::vec3{-1.0f, 0.0f, 0.0f},
    glm::vec3{-1.0f, 0.0f, -1.0f},
    glm::vec3{0.0f, 0.0f, -1.0f},
    glm::vec3{1.0f, 0.0f, -1.0f}
};

const float properties::PLAYER_VELOCITY{4.0f};