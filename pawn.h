#pragma once

#include "glnode.h"

struct Pawn
{
    uint32_t id;
    enum Type
    {
        PEASANT,
        TOWER,
        RUNNER,
        HORSE
    } type;
    std::shared_ptr<lix::TRS> trs;
};