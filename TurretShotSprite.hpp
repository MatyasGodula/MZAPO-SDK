#pragma once
#include "Sprite.hpp"

class TurretShotSprite : public Sprite {
    private:
        static constexpr int raw_w = 2, raw_h = 4;
        static constexpr uint8_t raw[raw_h][raw_w] = {{1, 1}, {1, 1}, {1, 1}, {1, 1}};

    public:
        TurretShotSprite() {
            width = 4;
            height = 20;
        }

        uint8_t at(int x, int y) const override {
            int sx = x * raw_w / width;
            int sy = y * raw_h / height;
            return raw[sy][sx];
        }
};
