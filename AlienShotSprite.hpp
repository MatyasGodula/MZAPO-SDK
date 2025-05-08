#pragma once
#include "Sprite.hpp"

class AlienShotSprite : public Sprite {
    private:
        static constexpr int rawW = 4, rawH = 8;
        static constexpr uint8_t raw[rawH][rawW] = {
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
            int sx = x * rawW / width;
            int sy = y * rawH / height;
            return raw[sy][sx];
        }
};
