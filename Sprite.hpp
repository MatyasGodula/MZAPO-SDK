#pragma once
#include <cstdint>

struct Sprite {
    int width;
    int height;
    
    virtual uint8_t at(int x, int y) const = 0;

    virtual ~Sprite() = default;
};