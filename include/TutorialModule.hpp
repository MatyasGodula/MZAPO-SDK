// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Matyas Godula

/// @file TutorialModule.hpp
/// @brief TutorialModule class for handling the tutorial screen.
/// @author Matyas Godula
/// @date 5.5.2025

#include "include/AudioDriver.hpp"
#include "include/DisplayDriver.hpp"
#include "include/Module.hpp"
#include "include/SpiledDriver.hpp"
#include "include/Theme.hpp"

namespace Constants {
    namespace Text {
        constexpr int vertical_limit_pos = 0;
        constexpr int vertical_limit_neg = -95;
    } // namespace Text
} // namespace Constants

/// @brief TutorialModule class for handling the tutorial screen.
/// @details This class is responsible for displaying the tutorial screen and handling user input.
/// @note The tutorial screen displays the instructions for the game and allows the user to start
/// the game.
/// @note This class implements the Module trait interface.
/// @note The tutorial screen is displayed only once, next time the game is started, the
/// tutorial is skipped.
class TutorialModule : public Module {
    public:
        TutorialModule(
            DisplayDriver *screen_ptr, 
            AudioDriver *buzzer_ptr, 
            SpiledDriver *spiled_ptr,
            Theme *main_theme_ptr, 
            StateFlag *current_type_ptr
        );

        /// @brief Deleted copy constructor and assignment operator. 
        TutorialModule(const TutorialModule &) = delete;
        TutorialModule &operator=(const TutorialModule &) = delete;

        ~TutorialModule() override;

        /// @brief Module trait interface implementation.
        void update() override;
        void redraw() override;
        void switch_setup() override;
        void switch_to(StateFlag new_mod) override;

    private:
        /// @brief Tracks whether the the tutorial has been shown.
        bool tutorial_shown = false;
        int text_position = 0;
        DisplayDriver *const screen;
        AudioDriver *const buzzer;
        SpiledDriver *const spiled;
        Theme *main_theme;
        StateFlag *current_type;
};