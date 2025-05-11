// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Albert Bastl

#pragma once
#include "include/Sprite.hpp"

class AlienShotSprite : public Sprite {
    private:
        static constexpr int raw_w = 4, raw_h = 8;
        static constexpr uint8_t raw[raw_h][raw_w] = {
        {0, 1, 1, 1}, 
        {1, 1, 1, 0}, 
        {1, 1, 1, 0},                                  
        {1, 1, 1, 0}, 
        {0, 1, 1, 1}, 
        {0, 1, 1, 1},                                   
        {0, 1, 1, 1}, 
        {1, 1, 1, 0}};

    public:
        AlienShotSprite() {
            width = 8;
            height = 20;
        }

        uint8_t at(int x, int y) const override {
            int sx = x * raw_w / width;
            int sy = y * raw_h / height;
            return raw[sy][sx];
        }
};
