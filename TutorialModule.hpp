#include "AudioDriver.hpp"
#include "DisplayDriver.hpp"
#include "Module.hpp"
#include "SpiledDriver.hpp"
#include "Theme.hpp"

namespace Constants {
    namespace Text {
        constexpr int vertical_limit_pos = 0;
        constexpr int vertical_limit_neg = -95;
    } // namespace Text
} // namespace Constants

class TutorialModule : public Module {
    public:
        TutorialModule(
            DisplayDriver *screen_ptr, 
            AudioDriver *buzzer_ptr, 
            SpiledDriver *spiled_ptr,
            Theme *main_theme_ptr, 
            StateFlag *current_type_ptr
        );

        TutorialModule(const TutorialModule &) = delete;
        TutorialModule &operator=(const TutorialModule &) = delete;

        ~TutorialModule() override;

        void update() override;
        void redraw() override;
        void switch_setup() override;
        void switch_to(StateFlag new_mod) override;

    private:
        bool tutorial_shown = false;
        int text_position = 0;
        DisplayDriver *const screen;
        AudioDriver *const buzzer;
        SpiledDriver *const spiled;
        Theme *main_theme;
        StateFlag *current_type;
};