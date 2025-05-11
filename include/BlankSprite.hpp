// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Albert Bastl

#pragma once

#include "include/Sprite.hpp"

class BlankSprite : public Sprite {
    public:
        BlankSprite() {
            width = 1;
            height = 1;
        }

        uint8_t at(int, int) const override { return 0; }
};