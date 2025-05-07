#include "Module.hpp"
#include "DisplayDriver.hpp"
#include "AudioDriver.hpp"
#include "SpiledDriver.hpp"

#include "Theme.hpp"

namespace WinModuleConstants {
    namespace Text {
        constexpr int text_x_pos = 50;
        constexpr int text_y_pos = 50;
    }
}

class WinModule : public Module{
public:
    WinModule(
        DisplayDriver *screen_ptr, 
        AudioDriver *buzzer_ptr, 
        SpiledDriver *spiled_ptr, 
        Theme *main_theme_ptr, 
        ModuleType *current_type_ptr
    );

    WinModule(const WinModule&) = delete;
    WinModule& operator=(const WinModule&) = delete;

    ~WinModule() override;

    /// @brief Module trait interface.
    void update() override;
    void redraw() override;
    void switch_setup() override;
    void switch_to(ModuleType new_mod) override;

private:
    DisplayDriver *screen;
    AudioDriver *buzzer; 
    SpiledDriver *spiled; 
    Theme *main_theme; 
    ModuleType *current_type;
};