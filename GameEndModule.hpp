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

namespace WinModuleConstants {
    namespace Text {
        constexpr int text_x_pos = 50;
        constexpr int text_y_pos = 50;
    } // namespace Text
} // namespace WinModuleConstants

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

        void set_game_end_state(GameEndState state);

    private:
        GameEndState game_end_state = GameEndState::Ongoing;
        DisplayDriver *screen;
        AudioDriver *buzzer;
        SpiledDriver *spiled;
        Theme *main_theme;
        StateFlag *current_type;
};