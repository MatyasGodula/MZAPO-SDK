// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Matyas Godula

/// @file SettingsModule.hpp
/// @brief SettingsModule class for handling the settings screen.
/// @author Matyas Godula
/// @date 30.4.2025

#pragma once

#include "include/core/Theme.hpp"

#include "include/drivers/AudioDriver.hpp"
#include "include/drivers/DisplayDriver.hpp"
#include "include/drivers/SpiledDriver.hpp"

#include "include/modules/Module.hpp"


constexpr int selection_height = 32;

/// @brief SettingsModule class for handling the settings screen.
/// @details This class is responsible for displaying the settings screen and handling user input.
/// @note The settings screen allows the user to change the theme.
/// @note This class implements the Module trait interface.
class SettingsModule : public Module {
    public:
        SettingsModule(
            DisplayDriver *screen_ptr, 
            AudioDriver *buzzer_ptr, 
            SpiledDriver *spiled_ptr,
            Theme *main_theme_ptr, 
            StateFlag *current_type_ptr
        );

        /// @brief Deleted copy constructor and assignment operator.
        SettingsModule(const SettingsModule &) = delete;
        SettingsModule &operator=(const SettingsModule &) = delete;

        ~SettingsModule() override;

        /// @brief Module trait interface implementation.
        void update() override;
        void redraw() override;
        void switch_setup() override;
        void switch_to(StateFlag new_mod) override;

    private:
        int setting_selected = 0;
        DisplayDriver *const screen;
        AudioDriver *const buzzer;
        SpiledDriver *const spiled;
        Theme *main_theme;
        StateFlag *current_type;
};