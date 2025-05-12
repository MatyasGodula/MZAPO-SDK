#include "include/DisplayDriver.hpp"

#include "assets/fonts/font_types.h"

#include "third_party/mzapo/mzapo_parlcd.h"
#include "third_party/mzapo/mzapo_phys.h"
#include "third_party/mzapo/mzapo_regs.h"

#include <iostream>
#include <stdexcept>
#include <string_view>
#include <utility>

DisplayDriver::DisplayDriver(DisplayOrientation orientation) : orientation(orientation) {

    lcd = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
    if (lcd == nullptr) {
        throw std::runtime_error("Failed to map physical address");
    }

    parlcd_hx8357_init(static_cast<uint8_t *>(lcd));
}

DisplayDriver::~DisplayDriver() {
    std::cout << "DisplayDriver ending!..." << std::endl;
}

inline std::pair<int, int> DisplayDriver::map_coords(int x, int y) {
    switch (orientation) {
    case DisplayOrientation::Landscape:
        return {x, y};

    case DisplayOrientation::Portrait:
        return {y, screen_height - 1 - x};

    default:
        return {x, y};
    }
}

void DisplayDriver::draw_pixel(int x, int y, Color color) {
    if (in_bounds(x, y)) {
        auto [mapped_x, mapped_y] = map_coords(x, y);
        fb[mapped_y * screen_width + mapped_x] = color.to_rgb565();
    }
}

void DisplayDriver::draw_pixel(int x, int y, uint16_t color) {
    if (in_bounds(x, y)) {
        auto [mapped_x, mapped_y] = map_coords(x, y);
        fb[mapped_y * screen_width + mapped_x] = color;
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

void DisplayDriver::draw_letter(int x, int y, FontType font, char ch, Color color) {
    font_descriptor_t *fdes = get_font_descriptor(font);
    if (fdes == nullptr) { // Invalid font type
        std::cout << "Invalid font type\n";
        return;
    }
    int glyph_index = ch - fdes->firstchar;
    if (glyph_index < 0 || glyph_index >= fdes->size) { // Invalid index
        std::cout << "Invalid index\n";
        return;
    }
    const font_bits_t *glyph_bits = fdes->bits + glyph_index * fdes->height;

    int glyph_width = (fdes->width) ? fdes->width[glyph_index] : fdes->maxwidth;

    for (int row = 0; row < fdes->height; ++row) {
        uint16_t row_data = glyph_bits[row];
        for (int col = 0; col < glyph_width; ++col) {
            if (row_data & (1 << (15 - col))) {
                draw_pixel(x + col, y + row, color);
            }
        }
    }
}

void DisplayDriver::draw_text(int x, int y, FontType font, std::string_view text, Color color) {
    font_descriptor_t *fdes = get_font_descriptor(font);
    if (fdes == nullptr) { // Invalid font type
        return;
    }

    int start_x = x;

    for (char letter : text) {
        if (letter == '\n') {
            x = start_x; // Reset x to the start position
            y += fdes->height + DisplayConstants::Text::VerticalSpacing; // Move to the next line
            continue;
        }

        if (letter < fdes->firstchar || letter >= fdes->firstchar + fdes->size) {
            letter = fdes->defaultchar; // Replace invalid characters with the default character
        }

        draw_letter(x, y, font, letter, color);

        int char_width = (fdes->width) ? fdes->width[letter - fdes->firstchar] : fdes->maxwidth;
        x += char_width + DisplayConstants::Text::HorizontalSpacing; // Move to the next character
                                                                     // position + spacing
    }
}

void DisplayDriver::draw_sprite(int x, int y, const Sprite &sprite, Color color) {
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
            fb[x + screen_width * y] = color.to_rgb565();
        }
    }
}

void DisplayDriver::flush() {
    parlcd_write_cmd(static_cast<uint8_t *>(lcd), 0x2C); // Write to RAM command
    for (int y = 0; y < screen_height; ++y) {
        for (int x = 0; x < screen_width; ++x) {
            parlcd_write_data(
                static_cast<uint8_t *>(lcd),
                fb[y * screen_width + x]); // Copy all of the framebuffer data to the display
        }
    }
}

void DisplayDriver::set_orientation(DisplayOrientation orientation) {
    this->orientation = orientation;
    fill_screen(Color::Black); // Clear the screen when changing orientation
    flush();
}

int DisplayDriver::get_width() const {
    if (orientation == DisplayOrientation::Landscape) {
        return screen_width;
    } else {
        return screen_height;
    }
}

int DisplayDriver::get_height() const {
    if (orientation == DisplayOrientation::Landscape) {
        return screen_height;
    } else {
        return screen_width;
    }
}