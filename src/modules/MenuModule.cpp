#include "include/modules/MenuModule.hpp"

#include "include/core/Theme.hpp"

#include "include/drivers/AudioDriver.hpp"
#include "include/drivers/DisplayDriver.hpp"
#include "include/drivers/SpiledDriver.hpp"

#include "include/modules/Module.hpp"

#include <iostream>
#include <stdexcept>
#include <string_view>

MenuModule::MenuModule(
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
        throw std::invalid_argument("Invalid argument passed to MenuModule constructor");
    }
}

MenuModule::~MenuModule() {
    std::cout << "Menu module ending!..." << std::endl;
}

void MenuModule::update() {
    if (spiled->read_knob_press(KnobColor::Green)) {
        switch_to(MenuModuleTypes::SelectionTypes[selection]);
        return;
    }
    int selected_copy = selection;
    int delta = spiled->read_knob_change(KnobColor::Green);
    aretation += delta;
    if (aretation >= static_cast<int>(MenuModuleTypes::SelectionCount * MenuConstants::selection_aretation)) {
        aretation = static_cast<int>(MenuModuleTypes::SelectionCount * MenuConstants::selection_aretation) - 1;
    } else if (aretation < 0) {
        aretation = 0;
    }
    selection = aretation / MenuConstants::selection_aretation;
    if (selected_copy != selection) {
        buzzer->play_tone(Tone::Selection, 100);
    }
}

void MenuModule::redraw() {
    screen->fill_screen(main_theme->background);
    int vertical_index = MenuConstants::selection_pos_y;
    screen->draw_text(
        MenuConstants::selection_pos_x + 10,
        vertical_index - MenuConstants::selection_height + 8,
        main_theme->font, 
        "SPACE INVADERS", 
        main_theme->text
    );

    screen->draw_rectangle(
        MenuConstants::selection_pos_x,
        MenuConstants::selection_pos_y + selection * MenuConstants::selection_height,
        MenuConstants::selection_width, 
        MenuConstants::selection_height, 
        main_theme->selection
    );

    for (std::string_view selection_name : MenuModuleTypes::SelectionNames) {
        screen->draw_text(
            MenuConstants::selection_pos_x + 10, 
            vertical_index + 8,
            main_theme->font, 
            selection_name, 
            main_theme->text
        );
        vertical_index += MenuConstants::selection_height;
    }

    screen->flush();
    //std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void MenuModule::switch_setup() {
    screen->set_orientation(DisplayOrientation::Landscape);
}

void MenuModule::switch_to(StateFlag new_mod) {
    *current_type = new_mod;
}
