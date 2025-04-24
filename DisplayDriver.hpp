/// @file DisplayDriver.hpp
/// @brief An abstraction layer for the display driver of the MZAPO board.
/// @author Matyas Godula
/// @date 22.4.2025
/// @note For some reason copilot has gotten really good at generating comments and docs.

#pragma once
#include <cstdint>

/// @brief Color class for representing colors in RGB565 format.
/// @details This class provides a way to create colors using RGB888 values and convert them to RGB565 format.
/// @note It should serve mostly to communicate with the display driver.
struct Color {
private:
    uint16_t value;

    /// @brief Converts RGB888 values to RGB565 format.
    /// @param r Red component (0-255)
    /// @param g Green component (0-255)
    /// @param b Blue component (0-255)
    /// @return RGB565 value
    /// @note This function is called in the constructor and is not intended to be used directly.
    /// @note The conversion is done by scaling the RGB888 values to fit into the RGB565 format so detail can be lost.
    static constexpr uint16_t to565(uint8_t r, uint8_t g, uint8_t b) {
        uint16_t red = (r * 31) / 255;
        uint16_t green = (g * 63) / 255;
        uint16_t blue = (b * 31) / 255;
        return (red << 11) | (green << 5) | blue;
    }

public:

    /// @brief Default constructor for Color.
    /// @details Takes in RGB888 values and converts them to RGB565 format.
    /// @param r Red component (0-255)
    /// @param g Green component (0-255)
    /// @param b Blue component (0-255)
    constexpr Color(uint8_t r, uint8_t g, uint8_t b)
        : value(to565(r, g, b)) {}


    /// @brief Gets the RGB565 color as a uint16_t.
    constexpr uint16_t to_rgb565() const {
        return value;
    }

    static const Color White;
    static const Color Black;
    static const Color Red;
    static const Color Green;
    static const Color Blue;
    static const Color Yellow;
    static const Color Cyan;
    static const Color Magenta;
};

/// @brief Predefined colors in RGB565 format
/// @note Feel free to add more colors as needed.
constexpr Color Color::White   = Color(255, 255, 255);
constexpr Color Color::Black   = Color(0, 0, 0);
constexpr Color Color::Red     = Color(255, 0, 0);
constexpr Color Color::Green   = Color(0, 255, 0);
constexpr Color Color::Blue    = Color(0, 0, 255);
constexpr Color Color::Yellow  = Color(255, 255, 0);
constexpr Color Color::Cyan    = Color(0, 255, 255);
constexpr Color Color::Magenta = Color(255, 0, 255);


#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"

#include "font_types.h"
#include "Sprite.hpp"

constexpr int SCREEN_WIDTH = 480;  // Screen width in pixels in landscape mode
constexpr int SCREEN_HEIGHT = 320; // Screen height in pixels in landscape mode
constexpr int SCREEN_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT; // Total number of pixels on the screen
constexpr int SCREEN_SIZE_BYTES = SCREEN_SIZE * sizeof(uint16_t); // Total size of the screen in bytes


/// @brief FontType enum for different font types.
/// @details This enum is used to specify the font type when drawing text on the display.
/// @note The font types are defined in the font_types.h file.
/// @note Add more font types as needed.
enum class FontType {
    ROM8x16,
    WinFreeSystem14x16,
};

/// @brief DisplayDriver class for controlling the MZAPO display.
class DisplayDriver {
    private:
        void* lcd = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0); // Map the physical address of the display
        uint16_t fb[SCREEN_WIDTH][SCREEN_HEIGHT];

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
            return (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT);
        } 

    public:
        /// @brief Constructor for DisplayDriver.
        DisplayDriver();

        /// @brief Destructor for DisplayDriver.
        ~DisplayDriver();

        /// @brief Sets a pixel on the display to a specific color.
        /// @param x X coordinate of the pixel
        /// @param y Y coordinate of the pixel
        /// @param color Color to set the pixel to
        /// @note The coordinates are in the range of 0 to SCREEN_WIDTH-1 and 0 to SCREEN_HEIGHT-1.
        /// @note If out of bounds the pixel will not be set.
        void draw_pixel(int x, int y, Color color);

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