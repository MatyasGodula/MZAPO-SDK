/// @file MenuModule.hpp
/// @brief Implements the Module trait, manages the main menu.
/// @author Matyas Godula
/// @date 29.4.2025

#pragma once

#include "AudioDriver.hpp"
#include "DisplayDriver.hpp"
#include "Module.hpp"
#include "SpiledDriver.hpp"
#include "Theme.hpp"

namespace MenuConstants {
    constexpr int selection_height = 32;
    constexpr int selection_width = 100;
    constexpr int selection_pos_x = 50;
    constexpr int selection_pos_y = 82; // change later
} // namespace MenuConstants

namespace MenuModuleTypes {
    constexpr StateFlag SelectionTypes[] = {
        StateFlag::Tutorial, 
        StateFlag::Settings,
        StateFlag::Exit
    };

    constexpr std::string_view SelectionNames[] = {
        "Play", 
        "Settings", 
        "Exit"
    };

    constexpr std::size_t SelectionCount = sizeof(MenuModuleTypes::SelectionTypes) / sizeof(MenuModuleTypes::SelectionTypes[0]);
} // namespace MenuModuleTypes

/// @brief Class implements the main menu using the Module trait.
class MenuModule : public Module {
    public:
        /// @brief Constructor with all required drivers and references.
        /// @param screen_ptr Reference to the display driver.
        /// @param buzzer_ptr Reference to the audio driver.
        /// @param spiled_ptr Reference to the spiled driver.
        /// @param main_theme_ptr Reference to the currently active theme.
        /// @param current_type_ptr Reference to the current module type, used when switching
        /// modules.
        MenuModule(
            DisplayDriver *screen_ptr, 
            AudioDriver *buzzer_ptr, 
            SpiledDriver *spiled_ptr,
            Theme *main_theme_ptr, 
            StateFlag *current_type_ptr
        );

        MenuModule(const MenuModule &) = delete;
        MenuModule &operator=(const MenuModule &) = delete;

        ~MenuModule() override;

        /// @brief Module trait interface.
        void update() override;
        void redraw() override;
        void switch_setup() override;
        void switch_to(StateFlag new_mod) override;

    private:
        int selection = 0;
        DisplayDriver *const screen;
        AudioDriver *const buzzer;
        SpiledDriver *const spiled;
        Theme *main_theme;
        StateFlag *current_type;
};