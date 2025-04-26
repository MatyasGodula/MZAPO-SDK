#pragma once
#include "Sprite.hpp"

class ShotSprite : public Sprite {
private:
    static constexpr int rawW = 2, rawH = 4;
    static constexpr uint8_t raw[rawH][rawW] = {
        {1, 1},
        {1, 1},
        {1, 1},
        {1, 1}
    };

public:
    ShotSprite() { width = 5; height = 15; }

    uint8_t at(int x, int y) const override {
        int sx = x * rawW / width;
        int sy = y * rawH / height;
        return raw[sy][sx];
    }
};
