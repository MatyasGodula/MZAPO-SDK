#include "TutorialModule.hpp"
#include "Theme.hpp"
#include "SpiledDriver.hpp"
#include "AudioDriver.hpp"
#include "DisplayDriver.hpp"
#include "Module.hpp"

TutorialModule::TutorialModule(
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
    
{}

TutorialModule::~TutorialModule() {
    // Nothing to do here
}

void TutorialModule::switch_setup() {
    screen->set_orientation(DisplayOrientation::Portrait);
}

void TutorialModule::switch_to(ModuleType new_mod) {
    *current_type = new_mod;
    buzzer->play_tone(Tone::Selection, 250);
    spiled->clear_led_line();
}

void TutorialModule::update() {
    if (spiled->read_knob_press(KnobColor::Red)) {
        buzzer->play_tone(Tone::Selection, 250);
        *current_type = ModuleType::Menu;
    } else if (spiled->read_knob_press(KnobColor::Blue) || 
               spiled->read_knob_press(KnobColor::Green)) {
        buzzer->play_tone(Tone::Selection, 250);
        *current_type = ModuleType::Game;
    }
    int pos_change = spiled->read_knob_change(KnobColor::Green);
    if (pos_change != 0) {
        if (text_position + pos_change < Constants::Text::vertical_limit_pos) {
            text_position = Constants::Text::vertical_limit_pos;
        } else if (text_position + pos_change > Constants::Text::vertical_limit_neg) {
            text_position = Constants::Text::vertical_limit_neg;
        } else {
            text_position += pos_change;
        }
    }
}

void TutorialModule::redraw() {
    const char* text = 
    "Space Invaders\n";
}