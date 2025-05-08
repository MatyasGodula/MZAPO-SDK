#pragma once
#include "Sprite.hpp"

class BaseSprite : public Sprite {
    private:
        static constexpr int rawW = 8, rawH = 5;
        static constexpr uint8_t raw[rawH][rawW] = {
            {0, 0, 0, 1, 1, 0, 0, 0}, 
            {0, 0, 1, 1, 1, 1, 0, 0}, 
            {1, 1, 1, 1, 1, 1, 1, 1},
            {1, 1, 1, 1, 1, 1, 1, 1}, 
            {1, 1, 1, 1, 1, 1, 1, 1},
        };

    public:
        BaseSprite() {
            width = 32;
            height = 24;
        }

        uint8_t at(int x, int y) const override {
            int sx = x * rawW / width;
            int sy = y * rawH / height;
            return raw[sy][sx];
        }
};
