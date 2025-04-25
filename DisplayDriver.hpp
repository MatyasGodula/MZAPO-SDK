/// @file DisplayDriver.hpp
/// @brief An abstraction layer for the display driver of the MZAPO board.
/// @author Matyas Godula
/// @date 22.4.2025
/// @note For some reason copilot has gotten really good at generating comments and docs.

#pragma once
#include <cstdint>

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"

#include "font_types.h"
#include "Sprite.hpp"
#include "Color.hpp"

constexpr int SCREEN_WIDTH = 480;
constexpr int SCREEN_HEIGHT = 320;

/// @brief FontType enum for different font types.
/// @details This enum is used to specify the font type when drawing text on the display.
/// @note The font types are defined in the font_types.h file.
/// @note Add more font types as needed.
enum class FontType {
    ROM8x16,
    WinFreeSystem14x16,
};

enum class DisplayOrientation: uint8_t {
    Portrait,
    Landscape,
};

/// @brief DisplayDriver class for controlling the MZAPO display.
class DisplayDriver {
    private:
        void* lcd;
        uint16_t fb[SCREEN_WIDTH * SCREEN_HEIGHT];
        int screen_width = SCREEN_WIDTH;
        int screen_height = SCREEN_HEIGHT;
        DisplayOrientation orientation;


        /// @brief Given a font type, returns the corresponding font descriptor.
        /// @param font The better user accessible font type.
        /// @return A pointer defined in font_types.h to the font descriptor.
        static font_descriptor_t* get_font_descriptor(FontType font) {
            switch (font) {
                case FontType::ROM8x16:
                    return &font_rom8x16;
                case FontType::WinFreeSystem14x16:
                    return &font_winFreeSystem14x16;
                default:
                    return nullptr; // Invalid font type
            }
        }

        /// @brief Checks whether a given pixel is within the bounds of the screen.
        bool in_bounds(int x, int y) const {
            if (orientation == DisplayOrientation::Landscape) {
                return (x >= 0 && x < screen_width && y >= 0 && y < screen_height);

            } else {
                return (y >= 0 && y < screen_width && x >= 0 && x < screen_height);
            }
        } 

    public:
        /// @brief Constructor for DisplayDriver.
        /// @param orientation The orientation of the display (Portrait or Landscape).
        /// @throw std::runtime_error if the physical address mapping fails.
        DisplayDriver(DisplayOrientation);

        /// @brief Destructor for DisplayDriver.
        ~DisplayDriver();

        /// @brief Sets a pixel on the display to a specific color.
        /// @param x X coordinate of the pixel
        /// @param y Y coordinate of the pixel
        /// @param color Color to set the pixel to
        /// @note The coordinates are in the range of 0 to SCREEN_WIDTH-1 and 0 to SCREEN_HEIGHT-1.
        /// @note If out of bounds the pixel will not be set.
        void draw_pixel(int x, int y, Color color);

        /// @brief Sets a pixel on the display to a specific color.
        /// @param x X coordinate of the pixel
        /// @param y Y coordinate of the pixel
        /// @param color Color to set the pixel to 
        /// @note This is an overload to draw a pixel instead of taking a Color enum it takes a raw RGB565 value.
        void draw_pixel(int x, int y, uint16_t color);

        /// @brief Draws a rectangle on the display of a specific color, width and height.
        /// @param x X coordinate of the top-left corner of the rectangle
        /// @param y Y coordinate of the top-left corner of the rectangle
        /// @param width Width of the rectangle
        /// @param height Height of the rectangle
        /// @param color Color to fill the rectangle with
        /// @note Out of bounds pixels will not be set.
        void draw_rectangle(int x, int y, int width, int height, Color color);

        /// @brief Draws a specific character on the display at a specific position.
        /// @param x X coordinate of the character top left corner
        /// @param y Y coordinate of the character top left corner
        /// @param font Font type to use for the character
        /// @param ch Character to draw
        /// @note The character will be drawn using the specified font type.
        void draw_letter(int x, int y, FontType font, char ch);

        /// @brief Draws a sprite on the display given by its x and y coordinates.
        /// @param x X top left corner of the sprite
        /// @param y Y top left corner of the sprite
        /// @param sprite The sprite to draw, which is a struct containing the width, height and data of the sprite.
        /// @param color The color to draw the sprite with.
        void draw_sprite(int x, int y, const Sprite& sprite, Color color);

        /// @brief Fills the entire screen with a specific color.
        /// @param color The color to fill the screen with.
        /// @note Mostly used for black but I added color specification for funsies. Might be useful in the future.
        void fill_screen(Color color);  

        /// @brief Flushes the frame buffer into the display memory
        /// @note The display orietnation in memory is different from the buffer, so I have to pay the rotation tax somewhere
        /// so I decided that it should be paid in flush();
        void flush();
};