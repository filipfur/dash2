#pragma once

#include <cstdint>
#include <memory>
#include "glnode.h"

struct Player
{
    uint32_t id;
    std::shared_ptr<lix::Node> node;
    //glm::vec3 velocity;
    //glm::vec3 target;
    glm::quat targetRotation{1.0f, 0.0f, 0.0f, 0.0f};
    int health{5};
};