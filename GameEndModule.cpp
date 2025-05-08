#include "WinModule.hpp"
#include "AudioDriver.hpp"
#include "DisplayDriver.hpp"
#include "Module.hpp"
#include "SpiledDriver.hpp"
#include "Theme.hpp"

#include <exception>

GameEndModule::GameEndModule(
    DisplayDriver *screen_ptr, 
    AudioDriver *buzzer_ptr, 
    SpiledDriver *spiled_ptr,
    Theme *main_theme_ptr, 
    StateFlag *current_type_ptr
): 
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

GameEndModule::~GameEndModule() {
    // Nothing happens
}

void GameEndModule::update() {
    if (spiled->read_knob_press(KnobColor::Red) || 
        spiled->read_knob_press(KnobColor::Blue) ||
        spiled->read_knob_press(KnobColor::Green)) {
        switch_to(StateFlag::Exit);
    }
}

void GameEndModule::redraw() {
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

void GameEndModule::switch_setup() {
    screen->set_orientation(DisplayOrientation::Portrait);
    buzzer->play_tone(Tone::Explosion, 500); // Sad sound
}

void GameEndModule::switch_to(StateFlag new_mod) {
    *current_type = new_mod;
}