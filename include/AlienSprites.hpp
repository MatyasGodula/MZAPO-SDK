// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Albert Bastl

#pragma once

#include "include/Sprite.hpp"

class InvaderA : public Sprite {
    private:
        static constexpr int raw_w = 8, raw_h = 8;
        static constexpr uint8_t raw[raw_h][raw_w] = {
            {0, 0, 1, 0, 0, 1, 0, 0},
            {1, 1, 1, 1, 1, 1, 1, 1},
            {0, 1, 1, 1, 1, 1, 1, 0},
            {1, 1, 0, 1, 1, 0, 1, 1},
            {1, 1, 1, 1, 1, 1, 1, 1},
            {0, 1, 1, 1, 1, 1, 1, 0},
            {0, 0, 1, 0, 0, 1, 0, 0}, 
            {0, 0, 1, 1, 1, 1, 0, 0},
        };

    public:
        InvaderA() {
            width = 32;
            height = 32;
        }

        uint8_t at(int x, int y) const override {
            int sx = x * raw_w / width;
            int sy = y * raw_h / height;
            return raw[sy][sx];
        }
};

class InvaderB : public Sprite {
    private:
        static constexpr int raw_w = 8, raw_h = 8;
        static constexpr uint8_t raw[raw_h][raw_w] = {
            {0, 0, 1, 1, 1, 1, 0, 0}, 
            {0, 1, 1, 1, 1, 1, 1, 0}, 
            {1, 1, 0, 1, 1, 0, 1, 1},
            {1, 1, 1, 1, 1, 1, 1, 1}, 
            {1, 1, 1, 0, 0, 1, 1, 1}, 
            {0, 1, 1, 1, 1, 1, 1, 0},
            {0, 1, 0, 0, 0, 0, 1, 0}, 
            {1, 0, 1, 0, 0, 1, 0, 1},
        };

    public:
        InvaderB() {
            width = 32;
            height = 32;
        }

        uint8_t at(int x, int y) const override {
            int sx = x * raw_w / width;
            int sy = y * raw_h / height;
            return raw[sy][sx];
        }
};

class InvaderC : public Sprite {
    private:
        static constexpr int raw_w = 8, raw_h = 8;
        static constexpr uint8_t raw[raw_h][raw_w] = {
            {0, 1, 1, 0, 0, 1, 1, 0}, 
            {1, 1, 1, 1, 1, 1, 1, 1}, 
            {1, 0, 1, 1, 1, 1, 0, 1},
            {1, 1, 0, 1, 1, 0, 1, 1}, 
            {1, 1, 1, 1, 1, 1, 1, 1}, 
            {0, 1, 1, 1, 1, 1, 1, 0},
            {0, 1, 0, 0, 0, 0, 1, 0}, 
            {1, 0, 1, 0, 0, 1, 0, 1},
        };

    public:
        InvaderC() {
            width = 32;
            height = 32;
        }

        uint8_t at(int x, int y) const override {
            int sx = x * raw_w / width;
            int sy = y * raw_h / height;
            return raw[sy][sx];
        }
};
