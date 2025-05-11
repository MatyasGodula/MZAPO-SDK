// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Albert Bastl

/// @file Entity.hpp
/// @brief Represents a game object instance with position and sprite.
/// @details Stores coordinates, sprite reference, and shooter capability flag for game entities.

#pragma once

#include "include/Sprite.hpp"

#include <cstdint>

/// @brief Holds runtime data for an in-game entity.
struct Entity {
    int pos_x;       ///< X-coordinate of entity on screen
    int pos_y;       ///< Y-coordinate of entity on screen
    Sprite *sprite;  ///< Pointer to sprite defining appearance
    bool is_shooter = false;  ///< Flag indicating if entity can shoot
};
