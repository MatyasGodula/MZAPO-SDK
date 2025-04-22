#include <cstdint>

#include "DisplayDriver.hpp" // Include the DisplayDriver header for the Color class

struct Sprite {
    int width;
    int height;
    uint8_t *data;
};