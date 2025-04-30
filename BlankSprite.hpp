#pragma once
#include "Sprite.hpp"

class BlankSprite : public Sprite {
public:
    BlankSprite() {
        width = 1;
        height = 1;
    }

    uint8_t at(int, int) const override {
        return 0;
    }
};