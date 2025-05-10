/// @file GameEndModule.hpp
/// @brief GameEndModule class for handling the endgame screen.
/// @author Matyas Godula
/// @date 8.5.2025

#include "AudioDriver.hpp"
#include "DisplayDriver.hpp"
#include "Module.hpp"
#include "SpiledDriver.hpp"

#include "Theme.hpp"

/// @brief GameEndModule class for handling the current end state of the game.
enum class GameEndState {
    Win,
    Loss,
    Ongoing
};

namespace GameEndModuleConstants {
    namespace Selections {
        constexpr int selection_height = 32;
        constexpr int selection_width = 100;
        constexpr int selection_x_pos = 50;
        constexpr int selection_y_pos = 50;

        constexpr std::string_view selections[] {
            "Play Again",
            "Exit"
        };

        constexpr StateFlag selection_types[] {
            StateFlag::ResetGame,
            StateFlag::Exit
        };

        constexpr int selection_count = sizeof(selections) / sizeof(selections[0]);
    } // namespace Selections
} // namespace GameEndModuleConstants

/// @brief GameEndModule class for handling the endgame screen.
/// @details This class is responsible for displaying the endgame screen and handling user input.
/// @note The endgame screen displays the result of the game (win or loss) and allows the user to
/// play again or exit the game.
class GameEndModule : public Module {
    public:
        /// @brief Constructor for the GameEndModule class.
        /// @param screen_ptr Pointer to the display driver.
        /// @param buzzer_ptr Pointer to the audio driver.
        /// @param spiled_ptr Pointer to the SPILED driver.
        /// @param main_theme_ptr Pointer to the main theme.
        /// @param current_type_ptr Pointer to the current system flag.
        /// @note This constructor follows the Module trait interface.
        GameEndModule(
            DisplayDriver *screen_ptr, 
            AudioDriver *buzzer_ptr, 
            SpiledDriver *spiled_ptr, 
            Theme *main_theme_ptr, 
            StateFlag *current_type_ptr
        );

        GameEndModule(const GameEndModule &) = delete;
        GameEndModule &operator=(const GameEndModule &) = delete;

        ~GameEndModule() override;

        /// @brief Module trait interface.
        void update() override;
        void redraw() override;
        void switch_setup() override;
        void switch_to(StateFlag new_mod) override;

        /// @brief Used to signal a changed game state.
        /// @param state The current state of the game.
        void set_game_end_state(GameEndState state);

    private:
        GameEndState game_end_state = GameEndState::Ongoing;
        int selection = 0;
        DisplayDriver *screen;
        AudioDriver *buzzer;
        SpiledDriver *spiled;
        Theme *main_theme;
        StateFlag *current_type;
};