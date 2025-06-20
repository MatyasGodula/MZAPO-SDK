#include "internal/modules/GameEndModule.hpp"

#include "include/utils/Theme.hpp"

#include "include/drivers/AudioDriver.hpp"
#include "include/drivers/DisplayDriver.hpp"
#include "include/drivers/SpiledDriver.hpp"

#include "include/modules/Module.hpp"

#include <exception>
#include <string_view>
#include <iostream>

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
        throw std::invalid_argument("Invalid argument passed to GameEndModule constructor");
    }
}

GameEndModule::~GameEndModule() {
    std::cout << "GameEndModule ending!..." << std::endl; 
}

void GameEndModule::update() {
    if (spiled->read_knob_press(KnobColor::Green)) {
        switch_to(GameEndModuleConstants::Selections::selection_types[selection]);
        return;
    }
    int selected_copy = selection;
    delta = spiled->read_knob_change(KnobColor::Green);
    aretation += delta;
    if (aretation <= (- GameEndModuleConstants::Selections::selection_aretation)) {
        selection += aretation / GameEndModuleConstants::Selections::selection_aretation;
        aretation = aretation % GameEndModuleConstants::Selections::selection_aretation;
    } else if (aretation >= GameEndModuleConstants::Selections::selection_aretation) {
        selection += aretation / GameEndModuleConstants::Selections::selection_aretation;
        aretation = aretation % GameEndModuleConstants::Selections::selection_aretation;
    }

    if (selection >= static_cast<int>(GameEndModuleConstants::Selections::selection_count)) {
        selection = static_cast<int>(GameEndModuleConstants::Selections::selection_count) - 1;
    } else if (selection < 0) {
        selection = 0;
    }
    if (selected_copy != selection) {
        buzzer->play_tone(Tone::Selection, 100);
    }
}

void GameEndModule::redraw() {
    screen->fill_screen(main_theme->background);
    std::string_view text = "NULL";
    if (game_end_state == GameEndState::Win) {
        text = "You Won!";
    } else if (game_end_state == GameEndState::Loss) {
        text = "Game Over!";
    }
    int vertical_index = GameEndModuleConstants::Selections::selection_y_pos;
    screen->draw_text(
        GameEndModuleConstants::Selections::selection_x_pos + 10, 
        vertical_index - GameEndModuleConstants::Selections::selection_height + 8,       
        main_theme->font, 
        text,
        main_theme->text
    );

    screen->draw_rectangle(
        GameEndModuleConstants::Selections::selection_x_pos,
        (GameEndModuleConstants::Selections::selection_y_pos + 
            selection * GameEndModuleConstants::Selections::selection_height),
        GameEndModuleConstants::Selections::selection_width,
        GameEndModuleConstants::Selections::selection_height,
        main_theme->selection
    );

    for (std::string_view selection_name : GameEndModuleConstants::Selections::selections) {
        screen->draw_text(
            GameEndModuleConstants::Selections::selection_x_pos + 10, 
            vertical_index + 8,
            main_theme->font, 
            selection_name, 
            main_theme->text
        );
        vertical_index += GameEndModuleConstants::Selections::selection_height;
    }

    screen->flush();
}

void GameEndModule::switch_setup() {
    screen->set_orientation(DisplayOrientation::Portrait);
    buzzer->play_tone(Tone::Explosion, 500); // Sad sound
}

void GameEndModule::switch_to(StateFlag new_mod) {
    *current_type = new_mod;
}

void GameEndModule::set_game_end_state(GameEndState state) {
    game_end_state = state;
}