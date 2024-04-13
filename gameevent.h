#pragma once

#include <stdint.h>
#include <memory>
#include "hex/eventqueue.h"

struct DeletePawnEvent : public hex::Event
{
    std::shared_ptr<class Pawn> pawn;
};