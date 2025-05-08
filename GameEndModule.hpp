#include "AudioDriver.hpp"
#include "DisplayDriver.hpp"
#include "Module.hpp"
#include "SpiledDriver.hpp"

#include "Theme.hpp"

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

class GameEndModule : public Module {
    public:
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