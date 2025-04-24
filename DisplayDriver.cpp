#include "DisplayDriver.hpp"
#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "font_types.h"
#include <stdexcept>

DisplayDriver::DisplayDriver(DisplayOrientation orientation) 
    : orientation(orientation) {

    // Set the screen orientation through MADCTL
    parlcd_write_cmd(static_cast<uint8_t*>(lcd), 0x36);
    parlcd_write_data(static_cast<uint8_t*>(lcd), static_cast<uint8_t>(orientation));

    switch (orientation) {
        case DisplayOrientation::Portrait:
            screen_width = 320;
            screen_height = 480;
            break;
        case DisplayOrientation::Landcape:
            screen_width = 480;
            screen_height = 320;
            break;
    }

    lcd = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
    if (lcd == nullptr) {
        throw std::runtime_error("Failed to map physical address");
    }

}

DisplayDriver::~DisplayDriver() {
    // No need to do anything here
}

void DisplayDriver::draw_pixel(int x, int y, Color color) {
    if (in_bounds(x, y)) {
        fb[x + screen_width * y] = color.to_rgb565();
    }
}

void DisplayDriver::draw_rectangle(int x, int y, int width, int height, Color color) {
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            draw_pixel(x + i, y + j, color);
        }
    }
}

void DisplayDriver::draw_letter(int x, int y, FontType font, char ch) {
    font_descriptor_t* fdes = get_font_descriptor(font);
    // TODO: Somehow design a way to draw variable width letters
}

void DisplayDriver::draw_sprite(int x, int y, const Sprite& sprite, Color color) {
    for (int i = 0; i < sprite.width; ++i) {
        for (int j = 0; j < sprite.height; ++j) {
            uint8_t pixel = sprite.data[j * sprite.width + i];
            if (pixel != 0) { 
                draw_pixel(x + i, y + j, color);
            }
        }
    }
}

void DisplayDriver::fill_screen(Color color) {
    for (int i = 0; i < screen_width * screen_height; ++i) {
        fb[i] = color.to_rgb565();
    }
}

void DisplayDriver::flush() {
    parlcd_write_cmd(static_cast<uint8_t*>(lcd), 0x2C); // Write to RAM command
    for (int i = 0; i < screen_width * screen_height; ++i) {
        // Write the pixel data to the display
        // fb[i] is the pixel data in RGB565 format
        parlcd_write_data(static_cast<uint8_t*>(lcd), fb[i]);
    }
}
