#include "DisplayDriver.hpp"
#include "AudioDriver.hpp"
#include "SpiledDriver.hpp"
#include "Module.hpp"
#include "Theme.hpp"


namespace Constants {
    namespace Text {
        constexpr int vertical_limit_pos = 480;
        constexpr int vertical_limit_neg = -480;
    }
}

class TutorialModule : public Module {
    public: 
        TutorialModule(
            DisplayDriver *screen_ptr, 
            AudioDriver *buzzer_ptr, 
            SpiledDriver *spiled_ptr, 
            Theme *main_theme_ptr,
            ModuleType *current_type_ptr
        );

        TutorialModule(const TutorialModule&) = delete;
        TutorialModule& operator=(const TutorialModule&) = delete;

        ~TutorialModule() override;

        void update() override;
        void redraw() override;
        void switch_setup() override;
        void switch_to(ModuleType new_mod) override;

    private:
        int text_position = 0;
        DisplayDriver *const screen;
        AudioDriver *const buzzer;
        SpiledDriver *const spiled;
        Theme *main_theme;
        ModuleType *current_type;
        
};