#include "include/modules/TutorialModule.hpp"

#include "include/core/Theme.hpp"

#include "include/drivers/AudioDriver.hpp"
#include "include/drivers/DisplayDriver.hpp"
#include "include/drivers/SpiledDriver.hpp"

#include "include/modules/Module.hpp"

#include <iostream>

TutorialModule::TutorialModule(
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
        throw std::invalid_argument("Null pointer passed to TutorialModule constructor.");
    }
}

TutorialModule::~TutorialModule() {
    std::cout << "Tutorial module ending!..." << std::endl;
}

void TutorialModule::switch_setup() {
    screen->set_orientation(DisplayOrientation::Portrait);
}

void TutorialModule::switch_to(StateFlag new_mod) {
    *current_type = new_mod;
}

void TutorialModule::update() {
    if (tutorial_shown) {
        switch_to(StateFlag::Game);
        return;
    }
    if (spiled->read_knob_press(KnobColor::Red)) {
        switch_to(StateFlag::Menu);
    } else if (spiled->read_knob_press(KnobColor::Blue) ||
               spiled->read_knob_press(KnobColor::Green)) {
        switch_to(StateFlag::Game);
        tutorial_shown = true;
    }
    int pos_change = spiled->read_knob_change(KnobColor::Green);
    if (pos_change != 0) {
        if (text_position + pos_change < Constants::Text::vertical_limit_neg) {
            text_position = Constants::Text::vertical_limit_neg;
        } else if (text_position + pos_change > Constants::Text::vertical_limit_pos) {
            text_position = Constants::Text::vertical_limit_pos;
        } else {
            text_position += pos_change;
        }
    }
}

void TutorialModule::redraw() {
    if (tutorial_shown) {
        return;
    }
    const char *text = "\
Space Invaders\n\n\n\
The goal of this game is to destroy all the\naliens before they reach the bottom of\nthe screen.\n\n\
To control the turret, rotate the Green\nknob to move it left and right.\n\n\
To shoot, press the Blue knob.\n\n\
To return to the main menu press the Red\nknob.\n\n\
To start the game, press the Green or the\nBlue knob.\n\n\
Good luck!\n\n\
Fun fact, you can scroll on this page\nusing the Green knob, try it!\n\n\
\n\n\n\n\n\n\n\n\
Good job! :)\
";
    screen->fill_screen(main_theme->background);
    screen->draw_text(
        0, 
        text_position, 
        main_theme->font, 
        text, 
        main_theme->text
    );
    screen->flush();
}