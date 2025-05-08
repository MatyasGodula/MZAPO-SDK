#pragma once
#include "Sprite.hpp"

class TurretShotSprite : public Sprite {
    private:
        static constexpr int rawW = 2, rawH = 4;
        static constexpr uint8_t raw[rawH][rawW] = {{1, 1}, {1, 1}, {1, 1}, {1, 1}};

    public:
        TurretShotSprite() {
            width = 4;
            height = 20;
        }

        uint8_t at(int x, int y) const override {
            int sx = x * rawW / width;
            int sy = y * rawH / height;
            return raw[sy][sx];
        }
};
