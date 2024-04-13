#pragma once

#include "gltrs.h"
#include "gltexture.h"

struct Sprite
{
    lix::TRS trs;
    std::shared_ptr<lix::Texture> texture;
    glm::vec4 uvWindow;
    bool visible{true};
};