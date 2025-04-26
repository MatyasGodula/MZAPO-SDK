#include "DisplayDriver.hpp"

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "font_types.h"

#include <stdexcept>
#include <iostream>
#include <utility>
#include <string_view>

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
    // TODO: Test and debug this function
    font_descriptor_t* fdes = get_font_descriptor(font);
    if (fdes == nullptr) { // Invalid font type
        return;
    }
    // Get the index of the character in the whole font array
    int glyph_index = ch - fdes->firstchar;
    if (glyph_index < 0 || glyph_index >= fdes->size) { // Invalid index
        return;
    }
    const font_bits_t* glyph_bits = fdes->bits + fdes->offset[glyph_index]; // Get a pointer to the bits of the current glyph
    int glyph_width = (fdes->width != 0) ? fdes->width[glyph_index] : fdes->maxwidth; // Get the width of the glyph
    int glyph_height = fdes->height; // Get the height of the glyph

    int words_per_row = (glyph_width + 15) / 16; // Calculate the number of words per row

    for (int row = 0; row < glyph_height; ++row) {
        for (int col = 0; col < glyph_width; ++col) {
            int word_index = (col / 16);
            int bit_index = 15 - (col % 16); // Calculates the correct index from the right side 

            font_bits_t word = glyph_bits[row * words_per_row + word_index];

            if (word & (1 << bit_index)) {
                draw_pixel(x + col, y + row, color);
            }
        }
    }
    /// @note I wrote this whole function and then I realized that the fonts are always at most 16 pixels wide.
    /// @note So I could have just used a single loop and the bit index would be 15 - column index.
    /// @note But I decided to keep it because I spent a lot of time on it.
}

void DisplayDriver::draw_text(int x, int y, FontType font, std::string_view text, Color color) {
    // TODO: Test and debug this function
    font_descriptor_t* fdes = get_font_descriptor(font);
    if (fdes == nullptr) { // Invalid font type
        return;
    }
    int start_x = x;
    std::string_view line;
    while (!text.empty()) {
        auto nline_pos = text.find('\n'); // Find the next newline character so we can split the text
        if (nline_pos == std::string_view::npos) { // No newline found
            line = text; // Take the rest of the text
            text = ""; // Clear all else
        } else {
            line = text.substr(0, nline_pos); // Take the part of the text before the newline
            text = text.substr(nline_pos + 1); // Remove the part of the text we just took
        } 

        for (char letter : line) {
            if (letter < fdes->firstchar || letter >= fdes->firstchar + fdes->size) {
                letter = fdes->defaultchar; // Replace invalid characters with the default character
            }

            draw_letter(x, y, font, letter, color);

            int char_width = (fdes->width != 0) ? fdes->width[letter - fdes->firstchar] : fdes->maxwidth;
            x += char_width + Constants::Text::HorizontalSpacing; // Move to the next character position + 1 pixel for spacing
        }

        y += fdes->height + Constants::Text::VerticalSpacing; // Move to the next line
        x = start_x; // Reset x to the start position
    }
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
            fb[x + screen_width * y] = color.to_rgb565();
        }
    }
}

void DisplayDriver::flush() {
    parlcd_write_cmd(static_cast<uint8_t*>(lcd), 0x2C); // Write to RAM command
    for (int y = 0; y < screen_height; ++y) {
        for (int x = 0; x < screen_width; ++x) {
            parlcd_write_data(static_cast<uint8_t*>(lcd), fb[y * screen_width + x]); // Copy all of the framebuffer data to the display
        }
    }
    
}
