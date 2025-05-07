#include "Module.hpp"
#include "WinModule.hpp"
#include "DisplayDriver.hpp"
#include "AudioDriver.hpp"
#include "SpiledDriver.hpp"
#include "Theme.hpp"

#include <exception>

WinModule::WinModule(
    DisplayDriver *screen_ptr, 
    AudioDriver *buzzer_ptr, 
    SpiledDriver *spiled_ptr, 
    Theme *main_theme_ptr, 
    ModuleType *current_type_ptr
) :
    screen(screen_ptr),
    buzzer(buzzer_ptr),
    spiled(spiled_ptr),
    main_theme(main_theme_ptr),
    current_type(current_type_ptr)
{
    if (!screen || !buzzer || !spiled || !main_theme || !current_type) {
        throw std::invalid_argument("Invalid argument passed to WinModule constructor");
    }
}

WinModule::~WinModule() {
    // Nothing happens
}

void WinModule::update() {
    if (spiled->read_knob_press(KnobColor::Red) ||
        spiled->read_knob_press(KnobColor::Blue) || 
        spiled->read_knob_press(KnobColor::Green)) 
    {
        switch_to(ModuleType::Exit);
    }
}

void WinModule::redraw() {
    screen->fill_screen(main_theme->background);
    screen->draw_text(
        WinModuleConstants::Text::text_x_pos, 
        WinModuleConstants::Text::text_y_pos, 
        FontType::WinFreeSystem14x16,
        "You Won!\n\nPress any knob to exit", 
        main_theme->text
    );
    screen->flush();
}

void WinModule::switch_setup() {
    screen->set_orientation(DisplayOrientation::Portrait);
    buzzer->play_tone(Tone::Explosion, 500); // Sad sound
}

void WinModule::switch_to(ModuleType new_mod) {
    *current_type = new_mod;
}