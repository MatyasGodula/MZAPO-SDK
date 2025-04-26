/// @file Color.hpp
/// @brief Color class for representing colors in RGB565 format.
/// @details This class provides a way to create colors using RGB888 values and convert them to RGB565 format.
/// @author Matyas Godula
/// @date 22.4.2025
/// @note This class is mostly used for readability and color standardization.

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