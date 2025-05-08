#include "AudioDriver.hpp"
#include "DisplayDriver.hpp"
#include "Module.hpp"
#include "SpiledDriver.hpp"

#include "Theme.hpp"

namespace WinModuleConstants {
    namespace Text {
        constexpr int text_x_pos = 50;
        constexpr int text_y_pos = 50;
    } // namespace Text
} // namespace WinModuleConstants

class WinModule : public Module {
    public:
        WinModule(
            DisplayDriver *screen_ptr, 
            AudioDriver *buzzer_ptr, 
            SpiledDriver *spiled_ptr, 
            Theme *main_theme_ptr, 
            StateFlag *current_type_ptr
        );

        WinModule(const WinModule &) = delete;
        WinModule &operator=(const WinModule &) = delete;

        ~WinModule() override;

        /// @brief Module trait interface.
        void update() override;
        void redraw() override;
        void switch_setup() override;
        void switch_to(StateFlag new_mod) override;

    private:
        DisplayDriver *screen;
        AudioDriver *buzzer;
        SpiledDriver *spiled;
        Theme *main_theme;
        StateFlag *current_type;
};