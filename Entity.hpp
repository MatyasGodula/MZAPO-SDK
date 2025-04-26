#pragma once
#include <cstdint>

#include "Sprite.hpp"


struct Entity {
    int pos_x;
    int pos_y;
    Sprite* sprite;
};