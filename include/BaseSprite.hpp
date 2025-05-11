// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Albert Bastl

#pragma once

#include "include/Sprite.hpp"

class BaseSprite : public Sprite {
    private:
        static constexpr int raw_w = 8, raw_h = 5;
        static constexpr uint8_t raw[raw_h][raw_w] = {
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
            int sx = x * raw_w / width;
            int sy = y * raw_h / height;
            return raw[sy][sx];
        }
};
