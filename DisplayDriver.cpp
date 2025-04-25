#include "DisplayDriver.hpp"
#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "font_types.h"
#include <stdexcept>
#include <iostream>

DisplayDriver::DisplayDriver(DisplayOrientation orientation) 
    : orientation(orientation) {

    lcd = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
    if (lcd == nullptr) {
        throw std::runtime_error("Failed to map physical address");
    }

    parlcd_hx8357_init(static_cast<uint8_t*>(lcd));
}

DisplayDriver::~DisplayDriver() {
    // No need to do anything here
}

void DisplayDriver::draw_pixel(int x, int y, Color color) {
    if (in_bounds(x, y)) {
        if (orientation == DisplayOrientation::Landscape) {
            fb[x + screen_width * y] = color.to_rgb565();
        } else {
            int src_x = y;
            int src_y = screen_height - 1 - x;
            fb[src_y * screen_width + src_x] = color.to_rgb565();
        }
    }
}

void DisplayDriver::draw_pixel(int x, int y, uint16_t color) {
    if (in_bounds(x, y)) {
        if (orientation == DisplayOrientation::Landscape) {
            fb[x + screen_width * y] = color;
        } else {
            int src_x = y;
            int src_y = screen_height - 1 - x;
            fb[src_y * screen_width + src_x] = color;
        }
    }
}

void DisplayDriver::draw_rectangle(int x, int y, int width, int height, Color color) {
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            if (in_bounds(x + i, y + j)) {
                draw_pixel(x + i, y + j, color);
            }
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
            uint8_t pixel = sprite.at(i, j);
            if (pixel != 0 && in_bounds(x + i, y + j)) { 
                draw_pixel(x + i, y + j, color);
            }
        }
    }
}

void DisplayDriver::fill_screen(Color color) {
    for (int y = 0; y < screen_height; ++y) {
        for (int x = 0; x < screen_width; ++x) {
            //std::cout << "Filling pixel x = " << x << " y = " << y << "\n";
            fb[x + screen_width * y] = color.to_rgb565();
        }
    }
}

void DisplayDriver::flush() {
    parlcd_write_cmd(static_cast<uint8_t*>(lcd), 0x2C); // Write to RAM command
    //std::cout << "Flushing\n";
    for (int y = 0; y < screen_height; ++y) {
        for (int x = 0; x < screen_width; ++x) {
            parlcd_write_data(static_cast<uint8_t*>(lcd), fb[x + screen_width * y]);
        }
    }
    
}
