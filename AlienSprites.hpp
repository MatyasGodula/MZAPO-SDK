#pragma once
#include "Sprite.hpp"

class InvaderA : public Sprite {
private:
    static constexpr int rawW = 8, rawH = 8;
    static constexpr uint8_t raw[rawH][rawW] = {
        {0,0,1,0,0,1,0,0},
        {1,1,1,1,1,1,1,1},
        {0,1,1,1,1,1,1,0},
        {1,1,0,1,1,0,1,1},
        {1,1,1,1,1,1,1,1},
        {0,1,1,1,1,1,1,0},
        {0,0,1,0,0,1,0,0},
        {0,0,1,1,1,1,0,0},
    };

public:
    InvaderA() { width = 32; height = 32; }

    uint8_t at(int x, int y) const override {
        int sx = x * rawW / width;
        int sy = y * rawH / height;
        return raw[sy][sx];
    }
};


class InvaderB : public Sprite {
private:
    static constexpr int rawW = 8, rawH = 8;
    static constexpr uint8_t raw[rawH][rawW] = {
        {0,0,1,1,1,1,0,0},
        {0,1,1,1,1,1,1,0},
        {1,1,0,1,1,0,1,1},
        {1,1,1,1,1,1,1,1},
        {1,1,1,0,0,1,1,1},
        {0,1,1,1,1,1,1,0},
        {0,1,0,0,0,0,1,0},
        {1,0,1,0,0,1,0,1},
    };

public:
    InvaderB() { width = 32; height = 32; }

    uint8_t at(int x, int y) const override {
        int sx = x * rawW / width;
        int sy = y * rawH / height;
        return raw[sy][sx];
    }
};

class InvaderC : public Sprite {
private:
    static constexpr int rawW = 8, rawH = 8;
    static constexpr uint8_t raw[rawH][rawW] = {
        {0,1,1,0,0,1,1,0},
        {1,1,1,1,1,1,1,1},
        {1,0,1,1,1,1,0,1},
        {1,1,0,1,1,0,1,1},
        {1,1,1,1,1,1,1,1},
        {0,1,1,1,1,1,1,0},
        {0,1,0,0,0,0,1,0},
        {1,0,1,0,0,1,0,1},
    };

public:
    InvaderC() { width = 32; height = 32; }

    uint8_t at(int x, int y) const override {
        int sx = x * rawW / width;
        int sy = y * rawH / height;
        return raw[sy][sx];
    }
};
