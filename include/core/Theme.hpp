// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Matyas Godula

/// @file Theme.hpp
/// @brief Theme class for representing different themes in the game.
/// @author Matyas Godula
/// @date 29.4.2025

#pragma once

#include "include/core/Color.hpp"

#include "include/drivers/DisplayDriver.hpp" // For fonts

#include <string_view>

struct Theme {
    Color background;
    Color text;
    Color turret;
    Color shield;
    Color aliens;
    Color selection;
    FontType font;
};

constexpr Theme DefaultTheme = {
    .background = Color::Black,
    .text = Color::White,
    .turret = Color::Green,
    .shield = Color::Green,
    .aliens = Color::White,
    .selection = Color::Green,
    .font = FontType::WinFreeSystem14x16,
};

constexpr Theme LightTheme = {
    .background = Color::White,
    .text = Color::Black,
    .turret = Color::Black,
    .shield = Color::Blue,
    .aliens = Color::Black,
    .selection = Color::Magenta,
    .font = FontType::WinFreeSystem14x16,
};

constexpr Theme Gruvbox = {
    .background = Color(60, 56, 54), // Dark Grey
    .text = Color::White,
    .turret = Color(215, 153, 33), // Yellowish-Gold
    .shield = Color(69, 133, 136), // Teal
    .aliens = Color::Red,
    .selection = Color(215, 153, 33), // Yellowish-Gold
    .font = FontType::WinFreeSystem14x16,
};

constexpr Theme Solarized = {
    .background = Color(0, 43, 54),  // Dark Blue
    .text = Color(253, 246, 227),    // Light Cream
    .turret = Color(108, 113, 196),  // Violet
    .shield = Color(133, 153, 0),    // Olive Green
    .aliens = Color(220, 50, 47),    // Red
    .selection = Color(133, 153, 0), // Olive Green
    .font = FontType::ROM8x16,
};

constexpr Theme Ayu = {
    .background = Color(39, 44, 51),   // #272c33
    .text = Color(255, 255, 255),      // #ffffff
    .turret = Color(255, 203, 107),    // #ffc36b
    .shield = Color(114, 217, 211),    // #72d9d3
    .aliens = Color(255, 135, 162),    // #ff87a2
    .selection = Color(144, 222, 241), // #90def1
    .font = FontType::WinFreeSystem14x16,
};

constexpr Theme Monokai = {
    .background = Color(87, 47, 0),   // #572f00
    .text = Color(102, 216, 239),     // #66d9ef
    .turret = Color(253, 151, 31),    // #fd971f (orange)
    .shield = Color(166, 226, 46),    // #a6e22e
    .aliens = Color(249, 38, 114),    // #f92672 (pink/red)
    .selection = Color(249, 38, 114), // #f92672
    .font = FontType::ROM8x16,
};

constexpr Theme ThemeList[] = {
    DefaultTheme, 
    LightTheme, 
    Gruvbox, 
    Solarized, 
    Ayu, 
    Monokai
};

constexpr std::string_view ThemeNames[] = {
    "Default",   
    "Light", 
    "Gruvbox",                                 
    "Solarized", 
    "Ayu",   
    "Monokai"
};

constexpr int ThemeCount = sizeof(ThemeList) / sizeof(Theme);
