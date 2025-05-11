/// @file Sprite.hpp
/// @brief Abstract base class representing a drawable sprite.
/// @details Provides dimensions and an interface for pixel-level access.
/// @note Derived sprite classes must implement the at() method to supply their pixel data.
#pragma once

#include <cstdint>

/// @brief Represents a generic 2D sprite.
struct Sprite {
    int width;   ///< Width of the sprite in pixels
    int height;  ///< Height of the sprite in pixels

    /// @brief Retrieves the pixel value at a given coordinate.
    /// @param x The x-coordinate of the pixel (0 <= x < width).
    /// @param y The y-coordinate of the pixel (0 <= y < height).
    /// @return An 8-bit intensity or color index at (x, y).
    virtual uint8_t at(int x, int y) const = 0;

    /// @brief Virtual destructor to allow proper cleanup in derived classes.
    virtual ~Sprite() = default;
};
