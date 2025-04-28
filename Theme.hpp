/// @file Theme.hpp
/// @brief Theme class for representing different themes in the game.
/// @author Matyas Godula
/// @date 29.4.2025

#pragma once

#include "Color.hpp"

#include <string_view>

struct Theme {
    Color background;
    Color text;
    Color turret;
    Color shield;
    Color aliens;
};

constexpr Theme DefaultTheme = {
    .background = Color::Black,
    .text = Color::White,
    .turret = Color::Green,
    .shield = Color::Green,
    .aliens = Color::White
};

constexpr Theme LightTheme = {
    .background = Color::White,
    .text = Color::Black,
    .turret = Color::Black,
    .shield = Color::Blue,
    .aliens = Color::Black
};

constexpr Theme Gruvbox = {
    .background = Color(60, 56, 54), // Dark Grey
    .text = Color::White,
    .turret = Color(215, 153, 33), // Yellowish-Gold
    .shield = Color(69, 133, 136), // Teal
    .aliens = Color::Red
};

constexpr Theme Solarized = {
    .background = Color(0, 43, 54), // Dark Blue
    .text = Color(253, 246, 227), // Light Cream
    .turret = Color(108, 113, 196), // Violet
    .shield = Color(133, 153, 0), // Olive Green
    .aliens = Color(220, 50, 47) // Red
};

constexpr Theme ThemeList[] = {
    DefaultTheme,
    LightTheme,
    Gruvbox,
    Solarized
};

constexpr std::string_view ThemeNames[] = {
    "Default",
    "Light",
    "Gruvbox",
    "Solarized"
};

constexpr int ThemeCount = sizeof(ThemeList) / sizeof(Theme);

